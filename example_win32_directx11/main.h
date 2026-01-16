#pragma once

//Libs
#include <fstream>

#include <cstring>  

//ImGui Includes
#include "imgui.h"
#include "imgui_edited.hpp"
#include "imgui_freetype.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

//System Includes
#include <algorithm>
#include <chrono>
#include <dwmapi.h>
#include <filesystem>
#include <future>
#include <iostream>
#include <Lmcons.h>
#include <ShObjIdl_core.h>
#include <string>
#include <strsafe.h>
#include <tchar.h>
#include <thread>
#include <TlHelp32.h>
#include <vector>
#include <winhttp.h>
#include <wininet.h>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <shlobj.h>
#include <codecvt>
#include <locale>

//DirectX Includes
#include <d3d11.h>
#include <D3DX11tex.h>

//External Includes
#include "font.h"
#include "texture.h"
#include "DiscordSDK/src/discord_register.h"
#include "DiscordSDK/src/discord_rpc.h"



//Libs
#pragma comment(lib, "D3DX11.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "ntdll.lib")

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

HWND hwnd;
RECT rc;

static char username[64] = "";
static char password[64] = "";

static bool stream;
bool setup_done = false;
float rgbSpeed2 = 1.0f;

int rotation_start_index;

std::vector<ImFont*> g_FontFallbackChain;

ImVec4 GetBlinkingColor(const ImVec4& defaultColor, const ImVec4& blinkColor, float blinkInterval = 0.5f) {
    static float lastSwitchTime = 0.0f;
    static bool toggle = false;

    float currentTime = ImGui::GetTime();
    if (currentTime - lastSwitchTime >= blinkInterval) {
        toggle = !toggle;
        lastSwitchTime = currentTime;
    }

    return toggle ? blinkColor : defaultColor;
}

std::string GetCredentialsFilePath()
{
    char appDataPath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath);

    std::string folder = std::string(appDataPath) + "\\Bruuuh";
    CreateDirectoryA(folder.c_str(), NULL);

    return folder + "\\auth.txt";
}

void SaveCredentials(const char* user, const char* pass)
{
    std::ofstream file(GetCredentialsFilePath(), std::ios::trunc);
    if (file.is_open()) {
        file << user << '\n' << pass;
        file.close();
    }
}
void move_window() {
    ImGui::SetCursorPos(ImVec2(0, 0));
    if (ImGui::InvisibleButton("Move_detector", ImVec2(c::background::size.x, c::background::size.y)));
    if (ImGui::IsItemActive()) {

        GetWindowRect(hwnd, &rc);
        MoveWindow(hwnd, rc.left + ImGui::GetMouseDragDelta().x, rc.top + ImGui::GetMouseDragDelta().y, c::background::size.x, c::background::size.y, TRUE);
    }
}

void move_window2() {
    ImGui::SetCursorPos(ImVec2(0, 0));
    if (ImGui::InvisibleButton("Move_detector", ImVec2(c::background::size2.x, c::background::size2.y)));
    if (ImGui::IsItemActive()) {

        GetWindowRect(hwnd, &rc);
        MoveWindow(hwnd, rc.left + ImGui::GetMouseDragDelta().x, rc.top + ImGui::GetMouseDragDelta().y, c::background::size2.x, c::background::size2.y, TRUE);
    }
}

void LoadCredentials()
{
    std::ifstream file(GetCredentialsFilePath());
    if (file.is_open()) {
        std::string user, pass;
        std::getline(file, user);
        std::getline(file, pass);
        file.close();


        if (!user.empty() && !pass.empty()) {
            strncpy(username, user.c_str(), sizeof(username) - 1);
            username[sizeof(username) - 1] = '\0';

            strncpy(password, pass.c_str(), sizeof(password) - 1);
            password[sizeof(password) - 1] = '\0';
        }
    }
}

void ImRotateStart()
{
    rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

ImVec2 ImRotationCenter()
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
}


