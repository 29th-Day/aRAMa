#include "fs.h"
#include "../../arama/logger.h"

#include <coreinit/filesystem.h>

#include <string>
#include <memory>
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>

void FS::ReadFile(const Socket* socket)
{
    uint32_t l;
    CHECK_ERROR(socket->recv(l));
    char* p = new char[l];
    CHECK_ERROR(socket->recv(p, l));
    std::filesystem::path path(p);
    delete p;

    Logger::printf("%s | %s", __FUNCTION__, path.c_str());

    if (!std::filesystem::is_regular_file(path))
    {
        socket->send(FS_STATUS_NOT_FILE);
        return;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        socket->send(FS_STATUS_ALREADY_OPEN);
        return;
    }

    socket->send(FS_STATUS_OK);

    uint32_t size = std::filesystem::file_size(path);
    socket->send(size);

    char buffer[DATA_BUFFER_SIZE+1] = { 0 }; // ensure this is 0 terminated
    while (file)
    {
        file.read(buffer, sizeof(buffer)-1);
        std::streamsize n = file.gcount();
        socket->send(buffer, n);
    }

    file.close();
}

void FS::ReadDirectory(const Socket* socket)
{
    uint32_t l;
    CHECK_ERROR(socket->recv(l));
    char* p = new char[l];
    CHECK_ERROR(socket->recv(p, l));
    std::filesystem::path path(p);
    delete p;

    Logger::printf("%s | %s", __FUNCTION__, path.c_str());

    if (!std::filesystem::is_directory(path))
    {
        socket->send(FS_STATUS_NOT_DIR);
        return;
    }

    socket->send(FS_STATUS_OK);

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        std::string s = entry.path().string();

        if (entry.is_directory())
            s.append("/");

        Logger::printf("%s (%u)", s.c_str(), static_cast<uint32_t>(s.length()));

        socket->send(static_cast<uint32_t>(s.length()));
        socket->send(s.c_str(), static_cast<uint32_t>(s.length()));
    }
    socket->send(0);
}

void FS::WriteFile(const Socket* socket)
{
    uint32_t l;
    CHECK_ERROR(socket->recv(l));
    char* p = new char[l];
    CHECK_ERROR(socket->recv(p, l));
    std::filesystem::path path(p);
    delete p;

    Logger::printf("%s | %s", __FUNCTION__, path.c_str());

    // this is not in the original tcpGecko but makes soo much sense
    uint32_t length;
    CHECK_ERROR(socket->recv(length));

    if (!std::filesystem::is_regular_file(path))
    {
        socket->send(FS_STATUS_NOT_FILE);
        return;
    }

    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
        socket->send(FS_STATUS_ALREADY_OPEN);
        return;
    }

    socket->send(FS_STATUS_OK);

    // for some fk reason they send this??
    // shouldn't this be exactly DATA_BUFFER_SIZE?
    // why would anyone do extrawürschtl just in this function?
    // socket->send(DATA_BUFFER_SIZE);

    char buffer[DATA_BUFFER_SIZE] = { 0 };
    for (uint32_t done = 0, n = 0; done < length; done += n)
    {
        n = std::min(length - done, DATA_BUFFER_SIZE);
        CHECK_ERROR(socket->recv(buffer, n));
        file.write(buffer, n);

        if (file.fail())
        {
            Logger::printf("failed to write to: %s", path.c_str());
            break;
        }
    }

    file.close();
}
