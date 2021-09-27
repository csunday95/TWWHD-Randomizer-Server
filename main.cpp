
#include "utility/platform.hpp"
#include "utility/platform_socket.hpp"

#include <thread>
#include <chrono>
#include <cstring>
#include <fstream>

#include "ProtocolServer.hpp"
#include "command/RandoSession.hpp"

#define SERVER_TCP_PORT 1234

int waitAndCleanup(const char* msg)
{
    Utility::platformLog("%s\n", msg);
    Utility::waitForPlatformStop();
    Utility::platformShutdown();
    Utility::netShutdown();
    return 0;
}

int
main(int argc, char** argv)
{
    Utility::netInit();
    if(!Utility::platformInit())
    {
        Utility::waitForPlatformStop();
        Utility::netShutdown();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return 0;
    }

    RandoSession rs{
        R"~(E:\CEMU\GAMES\The_Wind_Waker_HD)~",
        R"~(C:\workspace\temp\workdir)~",
        R"~(C:\workspace\temp\outdir)~"
    };
    RandoSession::fspath relPath;
    // workflow for edits:
    // (1) make backups of szs files from game dir into workdir
    // (2) extract constituent files in a deterministic manner
    // (3) modify those files and write modified version to outdir
    // (4) repack files into outdir
    // (5) copy files into gamedir
    auto modelsharcfb = rs.openGameFile({R"~(content\Common\Stage\M_Dai_Room20.szs)~", "YAZ0", "SARC", "model.sharcfb"}, relPath);
    char out[64];
    modelsharcfb.read(out, 64);
    auto room20 = rs.openGameFile({ R"~(content\Common\Stage\M_Dai_Room20.szs)~", "YAZ0", "SARC", "Room20.bfres" }, relPath);

    ProtocolServer server(SERVER_TCP_PORT);

    if (!server.initialize())
    {
        return waitAndCleanup("server.initialize() failed\n");
    }
    else
    {
        server.start();
    }   

    // wait until termination signal is received. In the case of wii u this is
    // the home button, otherwise, ctrl-c
    Utility::waitForPlatformStop();

    // if server never started, this does nothing
    server.stop();
    Utility::platformLog("server successfully stopped\n");

    Utility::platformShutdown();
    Utility::netShutdown();
    return 0;
}
