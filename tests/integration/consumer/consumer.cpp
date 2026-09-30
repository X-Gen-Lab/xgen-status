/**
 * \file            consumer.cpp
 * \brief           Standalone C++ consumer including public C declarations
 */

#include <xgen/status/status.h>
#include <xgen/status/version.h>

#if XGS_CONSUME_STRINGS
#include <cstring>
#endif

int main() {
    const xgs_status_t status = XGS_INVALID_ARGUMENT;
    if (status != -1 || XGS_VERSION_MINOR != 1) {
        return 1;
    }
#if XGS_CONSUME_STRINGS
    if (std::strcmp(xgs_status_string(status), "Invalid argument") != 0) {
        return 2;
    }
#endif
    return 0;
}
