/*
 * A simple 8-bit element first-in, first-out (FIFO) queue.
 * Target for this small helper module are small programs on MCUs or
 * linux kernel drivers, where reading/writing bytes is asynchronous
 * and no hardware support is available (hardware FIFO or DMA).
 *
 * /!\ WARNING:/!\ This implementation is not thread-safe! Use an appropriate
 * locking mechanism in the calling code. This can be a mutex when an OS or an
 * RTOS is available, or a mechanism that disables interrupts on a bare-metal
 * application.
 *
 * This file is part of simple-fifo.
 *
 * simple-fifo is licensed under the MIT License. See the LICENSE file in the
 * project's root directory for more information.
 */

#ifndef BYTE_FIFO_H_
#define BYTE_FIFO_H_

#include <stdint.h>

#define BF_ERR_OK      0
#define BF_ERR_NULLPTR (-1)
#define BF_ERR_INVAL   (-2)

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Structure representing a FIFO buffer.
 *
 * This structure is used to manage a byte FIFO (First-In-First-Out) buffer.
 * It contains the necessary metadata to track the state of the FIFO, such as
 * the read and write indices, the number of elements currently in the buffer,
 * and the buffer's size.
 *
 * @note This structure should be considered private and only used during
 *       initialization. The members are not part of the public API and should
 *       not be accessed directly after initialization.
 *
 * Members:
 * - `data`: Pointer to the memory buffer used to store the FIFO's elements.
 *           This buffer must be allocated and managed by the caller.
 * - `size`: The total size of the FIFO buffer in bytes.
 * - `write_index`: The index where the next byte will be written.
 * - `read_index`: The index where the next byte will be read.
 * - `n_elements`: The current number of elements in the FIFO.
 */
struct byte_fifo_t
{
    uint8_t* const data;
    const uint16_t size;
    uint16_t write_index;
    uint16_t read_index;
    uint16_t n_elements;
};

/**
 * @brief Type definition for error codes used in the FIFO API.
 *
 * This type is used to represent error codes returned by the FIFO functions.
 * Error codes are negative values, with `BF_ERR_OK` (0) indicating success.
 *
 * Common usage: when no information other than success or failure is returned.
 */
typedef int32_t bf_err_t;

/**
 * @brief Type definition for result values used in the FIFO API.
 *
 * This type is used to represent the result of FIFO operations. Positive
 * values indicate the number of bytes successfully processed, while negative
 * values represent error codes.
 *
 * Common usage: when functions return either a value or an error code.
 */
typedef int32_t bf_res_t;

/**
 * @brief Extracts the error code from a result value.
 *
 * This function checks the result value and determines if it represents an error.
 *
 * @param res The result value to check. *
 * @return The extracted error code:
 *         - `BF_ERR_OK` if the result value is non-negative.
 *         - The negative result value if it represents an error.
 *
 * @note This function is a utility to simplify error handling in the FIFO API.
 */
static inline bf_err_t byte_fifo_get_error(bf_res_t res)
{
    return (res < 0) ? res : BF_ERR_OK;
}

/**
 * @brief Initializes a byte FIFO (First-In-First-Out) buffer.
 *
 * This function sets up the FIFO structure by resetting its indices,
 * clearing the data buffer, and ensuring the FIFO is ready for use.
 *
 * @param fifo Pointer to the `struct byte_fifo_t` to initialize.
 *             - `fifo->data` must point to a valid memory buffer.
 *             - `fifo->size` must be greater than 0.
 *
 * @return BF_ERR_OK on success.
 * @return BF_ERR_NULLPTR if `fifo` is NULL or `fifo->data` is NULL.
 * @return BF_ERR_INVAL if `fifo->size` is 0.
 *
 * @note This function clears the memory buffer pointed to by `fifo->data`.
 *       Ensure the buffer is properly allocated before calling this function.
 * @note This function is not thread-safe. If used in a multi-threaded
 *       environment, appropriate locking mechanisms should be implemented.
 */
bf_err_t byte_fifo_init(struct byte_fifo_t* const fifo);

