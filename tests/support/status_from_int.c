/**
 * \file            status_from_int.c
 * \brief           Exercise unknown integer status conversion in the C language
 */

#include <xgen/status/status.h>

const char* xgs_test_status_string_from_int(int status);

const char* xgs_test_status_string_from_int(int status) {
    return xgs_status_string((xgs_status_t)status);
}
