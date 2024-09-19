#include "server.h"
#include "../arama/logger.h"

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <memory>
#include <atomic>

#define close_fd(fd) {if (fd > 0) { close(fd); fd = -1; }}

static int server = -1;
static int client = -1;
static std::atomic_bool running = false;

static std::unique_ptr<std::thread> tcpThread = nullptr;

const uint32_t TCP::PORT = 7331;

void run_server(ClientReadyCallback callback)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = TCP::PORT;

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bind(server, (sockaddr*)&addr, sizeof(addr));
    listen(server, 5);

    running = true;

    Logger::print("---- Opening TCP ----");

    while (running)
    {
        client = accept(server, nullptr, nullptr);
        if (client < 0) break;

        Logger::print("---  new client  ---");

        callback(Socket(client));

        Logger::print("---  del client  ---");

        close_fd(client);
    }

    Logger::print("---- Closing TCP ----");

    close_fd(client);
    close_fd(server);
}

void TCP::start(ClientReadyCallback callback)
{
    if (tcpThread)
    {
        Logger::print("TCP server already started");
        return;
    }

    tcpThread = std::make_unique<std::thread>(run_server, callback);

    Logger::print("TCP server started");
}

void TCP::stop()
{
    if (!tcpThread)
    {
        Logger::print("TCP server not started");
        return;
    }

    Logger::printf("stopping tcp server");

    running = false;

    if (client > 0)
        shutdown(client, SHUT_RDWR);
    if (server > 0)
        shutdown(server, SHUT_RDWR);

    tcpThread->join();
    tcpThread = nullptr;

    Logger::print("TCP server stopped");
}
