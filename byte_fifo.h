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
    unsigned char* const data;
    const unsigned int size;
    unsigned int write_index;
    unsigned int read_index;
    unsigned int n_elements;
};

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
 * @return 0 on success.
 * @return -EFAULT if `fifo` is NULL or `fifo->data` is NULL.
 * @return -EINVAL if `fifo->size` is 0.
 *
 * @note This function clears the memory buffer pointed to by `fifo->data`.
 *       Ensure the buffer is properly allocated before calling this function.
 * @note This function is not thread-safe. If used in a multi-threaded
 *       environment, appropriate locking mechanisms should be implemented.
 */
int byte_fifo_init(struct byte_fifo_t* const fifo);

/**
 * @brief Resets the FIFO buffer.
 *
 * This function resets the FIFO by clearing its data buffer and resetting
 * the read and write indices to their initial state.
 *
 * @param fifo Pointer to an instantiated `struct byte_fifo_t`.
 *
 * @return 0 on success.
 * @return -EFAULT if `fifo` is NULL or `fifo->data` is NULL.
 * @return -EINVAL if `fifo->size` is 0.
 *
 * @note Use appropriate locking mechanisms in multi-threaded
 *       environments.
 */
int byte_fifo_reset(struct byte_fifo_t* const fifo);

/**
 * @brief Checks if the FIFO is empty.
 *
 * This function checks whether the FIFO contains any elements.
 *
 * @param fifo Pointer to an instantiated `struct byte_fifo_t`.
 *
 * @return 1 if the FIFO is empty.
 * @return 0 if the FIFO is not empty.
 * @return -EFAULT if `fifo` is NULL.
 *
 * @note This function does not modify the FIFO.
 */
int byte_fifo_is_empty(const struct byte_fifo_t* const fifo);

/**
 * @brief Checks if the FIFO is full.
 *
 * This function checks whether the FIFO is completely filled with elements.
 *
 * @param fifo Pointer to an instantiated `struct byte_fifo_t`.
 *
 * @return 1 if the FIFO is full.
 * @return 0 if the FIFO is not full.
 * @return -EFAULT if `fifo` is NULL.
 *
 * @note This function does not modify the FIFO.
 */
int byte_fifo_is_full(const struct byte_fifo_t* const fifo);

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
 * @return -EFAULT if any provided pointer is NULL.
 * @return The number of bytes successfully written to the FIFO. A return value
 *         of 0 indicates that no bytes could be written.
 */
int byte_fifo_write(struct byte_fifo_t* const fifo,
                    const unsigned char* const src,
                    unsigned int len);

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
 * @return -EFAULT if any provided pointer is NULL.
 * @return The number of bytes that were overwritten in the FIFO. A return value
 *         of 0 indicates that all source bytes were written without overwriting.
 */
int byte_fifo_overwrite(struct byte_fifo_t* const fifo,
                        const unsigned char* const src,
                        unsigned int len);

/**
 * @brief Reads bytes from the FIFO into a destination buffer.
 *
 * This function reads data from the FIFO into the destination buffer. If the
 * FIFO does not contain enough data, it reads as much as possible.
 *
 * @param fifo Pointer to an initialized FIFO, see `byte_fifo_init`.
 * @param dest Pointer to the destination buffer.
 * @param len Length of the destination buffer (must be positive).
 *
 * @return -EFAULT if any provided pointer is NULL.
 * @return -EINVAL if `len` is negative.
 * @return The number of bytes successfully read from the FIFO. A return value
 *         of 0 indicates that the FIFO is empty.
 */
int byte_fifo_read(struct byte_fifo_t* const fifo,
                   unsigned char* const dest,
                   int len);
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // BYTE_FIFO_H_