void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
{
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

namespace notifications
{
    const char* notif_name;
    const char* notif_description;
    static int notif_state;
    static float notif_offset;
    static float notif_timer;
    static float notif_width;

    void Message(const char* name, const char* description)
    {
        notif_name = name;
        notif_description = description;
        notif_state = 1;
        notif_offset = 100.f;
        notif_timer = 0;
        notif_width = 0;
    }

    void NotifyUpdate(ImVec2 p)
    {
        if (ImGui::CalcTextSize(notif_name).x < 1.f)
            return;

        notif_offset = ImLerp(notif_offset, notif_state == 0 ? 200.f : 0.f, ImGui::GetIO().DeltaTime * 10.f);
        notif_timer += ImGui::GetIO().DeltaTime * 20.f;

        if (notif_timer > 70.f)
            notif_state = 0;

        if (notif_timer > 100.f)
            notif_name = "";

        ImVec2 notif_size = ImVec2(ImGui::CalcTextSize(notif_description).x + 30, ImGui::CalcTextSize(notif_name).y + 50 + notif_offset);

        ImVec2 notif_pos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - notif_size.x / 2, ImGui::GetIO().DisplaySize.y - notif_size.y);

        ImGui::SetNextWindowPos(notif_pos);
        ImGui::SetNextWindowBgAlpha(0.8f);
        ImGui::Begin("Notification", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::SetWindowSize(notif_size);

        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(notif_pos.x, notif_pos.y + notif_offset), ImVec2(notif_pos.x + notif_size.x, notif_pos.y), IM_COL32(14, 14, 14, 200));



        ImGui::GetWindowDrawList()->AddText(ImVec2(notif_pos.x, notif_pos.y + notif_offset) + ImVec2(10, 10), text_color[0], notif_name);


        ImGui::GetWindowDrawList()->AddText(ImVec2(notif_pos.x, notif_pos.y + notif_offset) + ImVec2(10, 30), text_color[1], notif_description);


        ImGui::GetWindowDrawList()->AddRect(ImVec2(notif_pos.x, notif_pos.y + notif_offset), ImVec2(notif_pos.x + notif_size.x, notif_pos.y), ImGui::GetColorU32(c::accent), 5.0f);

        ImGui::End();
    }
}

namespace ImGui
{
    int rotation_start_index;
    void ImRotateStart()
    {
        rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
    }

    ImVec2 ImRotationCenter()
    {
        ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

        const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
        for (int i = rotation_start_index; i < buf.Size; i++)
            l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

        return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
    }


    void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
    {
        float s = sin(rad), c = cos(rad);
        center = ImRotate(center, s, c) - center;

        auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
        for (int i = rotation_start_index; i < buf.Size; i++)
            buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
    }
}

void ParticlesV()
{
    ImVec2 screen_size = { (float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN) };
    static ImVec2 partile_pos[150];
    static float partile_speed[150];
    static float partile_size[150];
    static float partile_radius[150];
    static float partile_rotate[150];
    static float partile_alpha[150];
    static float partile_fade_speed[150];
    static bool partile_fading_in[150];
    static float partile_phase[150];
    static ImVec2 partile_base_pos[150];  // الموقع الأساسي الثابت
    static float partile_amplitude_y[150];  // مدى الحركة العمودية فقط

    for (int i = 1; i < 130; i++)  // زدنا من 100 لـ 130
    {
        if (partile_pos[i].x == 0 || partile_pos[i].y == 0)
        {
            // موقع ثابت عشوائي في كامل الواجهة
            partile_base_pos[i].x = rand() % (int)screen_size.x;
            partile_base_pos[i].y = rand() % (int)screen_size.y;
            partile_pos[i] = partile_base_pos[i];

            partile_speed[i] = 0.4f + (rand() % 8) / 10.0f;  // نقصنا السرعة من 0.8 لـ 0.4
            partile_size[i] = 0.8f + (rand() % 3) / 2.0f;
            partile_alpha[i] = 0.0f;
            partile_fade_speed[i] = 0.3f + (rand() % 20) / 10.0f;
            partile_fading_in[i] = true;
            partile_phase[i] = (rand() % 628) / 100.0f;
            partile_amplitude_y[i] = 20.0f + (rand() % 30);  // مسافة الطلوع والنزول
        }

        // حركة موجية عمودية فقط (طلوع ونزول)
        partile_phase[i] += ImGui::GetIO().DeltaTime * partile_speed[i];

        // الموقع الجديد - حركة عمودية فقط
        partile_pos[i].x = partile_base_pos[i].x;  // الموقع الأفقي ثابت
        partile_pos[i].y = partile_base_pos[i].y + sinf(partile_phase[i]) * partile_amplitude_y[i];

        // تأثير الظهور والاختفاء
        if (partile_fading_in[i])
        {
            partile_alpha[i] += ImGui::GetIO().DeltaTime * partile_fade_speed[i];
            if (partile_alpha[i] >= 1.0f)
            {
                partile_alpha[i] = 1.0f;
                partile_fading_in[i] = false;
            }
        }
        else
        {
            partile_alpha[i] -= ImGui::GetIO().DeltaTime * partile_fade_speed[i];
            if (partile_alpha[i] <= 0.0f)
            {
                partile_alpha[i] = 0.0f;
                partile_pos[i].x = 0;
                partile_pos[i].y = 0;
            }
        }

        partile_rotate[i] += ImGui::GetIO().DeltaTime * 0.3f;

        // رسم الجزيء مع الشفافية
        ImVec4 color = c::accent;
        color.w = partile_alpha[i];
        ImGui::GetWindowDrawList()->AddCircleFilled(
            partile_pos[i],
            partile_size[i] + 0.7f,
            ImGui::GetColorU32(color),
            20
        );

        // التوهج مع الشفافية
        color.w = partile_alpha[i] * 0.5f;
        ImGui::GetWindowDrawList()->AddShadowCircle(
            partile_pos[i],
            5.f,
            ImGui::GetColorU32(color),
            32.f + partile_size[i],
            ImVec2(0, 0),
            0,
            20
        );
    }
}

