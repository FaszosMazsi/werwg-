# Evo Menu System Integration

## Overview
This document describes the integration of the evolutionary menu system from @FaszosMazsi/menu into the werwg- project.

## Changes Made

### 1. Menu System Files Added
The following directories and files were copied from @FaszosMazsi/menu:

**evo-sdk/**
- `animation_system/` - Animation controller for smooth UI transitions
- `input_system/` - Mouse and keyboard input handling
- `render/` - Custom rendering system built on ImGui
  - `render_system.cpp/hpp` - Core rendering functions
  - `fonts.hpp` - Embedded font data (121k+ lines)
- `sdk/` - Core data structures (vec2_t, col_t, rect_t)

**ui/**
- `container/` - Menu container and theming system
- `menu/` - Main menu initialization
- `controls/` - UI components:
  - button, checkbox, child, colorpicker, combo, esp_preview
  - keybind, listbox, multicombo, popup, slider, text, textinput, window

### 2. DirectX 9 to DirectX 11 Adaptation

#### Key Changes:
- **Render System**: Adapted from DX9 to DX11
  - Removed `D3DXCreateTextureFromFileInMemoryEx` calls
  - Disabled blur shader functionality (was using DX9 pixel shaders)
  - Changed device references from `IDirect3DDevice9*` to `void*`
  
- **Font System**: Simplified loading
  - Removed ImGuiFreeType dependency (commented out)
  - Using system font (verdana.ttf) for all font slots
  - Font atlas is built with standard ImGui::Build()

- **Blur Effects**: Disabled
  - Original used DX9 shaders (blur_x, blur_y)
  - Replaced with simple semi-transparent overlays
  - Can be re-implemented with DX11 shaders if needed

### 3. Main.cpp Integration

#### Removed:
```cpp
// Old tab-based menu code
if (g_ShowMenu) {
    ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Devox");
    if (ImGui::BeginTabBar("MainTabs")) {
        // ESP and Aimbot tabs
    }
    ImGui::End();
}
```

#### Added:
```cpp
// Initialize evo menu system fonts (in InitImGui)
evo::_render->initialize_imgui(window, pDevice);

// Update input for evo menu system (in hkPresent)
evo::_input->init_input();

// Initialize container
evo::_container->initialize_container();

// Initialize the new evo menu
evo::_menu->init();

// Begin external drawing
evo::externals::_ext_b->begin();

// Popup drawing
evo::externals::_ext_b_p->begin_popup();
```

#### WndProc Updates:
```cpp
// Update mouse position for evo input system
if (uMsg == WM_MOUSEMOVE)
    evo::_input->set_mouse_position(evo::vec2_t(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));

if (uMsg == WM_MOUSEWHEEL)
    evo::_input->set_mouse_wheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
```

### 4. Project Configuration

Updated `CS2_internal_cheat.vcxproj`:
- Added all evo-sdk .cpp files to compilation
- Added all ui control .cpp files to compilation
- Added all corresponding .hpp files to includes
- Added `evo_inc.hpp` as main header

### 5. Include Structure

Created `src/evo_inc.hpp` which includes:
- Standard Windows/DirectX headers (DX11)
- ImGui and DX11 implementation
- All evo-sdk modules
- All UI control modules

All menu system files now include `../../evo_inc.hpp` instead of the original `../../inc.hpp`.

## Architecture

### Menu Rendering Flow:
1. **Input Update**: `evo::_input->init_input()` - Captures keyboard/mouse state
2. **Container Init**: `evo::_container->initialize_container()` - Sets up theme colors, handles menu toggle
3. **Menu Draw**: `evo::_menu->init()` - Draws menu structure and controls
4. **External Draw**: `evo::externals::_ext_b->begin()` - Renders queued drawing commands
5. **Popup Draw**: `evo::externals::_ext_b_p->begin_popup()` - Renders popup elements

### Animation System:
- Uses `animation_controller_t` for smooth transitions
- Supports easing functions (in_circ, out_circ, in_quad, etc.)
- Delta-time based for consistent animation speeds

### Theme System:
Located in `ui/container/container.hpp`:
- `theme::menu_spawn` - Menu position
- `theme::menu_size` - Menu dimensions
- `theme::selected_tab` - Current tab index
- Colors defined in `_container->initialize_container()`

## Menu Toggle

The menu is still toggled with **INSERT** key (VK_INSERT) as configured in the container initialization.

## Customization

### Adding Menu Elements:
Edit `src/ui/menu/menu.cpp` in the `evo::menu_t::init()` function:

```cpp
auto child = new evo::child_t("Child Name", 0, evo::child_pos_t::_top_left, evo::child_size_t::_full, 0);
{
    window->make_child(child);
    child->make_checkbox("Option", &variable);
    child->make_slider_float("Slider", &float_var, 0, 100, "");
    // ... more controls
}
delete child;
```

### Styling:
Modify colors in `container_t::initialize_container()`:
- `window_backround` - Main window background
- `window_outline` - Window border
- `window_accent` - Accent color (red by default)
- Colors support alpha modification with `modify_alpha()`

## Known Limitations

1. **Blur Effects**: Disabled due to DX9→DX11 conversion
   - Original used custom pixel shaders
   - Can be re-implemented with DX11 compute shaders

2. **Fonts**: Simplified to system font
   - Original used embedded custom fonts (lexend, icons)
   - Font data still present in fonts.hpp but not loaded
   - Can be re-enabled with proper ImGui font loading

3. **Background Textures**: Not loaded
   - `backround` and `preview` textures referenced but not used
   - Were loaded with D3DXCreateTextureFromFileInMemoryEx

## Build Instructions

The project should build with Visual Studio using the existing solution file.

Requirements:
- Windows SDK with DirectX 11
- C++17 or later (project uses std::filesystem, std::optional)
- ImGui library (already included in src/imgui/)

## Testing

After building:
1. Inject the DLL into Counter-Strike 2
2. Press INSERT to toggle menu
3. Menu should display with tabs at top
4. ESP functionality should still work alongside menu

## Future Improvements

1. Implement DX11-based blur effects
2. Load custom fonts from fonts.hpp
3. Add configuration save/load
4. Integrate more game features into menu tabs
5. Add theme customization UI

## Credits

Original menu system by @FaszosMazsi from https://github.com/FaszosMazsi/menu
Adapted for DirectX 11 compatibility for the werwg- project.
