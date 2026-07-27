#ifndef MLK_ACCEL_CLIENT_H
#define MLK_ACCEL_CLIENT_H

#include <stdint.h>

void mlk_accel_ntt(int16_t p[256]);
void mlk_accel_intt(int16_t p[256]);
void mlk_accel_basemul_acc_k3(int16_t r[256], const int16_t a[768],
                              const int16_t b[768],
                              const int16_t b_cache[384]);

#endif
