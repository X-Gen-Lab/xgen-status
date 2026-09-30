/**
 * \file            include_version.cpp
 * \brief           Verify the generated version header in C++17
 */
#include <xgen/status/version.h>
static_assert(XGS_ABI_VERSION == 1, "Expected the independent status ABI");
