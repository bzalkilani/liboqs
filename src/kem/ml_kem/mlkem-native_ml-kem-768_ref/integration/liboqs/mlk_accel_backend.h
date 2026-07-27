#ifndef MLK_ACCEL_BACKEND_H
#define MLK_ACCEL_BACKEND_H

#define MLK_USE_NATIVE_NTT
#define MLK_USE_NATIVE_INTT
#define MLK_USE_NATIVE_POLYVEC_BASEMUL_ACC_MONTGOMERY_CACHED

#if !defined(__ASSEMBLER__)
#include <stdint.h>
#include "mlk_accel_client.h"

static MLK_INLINE void mlk_ntt_native(int16_t p[MLKEM_N])
{
  mlk_accel_ntt(p);
}

static MLK_INLINE void mlk_intt_native(int16_t p[MLKEM_N])
{
  mlk_accel_intt(p);
}

static MLK_INLINE void mlk_polyvec_basemul_acc_montgomery_cached_k3_native(
    int16_t r[MLKEM_N], const int16_t a[3 * MLKEM_N],
    const int16_t b[3 * MLKEM_N], const int16_t b_cache[3 * (MLKEM_N / 2)])
{
  mlk_accel_basemul_acc_k3(r, a, b, b_cache);
}
#endif /* !__ASSEMBLER__ */

#endif
