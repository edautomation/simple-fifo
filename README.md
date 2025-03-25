# Simple Byte FIFO  

## Overview  
The Simple Byte FIFO is a lightweight, non-thread-safe implementation of an 8-bit element first-in, first-out (FIFO) queue. It is designed for use in small programs on microcontrollers (MCUs) or Linux kernel drivers where asynchronous byte reading/writing is required, and no hardware FIFO or DMA support is available.  

## Features  
- **FIFO Buffer Management**: Provides a simple structure to manage a byte FIFO buffer.  
- **Initialization and Reset**: Functions to initialize and reset the FIFO.  
- **State Checks**: Functions to check if the FIFO is empty or full.  
- **Data Operations**:  
    - Write data to the FIFO.  
    - Overwrite data in the FIFO when full.  
    - Read data from the FIFO.  
- **Error Handling**: Returns error codes for invalid inputs or operations.  

## Limitations  
- **Not Thread-Safe**: This implementation is not thread-safe. Use appropriate locking mechanisms (e.g., mutexes or disabling interrupts) in multi-threaded or interrupt-driven environments.  

## How to run the tests
The tests are located in the `tests` directory. To run the tests, follow these steps:

```bash
cd tests
mkdir build
cd build
cmake ..
cmake --build .
```

Depending on your platform, run the executable from different places.

- Windows :`./Debug/tests.exe`
- Linux : `./tests`

## Usage Example

```c
#include "byte_fifo.h" 
#include <stdio.h>

#define BUFFER_SIZE 128

int main() 
{ 

    // Allocate a buffer for the FIFO
    unsigned char buffer[BUFFER_SIZE]; 
    
    // Create and initialize the FIFO structure
    struct byte_fifo_t fifo = { 
        .data = buffer, 
        .size = BUFFER_SIZE,
        };
    
    if (byte_fifo_init(&fifo) != 0) 
    {
        printf("Failed to initialize FIFO.\n");
        return -1;
    }
    
    // Write data to the FIFO
    unsigned char data_to_write[] = { 1, 2, 3, 4, 5 };
    int bytes_written = byte_fifo_write(&fifo, data_to_write, sizeof(data_to_write));
    printf("Bytes written to FIFO: %d\n", bytes_written);
    
    // Check if the FIFO is full
    if (byte_fifo_is_full(&fifo)) 
    {
        printf("FIFO is full.\n");
    } else 
    {
        printf("FIFO is not full.\n");
    }
    
    // Read data from the FIFO
    unsigned char data_read[5];
    int bytes_read = byte_fifo_read(&fifo, data_read, sizeof(data_read));
    printf("Bytes read from FIFO: %d\n", bytes_read);
    
    // Print the data read
    printf("Data read from FIFO: ");
    for (int i = 0; i < bytes_read; i++) 
    {
        printf("%d ", data_read[i]);
    }
    printf("\n");
    
    // Check if the FIFO is empty
    if (byte_fifo_is_empty(&fifo)) 
    {
        printf("FIFO is empty.\n");
    } else 
    {
        printf("FIFO is not empty.\n");
    }
    
    return 0;

}
```

## License  
This project is licensed under the MIT License. See the LICENSE file for details.
