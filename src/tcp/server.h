#pragma once

#include "socket.h"

#include <unistd.h>
#include <sys/socket.h>



// typedef int Socket;
typedef void(*ClientReadyCallback)(const Socket);

namespace TCP
{
    extern const uint32_t PORT;

    /**
     * @brief Start asyncronos TCP server
     *
     * Server will wait for incoming connection requests, accept first one and run callback function. Returning from callback will close that connection and wait for new connection request.
     *
     * @attention Callback runs inside server thread - be aware of thread safety!
     *
     * @param callback Execute code once a connection has been established.
     */
    void start(ClientReadyCallback callback);

    /**
     * @brief Joins asyncronos TCP server
     *
     * Closes sockets, which exits the server loop, and joins thread
     */
    void stop();
}

// /**
//  * @brief Read `sizeof(T)` bytes from socket.
//  *
//  * Reads data from the specified socket into the provided value after waiting for the data to be available. If `false` is returned, the state of `value` is indeterminate but can safely be overwritten.
//  *
//  * @tparam T Type of data to read.
//  * @param socket The socket to read from.
//  * @param value The reference to the variable where the read data will be stored.
//  * @return `true` on success
//  */
// template <typename T>
// inline bool readwait(const Socket socket, T& value)
// {
//     ssize_t n = 0;
//     ssize_t bytes = sizeof(T);
//     uint8_t* buffer = reinterpret_cast<uint8_t*>(&value);

//     while (bytes > 0)
//     {
//         n = read(socket, buffer, bytes);
//         if (n <= 0) return false;
//         bytes -= n;
//         buffer += n;
//     }

//     return true;
// }

// /**
//  * @brief Read `length` bytes from socket.
//  *
//  * Reads data from the specified socket into the provided value after waiting for the data to be available. If `false` is returned, the state of `buffer` is indeterminate but can safely be overwritten.
//  *
//  * @param socket The socket to read from.
//  * @param buffer Buffer to write into.
//  * @param bytes Length of buffer.
//  * @return `true` on success
//  */
// inline bool readwait(const Socket socket, void* buffer, ssize_t length)
// {
//     ssize_t n = 0;
//     while (length > 0)
//     {
//         n = read(socket, buffer, length);
//         if (n <= 0) return false;
//         length -= n;
//         buffer = reinterpret_cast<uint8_t*>(buffer) + n;
//     }

//     return true;
// }
