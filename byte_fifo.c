#include "byte_fifo.h"

#ifdef __KERNEL__
#include <linux/errno.h>   // EFAULT, EINVAL
#include <linux/string.h>  // memset
#else
#include <errno.h>   // EFAULT, EINVAL
#include <string.h>  // memset
#endif

#define RETURN_IF(x, y) \
    if ((x)) return (y)

int byte_fifo_init(struct byte_fifo_t* const fifo)
{
    RETURN_IF(NULL == fifo, -EFAULT);
    RETURN_IF(NULL == fifo->data, -EFAULT);
    RETURN_IF(0 == fifo->size, -EINVAL);

    fifo->write_index = 0U;
    fifo->read_index = 0U;
    fifo->n_elements = 0U;

    memset((void*)fifo->data, 0, fifo->size);

    return 0;
}

int byte_fifo_reset(struct byte_fifo_t* const fifo)
{
    return byte_fifo_init(fifo);
}

int byte_fifo_is_empty(const struct byte_fifo_t* const fifo)
{
    RETURN_IF(NULL == fifo, -EFAULT);
    return (fifo->n_elements == 0);
}

int byte_fifo_is_full(const struct byte_fifo_t* const fifo)
{
    RETURN_IF(NULL == fifo, -EFAULT);
    return (fifo->n_elements == fifo->size);
}

int byte_fifo_write(struct byte_fifo_t* const fifo, const unsigned char* const src, unsigned int len)
{
    RETURN_IF(NULL == fifo, -EFAULT);
    RETURN_IF(NULL == fifo->data, -EFAULT);
    RETURN_IF(NULL == src, -EFAULT);

    int n_bytes_written = 0;
    while (len > 0)
    {
        if (fifo->n_elements < fifo->size)
        {
            unsigned int write_index = fifo->write_index;
            fifo->data[write_index] = src[n_bytes_written];
            fifo->write_index = (write_index < (fifo->size - 1)) ? write_index + 1 : 0;
            fifo->n_elements++;

            // Update
            n_bytes_written++;
            len--;
        }
        else
        {
            // No more available bytes
            break;
        }
    }

    return n_bytes_written;
}

int byte_fifo_overwrite(struct byte_fifo_t* const fifo, const unsigned char* const src, unsigned int len)
{
    RETURN_IF(NULL == fifo, -EFAULT);
    RETURN_IF(NULL == fifo->data, -EFAULT);
    RETURN_IF(NULL == src, -EFAULT);

    int n_bytes_written = 0;
    int n_bytes_overwritten = 0;
    while (len > 0)
    {
        unsigned int write_index = fifo->write_index;
        fifo->data[write_index] = src[n_bytes_written + n_bytes_overwritten];
        fifo->write_index = (write_index < (fifo->size - 1)) ? write_index + 1 : 0;

        if (fifo->n_elements < fifo->size)
        {
            n_bytes_written++;
            fifo->n_elements++;
        }
        else
        {
            unsigned int read_index = fifo->read_index;
            fifo->read_index = (read_index < (fifo->size - 1)) ? read_index + 1 : 0;
            n_bytes_overwritten++;
        }
        len--;
    }

    return n_bytes_overwritten;
}

int byte_fifo_read(struct byte_fifo_t* const fifo, unsigned char* const dest, int len)
{
    RETURN_IF(NULL == fifo, -EFAULT);
    RETURN_IF(NULL == fifo->data, -EFAULT);
    RETURN_IF(NULL == dest, -EFAULT);
    RETURN_IF(len < 0, -EINVAL);

    int n_bytes_read = 0;
    while (n_bytes_read < len)
    {
        if (fifo->n_elements > 0)
        {
            unsigned int read_index = fifo->read_index;
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
