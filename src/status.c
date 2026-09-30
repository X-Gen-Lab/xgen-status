/**
 * \file            status.c
 * \brief           Optional diagnostic strings for common status codes
 * \author          X-Gen Lab
 */

#include <xgen/status/status.h>

const char* xgs_status_string(xgs_status_t status) {
    switch (status) {
        case XGS_OK:
            return "OK";
        case XGS_INVALID_ARGUMENT:
            return "Invalid argument";
        case XGS_NO_MEMORY:
            return "No memory";
        case XGS_CAPACITY:
            return "Capacity exhausted";
        case XGS_NOT_FOUND:
            return "Not found";
        case XGS_BUSY:
            return "Busy";
        case XGS_UNSUPPORTED:
            return "Unsupported";
        case XGS_ALREADY_EXISTS:
            return "Already exists";
        default:
            return "Unknown status";
    }
}
