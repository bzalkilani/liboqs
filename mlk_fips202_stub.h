/* oqs-free FIPS202 stub for the oracle. Declares the names symmetric.h maps to
 * via macros; none are called by the oracle, so no definitions are needed. */
#ifndef MLK_FIPS202_STUB_H
#define MLK_FIPS202_STUB_H
#include <stddef.h>
#include <stdint.h>

typedef struct { uint64_t ctx[26]; } shake128ctx;
void shake128_init(shake128ctx *state);
void shake128_absorb_once(shake128ctx *state, const uint8_t *in, size_t inlen);
void shake128_squeezeblocks(uint8_t *out, size_t nblocks, shake128ctx *state);
void shake128_release(shake128ctx *state);
void shake256(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen);
void sha3_256(uint8_t *out, const uint8_t *in, size_t inlen);
void sha3_512(uint8_t *out, const uint8_t *in, size_t inlen);

#define mlk_shake128ctx shake128ctx
#define mlk_shake128_init shake128_init
#define mlk_shake128_absorb_once shake128_absorb_once
#define mlk_shake128_squeezeblocks shake128_squeezeblocks
#define mlk_shake128_release shake128_release
#define mlk_shake256 shake256
#define mlk_sha3_256 sha3_256
#define mlk_sha3_512 sha3_512

#endif /* MLK_FIPS202_STUB_H */
