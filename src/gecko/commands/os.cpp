#include "os.h"

#include "../../arama/logger.h"

#include <coreinit/mcp.h>
#include <coreinit/thread.h>
#include <coreinit/interrupts.h>
#include <coreinit/scheduler.h>

#include <vector>

void OS::Version(const Socket* socket)
{
    int handle = MCP_Open();
    MCPSystemVersion version;
    MCP_GetSystemVersion(handle, &version);
    socket->send(version);
    MCP_Close(handle);
}

void OS::GetThreads(const Socket* socket)
{
    uint32_t count = OSCheckActiveThreads();

    Logger::printf("%s | count: %u", __FUNCTION__, count);

    if (count <= 0)
    {
        socket->send(0);
        return;
    }

    std::vector<OSThread*> threads;
    threads.reserve(count);

    OSThread* currentThread = OSGetCurrentThread();
    OSThread* thread = OSGetDefaultThread(1);

    /*
     * Here as little as possible should happen since we lock the OS scheduler
     */
    bool state = OSDisableInterrupts();
    __OSLockScheduler(currentThread);

    for (uint32_t i = 0; thread && i < count; i++)
    {
        threads.push_back(thread);
        thread = thread->activeLink.next;
    }

    __OSUnlockScheduler(currentThread);
    OSRestoreInterrupts(state);
    /*
     * Now everything is fine again
     */

    socket->send(threads.size());

    for (const auto thread : threads)
    {
        CHECK_ERROR(socket->send(thread));
        CHECK_ERROR(socket->send(*thread));
        // sending it too fast leads to dropped packages
        usleep(15'000);
    }
}
