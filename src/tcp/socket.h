#pragma once

#include <cstdint>
#include <unistd.h>

#include "../arama/logger.h"

class Socket
{
    const int fd_;

public:
    Socket(int fd) : fd_(fd) { };

    template <typename T>
    inline bool send(const T value) const
    {
        return write(fd_, &value, sizeof(value)) <= 0 ? false : true;
    }

    inline bool send(const void* buffer, uint32_t length) const
    {
        uint32_t n = 0;
        void* buf = const_cast<void*>(buffer);
        while (length > 0)
        {
            n = write(fd_, buf, length);
            if (n <= 0) return false;
            length -= n;
            buf = reinterpret_cast<uint8_t*>(buf) + n;
        }
        return true;
    }

    /**
     * @brief Read `sizeof(T)` bytes from socket.
     *
     * Reads data from the specified socket into the provided value after waiting for the data to be available. If `false` is returned, the state of `value` is indeterminate but can safely be overwritten.
     *
     * @tparam T Type of data to read.
     * @param socket The socket to read from.
     * @param value The reference to the variable where the read data will be stored.
     * @return `true` on success
     */
    template <typename T>
    inline bool recv(T& value) const
    {
        uint32_t n = 0;
        uint32_t bytes = sizeof(T);
        uint8_t* buffer = reinterpret_cast<uint8_t*>(&value);

        while (bytes > 0)
        {
            n = read(fd_, buffer, bytes);
            Logger::printf("recv - n: %u", n);
            if (n <= 0) return false;
            bytes -= n;
            buffer += n;
        }

        return true;
    }

    /**
     * @brief Read `length` bytes from socket.
     *
     * Reads data from the specified socket into the provided value after waiting for the data to be available. If `false` is returned, the state of `buffer` is indeterminate but can safely be overwritten.
     *
     * @param socket The socket to read from.
     * @param buffer Buffer to write into.
     * @param bytes Length of buffer.
     * @return `true` on success
     */
    inline bool recv(void* buffer, uint32_t length) const
    {
        uint32_t n = 0;
        while (length > 0)
        {
            n = read(fd_, buffer, length);
            if (n <= 0) return false;
            length -= n;
            buffer = reinterpret_cast<uint8_t*>(buffer) + n;
        }

        return true;
    }
};
