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
2. Log in with your credentials (provided via AuthlyX).
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