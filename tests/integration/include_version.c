/**
 * \file            include_version.c
 * \brief           Verify that the generated version header is self-contained
 */
#include <xgen/status/version.h>
_Static_assert(XGS_ABI_VERSION == 1, "Expected the independent status ABI");