/**
 * @brief Resets the FIFO buffer.
 *
 * This function resets the FIFO by clearing its data buffer and resetting
 * the read and write indices to their initial state.
 *
 * @param fifo Pointer to an instantiated `struct byte_fifo_t`.
 *
 * @return BF_ERR_OK on success.
 * @return BF_ERR_NULLPTR if `fifo` is NULL or `fifo->data` is NULL.
 * @return BF_ERR_INVAL if `fifo->size` is 0.
 *
 * @note Use appropriate locking mechanisms in multi-threaded
 *       environments.
 */
bf_err_t byte_fifo_reset(struct byte_fifo_t* const fifo);

/**
 * @brief Checks if the FIFO is empty.
 *
 * This function checks whether the FIFO contains any elements.
 *
 * @param fifo Pointer to an instantiated `struct byte_fifo_t`.
 *
 * @return 1 if the FIFO is empty.
 * @return 0 if the FIFO is not empty.
 * @return BF_ERR_NULLPTR if `fifo` is NULL.
 *
 * @note This function does not modify the FIFO.
 */
bf_res_t byte_fifo_is_empty(const struct byte_fifo_t* const fifo);

/**
 * @brief Checks if the FIFO is full.
 *
 * This function checks whether the FIFO is completely filled with elements.
 *
 * @param fifo Pointer to an instantiated `struct byte_fifo_t`.
 *
 * @return 1 if the FIFO is full.
 * @return 0 if the FIFO is not full.
 * @return BF_ERR_NULLPTR if `fifo` is NULL.
 *
 * @note This function does not modify the FIFO.
 */
bf_res_t byte_fifo_is_full(const struct byte_fifo_t* const fifo);

/**
 * @brief Writes bytes to the FIFO until it is full or the source buffer is exhausted.
 *
 * This function writes data from the source buffer into the FIFO. If the FIFO
 * becomes full, the function stops writing.
 *
 * @param fifo Pointer to an initialized FIFO, see `byte_fifo_init`.
 * @param src Pointer to the source buffer.
 * @param len Length of the source buffer.
 *
 * @return BF_ERR_NULLPTR if any provided pointer is NULL.
 * @return The number of bytes successfully written to the FIFO. A return value
 *         of 0 indicates that no bytes could be written.
 */
bf_res_t byte_fifo_write(struct byte_fifo_t* const fifo,
                         const uint8_t* const src,
                         uint16_t len);

/**
 * @brief Writes bytes to the FIFO and overwrites existing data if the FIFO is full.
 *
 * This function writes data from the source buffer into the FIFO. If the FIFO
 * becomes full, it overwrites the oldest data in the FIFO.
 *
 * @param fifo Pointer to an initialized FIFO, see `byte_fifo_init`.
 * @param src Pointer to the source buffer.
 * @param len Length of the source buffer.
 *
 * @return BF_ERR_NULLPTR if any provided pointer is NULL.
 * @return The number of bytes that were overwritten in the FIFO. A return value
 *         of 0 indicates that all source bytes were written without overwriting.
 */
bf_res_t byte_fifo_overwrite(struct byte_fifo_t* const fifo,
                             const uint8_t* const src,
                             uint16_t len);

/**
 * @brief Reads bytes from the FIFO into a destination buffer.
 *
 * This function reads data from the FIFO into the destination buffer. If the
 * FIFO does not contain enough data, it reads as much as possible.
 *
 * @param fifo Pointer to an initialized FIFO, see `byte_fifo_init`.
 * @param dest Pointer to the destination buffer.
 * @param len Length of the destination buffer (must be non-negative).
 *
 * @return BF_ERR_NULLPTR if any provided pointer is NULL.
 * @return The number of bytes successfully read from the FIFO. A return value
 *         of 0 indicates that the FIFO is empty.
 */
bf_res_t byte_fifo_read(struct byte_fifo_t* const fifo,
                        uint8_t* const dest,
                        uint16_t len);
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // BYTE_FIFO_H_
