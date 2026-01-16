#define IMGUI_DEFINE_MATH_OPERATORS

//MAIN
#include "main.h"
#include "image.h"

//Auth
#include "auth/AuthlyX.h"


//Key Binds
static int aim_head;
static int aim_neck;
static int aim_drag;

static int wall_hack;
static int speed_hack;
static int camera_hack;

static int fakelag_key;
static int streamer_mode;

//Checkboxes
static bool aimbot_h = false;
static bool aimbot_d = false;
static bool aimbot_n = false;
static bool noRecoil = false;
static bool aimbot_b = false;

static bool aimbot_2x = false;
static bool aimbot_4x = false;

static bool unlimitedAmmo = false;
static bool fastReload = false;
static bool femaleFix = false;
static bool resetGuest = false;


static bool loadWallHack = false;
static bool wallHack = false;
static bool loadSpeedHack = false;
static bool speedHack = false;
static bool loadCameraHack = false;
static bool cameraHack = false;

static bool sniperScope = false;
static bool sniperSwitch = false;
static bool sniperAim = false;
static bool sniperDelay = false;

AuthlyX AuthlyXApp(
    "469e4d9235d1",
    "EXTERNAL",
    "8.1",
    "BcO8UJa9Vb94IJM9FiAF1eAPaZ552HXzwG47RHZ6"
);


namespace texture
{
    ID3D11ShaderResourceView* JINO = nullptr;
    ID3D11ShaderResourceView* logo = nullptr;
    ID3D11ShaderResourceView* logo12 = nullptr;

}

static float fakelag2 = 2.0f;

namespace esp
{
    bool money = true;
    bool nickname = true;
    bool weapon = true;
    bool zoom = true;

    bool c4 = true;
    bool HP_line = true;
    bool hit = true;
    bool box = true;
    bool bomb = true;

    static float box_color[4] = { 37 / 255.f, 37 / 255.f, 47 / 255.f, 1.f };
    static float nick_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float money_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float zoom_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float c4_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float bomb_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float hp_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float hp_line_color[4] = { 112 / 255.f, 109 / 255.f, 214 / 255.f, 1.f };
    static float weapon_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float hit_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };

    int hp = 85;
}

int page = 0;

static float tab_alpha = 0.f; /* */ static float tab_add; /* */ static int active_tab = 0;

std::string btn_txt = "Login";

DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

bool done = false;
bool fake_lag;
bool show_menu = true;
static bool streammode = false;
static bool autosave = false;
static bool beepsound = false;
static bool discordrpc= true;
static bool disableAllAnimations = false;
static bool disableParticles = false;
static bool disableCheckboxEffects = false;

bool initialized = false;
bool initialized2 = false;
bool authed = false;