void ParticlesX()
{
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 win_size = ImGui::GetWindowSize();
    ImVec2 mouse = ImGui::GetIO().MousePos;
    float time = ImGui::GetTime();
    const float spacing_x = 30.0f;
    const float spacing_y = 15.0f;
    const float max_light_distance = 220.0f;
    const float speed = 35.0f;
    int count_x = (int)(win_size.x / spacing_x) + 3;
    int count_y = (int)(win_size.y / spacing_y) + 3;
    ImGui::PushFont(font::lexend_bold);
    for (int ix = 0; ix < count_x; ++ix)
    {
        float x = win_pos.x + ix * spacing_x;
        for (int iy = 0; iy < count_y; ++iy)
        {
            int direction = ((ix + iy) % 2 == 0) ? 1 : -1;
            float offset = (ix + iy * count_x) * 0.2f;
            float path_length = win_size.y + spacing_y;
            float pos_in_path = fmodf(time * speed + offset * speed, path_length);
            float y = (direction == 1)
                ? win_pos.y + pos_in_path
                : win_pos.y + path_length - pos_in_path;
            float dx = mouse.x - x;
            float dy = mouse.y - y;
            float dist = sqrtf(dx * dx + dy * dy);
            float intensity = 1.0f - (dist / max_light_distance);
            if (intensity < 0.0f) intensity = 0.0f;
            float glow = powf(intensity, 3.0f);
            char num = '0' + ((ix * 31 + iy * 17) % 10);
            ImVec4 col = c::accent;
            col.w = 0.04f + glow * 0.6f;  // نقصت الوضوح: من 0.08 + 0.92 لـ 0.04 + 0.6
            draw->AddText(
                ImVec2(x, y),
                ImGui::GetColorU32(col),
                &num,
                &num + 1
            );
        }
    }
    ImGui::PopFont();
}
void ParticlesY()
{
    constexpr int cols = 20;
    constexpr int rows = 40;
    static ImVec2 base_pos[rows][cols];
    static float phase[rows][cols];
    static bool initialized = false;

    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 win_size = ImGui::GetWindowSize();
    ImVec2 spacing = ImVec2(win_size.x / (float)(cols + 1), win_size.y / (float)(rows + 1));
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mouse = io.MousePos;

    if (!initialized)
    {
        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < cols; ++x)
            {
                base_pos[y][x] = ImVec2(
                    win_pos.x + spacing.x * (x + 1),
                    win_pos.y + spacing.y * (y + 1)
                );
                phase[y][x] = (rand() % 628) / 100.f;
            }
        }
        initialized = true;
    }

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            phase[y][x] += io.DeltaTime * 2.0f;
            if (phase[y][x] > IM_PI * 2.0f) phase[y][x] -= IM_PI * 2.0f;

            ImVec2 center = base_pos[y][x];
            float dx = center.x - mouse.x;
            float dy = center.y - mouse.y;
            float dist = sqrtf(dx * dx + dy * dy);
            float wave = sinf(phase[y][x] + dist * 0.03f) * 4.0f;
            float alpha = ImClamp(1.0f - dist / 250.f, 0.f, 1.f);

            ImVec2 final_pos = ImVec2(center.x, center.y + wave);

            // استخدام لون accent
            ImVec4 col = c::accent;
            col.w = 0.04f + alpha * 0.6f;

            draw->AddCircleFilled(
                final_pos,
                1.2f + alpha * 0.8f,
                ImGui::GetColorU32(col),
                8
            );
        }
    }
}






