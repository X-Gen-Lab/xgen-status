/**
 * \file            status.h
 * \brief           Common status codes and optional diagnostic strings
 * \author          X-Gen Lab
 */

#ifndef XGS_STATUS_H
#define XGS_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Common operation results with stable numeric values.
 * \details         The type and constants require no storage or initialization.
 *                  Enum representation follows the selected C ABI; this is not
 *                  a wire encoding. There is no dependency on a platform,
 *                  allocator, clock, or mutable global state.
 */
typedef enum {
    XGS_OK = 0, /**< Operation completed. */
    XGS_INVALID_ARGUMENT =
        -1,                 /**< An argument violates the operation contract. */
    XGS_NO_MEMORY = -2,     /**< Allocation failed. */
    XGS_CAPACITY = -3,      /**< Bounded storage is exhausted. */
    XGS_NOT_FOUND = -4,     /**< The requested entry was not found. */
    XGS_BUSY = -5,          /**< The operation cannot proceed yet. */
    XGS_UNSUPPORTED = -6,   /**< The requested operation is unsupported. */
    XGS_ALREADY_EXISTS = -7 /**< The requested entry already exists. */
} xgs_status_t;

/**
 * \brief           Describe a status code with static diagnostic text.
 * \param[in]       status: Status value; unrecognized values are accepted.
 * \return          Non-NULL, immutable, NUL-terminated text with program
 * lifetime. Unrecognized values return "Unknown status".
 * \note            Requires the optional xgs::strings target. Merely including
 *                  this declaration does not link its implementation. No heap,
 *                  initialization, locking, I/O, or mutable state is used.
 * Calls are reentrant; the caller must not modify or free the text.
 */
const char* xgs_status_string(xgs_status_t status);

#ifdef __cplusplus
}
#endif

#endif
