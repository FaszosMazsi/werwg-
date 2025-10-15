#include "includes.h"
#include "./futures/Esp.h"
#include "./futures/Misc.h"
#include "evo_inc.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = nullptr;
WNDPROC oWndProc = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* mainRenderTargetView = nullptr;
bool init = false;

// hide the menu
int g_toggleKey = VK_INSERT;
bool g_ShowMenu = true;

Esp esp;
//Misc misc;

void InitImGui()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(pDevice, pContext);
    
    // Initialize evo menu system
    evo::_render->initialize_imgui(window, pDevice);
    evo::_container->initialize_container();
}

void Cleanup()
{
    if (!init) return;

    // Unhook Present
    Lolz::unbind(8);

    // Restore WndProc
    if (window && oWndProc)
        SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);

    // Cleanup ImGui
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // Release DX resources
    if (mainRenderTargetView)
    {
        mainRenderTargetView->Release();
        mainRenderTargetView = nullptr;
    }

    pContext = nullptr;
    pDevice = nullptr;
    window = nullptr;
    oWndProc = nullptr;
    init = false;
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Update mouse position for evo input system
    if (uMsg == WM_MOUSEMOVE)
        evo::_input->set_mouse_position(evo::vec2_t(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
    
    if (uMsg == WM_MOUSEWHEEL)
        evo::_input->set_mouse_wheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
    
    if (init && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
        {
            pDevice->GetImmediateContext(&pContext);

            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
            pBackBuffer->Release();

            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            InitImGui();
            init = true;
        }
        else
            return oPresent(pSwapChain, SyncInterval, Flags);
    }

    // Toggle menu by insert key
    if (GetAsyncKeyState(g_toggleKey) & 1)
    {
        g_ShowMenu = !g_ShowMenu;
    }

    // Update input for evo menu system
    evo::_input->init_input();
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Initialize container
    evo::_container->initialize_container();
    
    // Initialize the new evo menu
    evo::_menu->init();
    
    // Begin external drawing
    evo::externals::_ext_b->begin();
    
    // Popup drawing
    evo::externals::_ext_b_p->begin_popup();

    // Still draw ESP
    esp.drawESP();
    
    ImGui::Render();
    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI UnloadThread(LPVOID hModule)
{
    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            Cleanup(); // Minden erőforrás felszabadítása
            FreeLibraryAndExitThread((HMODULE)hModule, 0);
            // break; // Nem kell, mert ExitThread úgyis kilép
        }
        Sleep(30);
    }
    return 0;
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
    //AllocConsole();
    //FILE* f;
    //freopen_s(&f, "CONOUT$", "w", stdout);

    //HWND consoleWnd = GetConsoleWindow();
    //if (consoleWnd)
    //    SetWindowPos(consoleWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    //std::cout << "[+] Console initialized :)" << std::endl;

    bool init_hook = false;
    do
    {
        if (Lolz::init(Lolz::RenderType::D3D11) == Lolz::Status::Success)
        {
            Lolz::bind(8, (void**)&oPresent, hkPresent);
            init_hook = true;
        }

        CreateThread(nullptr, 0, UnloadThread, lpReserved, 0, nullptr);

    } while (!init_hook);
    return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        Cleanup(); // Safe unload
        break;
    }
    return TRUE;
}