void DisableDiscordRPC() {
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.details = "";
    discordPresence.state = "";
    discordPresence.largeImageKey = "";
    discordPresence.smallImageKey = "";
    Discord_UpdatePresence(&discordPresence);

    Discord_Shutdown();
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //AuthlyXApp.Init();
    if (!AuthlyXApp.Init()) {
        std::cout << "Connection Failed: " << AuthlyXApp.response.message << std::endl;
        std::cout << "Change your DNS to Google DNS or Cloudfare and try again if that didnt work try using a VPN or Contact Support";
        std::cout << "Press any key to exit...";
        std::cin.get();
        return 1;
    }
    WNDCLASSEXW wc;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = nullptr;
    wc.hIcon = nullptr;
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = L"BRUUUH CHEATS";
    wc.lpszClassName = L"BRUUUH CHEATS";
    wc.hIconSm = nullptr;

    RegisterClassExW(&wc);
    hwnd = CreateWindowExW(NULL, wc.lpszClassName, L"BRUUUH CHEATS", WS_POPUP, (GetSystemMetrics(SM_CXSCREEN) / 2) - (c::background::size.x / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (c::background::size.y / 2), c::background::size.x, c::background::size.y, 0, 0, 0, 0);

    SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    POINT mouse;
    rc = { 0 };
    GetWindowRect(hwnd, &rc);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
    bool isClickable = true;
    ToggleClickability(isClickable);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; 

    ImFontConfig cfg;
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;

    io.Fonts->AddFontFromMemoryTTF(&PoppinsRegular, sizeof PoppinsRegular, 20, NULL, io.Fonts->GetGlyphRangesCyrillic());

    font::primary_font = io.Fonts->AddFontFromMemoryTTF(&PoppinsRegular, sizeof PoppinsRegular, 20, NULL, io.Fonts->GetGlyphRangesCyrillic());
    
    font::second_font = io.Fonts->AddFontFromMemoryTTF(&PoppinsRegular, sizeof(PoppinsRegular), 18, NULL, io.Fonts->GetGlyphRangesCyrillic());

    font::icon_font = io.Fonts->AddFontFromMemoryTTF(&ico_moon, sizeof(ico_moon), 20, NULL, io.Fonts->GetGlyphRangesCyrillic());

    font::lexend_general_bold = io.Fonts->AddFontFromMemoryTTF(lexend_bold, sizeof(lexend_bold), 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::lexend_general_bold2 = io.Fonts->AddFontFromMemoryTTF(lexend_bold2, sizeof(lexend_bold2), 30.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::lexend_bold = io.Fonts->AddFontFromMemoryTTF(lexend_regular, sizeof(lexend_regular), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::lexend_regular = io.Fonts->AddFontFromMemoryTTF(lexend_regular, sizeof(lexend_regular), 14.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::icomoon = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 20.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::picomoon = io.Fonts->AddFontFromMemoryTTF(picomoon, sizeof(picomoon), 20.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    font::Nevan = io.Fonts->AddFontFromMemoryTTF(Nevan, sizeof(Nevan), 20.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::ContiB = io.Fonts->AddFontFromMemoryTTF(continuum_bold, sizeof(continuum_bold), 32.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::ContiM = io.Fonts->AddFontFromMemoryTTF(continuum_medium, sizeof(continuum_medium), 45.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    ImFont* ContiM2 = io.Fonts->AddFontFromMemoryTTF(continuum_medium, sizeof(continuum_medium), 32.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::tab_font = io.Fonts->AddFontFromMemoryTTF(continuum_medium, sizeof(continuum_medium), 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    font::icomoon_widget = io.Fonts->AddFontFromMemoryTTF(icomoon_widget, sizeof(icomoon_widget), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::icomoon_widget2 = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::icon_font2 = io.Fonts->AddFontFromMemoryTTF(&icomoon2, sizeof icomoon2, 35, NULL, io.Fonts->GetGlyphRangesCyrillic());

    D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
    if (texture::logo == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, logo, sizeof(logo), &info, pump, &texture::logo, 0);
    if (texture::logo12 == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, logo12, sizeof(particulas_xd), &info, pump, &texture::logo12, 0);
    if (texture::JINO == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, LOGOJINO, sizeof(LOGOJINO), &info, pump, &texture::JINO, 0);
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    


    while (!done)
    {

        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done) break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        static int opacity = 255;
        static bool hide = true;

        if (GetAsyncKeyState(VK_INSERT) & 1) 
        {
            isClickable = !isClickable;
            hide = !hide;
            ToggleClickability(isClickable);
        }

        if (GetAsyncKeyState(aim_head) & 1)
        {
            aimbot_h = !aimbot_h;
            if (aimbot_h) {
                notificationSystem.AddNotification("Aimbot", "Head Aimbot Enabled", ImGui::GetColorU32(c::accent));
            }
            else {
                notificationSystem.AddNotification("Aimbot", "Head Aimbot Disabled", ImGui::GetColorU32(c::accent));
            }
        }

        if (GetAsyncKeyState(aim_neck) & 1)
        {
            
        }
        if (GetAsyncKeyState(streamer_mode) & 1)
        {
            streammode = !streammode;

            HWND hwnd = GetActiveWindow();

            if (streammode)
            {
                notificationSystem.AddNotification("Done", "Stream Mode Enabled!", ImGui::GetColorU32(c::accent));

                SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE);

                ITaskbarList* pTaskList = nullptr;
                CoInitialize(nullptr);
                if (SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskbarList, (LPVOID*)&pTaskList)))
                {
                    pTaskList->DeleteTab(hwnd);
                    pTaskList->Release();
                }
                CoUninitialize();
            }
            else
            {
                notificationSystem.AddNotification("Done", "Stream Mode Disabled!", ImGui::GetColorU32(c::accent));

                SetWindowDisplayAffinity(hwnd, WDA_NONE);

                ITaskbarList* pTaskList = nullptr;
                CoInitialize(nullptr);
                if (SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskbarList, (LPVOID*)&pTaskList)))
                {
                    pTaskList->AddTab(hwnd);
                    pTaskList->Release();
                }
                CoUninitialize();
            }
        }

        if (!disableAllAnimations) {
            opacity = ImLerp(opacity, opacity <= 255 && hide ? 300 : 0, ImGui::GetIO().DeltaTime * 8.f);
        }
        else {
            opacity = opacity <= 255 && hide ? 300 : 0;
        }

        if (opacity > 255) opacity = 255;
        SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), opacity, LWA_ALPHA);

        if (GetAsyncKeyState(VK_DELETE) & 1)
        {
            exit(0);
        }

        static bool isKeyPressed = false;

        if ((GetAsyncKeyState(fakelag_key) & 0x8000) && fakelag2 > 0)
        {
            if (!isKeyPressed && fake_lag)
            {
                isKeyPressed = true;
                std::thread([]
                    {
                        Beep(444, 500);
                        auto start = std::chrono::steady_clock::now();

                        PauseNetwork();

                        auto elapsed = std::chrono::steady_clock::now() - start;
                        auto delay = ToMilliseconds(fakelag2); 

                        auto remaining = delay - std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

                        if (remaining.count() > 0)
                            std::this_thread::sleep_for(remaining); 

                        ResumeNetwork();
                        Beep(666, 470);
                    }).detach();
            }
        }
        else isKeyPressed = false;



        ImGui::NewFrame();
        {


            ImGuiStyle* style = &ImGui::GetStyle();

            style->WindowPadding = ImVec2(0, 0);
            style->ItemSpacing = ImVec2(10, 10);
            style->WindowBorderSize = 0;
            style->ScrollbarSize = 8.f;

            //static float color[4] = { 230 / 255.f, 25 / 255.f, 25 / 255.f, 1.f };//Red
            //static float color[4] = { 25 / 255.f, 100 / 255.f, 230 / 255.f, 1.f };// Light blue
            static float color[4] = { 79 / 255.f, 7 / 255.f, 168 / 255.f, 1.f };
            c::accent = { color[0], color[1], color[2], 1.f };


            if (authed == false)
            {
                //Resize window to prevent RenderBlur Glitches
                if (!initialized)
                {
                    SetWindowPos(hwnd, nullptr, (GetSystemMetrics(SM_CXSCREEN) / 2) - (c::background::size2.x / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (c::background::size2.y / 2), (int)c::background::size2.x, (int)c::background::size2.y, SWP_NOZORDER | SWP_SHOWWINDOW);
                    initialized = true;
                }

                ImVec2 windowSize = ImGui::GetWindowSize();
                ImVec2 screenSize = ImGui::GetIO().DisplaySize;

                ImVec2 centerPosition = ImVec2((screenSize.x - windowSize.x) * 0.5f, (screenSize.y - windowSize.y) * 0.5f);
                ImGui::SetNextWindowPos(ImVec2(0, 0));

                ImGui::SetNextWindowSize(c::background::size2);

                ImGui::Begin("Login", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus);
                {
                    LoadCredentials();
                    if (!disableAllAnimations && !disableParticles) {
                        ParticlesV();
                    }

                    static std::vector<std::string> captions = {
                        "WELCOME TO BRUUUH CHEATS",
                        "YOUR UNFAIR ADVANTAGE",
                        "THE WIN CONDITION",
                        //"BRUUUH CHEATS "
                        
                    };

                    const ImVec2& pos = ImGui::GetWindowPos();
                    const ImVec2& region = ImGui::GetContentRegionMax();
                    const ImVec2& spacing = style->ItemSpacing;

                  
                    auto draw = ImGui::GetWindowDrawList();
                    const auto& p = ImGui::GetWindowPos();

                    static bool login_page = true;
                    static float login_page_offset = 0.f;
                    static float animation_speed = 0.1f;

                    if (!disableAllAnimations) {
                        login_page_offset = ImLerp(login_page_offset, login_page ? 40.f : -100.f, animation_speed);
                    }
                    else {
                        login_page_offset = login_page ? 40.f : -100.f; 
                    }

                    //Background
                    ImGui::GetBackgroundDrawList()->AddRectFilled(pos, pos + c::background::size2, ImGui::GetColorU32(c::tab::border, 0.8f), c::background::rounding);
                    ImGui::GetBackgroundDrawList()->AddRectFilled(pos, pos + c::background::size2, ImGui::GetColorU32(c::accent, 0.04f), c::background::rounding);


                    //Title
                    ImGui::PushFont(font::ContiB);
                    draw->AddText(ImVec2(c::background::size2.x / 2 - ImGui::CalcTextSize("BRUUUH CHEATS").x / 2, 50), ImGui::ColorConvertFloat4ToU32(c::white), "BRUUUH");
                    draw->AddText(
                        ImVec2(
                            c::background::size2.x / 2 - ImGui::CalcTextSize("BRUUUH CHEATS").x / 2 + ImGui::CalcTextSize("BRUUUH ").x,
                            50
                        ),
                        ImColor(79 / 255.f, 7 / 255.f, 168 / 255.f, 1.f),
                        "CHEATS"
                    );
                    ImGui::PopFont();

                    //Caption
                    AnimatedCaption(captions, ImVec2(c::background::size2.x / 2 , 80));

                    ImGui::SetCursorPos(ImVec2(login_page_offset, 130));

                    ImGui::BeginChild("Inputs", ImVec2(255, 150), 0);

                    ImGui::InputTextExNew("v", "Username", username, 64, ImVec2(c::background::size2.x - 80, 40), 0,0,0);
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                    ImGui::InputTextExNew("x", "Password", password, 64, ImVec2(c::background::size2.x - 80, 40), 0, 0, 0);
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

                    if (ImGui::Button(btn_txt.c_str(), ImVec2(255, 40)))
                    {
                        btn_txt = "Logging in...";
                        std::thread([]() 
                            {
                                AuthlyXApp.Login(username, password);
                                if (AuthlyXApp.response.success) {

                                    SaveCredentials(username, password);

                                    authed = true;
                                    btn_txt = "Login";

                                    if (discordrpc) {
                                        std::time_t CurrentTime = std::time(0);
                                        std::string username = "User: " + AuthlyXApp.userData.username;
                                        std::string expiry = AuthlyXApp.userData.expiryDate;
                                        std::time_t expiryTime = std::atoi(expiry.c_str());
                                        std::string expdate = "Expiry: " + AuthlyXApp.userData.expiryDate;
                                        DiscordEventHandlers Handle;
                                        memset(&Handle, 0, sizeof(Handle));
                                        Discord_Initialize("1350087937236406292", &Handle, 1, NULL);
                                        DiscordRichPresence discordPresence;
                                        memset(&discordPresence, 0, sizeof(discordPresence));
                                        char formattedExpiry[17];
                                        std::tm* tmPtr = std::localtime(&expiryTime);
                                        std::strftime(formattedExpiry, sizeof(formattedExpiry), "Days: %m/%d/%y", tmPtr);
                                        discordPresence.state = expdate.c_str();;
                                        discordPresence.details = username.c_str();
                                        discordPresence.startTimestamp = CurrentTime;
                                        discordPresence.details = username.c_str();
                                        discordPresence.largeImageKey = "https://i.ibb.co/Pv2K78rK/bruuuh-no-bg-blue.png";
                                        discordPresence.largeImageText = "BRUUUH EXTERNAL C++";
                                        discordPresence.smallImageKey = "https://i.gifer.com/3OWpa.gif";
                                        discordPresence.smallImageText = "Your Unfair Advantage";
                                        discordPresence.button1_label = "Discord";
                                        discordPresence.button1_url = "https://discord.gg/5kAk9XwCpd";
                                        discordPresence.button2_label = "Website";
                                        discordPresence.button2_url = "https://bruuuhcheats.xyz";
                                        Discord_UpdatePresence(&discordPresence);
                                   }
                                    authed = true;
                                    notificationSystem.AddNotification("Success", "Logged In!", ImGui::GetColorU32(c::accent));
                                }
                                else 
                                {
                                    notificationSystem.AddNotification("Error", "Invalid Credentials", ImGui::GetColorU32(c::accent));
                                    authed = false;
                                    btn_txt = "Login";
                                }
                            }).detach();
                    }


                    ImGui::EndChild();

                    ImGui::ShadowText(
                        "No Account? Join Discord Server",
                        text_color[0],  // Text color
                        text_color[0],  // Shadow color
                        40.f,           // Shadow offset
                        ImVec2((c::background::size2.x - ImGui::CalcTextSize("No Account? Join Discord Server").x) / 2, c::background::size2.y - 130)
                    );

                    ImGui::SetCursorPos(ImVec2(c::background::size2.x / 2 - 27, c::background::size2.y - 50));

                    if (ImGui::TextButton("C", ImVec2(35, 35), 0));
                    if (ImGui::IsItemClicked()) ShellExecute(NULL, "open", "https://discord.gg/5kAk9XwCpd", NULL, NULL, SW_SHOW);

                    notificationSystem.DrawNotifications();
                    move_window2();

                    ImGui::End();
                    
                    RenderBlur(hwnd);
                    
                }
            }


            if (authed == true)
            {   
                //Resize window to prevent RenderBlur Glitches
                if (!initialized2)
                {
                    SetWindowPos(hwnd, nullptr, (GetSystemMetrics(SM_CXSCREEN) / 2) - (c::background::size.x / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (c::background::size.y / 2), (int)c::background::size.x, (int)c::background::size.y, SWP_NOZORDER | SWP_SHOWWINDOW);
                    initialized2 = true;
                }

                ImGui::SetNextWindowSize(c::background::size);

                ImGui::SetNextWindowPos(ImVec2(0, 0));

                ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollWithMouse);
                {
                    if (!disableAllAnimations && !disableParticles) {
                        ParticlesV();
                    }
                    // Quelque part dans ton code
                    if (ImGui::IsMouseDragging(0) && ImGui::IsWindowHovered())
                    {
                        ImVec2 delta = ImGui::GetMouseDragDelta(0);
                        // Move window
                    }
                    const ImVec2& pos = ImGui::GetWindowPos();
                    const ImVec2& region = ImGui::GetContentRegionMax();
                    const ImVec2& spacing = style->ItemSpacing;
                    
                    
                    auto draw = ImGui::GetWindowDrawList();
                    const auto& p = ImGui::GetWindowPos();

                    ImGui::GetBackgroundDrawList()->AddRectFilled(pos, pos + c::background::size, ImGui::GetColorU32(c::background::filling, 0.9f), c::background::rounding);

                    ImGui::GetBackgroundDrawList()->AddRect(pos, pos + c::background::size, ImGui::GetColorU32(c::background::stroke), c::background::rounding);

                    ImGui::GetForegroundDrawList()->AddShadowCircle(ImVec2(30, -120), 180.f, ImColor(c::accent.x, c::accent.y, c::accent.z, 0.8f), 400.f, ImVec2(0, 0), 0, 30);

                    ImGui::GetBackgroundDrawList()->AddRectFilled(pos, pos + ImVec2(190, c::background::size.y), ImGui::GetColorU32(c::background::filling, 0.7f), c::background::rounding, ImDrawFlags_RoundCornersLeft);
                   
                    ImGui::PushFont(font::lexend_bold);

                   
                    ImGui::PopFont();

                    //Title Stuff
                    ImGui::PushFont(font::ContiM);

                    const char* title = "   B R U U U H";
                    ImVec2 titlesz = ImGui::CalcTextSize(title);

                    draw->AddText(pos + ImVec2(190 / 2 - titlesz.x / 2, 5),
                        ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 0.5f)), title, 0);

                    
                   


                    ImGui::PopFont();

                  


                   


                    ImGui::SetCursorPos(ImVec2(310, 15));

                    ImGui::BeginGroup();

                    //Tabs
                    if (edited::Tab(page == 0, "c", "Aimbot", ImVec2(40, 40))) {
                        page = 0;
                    }
                    ImGui::SameLine(0, 27);

                    if (edited::Tab(page == 1, "b", "Miscs & Chams", ImVec2(40, 40))) {
                        page = 1;
                    }
                    ImGui::SameLine(0, 27);

                    if (edited::Tab(page == 2, "f", "Fake Lag", ImVec2(40, 40))) {
                        page = 2;
                    }
                    ImGui::SameLine(0, 27);

                    if (edited::Tab(page == 3, "d", "Key Binds", ImVec2(40, 40))) {
                        page = 3;
                    }
                    ImGui::SameLine(0, 27);

                    if (edited::Tab(page == 4, "e", "Settings", ImVec2(40, 40))) {
                        page = 4;
                    }

                    ImGui::EndGroup();

                    if (!disableAllAnimations) {
                        tab_alpha = ImLerp(tab_alpha, (page == active_tab) ? 1.f : 0.f, 15.f * ImGui::GetIO().DeltaTime);
                    }
                    else {
                        tab_alpha = (page == active_tab) ? 1.f : 0.f;
                        if (tab_alpha < 0.01f && tab_add < 0.01f) active_tab = page;
                    }

                    if (tab_alpha < 0.01f && tab_add < 0.01f) active_tab = page;
                    ImGui::SetCursorPos(ImVec2(20, 70));
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * style->Alpha);
                    {
                        if (active_tab == 0)
                        {
                            edited::BeginChild("a", "Aim Functions", ImVec2((c::background::size.x - 60) / 2, c::background::size.y - 60), ImGuiChildFlags_None);
                            {

                                
                                if (edited::Checkbox("Aimbot In-Game [ Head ]", "Works In-Game", &aimbot_h)) {

                                    if (aimbot_h) {
                                        notificationSystem.AddNotification("Aimbot", "Head Aimbot Enabled", ImGui::GetColorU32(c::accent));
                                        // Your enable code here
                                    }
                                    else {
                                        notificationSystem.AddNotification("Aimbot", "Head Aimbot Disabled", ImGui::GetColorU32(c::accent));
                                        // Your disable code here
                                    }
                                }

                                if (edited::Checkbox("Aimbot In-Game [ Drag ]", "Works In-Game", &aimbot_d)) {

                                    if (aimbot_d) {
                                        notificationSystem.AddNotification("Success", "Aimbot Drag Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "Aimbot Drag Disabled", ImGui::GetColorU32(c::accent));
                                    }

                                }

                                if (edited::Checkbox("Aimbot In-Game [ Neck ]", "Works In-Game", &aimbot_n)) {

                                    if (aimbot_n) {
                                        notificationSystem.AddNotification("Success", "Aimbot Neck Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "Aimbot Neck Disabled", ImGui::GetColorU32(c::accent));
                                    }

                                }

                                if (edited::Checkbox("No Recoil", "Works In-Game & Lobby", &noRecoil)) {

                                    if (noRecoil) {
                                        notificationSystem.AddNotification("Success", "No Recoil Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "No Recoil Disabled", ImGui::GetColorU32(c::accent));
                                    }

                                }

                                static std::string bodyType = "None";
                                if (edited::Checkbox("Aimbot In-Game [ Body ]", "Works Best in Lobby", &aimbot_b)) {

                                    if (bodyType == "None") {
                                        notificationSystem.AddNotification("Error", "Invalid Selection", ImGui::GetColorU32(c::accent));
                                    }
                                    else if (bodyType == "Body") {
                                        notificationSystem.AddNotification("Success", "Aimbot Body Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else if (bodyType == "Body V2") {
                                        notificationSystem.AddNotification("Success", "Aimbot Body V2 Enabled", ImGui::GetColorU32(c::accent));
                                    }

                                }

                                static int selected = 0;
                                static int previousSelect = -1;
                                const char* items[3]{ "None", "Body", "Body V2" };

                                ImGui::Combo("Aimbot Body Type", &selected, items, IM_ARRAYSIZE(items), 3);
                                if (selected != previousSelect) {

                                    if (selected == 0) {

                                        bodyType = "None";

                                    }
                                    else if (selected == 1) {

                                        bodyType = "Body";

                                    }
                                    else if (selected == 2) {

                                        bodyType = "Body V2";

                                    }
                                    
                                    previousSelect = selected;
                                }
                                /*
                                static int smooth = 165;
                                if (ImGui::SliderInt("Smoothness", &smooth, 0, 400)) {
                                    std::string message = "Smoothness:" + std::to_string(smooth) + "\n";

                                }

                                static int speed = 75;
                                if (ImGui::SliderInt("Speed", &speed, 0, 100, "%d%%")) {
                                    std::string message = "Speed:" + std::to_string(speed) + "\n";

                                }*/

                            }
                            edited::EndChild();

                            ImGui::SameLine(0, 10);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 40);

                            edited::BeginChild("f", "Scope Functions", ImVec2((c::background::size.x - 60) / 2, c::background::size.y - 60), ImGuiChildFlags_None);
                            {

                                if (edited::Checkbox("Scope Tracking [ 2X ]", "Works In-Game & Lobby", &aimbot_2x)) {

                                    if (aimbot_2x) {
                                        notificationSystem.AddNotification("Success", "Scope Tracking [ 2X ] Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "Scope Tracking [ 2X ] Disabled", ImGui::GetColorU32(c::accent));
                                    }

                                }

                                if (edited::Checkbox("Scope Tracking [ 4X ]", "Works In-Game & Lobby", &aimbot_4x)) {

                                    if (aimbot_4x) {
                                        notificationSystem.AddNotification("Success", "Scope Tracking [ 4X ] Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "Scope Tracking [ 4X ] Disabled", ImGui::GetColorU32(c::accent));
                                    }

                                }

                            }
                            edited::EndChild();

                            ImGui::SetCursorPos(ImVec2(325, 250));

                            edited::BeginChild("m", "Advanced Aim Functions", ImVec2((c::background::size.x - 60) / 2, c::background::size.y - 10), ImGuiChildFlags_None);
                            {
                                
                                if (edited::Checkbox("Unlimited Ammo", "Works In-Game & Lobby", &unlimitedAmmo)) {

                                    if (unlimitedAmmo) {
                                        notificationSystem.AddNotification("Success", "Unlimited Ammo Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "Unlimited Ammo Disabled", ImGui::GetColorU32(c::accent));
                                    }

                                }
                                
                                if (edited::Checkbox("Fast Reload", "Works In-Game & Lobby", &fastReload)) {

                                    if (fastReload) {
                                        notificationSystem.AddNotification("Success", "Fast Reload Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "Fast Reload Disabled", ImGui::GetColorU32(c::accent));
                                    }

                                }

                                if (edited::Checkbox("Female Fix", "Works In-Game & Lobby", &femaleFix)) {

                                    if (unlimitedAmmo) {
                                        notificationSystem.AddNotification("Success", "Female Fix Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "Female Fix Disabled", ImGui::GetColorU32(c::accent));
                                    }

                                }

                                if (edited::Checkbox("Reset Guest", "Works In-Login", &resetGuest)) {


                                    if (unlimitedAmmo) {
                                        notificationSystem.AddNotification("Success", "Reset Guest Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "Reset Guest Disabled", ImGui::GetColorU32(c::accent));
                                    }
                                }
                            }
                            edited::EndChild();


                        }
                        else if (active_tab == 1)
                        {
                            edited::BeginChild("b", "Misc Functions", ImVec2((c::background::size.x - 60) / 2, c::background::size.y - 60), ImGuiChildFlags_None);
                            {

                                if (edited::Checkbox("Load Wall Hack", "Works In-Game & Lobby", &loadWallHack)) {
                                    
                                    notificationSystem.AddNotification("Success", "Wall Hack Loaded", ImGui::GetColorU32(c::accent));
                                }
                                
                                if (edited::Checkbox("Wall Hack", "Works In-Game & Lobby", &wallHack)) {
                                    if (wallHack) {
                                        notificationSystem.AddNotification("Success", "Wall Hack Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "Wall Hack Disabled", ImGui::GetColorU32(c::accent));
                                    }
                                }
                                
                                if (edited::Checkbox("Load Speed Hack", "Works In-Game & Lobby", &loadSpeedHack)) {
                                    
                                    notificationSystem.AddNotification("Success", "Speed Hack Loaded", ImGui::GetColorU32(c::accent));
                                }

                                if (edited::Checkbox("Speed Hack", "Works In-Game & Lobby", &speedHack)) {
                                    if (speedHack) {
                                        notificationSystem.AddNotification("Success", "Speed Hack Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "Speed Hack Disabled", ImGui::GetColorU32(c::accent));
                                    }
                                }
                                
                                if (edited::Checkbox("Load Camera Hack", "Works In-Game & Lobby", &loadCameraHack)) {
                                    
                                    notificationSystem.AddNotification("Success", "Camera Hack Loaded", ImGui::GetColorU32(c::accent));
                                }

                                if (edited::Checkbox("Camera Hack", "Works In-Game & Lobby", &cameraHack)) {
                                    if (cameraHack) {
                                        notificationSystem.AddNotification("Success", "Camera Hack Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Success", "Camera Hack Disabled", ImGui::GetColorU32(c::accent));
                                    }
                                }

                            }
                            edited::EndChild();

                            ImGui::SameLine(0, 10);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 40);

                            edited::BeginChild("g", "Chams Functions", ImVec2((c::background::size.x - 60) / 2, c::background::size.y - 60), ImGuiChildFlags_None);
                            {
                                static bool chams3D = false;
                                if (edited::Checkbox("Chams 3D", "Works In-Game & Lobby", &chams3D)) {
                                    notificationSystem.AddNotification("Success", "Chams 3D Enabled", ImGui::GetColorU32(c::accent));
                                }
                                
                                static bool chamsGlow = false;
                                if (edited::Checkbox("Chams Glow", "Works In-Game & Lobby", &chamsGlow)) {
                                    notificationSystem.AddNotification("Success", "Chams Glow Enabled", ImGui::GetColorU32(c::accent));
                                }
                                
                                
                                static bool chamsHDR = false;
                                if (edited::Checkbox("Chams HDR", "Works In-Game & Lobby", &chamsHDR)) {
                                    notificationSystem.AddNotification("Success", "Chams HDR Enabled", ImGui::GetColorU32(c::accent));
                                }

                                static bool chamaMenu = false;
                                static std::string chamsMenuType = "None";
                                if (edited::Checkbox("Chams Menu", "Works In-Game & Lobby", &chamaMenu)) {

                                    if (chamsMenuType == "None") {
                                        notificationSystem.AddNotification("Error", "Invalid Selection", ImGui::GetColorU32(c::accent));
                                    }
                                    else if (chamsMenuType == "Old") {
                                        notificationSystem.AddNotification("Success", "Chams Menu Old Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else if (chamsMenuType == "New") {
                                        notificationSystem.AddNotification("Success", "Chams Menu New Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else if (chamsMenuType == "Overlay") {
                                        notificationSystem.AddNotification("Success", "Chams Menu Overlay Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else if (chamsMenuType == "OverlayV2") {
                                        notificationSystem.AddNotification("Success", "Chams Menu OverlayV2 Enabled", ImGui::GetColorU32(c::accent));
                                    }

                                }

                                static int selected = 0;
                                static int previousSelect = -1;
                                const char* items[5]{ "None", "Old", "New", "Overlay", "OverlayV2"};

                                ImGui::Combo("Chams Menu Type", &selected, items, IM_ARRAYSIZE(items), 5);
                                if (selected != previousSelect) {

                                    if (selected == 0) {

                                        chamsMenuType = "None";

                                    }
                                    else if (selected == 1) {

                                        chamsMenuType = "Old";

                                    }
                                    else if (selected == 2) {

                                        chamsMenuType = "New";

                                    }
                                    else if (selected == 3) {

                                        chamsMenuType = "Overlay";

                                    }
                                    else if (selected == 4) {

                                        chamsMenuType = "OverlayV2";

                                    }

                                    previousSelect = selected;
                                }

                                /*static int espdst = 150;
                                if (ImGui::SliderInt("ESP Distance", &espdst, 0, 200, "%dM%"))
                                {
                                    std::string message = "EspDistance:" + std::to_string(espdst) + "\n";

                                }

                                static int select1 = 0;
                                static int previousSelect5 = -1;
                                const char* items1[3]{ "Top", "Centre", "Bottom" };
                                ImGui::Combo("ESP Anchor Point", &select1, items1, IM_ARRAYSIZE(items1), 3);
                                if (select1 != previousSelect5) {

                                    if (select1 == 0) {


                                    }
                                    else if (select1 == 1) {


                                    }
                                    else if (select1 == 2) {


                                    }
                                    previousSelect5 = select1;
                                }*/

                            }
                            edited::EndChild();

                        }
                        else if (active_tab == 2)
                        {
                            edited::BeginChild("c", "Sniper Functions", ImVec2((c::background::size.x - 60) / 2, c::background::size.y - 60), ImGuiChildFlags_None);
                            {

                                if (edited::Checkbox("Sniper Scope", "Works In-Game & Lobby", &sniperScope))
                                {
                                    if (sniperScope) {
                                        notificationSystem.AddNotification("Notification", "Sniper Scope Enabled", ImGui::GetColorU32(c::accent));

                                    }
                                    else {
                                        notificationSystem.AddNotification("Notification", "Sniper Scope Disabled", ImGui::GetColorU32(c::accent));
                                    }
                                }
                                
                                static std::string sniperSwitchType = "V1";
                                if (edited::Checkbox("Sniper Switch", "Works In-Game & Lobby", &sniperSwitch)) {

                                    if (sniperSwitchType == "V1") {
                                        if (sniperSwitch) {
                                            notificationSystem.AddNotification("Error", "Sniper Switch V1 Enabled", ImGui::GetColorU32(c::accent));
                                        }
                                        else {
                                            notificationSystem.AddNotification("Error", "Sniper Switch V1 Disabled", ImGui::GetColorU32(c::accent));
                                        }
                                    }
                                    else if (sniperSwitchType == "V2") {
                                        if (sniperSwitch) {
                                            notificationSystem.AddNotification("Error", "Sniper Switch V2 Enabled", ImGui::GetColorU32(c::accent));
                                        }
                                        else {
                                            notificationSystem.AddNotification("Error", "Sniper Switch V2 Disabled", ImGui::GetColorU32(c::accent));
                                        }
                                    }

                                }

                                static int selected = 0;
                                static int previousSelect = -1;
                                const char* items[2]{ "V1", "V2"};

                                ImGui::Combo("Sniper Switch Type", &selected, items, IM_ARRAYSIZE(items), 2);
                                if (selected != previousSelect) {

                                    if (selected == 0) {

                                        sniperSwitchType = "V1";

                                    }
                                    else if (selected == 1) {

                                        sniperSwitchType = "V2";

                                    }

                                    previousSelect = selected;
                                }
                                
                                if (edited::Checkbox("Snper Aim", "Works In-Game & Lobby", &sniperAim))
                                {
                                    if (sniperAim) {
                                        notificationSystem.AddNotification("Notification", "Sniper Aim Enabled", ImGui::GetColorU32(c::accent));

                                    }
                                    else {
                                        notificationSystem.AddNotification("Notification", "Sniper Aim Disabled", ImGui::GetColorU32(c::accent));
                                    }
                                }
                                
                                if (edited::Checkbox("Snper Delay", "Works In-Game & Lobby", &sniperDelay))
                                {
                                    if (sniperDelay) {
                                        notificationSystem.AddNotification("Notification", "Sniper Delay Enabled", ImGui::GetColorU32(c::accent));

                                    }
                                    else {
                                        notificationSystem.AddNotification("Notification", "Sniper Delay Disabled", ImGui::GetColorU32(c::accent));
                                    }
                                }
                                
                            }
                            edited::EndChild();

                            ImGui::SameLine(0, 10);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 40);

                            edited::BeginChild("d", "Fake Lag", ImVec2((c::background::size.x - 60) / 2, c::background::size.y - 60), ImGuiChildFlags_None);
                            {

                                if (edited::Checkbox("Fake Lag - Enable", "Active Anywhere", &fake_lag))
                                {
                                    notificationSystem.AddNotification("Notification", "Fake Lag On", ImGui::GetColorU32(c::accent));


                                }


                                ImGui::SliderFloat("Delay", &fakelag2, 0.0f, 10.0f, "%0.01fS");
                                static int select1 = 0;
                                static int previousSelect4 = -1;
                                const char* items1[2]{ "Automatic", "Manual" };
                                ImGui::Combo("Mode", &select1, items1, IM_ARRAYSIZE(items1), 2);


                                if (select1 != previousSelect4) {

                                    if (select1 == 0) {



                                    }
                                    else if (select1 == 1) {



                                    }

                                    previousSelect4 = select1;
                                }

                                edited::Keybind("Fake Lag - Key", "Assign Key For Fake Lag", &fakelag_key);


                            }
                            edited::EndChild();
                        }
                        else if (active_tab == 3)
                        {
                            
                            edited::BeginChild("d", "Aimbot Key Binds", ImVec2((c::background::size.x - 60) / 2, c::background::size.y - 60), ImGuiChildFlags_None);
                            {

                                edited::Keybind("Aimbot Head - Key", "Assign Key For Aimbot Head", &aim_head);

                                edited::Keybind("Aimbot Drag - Key", "Assign Key For Aimbot Drag", &aim_drag);

                                edited::Keybind("Aimbot Neck - Key", "Assign Key For Aimbot Neck", &aim_neck);
                                
                                ImGui::Spacing();

                                edited::Keybind("Wall Hack - Key", "Assign Key For Wall Hack", &wall_hack);

                                edited::Keybind("Speed Hack - Key", "Assign Key For Speed Hack", &speed_hack);

                                edited::Keybind("Camera Hack - Key", "Assign Key For Camera Hack", &camera_hack);



                            }
                            edited::EndChild();


                            ImGui::SameLine(0, 10);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 40);

                            edited::BeginChild("i", "Streamer Key Binds", ImVec2((c::background::size.x - 60) / 2, c::background::size.y - 60), ImGuiChildFlags_None);
                            {

                                edited::Keybind("Streamer Mode - Key", "Assign Key For Streamer Mode", &streamer_mode);

                                ImGui::SetCursorPos(ImVec2(63.5, ImGui::GetCursorPosY() + 25 - ImGui::CalcTextSize("P").y / 2));
                                ImGui::PushFont(font::tab_font);
                                ImGui::TextColored(c::accent, "Press ");
                                ImGui::PopFont();

                                draw->AddRectFilled(ImVec2(637.5, 197), ImVec2(637.5 + ImGui::CalcTextSize("INS").x + 10, 215), ImGui::GetColorU32(c::accent, 0.4f), 3.f);

                                ImGui::SetCursorPos(ImVec2(151.5, 85));
                                ImGui::PushFont(font::tab_font);
                                ImGui::TextColored(c::accent, "to Hide the Menu");
                                ImGui::PopFont();

                                ImGui::SetCursorPos(ImVec2(113, 85));
                                ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.5f), "INS");

                            }
                            edited::EndChild();



                        }
                        else if (active_tab == 4)
                        {
                            edited::BeginChild("g", "Settings", ImVec2((c::background::size.x - 60) / 2, c::background::size.y - 60), ImGuiChildFlags_None);
                            {
                                edited::ColorEdit4("Main Color", "Setting the main color of the menu.", color, picker_flags);

                                if (edited::Checkbox("Streamer Mode", "Hides panel and cheats in stream", &streammode))
                                {

                                    stream = !stream;
                                    if (stream) {
                                        notificationSystem.AddNotification("Done", "Stream Mode Enabled!", ImGui::GetColorU32(c::accent));

                                        SetWindowDisplayAffinity(GetActiveWindow(), WDA_EXCLUDEFROMCAPTURE);
                                        ITaskbarList* pTaskList = NULL;
                                        HRESULT initRet = CoInitialize(NULL);
                                        HRESULT createRet = CoCreateInstance(CLSID_TaskbarList,
                                            NULL,
                                            CLSCTX_INPROC_SERVER,
                                            IID_ITaskbarList,
                                            (LPVOID*)&pTaskList);

                                        if (createRet == S_OK)
                                        {

                                            pTaskList->DeleteTab(GetActiveWindow());

                                            pTaskList->Release();
                                        }

                                        CoUninitialize();


                                    }
                                    else {
                                        notificationSystem.AddNotification("Done", "Stream Mode Disabled!", ImGui::GetColorU32(c::accent));

                                        SetWindowDisplayAffinity(GetActiveWindow(), WDA_NONE);
                                        ITaskbarList* pTaskList = NULL;
                                        HRESULT initRet = CoInitialize(NULL);
                                        HRESULT createRet = CoCreateInstance(CLSID_TaskbarList,
                                            NULL,
                                            CLSCTX_INPROC_SERVER,
                                            IID_ITaskbarList,
                                            (LPVOID*)&pTaskList);

                                        if (createRet == S_OK)
                                        {
                                            pTaskList->AddTab(GetActiveWindow());

                                            pTaskList->Release();
                                        }


                                    }

                                }
                                
                                if (edited::Checkbox("Auto Save Settings", "Auto Save & Load on Next Launch", &autosave))
                                {

                                    if (autosave) {
                                        notificationSystem.AddNotification("Info", "Auto Save Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Info", "Auto Save Disabled", ImGui::GetColorU32(c::accent));
                                    }

                                }
                                
                                if (edited::Checkbox("Discord RPC", "Display Application status on Discord", &discordrpc))
                                {
                                    if (discordrpc) {
                                        if (authed) { 
                                            std::time_t CurrentTime = std::time(0);
                                            std::string username = "User: " + AuthlyXApp.userData.username;
                                            std::string expiry = AuthlyXApp.userData.expiryDate;
                                            std::time_t expiryTime = std::atoi(expiry.c_str());
                                            std::string expdate = "Expiry: " + AuthlyXApp.userData.expiryDate;
                                            DiscordEventHandlers Handle;
                                            memset(&Handle, 0, sizeof(Handle));
                                            Discord_Initialize("1350087937236406292", &Handle, 1, NULL);
                                            DiscordRichPresence discordPresence;
                                            memset(&discordPresence, 0, sizeof(discordPresence));
                                            char formattedExpiry[17];
                                            std::tm* tmPtr = std::localtime(&expiryTime);
                                            std::strftime(formattedExpiry, sizeof(formattedExpiry), "Days: %m/%d/%y", tmPtr);
                                            discordPresence.state = expdate.c_str();
                                            discordPresence.details = username.c_str();
                                            discordPresence.startTimestamp = CurrentTime;
                                            discordPresence.largeImageKey = "https://i.ibb.co/Pv2K78rK/bruuuh-no-bg-blue.png";
                                            discordPresence.largeImageText = "BRUUUH EXTERNAL C++";
                                            discordPresence.smallImageKey = "https://i.gifer.com/3OWpa.gif";
                                            discordPresence.smallImageText = "Your Unfair Advantage";
                                            discordPresence.button1_label = "Discord";
                                            discordPresence.button1_url = "https://discord.gg/5kAk9XwCpd";
                                            discordPresence.button2_label = "Website";
                                            discordPresence.button2_url = "https://bruuuhcheats.xyz";
                                            Discord_UpdatePresence(&discordPresence);
                                        }
                                        notificationSystem.AddNotification("Info", "Discord RPC Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        DisableDiscordRPC();
                                        notificationSystem.AddNotification("Info", "Discord RPC Disabled", ImGui::GetColorU32(c::accent));
                                    }
                                }

                                static int selected = 1;
                                static int previousSelect = -1;
                                const char* items[2]{ "Yes", "No"};

                                ImGui::Combo("Beep Sound", &selected, items, IM_ARRAYSIZE(items), 5);
                                if (selected != previousSelect) {

                                    if (selected == 0) {

                                        beepsound = true;
                                        notificationSystem.AddNotification("Info", "Beep Sound Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else if (selected == 1) {

                                        beepsound = false;
                                        notificationSystem.AddNotification("Info", "Beep Sound Disabled", ImGui::GetColorU32(c::accent));
                                    }

                                    previousSelect = selected;
                                }

                                if (edited::Checkbox("Disable All Animations", "Turn off all UI animations", &disableAllAnimations))
                                {
                                    if (disableAllAnimations) {
                                        notificationSystem.AddNotification("Info", "All Animations Disabled", ImGui::GetColorU32(c::accent));
                                    }
                                    else {
                                        notificationSystem.AddNotification("Info", "Animations Enabled", ImGui::GetColorU32(c::accent));
                                    }
                                }

                                if (!disableAllAnimations) {
                                    if (edited::Checkbox("Disable Background Particles", "Turn off background particle effects", &disableParticles))
                                    {
                                        if (disableParticles) {
                                            notificationSystem.AddNotification("Info", "Background Particles Disabled", ImGui::GetColorU32(c::accent));
                                        }
                                        else {
                                            notificationSystem.AddNotification("Info", "Background Particles Enabled", ImGui::GetColorU32(c::accent));
                                        }
                                    }
                                }
                                ImGui::Spacing();
                            }
                            edited::EndChild();

                            ImGui::SameLine(0, 10);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 40);

                            edited::BeginChild("g", "Info", ImVec2((c::background::size.x - 60) / 2, c::background::size.y - 60), ImGuiChildFlags_None);
                            {
                                //// Title with icon
                                //ImGui::PushFont(font::icomoon_widget2);
                                //ImGui::TextColored(c::accent, "e ");
                                //ImGui::PopFont();
                                //ImGui::SameLine();
                                //ImGui::Text("Information & Links");
                                //ImGui::Separator();

                                //ImGui::Spacing();

                                // Buttons with icons
                                if (edited::IconButton("b", "Discord Server", ImVec2(ImGui::GetContentRegionAvail().x, 40)))
                                {
                                    ShellExecute(NULL, "open", "https://discord.gg/5kAk9XwCpd", NULL, NULL, SW_SHOW);
                                    notificationSystem.AddNotification("Info", "Opening Discord...", ImGui::GetColorU32(c::accent));
                                }

                                //if (CustomIconButton("C", "Discord Server"))
                                //{
                                //    ShellExecute(NULL, "open", "https://discord.gg/5kAk9XwCpd", NULL, NULL, SW_SHOW);
                                //    notificationSystem.AddNotification("Info", "Opening Discord...", ImGui::GetColorU32(c::accent));
                                //}
                                ImGui::Spacing();

                                if (edited::IconButton("d", "Official Website", ImVec2(ImGui::GetContentRegionAvail().x, 40)))
                                {
                                    ShellExecute(NULL, "open", "https://bruuuhcheats.xyz", NULL, NULL, SW_SHOW);
                                    notificationSystem.AddNotification("Info", "Opening Website...", ImGui::GetColorU32(c::accent));
                                }

                                ImGui::Spacing();

                                if (edited::IconButton("f", "User Guide", ImVec2(ImGui::GetContentRegionAvail().x, 40)))
                                {
                                    //ShellExecute(NULL, "open", "https://docs.bruuuhcheats.xyz", NULL, NULL, SW_SHOW);
                                    notificationSystem.AddNotification("Info", "Coming Soon", ImGui::GetColorU32(c::accent));
                                }

                                ImGui::Spacing();
                                ImGui::Separator();

                                edited::IconLabel("c", "Version: 8.1 External", ImVec4(0.8f, 0.8f, 0.8f, 1.0f));

                                std::string userText = "User: " + AuthlyXApp.userData.username;
                                edited::IconLabel("a", userText.c_str(), ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
                                
                                std::string keyText = "License: " + AuthlyXApp.userData.licenseKey;
                                edited::IconLabel("a", keyText.c_str(), ImVec4(0.8f, 0.8f, 0.8f, 1.0f));

                                std::string subText = "Subscription: " + AuthlyXApp.userData.subscription;
                                edited::IconLabel("h", subText.c_str(), ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

                                std::string expiryText = "Expires: " + AuthlyXApp.userData.expiryDate;
                                edited::IconLabel("g", expiryText.c_str(), ImVec4(0.8f, 0.8f, 0.8f, 1.0f));

                                ImGui::Spacing();
                                ImGui::Separator();
                            }
                            edited::EndChild();
                        }
                        

                        ImGui::PopStyleVar();

                    }
                    
                  
                    notificationSystem.DrawNotifications();
                    move_window();
                    ImGui::End();
                    RenderBlur(hwnd);

                }

            }
            
        }


        ImGui:: Render();

        const float clear_color_with_alpha[4] = { 0.f };

        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

   
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

