#include "os.h"

#include "../../arama/logger.h"

#include <coreinit/mcp.h>
#include <coreinit/thread.h>
#include <coreinit/interrupts.h>
#include <coreinit/scheduler.h>
#include <nn/act.h>

#include <coreinit/savedframe.h>
#include <gx2/texture.h>
#include <gx2/swap.h>

#include <vector>

void OS::TakeScreenshot(const Socket* socket)
{
    GX2Texture texture = {};

    uint32_t tmp[854] = { 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1 };

    CHECK_ERROR(socket->send(sizeof(tmp)));

    bool success = GX2GetLastFrame(GX2_SCAN_TARGET_DRC, &texture);

    Logger::printf("GX2GetLastFrame = %s", btos(success));

    // uint32_t drcBuffer[854*480] = {};

    // sizeof(drcBuffer) == 172'0320

    Logger::printf("width: %u, height: %u, pitch: %u, size: %u, use: %u, image: 0x%08x",
        texture.surface.width,
        texture.surface.height,
        texture.surface.pitch,
        texture.surface.imageSize,
        texture.surface.use,
        texture.surface.image);

    uint32_t* data = reinterpret_cast<uint32_t*>(texture.surface.image);

    // int end = texture.surface.imageSize;
    int end = 256;

    for (int i = 0; i < end; i += 4)
    {
        Logger::printf("%i: %08x %08x %08x %08x",
            i, (data+i)[0], (data+i)[1], (data+i)[2], (data+i)[3]);
    }



    CHECK_ERROR(socket->send(tmp, sizeof(tmp)));
}

void OS::Version(const Socket* socket)
{
    int handle = MCP_Open();
    MCPSystemVersion version;
    MCP_GetSystemVersion(handle, &version);
    socket->send(version);
    MCP_Close(handle);
}

void OS::AccountIdentifier(const Socket* socket)
{
    nn::act::Initialize();
    uint32_t id = nn::act::GetPersistentId();
    nn::act::Finalize();
    socket->send(id);
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