void AnimatedCaption(const std::vector<std::string>& captions, ImVec2 center_pos)
{
    auto draw = ImGui::GetWindowDrawList();

    static int current_index = 0;
    static float timer = 0.0f;
    static float display_duration = 2.5f;
    static float randomize_duration = 1.5f;

    static std::string display_text;
    static std::vector<char> char_states;

    static bool resolving = false;
    static bool waiting = false;

    ImGuiIO& io = ImGui::GetIO();
    timer += io.DeltaTime;

    std::string& target = const_cast<std::string&>(captions[current_index]);

    if (display_text.empty() || target.length() != display_text.length())
    {
        display_text = std::string(target.length(), ' ');
        char_states = std::vector<char>(target.length(), 0);
    }

    float t = timer / (resolving ? display_duration : randomize_duration);
    t = ImClamp(t, 0.0f, 1.0f);
    float curve_speed = resolving ? (1.0f - t) : t;
    curve_speed = powf(curve_speed, 2.0f);
    float rand_chance = 1.0f - curve_speed * 0.9f;

    if (!waiting)
    {
        for (size_t i = 0; i < target.length(); ++i)
        {
            if (target[i] == ' ')
            {
                display_text[i] = ' ';
                continue;
            }

            if (!resolving)
            {
                if (ImGui::GetIO().Framerate > 0.0f && ((rand() % 100) / 100.0f < curve_speed))
                    display_text[i] = (char)('A' + (rand() % 26));
            }
            else if (char_states[i] == 0)
            {
                if ((rand() / (float)RAND_MAX) < rand_chance)
                    display_text[i] = (char)('A' + (rand() % 26));

                if ((rand() % 100) < 10)
                {
                    display_text[i] = target[i];
                    char_states[i] = 1;
                }
            }
        }
    }

    if (!resolving && timer >= randomize_duration)
    {
        resolving = true;
        timer = 0.0f;
    }
    else if (resolving && timer >= display_duration)
    {
        waiting = true;
        timer = 0.0f;
    }
    else if (waiting && timer >= 1.0f)
    {
        current_index = (current_index + 1) % captions.size();
        display_text.clear();
        char_states.clear();
        resolving = false;
        waiting = false;
        timer = 0.0f;
    }


    ImGui::PushFont(font::lexend_regular);

    ImVec2 text_size = ImGui::CalcTextSize(display_text.c_str());
    ImVec2 pos = center_pos - ImVec2(text_size.x / 2.0f, 0.0f);

    draw->AddText(pos, ImGui::GetColorU32(c::white, 0.4f), display_text.c_str());
    ImGui::PopFont();

    ImGui::Dummy(ImVec2(0, text_size.y + 10));
}

std::string FormatExpiryDate(const std::string& timestampStr) {

    if (timestampStr.empty() || !std::all_of(timestampStr.begin(), timestampStr.end(), ::isdigit)) {
        return "Invalid timestamp";
    }

    std::time_t expiryTime = std::stoll(timestampStr);
    std::tm* tm = std::localtime(&expiryTime);

    if (!tm) {
        return "Invalid time";
    }

    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", tm);
    return std::string(buffer);
}


void RenderBlur(HWND hwnd)
{
    struct ACCENTPOLICY
    {
        int na;
        int nf;
        int nc;
        int nA;
    };
    struct WINCOMPATTRDATA
    {
        int na;
        PVOID pd;
        ULONG ul;
    };

    const HINSTANCE hm = LoadLibrary("user32.dll");
    if (hm)
    {
        typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);

        const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hm, "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute)
        {
            ACCENTPOLICY policy = { 3, 0, 0, 0 };

            WINCOMPATTRDATA data = { 19, &policy,sizeof(ACCENTPOLICY) };
            SetWindowCompositionAttribute(hwnd, &data);
        }
        FreeLibrary(hm);
    }
}

