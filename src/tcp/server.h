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
