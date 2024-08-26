#include "info.h"

#include "../../arama/logger.h"

#include "../version.h"
#include "../codeHandler.h"

void Info::Status(const Socket socket)
{
    uint8_t status = 1;
    send(socket, &status, sizeof(status), 0);
}

void Info::BufferSize(const Socket socket)
{
    uint32_t size = DATA_BUFFER_SIZE;
    send(socket, &size, sizeof(size), 0);
}

void Info::CodeHandlerAddress(const Socket socket)
{
    uint32_t address = CODE_HANDLER_INSTALL_ADDRESS;
    Logger::printf("code handler address: 0x%08x", address);
    send(socket, &address, sizeof(address), 0);
}

void Info::VersionHash(const Socket socket)
{
    uint32_t hash = GECKO_VERSION_HASH;
    Logger::printf("version hash: 0x%08x", hash);
    send(socket, &hash, sizeof(hash), 0);
}
