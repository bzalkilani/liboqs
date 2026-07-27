/* oracle.c - golden-vector generator for the ML-KEM-768 offload boundaries.
 *
 * Dumps the int16 arrays at each stage your accelerator replaces:
 *     a_in / b_in     normal-domain inputs (feed these to the FPGA)
 *     a_ntt / b_ntt   after NTT           (checks NTT + bitrev ordering)
 *     b_cache         after mulcache
 *     r_basemul       after k=3 basemul-accumulate
 *     r_invntt        after invNTT+tomont (checks invNTT + Montgomery scale)
 *
 * Uses the REAL library per-poly functions from poly.c (mlk_poly_ntt,
 * mlk_poly_mulcache_compute, mlk_poly_invntt_tomont, mlk_montgomery_reduce).
 * The polyvec wrappers are trivial loops; the k=3 basemul-accumulate body is
 * copied VERBATIM from poly_k.c (the portable-C arm, lines 146-174) so the
 * oracle needs no header beyond poly.h and drags in no FIPS202/oqs deps.
 * Read-only w.r.t. the liboqs tree.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "poly.h"

/* splitmix64: fixed, portable, so the FPGA-side driver reproduces inputs */
static uint64_t g_s;
static uint64_t nxt(void) {
  uint64_t z = (g_s += 0x9E3779B97F4A7C15ULL);
  z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
  z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
  return z ^ (z >> 31);
}

/* NTT precondition (native/api.h): |coeff| < q, i.e. in (-3329, 3329) */
static void fill_normal(mlk_poly v[MLKEM_K]) {
  for (int k = 0; k < MLKEM_K; k++)
    for (int i = 0; i < MLKEM_N; i++)
      v[k].coeffs[i] = (int16_t)((int)(nxt() % (2 * MLKEM_Q - 1)) - (MLKEM_Q - 1));
}

/* Verbatim from poly_k.c:146-174 (portable arm). Signature flattened to plain
 * arrays; semantics identical to mlk_polyvec_basemul_acc_montgomery_cached. */
static void basemul_acc(mlk_poly *r, const mlk_poly a[MLKEM_K],
                        const mlk_poly b[MLKEM_K],
                        const mlk_poly_mulcache b_cache[MLKEM_K]) {
  for (unsigned i = 0; i < MLKEM_N / 2; i++) {
    int32_t t[2] = {0};
    for (unsigned k = 0; k < MLKEM_K; k++) {
      t[0] += (int32_t)a[k].coeffs[2 * i + 1] * b_cache[k].coeffs[i];
      t[0] += (int32_t)a[k].coeffs[2 * i] * b[k].coeffs[2 * i];
      t[1] += (int32_t)a[k].coeffs[2 * i] * b[k].coeffs[2 * i + 1];
      t[1] += (int32_t)a[k].coeffs[2 * i + 1] * b[k].coeffs[2 * i];
    }
    r->coeffs[2 * i + 0] = mlk_montgomery_reduce(t[0]);
    r->coeffs[2 * i + 1] = mlk_montgomery_reduce(t[1]);
  }
}

static void dump(const char *tag, int k, const int16_t *p, int n) {
  printf("%s[%d]", tag, k);
  for (int i = 0; i < n; i++) printf(",%d", p[i]);
  printf("\n");
}

int main(int argc, char **argv) {
  g_s = (argc > 1) ? (uint64_t)strtoull(argv[1], NULL, 0) : 1ULL;

  mlk_poly a[MLKEM_K], b[MLKEM_K], r;
  mlk_poly_mulcache bc[MLKEM_K];

  fill_normal(a);
  fill_normal(b);
  for (int k = 0; k < MLKEM_K; k++) dump("a_in", k, a[k].coeffs, MLKEM_N);
  for (int k = 0; k < MLKEM_K; k++) dump("b_in", k, b[k].coeffs, MLKEM_N);

  for (int k = 0; k < MLKEM_K; k++) mlk_poly_ntt(&a[k]);
  for (int k = 0; k < MLKEM_K; k++) mlk_poly_ntt(&b[k]);
  for (int k = 0; k < MLKEM_K; k++) dump("a_ntt", k, a[k].coeffs, MLKEM_N);
  for (int k = 0; k < MLKEM_K; k++) dump("b_ntt", k, b[k].coeffs, MLKEM_N);

  for (int k = 0; k < MLKEM_K; k++) mlk_poly_mulcache_compute(&bc[k], &b[k]);
  for (int k = 0; k < MLKEM_K; k++) dump("b_cache", k, bc[k].coeffs, MLKEM_N / 2);

  basemul_acc(&r, a, b, bc);
  dump("r_basemul", 0, r.coeffs, MLKEM_N);

  mlk_poly_invntt_tomont(&r);
  dump("r_invntt", 0, r.coeffs, MLKEM_N);
  return 0;
}
