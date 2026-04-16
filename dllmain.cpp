#include "pch.h"
#include <windows.h>
#include <stdio.h>

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"

#include "MinHook.h"

using namespace RakNet;

typedef Packet* (__thiscall* tReceive)(RakPeerInterface* _this);
tReceive oReceive = nullptr;

Packet* __thiscall hkReceive(RakPeerInterface* _this)
{
    Packet* p = oReceive(_this);

    if (p && p->data)
    {
        printf("[HOOK] Packet ID: %d | Len: %d\n", p->data[0], p->length);
    }

    return p;
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
    HMODULE hModule = (HMODULE)lpParam;

    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    printf("[DLL] Injected!\n");

    RakPeerInterface* peer = nullptr;

    while (!peer) {
        Sleep(500);
    }

    printf("[INFO] Real RakPeerInterface at: %p\n", peer);

    void** vtable = *(void***)peer;
    printf("[INFO] VTable at: %p\n", vtable);
รับ)
    void* target = vtable[9];
    printf("[INFO] Target Receive Addr: %p\n", target);

    if (MH_Initialize() != MH_OK)
    {
        printf("[HOOK] MH_Initialize failed\n");
        FreeConsole();
        FreeLibraryAndExitThread(hModule, 0);
        return 0;
    }

    if (MH_CreateHook(target, &hkReceive, (void**)&oReceive) != MH_OK)
    {
        printf("[HOOK] CreateHook failed\n");
    }
    else if (MH_EnableHook(target) != MH_OK)
    {
        printf("[HOOK] EnableHook failed\n");
    }
    else
    {
        printf("[HOOK] Installed successfully! Press 'END' to uninject.\n");
    }

    while (!(GetAsyncKeyState(VK_END) & 1))
    {
        Sleep(100);
    }

    printf("[INFO] Unhooking...\n");
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    fclose(f);
    FreeConsole();

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, MainThread, hModule, 0, 0);
    }
    return TRUE;
}