#include "byte_fifo.h"

#include <string.h>  // memset

#define RETURN_IF(x, y) \
    if ((x)) return (y)

bf_err_t byte_fifo_init(struct byte_fifo_t* const fifo)
{
    RETURN_IF(NULL == fifo, BF_ERR_NULLPTR);
    RETURN_IF(NULL == fifo->data, BF_ERR_NULLPTR);
    RETURN_IF(0 == fifo->size, BF_ERR_INVAL);

    fifo->write_index = 0U;
    fifo->read_index = 0U;
    fifo->n_elements = 0U;

    memset((void*)fifo->data, 0, fifo->size);

    return 0;
}

bf_err_t byte_fifo_reset(struct byte_fifo_t* const fifo)
{
    return byte_fifo_init(fifo);
}

bf_res_t byte_fifo_is_empty(const struct byte_fifo_t* const fifo)
{
    RETURN_IF(NULL == fifo, BF_ERR_NULLPTR);
    return (fifo->n_elements == 0);
}

bf_res_t byte_fifo_is_full(const struct byte_fifo_t* const fifo)
{
    RETURN_IF(NULL == fifo, BF_ERR_NULLPTR);
    return (fifo->n_elements == fifo->size);
}

bf_res_t byte_fifo_write(struct byte_fifo_t* const fifo, const uint8_t* const src, uint16_t len)
{
    RETURN_IF(NULL == fifo, BF_ERR_NULLPTR);
    RETURN_IF(NULL == fifo->data, BF_ERR_NULLPTR);
    RETURN_IF(NULL == src, BF_ERR_NULLPTR);

    int32_t n_bytes_written = 0;
    int32_t n_bytes_to_write = (int32_t)len;
    while (n_bytes_written < n_bytes_to_write)
    {
        if (fifo->n_elements < fifo->size)
        {
            uint16_t write_index = fifo->write_index;
            fifo->data[write_index] = src[n_bytes_written];
            fifo->write_index = (write_index < (fifo->size - 1)) ? write_index + 1 : 0;
            fifo->n_elements++;

            // Update
            n_bytes_written++;
        }
        else
        {
            // No more available bytes
            break;
        }
    }

    return n_bytes_written;
}

bf_res_t byte_fifo_overwrite(struct byte_fifo_t* const fifo, const uint8_t* const src, uint16_t len)
{
    RETURN_IF(NULL == fifo, BF_ERR_NULLPTR);
    RETURN_IF(NULL == fifo->data, BF_ERR_NULLPTR);
    RETURN_IF(NULL == src, BF_ERR_NULLPTR);

    int32_t n_bytes_written = 0;
    int32_t n_bytes_overwritten = 0;
    int32_t n_bytes_to_write = (int32_t)len;
    while (n_bytes_to_write > 0)
    {
        uint16_t write_index = fifo->write_index;
        fifo->data[write_index] = src[n_bytes_written + n_bytes_overwritten];
        fifo->write_index = (write_index < (fifo->size - 1)) ? write_index + 1 : 0;

        if (fifo->n_elements < fifo->size)
        {
            n_bytes_written++;
            fifo->n_elements++;
        }
        else
        {
            uint16_t read_index = fifo->read_index;
            fifo->read_index = (read_index < (fifo->size - 1)) ? read_index + 1 : 0;
            n_bytes_overwritten++;
        }
        n_bytes_to_write--;
    }

    return n_bytes_overwritten;
}

bf_res_t byte_fifo_read(struct byte_fifo_t* const fifo, uint8_t* const dest, uint16_t len)
{
    RETURN_IF(NULL == fifo, BF_ERR_NULLPTR);
    RETURN_IF(NULL == fifo->data, BF_ERR_NULLPTR);
    RETURN_IF(NULL == dest, BF_ERR_NULLPTR);

    int32_t n_bytes_to_read = (int32_t)len;
    int32_t n_bytes_read = 0;
    while (n_bytes_read < n_bytes_to_read)
    {
        if (fifo->n_elements > 0)
        {
            uint16_t read_index = fifo->read_index;
            dest[n_bytes_read] = fifo->data[read_index];
            fifo->read_index = (read_index < (fifo->size - 1)) ? read_index + 1 : 0;
            fifo->n_elements--;
            n_bytes_read++;
        }
        else
        {
            break;
        }
    }

    return n_bytes_read;
}
