/* mlk_oracle_config.h - minimal, oqs-free mlkem-native config for the oracle.
 * Replaces integration/liboqs/config_c.h so the boundary functions compile
 * WITHOUT the liboqs public headers (<oqs/rand.h>, <oqs/sha3.h>).
 * The oracle only calls arithmetic (NTT/mulcache/basemul/invNTT); the FIPS202
 * and randombytes glue is never executed, so trivial stubs suffice. */
#ifndef MLK_ORACLE_CONFIG_H
#define MLK_ORACLE_CONFIG_H

/* MLK_CONFIG_PARAMETER_SET is passed on the command line (-D...=768). */

#define MLK_CONFIG_NAMESPACE_PREFIX MLK_ORACLE

/* Point FIPS202 at oqs-free stubs (declarations only; never called). */
#define MLK_CONFIG_FIPS202_CUSTOM_HEADER   "mlk_fips202_stub.h"
#define MLK_CONFIG_FIPS202X4_CUSTOM_HEADER "mlk_fips202x4_stub.h"

#endif /* MLK_ORACLE_CONFIG_H */
