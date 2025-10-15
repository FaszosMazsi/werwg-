# CS2 Internal Cheat

A Counter-Strike 2 internal cheat with an advanced evolutionary menu system.

## Features

- **Advanced UI System**: Professional evolutionary menu framework
- **ESP**: Player visualization with customizable options
- **Menu Controls**: 12+ control types (checkbox, slider, combo, colorpicker, keybind, etc.)
- **Animations**: Smooth transitions with easing functions
- **Theme System**: Customizable colors and styling
- **DirectX 11**: Full DX11 rendering support

## Menu System

The project uses an advanced evolutionary menu system integrated from [@FaszosMazsi/menu](https://github.com/FaszosMazsi/menu).

### Toggle
Press **INSERT** to open/close the menu

### Features
- Resizable and draggable windows
- Tab system with subtabs
- Child windows for organized layouts
- Popup support for advanced settings
- Integrated colorpickers and keybind selectors
- Smooth animations

## Documentation

Comprehensive documentation is provided in the following files:

1. **[EVO_MENU_INTEGRATION.md](EVO_MENU_INTEGRATION.md)** - Technical integration details and architecture
2. **[MENU_CUSTOMIZATION_GUIDE.md](MENU_CUSTOMIZATION_GUIDE.md)** - How to add features and customize the menu
3. **[BUILD_NOTES.md](BUILD_NOTES.md)** - Build configuration and troubleshooting
4. **[MIGRATION_SUMMARY.md](MIGRATION_SUMMARY.md)** - Before/after comparison of the menu system

## Building

### Requirements
- Visual Studio 2022 (or 2019)
- Windows 10 SDK
- DirectX 11
- C++20 support

### Build Steps
1. Open `CS2_internal_cheat.sln` in Visual Studio
2. Select **x64** platform and **Release** configuration
3. Build → Build Solution (Ctrl+Shift+B)
4. Output: `build/CS2_internal_cheat.dll`

For detailed build instructions and troubleshooting, see [BUILD_NOTES.md](BUILD_NOTES.md).

## Usage

1. Build the DLL using Visual Studio
2. Inject into Counter-Strike 2 process
3. Press **INSERT** to toggle menu
4. Configure settings as desired
5. ESP will render automatically

## Customization

To add your own features to the menu, edit `src/ui/menu/menu.cpp`. 

Example:
```cpp
static bool my_feature = false;
static float my_value = 50.0f;

child->make_checkbox("Enable Feature", &my_feature);
child->make_slider_float("Value", &my_value, 0, 100, "");
```

See [MENU_CUSTOMIZATION_GUIDE.md](MENU_CUSTOMIZATION_GUIDE.md) for complete instructions.

## Project Structure

```
werwg-/
├── src/
│   ├── Main.cpp              # Main DLL entry point
│   ├── evo_inc.hpp           # Menu system includes
│   ├── evo-sdk/              # Core SDK (render, input, animation)
│   ├── ui/                   # Menu UI components
│   │   ├── container/        # Menu container and theme
│   │   ├── controls/         # UI controls (12 types)
│   │   └── menu/             # Menu initialization
│   ├── imgui/                # ImGui library
│   ├── futures/              # ESP and other features
│   └── Lolz/                 # Hooking library
└── *.md                      # Documentation files
```

## Credits

- **Menu System**: [@FaszosMazsi/menu](https://github.com/FaszosMazsi/menu)
- **ImGui**: [ocornut/imgui](https://github.com/ocornut/imgui)
- **Lolz Hooking Library**: Included in project

## Disclaimer

This project is for educational purposes only. Use at your own risk.

![Screenshot](cs2.png)