void ToggleClickability(bool clickable) {
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (clickable) {
        exStyle &= ~WS_EX_TRANSPARENT;
    }
    else {
        exStyle |= WS_EX_TRANSPARENT;
    }
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ToggleClickability(bool clickable);

std::string GetUsername() {
    TCHAR usernamedx[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserName(usernamedx, &username_len)) {
#ifdef UNICODE
        std::wstring wstr(usernamedx);
        return std::string(wstr.begin(), wstr.end());
#else
        return std::string(usernamedx);
#endif
    }
    else {
        return "Unknown";
    }
}

void RunCommandHidden(const std::string& command)
{
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    std::string fullCmd = "cmd.exe /C " + command;

    if (CreateProcessA(nullptr, &fullCmd[0], nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

std::future<void> RunCommandAsync(const std::string& command) {
    return std::async(std::launch::async, [command]() {
        RunCommandHidden(command);
        });
}

std::chrono::milliseconds ToMilliseconds(float fakelag2)
{
    return std::chrono::milliseconds(static_cast<int>(std::round(fakelag2 * 1000.0f)));
}

void PauseNetwork()
{
    std::vector<std::string> paths = {
        R"(C:\Program Files\BlueStacks_nxt\HD-Player.exe)",
        R"(C:\ProgramData\BlueStacks_msi5\HD-Player.exe)",
        R"(C:\Program Files\BlueStacks\HD-Player.exe)",
        R"(C:\Program Files\BlueStacks_msi2\Bluestacks.exe)",
        R"(C:\Program Files\BlueStacks_msi2\HD-Player.exe)"
    };

    std::vector<std::future<void>> tasks;
    int i = 0;
    for (const auto& path : paths)
    {
        std::string name = "TempBlock" + std::to_string(++i);
        tasks.push_back(RunCommandAsync("netsh advfirewall firewall add rule name=\"" + name + "\" dir=in action=block profile=any program=\"" + path + "\""));
        tasks.push_back(RunCommandAsync("netsh advfirewall firewall add rule name=\"" + name + "\" dir=out action=block profile=any program=\"" + path + "\""));
    }

    for (auto& t : tasks)
        t.wait();
}

void ResumeNetwork()
{
    std::vector<std::string> paths = {
        R"(C:\Program Files\BlueStacks_nxt\HD-Player.exe)",
        R"(C:\ProgramData\BlueStacks_msi5\HD-Player.exe)",
        R"(C:\Program Files\BlueStacks\HD-Player.exe)",
        R"(C:\Program Files\BlueStacks_msi2\HD-Player.exe)"
    };

    std::vector<std::future<void>> tasks;
    for (const auto& path : paths)
    {
        tasks.push_back(RunCommandAsync("netsh advfirewall firewall delete rule name=all program=\"" + path + "\""));
    }

    for (auto& t : tasks)
        t.wait();
}

void DrawCircle(ImVec2 center, float radius, float thickness, int segments = 64) {
    ImDrawList* vList = ImGui::GetForegroundDrawList();
    float step = 1.0f / segments;
    float time = ImGui::GetTime();

    for (int i = 0; i < segments; ++i) {
        float t1 = i * step;
        float t2 = (i + 1) * step;

        float hue1 = fmod(t1 + time * rgbSpeed2, 1.0f);
        float hue2 = fmod(t2 + time * rgbSpeed2, 1.0f);

        ImU32 color1 = ImColor::HSV(hue1, 1.0f, 1.0f);
        ImU32 color2 = ImColor::HSV(hue2, 1.0f, 1.0f);

        float angle1 = t1 * 2.0f * IM_PI;
        float angle2 = t2 * 2.0f * IM_PI;

        ImVec2 p1(center.x + cos(angle1) * radius, center.y + sin(angle1) * radius);
        ImVec2 p2(center.x + cos(angle2) * radius, center.y + sin(angle2) * radius);

        vList->AddLine(p1, p2, color1, thickness);
    }
}


double getCurrentTime() {
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count() / 1000.0;
}

void FallBackESP(const char* utf8_text, ImVec2 pos, ImU32 col) {
    ImDrawList* draw = ImGui::GetWindowDrawList();

    const char* p = utf8_text;
    ImVec2 cursor = pos;

    while (*p) {
        unsigned int c;
        int bytes = ImTextCharFromUtf8(&c, p, NULL);
        if (bytes <= 0) break;

        p += bytes;

        // Skip control characters
        if (c < 32) continue;

        ImFont* font_to_use = nullptr;
        for (ImFont* font : g_FontFallbackChain) {
            if (font->FindGlyphNoFallback((ImWchar)c)) {
                font_to_use = font;
                break;
            }
        }

        if (!font_to_use) font_to_use = g_FontFallbackChain[0];

        ImFont* backup = ImGui::GetFont();
        ImGui::PushFont(font_to_use);

        const char* tmp = p - bytes;
        ImVec2 char_size = font_to_use->CalcTextSizeA(font_to_use->FontSize, FLT_MAX, 0, tmp, tmp + bytes);

        draw->AddText(font_to_use, font_to_use->FontSize, cursor, col, tmp, tmp + bytes);
        cursor.x += char_size.x;

        ImGui::PopFont();
    }
}

















bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK) return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}