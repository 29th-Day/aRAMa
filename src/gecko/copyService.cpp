#include "copyService.h"

#include <atomic>
#include <thread>
#include <memory>

#include <coreinit/memorymap.h>

#include "../kernel/kernel_utils.h"
#include "../arama/logger.h"

static std::unique_ptr<std::thread> service = nullptr;
static std::atomic_bool running = false;

static void runCopyService()
{
    while (running)
    {
        if (OSIsAddressValid(*CopyService::desination))
        {
            kernel::memcpy(CopyService::desination, CopyService::value, sizeof(*CopyService::value));

            *CopyService::desination = 0;
            *CopyService::value = 0;
        }
    }
}

uint32_t* const CopyService::desination = reinterpret_cast<uint32_t*>(COPY_SERVICE_ADDRESS);
uint32_t* const CopyService::value = reinterpret_cast<uint32_t*>(COPY_SERVICE_ADDRESS+4);

void CopyService::start()
{
    if (service)
        return;

    running = true;
    service = std::make_unique<std::thread>(runCopyService);

    Logger::print("CopyService started");
}

void CopyService::stop()
{
    if (!service)
        return;

    running = false;
    if (service->joinable())
        service->join();
    service = nullptr;
}
