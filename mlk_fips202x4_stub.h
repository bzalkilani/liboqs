/* oqs-free FIPS202-X4 stub for the oracle (declarations only; never called). */
#ifndef MLK_FIPS202X4_STUB_H
#define MLK_FIPS202X4_STUB_H
#include <stddef.h>
#include <stdint.h>

typedef struct { uint64_t ctx[100]; } shake128x4ctx;
void shake128x4_init(shake128x4ctx *state);
void shake128x4_absorb_once(shake128x4ctx *state, const uint8_t *in0,
                            const uint8_t *in1, const uint8_t *in2,
                            const uint8_t *in3, size_t inlen);
void shake128x4_squeezeblocks(uint8_t *out0, uint8_t *out1, uint8_t *out2,
                              uint8_t *out3, size_t nblocks,
                              shake128x4ctx *state);
void shake128x4_release(shake128x4ctx *state);
void shake256x4(uint8_t *out0, uint8_t *out1, uint8_t *out2, uint8_t *out3,
                size_t outlen, uint8_t *in0, uint8_t *in1, uint8_t *in2,
                uint8_t *in3, size_t inlen);

#define mlk_shake128x4ctx shake128x4ctx
#define mlk_shake128x4_init shake128x4_init
#define mlk_shake128x4_absorb_once shake128x4_absorb_once
#define mlk_shake128x4_squeezeblocks shake128x4_squeezeblocks
#define mlk_shake128x4_release shake128x4_release
#define mlk_shake256x4 shake256x4

#endif /* MLK_FIPS202X4_STUB_H */
