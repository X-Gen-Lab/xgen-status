/**
 * \file            consumer.c
 * \brief           Standalone C consumer of installed or source targets
 */

#include <xgen/status/status.h>
#include <xgen/status/version.h>

#if XGS_CONSUME_STRINGS
#include <string.h>
#endif

int main(void) {
    const xgs_status_t status = XGS_CAPACITY;
    if (status != -3 || XGS_ABI_VERSION != 1) {
        return 1;
    }
#if XGS_CONSUME_STRINGS
    if (strcmp(xgs_status_string(status), "Capacity exhausted") != 0) {
        return 2;
    }
#endif
    return 0;
}
