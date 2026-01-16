# BRUUUH CHEATS - External C++ Cheat Menu

## Overview

**BRUUUH CHEATS** is an advanced external cheat menu for Windows games, built using C++20, DirectX 11, and ImGui. The project provides a modern, interactive GUI for toggling a variety of in-game features, including aimbot, wallhack, speedhack, chams, fake lag, and more. It also includes user authentication, Discord Rich Presence integration, and customizable settings.

> **Note:** This project is for educational and research purposes only.

---

## Features

- **User Authentication:**  
  Secure login system using the AuthlyX API.

- **Aimbot:**  
  - Head, Neck, Drag, and Body targeting  
  - Scope tracking (2x, 4x)  
  - No recoil  
  - Unlimited ammo, fast reload, and more

- **Wallhack & Chams:**  
  - Toggle wallhack and chams (3D, Glow, HDR, Menu types)  
  - Customizable ESP (Extra Sensory Perception) features

- **Miscellaneous:**  
  - Speed hack, camera hack  
  - Sniper functions (scope, switch, aim, delay)  
  - Fake lag with adjustable delay and keybind

- **Key Bindings:**  
  - Assignable keys for all major features

- **Streamer Mode:**  
  - Hides the cheat UI and features from screen capture/streaming software

- **Settings:**  
  - Color customization  
  - Auto-save/load settings  
  - Toggle UI animations and background particles  
  - Discord Rich Presence integration

- **Notifications:**  
  - In-app notification system for feature toggles and status updates

- **Images:**
  <img width="1366" height="741" alt="sniper fakelag" src="https://github.com/user-attachments/assets/104e81bd-021d-40ba-b5f6-ec6eda4b7386" />
<img width="1361" height="736" alt="settings" src="https://github.com/user-attachments/assets/76791cc3-456e-4b43-a5f3-415bae7cc5f7" />
<img width="1366" height="737" alt="misc chams" src="https://github.com/user-attachments/assets/c3504a6c-d104-46ed-b063-3bef7dbfd4c3" />
<img width="1362" height="736" alt="login" src="https://github.com/user-attachments/assets/ec562fbb-1a26-4e54-8760-5463eea5c657" />
<img width="1364" height="736" alt="keybind" src="https://github.com/user-attachments/assets/d2c47bbb-aabb-49e6-b04a-58eb8f9f3f7f" />
<img width="1366" height="735" alt="aimbotwhite" src="https://github.com/user-attachments/assets/0718be11-1b08-43c4-b6d9-1297b5d53329" />
<img width="1366" height="743" alt="aimbot" src="https://github.com/user-attachments/assets/99a6ab47-f33d-4f71-9fec-ea5d6319669e" />
---

## Getting Started

### Prerequisites

- Windows 10/11
- Visual Studio 2022
- C++20 toolchain
- DirectX 11 SDK

### Building

1. Clone the repository.
2. Open `example_win32_directx11.sln` in Visual Studio 2022.
3. Restore any required NuGet packages.
4. Build the solution in Release mode.

### Running

1. Launch the compiled executable.
2. Log in with your credentials (provided via AuthlyX)
3. Use the menu to enable/disable features as needed.

---

## Usage

- **Menu Toggle:**  
  Press `INSERT` to show/hide the cheat menu.

- **Keybinds:**  
  Assign keys for each feature in the "Key Binds" tab.

- **Tabs:**  
  - **Aimbot:** Configure aim-related features.
  - **Miscs & Chams:** Toggle wallhack, chams, and other visual features.
  - **Fake Lag:** Enable and configure fake lag.
  - **Key Binds:** Set up custom key bindings.
  - **Settings:** Adjust UI, notifications, and integrations.

- **Streamer Mode:**  
  Enable in the Settings tab to hide the menu from screen capture.

---

## Important Files

- `main.cpp` - Main application logic, GUI, and feature toggles.
- `auth/AuthlyX.h` - Authentication logic.
- `image.h` - Image and texture loading.
- Additional headers and source files for feature implementations.

---

## Disclaimer

This software is intended for educational and ethical research purposes only.  
Using cheats in online games may violate terms of service and result in bans.  
The authors are not responsible for any misuse.

---

## Links

- [Discord Server](https://discord.gg/5kAk9XwCpd)
- [Official Website](https://bruuuhcheats.xyz)

---

## License

Distributed for educational use. See [LICENSE](LICENSE) for more information.
