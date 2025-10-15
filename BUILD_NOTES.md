# Build Notes for Evo Menu Integration

## Project Configuration

### Visual Studio Settings
- **Platform**: x64
- **Configuration**: Release (recommended) or Debug
- **Toolset**: v143 (Visual Studio 2022)
- **Windows SDK**: 10.0
- **C++ Standard**: C++20 (stdcpp20)
- **Character Set**: Unicode

### Build Output
- **DLL Output**: `build/CS2_internal_cheat.dll`
- **Intermediates**: `build/intermediates/`

## Potential Build Issues and Solutions

### Issue 1: Missing Windows SDK
**Error**: Cannot find DirectX headers (d3d11.h, dxgi.h)

**Solution**:
- Install Windows 10 SDK from Visual Studio Installer
- Or download from: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/

### Issue 2: C++ Standard Version
**Error**: std::filesystem, std::optional not found

**Solution**:
- Ensure C++ Language Standard is set to C++17 or higher
- Project Properties → C/C++ → Language → C++ Language Standard: ISO C++20 Standard

### Issue 3: Font Loading Warnings
**Warning**: AddFontFromFileTTF failed for verdana.ttf

**Solution**:
- This is non-critical; ImGui will use default font
- To fix: Ensure `C:\Windows\Fonts\verdana.ttf` exists
- Or modify `src/evo-sdk/render/render_system.cpp` to use a different font path

### Issue 4: Linker Errors with ImGui
**Error**: Unresolved external symbols in ImGui

**Solution**:
- Ensure all ImGui .cpp files are in the project:
  - imgui.cpp
  - imgui_draw.cpp
  - imgui_widgets.cpp
  - imgui_impl_dx11.cpp
  - imgui_impl_win32.cpp
  - imgui_demo.cpp (optional)

### Issue 5: Duplicate Symbol Errors
**Error**: Multiple definition of 'animation_controller'

**Solution**:
- Check that animation_system.hpp is only included through evo_inc.hpp
- Verify inline keyword is present in animation_system.hpp:
  ```cpp
  inline animation_controller_t animation_controller;
  ```

### Issue 6: GET_X_LPARAM / GET_Y_LPARAM Not Found
**Error**: Identifier not found

**Solution**:
- Add to includes.h or evo_inc.hpp:
  ```cpp
  #include <windowsx.h>
  ```

## Build Steps

1. **Open Solution**
   ```
   Open CS2_internal_cheat.sln in Visual Studio
   ```

2. **Select Configuration**
   - Platform: x64
   - Configuration: Release

3. **Build**
   - Build → Build Solution (Ctrl+Shift+B)
   - Or: Build → Rebuild Solution (for clean build)

4. **Check Output**
   - Output should be in `build/CS2_internal_cheat.dll`
   - Size should be approximately 2-5 MB

## Testing the Build

### 1. Pre-Injection Check
- Verify DLL size is reasonable (not 0 bytes)
- Check that all dependencies are present
- No missing DLL errors when loading

### 2. Injection
Use your preferred injector:
- Manual Map injection (recommended for anti-cheat)
- LoadLibrary injection (for testing)

### 3. Verify Menu
1. Once injected, press **INSERT** key
2. Menu should appear with tabs at the top
3. Check for:
   - Menu renders correctly
   - Tabs are clickable
   - Controls are interactive
   - Menu can be dragged by title bar
   - Can be toggled with INSERT

### 4. Check ESP
- ESP functionality should still work
- ESP should render alongside menu
- No crashes when menu is open

## Debug Build vs Release Build

### Debug Build
- **Pros**: Easier to debug, has debug symbols
- **Cons**: Larger file size, slower execution, more easily detected
- **Use for**: Development and testing

### Release Build
- **Pros**: Smaller size, optimized, better performance
- **Cons**: Harder to debug
- **Use for**: Final distribution

## Common Runtime Issues

### Menu Not Showing
**Symptoms**: Press INSERT but nothing happens

**Debug Steps**:
1. Check if `init` flag is true (ImGui initialized)
2. Verify evo::_menu->init() is being called
3. Check console output for errors (if AllocConsole is enabled)
4. Ensure ImGui context is created

**Quick Fix**: Re-inject the DLL

### Crashes on Menu Open
**Symptoms**: Game crashes when pressing INSERT

**Causes**:
- Null pointer dereference in menu initialization
- Invalid memory access in render functions
- Font loading failure causing crash

**Debug**:
- Enable console in MainThread (uncomment AllocConsole)
- Add try-catch around menu->init()
- Check all static variables are initialized

### Menu Renders But Not Interactive
**Symptoms**: Menu visible but can't click/interact

**Causes**:
- Input system not updating
- WndProc not forwarding messages
- ImGui_ImplWin32_WndProcHandler not called

**Fix**:
- Verify evo::_input->init_input() is called every frame
- Check WndProc is properly hooked

### ESP Not Rendering
**Symptoms**: Menu works but ESP is gone

**Causes**:
- esp.drawESP() not called
- Render order issue

**Fix**:
- Ensure esp.drawESP() is called after menu rendering
- Check esp object is still valid

## Performance Optimization

### If menu causes FPS drop:

1. **Reduce Animation Complexity**
   - In container.cpp, reduce animation updates
   - Decrease get_min_deltatime scale factor

2. **Optimize Rendering**
   - Disable unnecessary visual effects
   - Reduce draw calls in custom controls

3. **Lazy Update**
   - Only update menu when visible
   - Skip heavy computations when menu is closed

## File Checklist

Ensure these files exist and are compiled:

**Core**:
- [x] src/Main.cpp
- [x] src/evo_inc.hpp

**evo-sdk**:
- [x] src/evo-sdk/animation_system/animation_system.cpp
- [x] src/evo-sdk/render/render_system.cpp

**UI Container**:
- [x] src/ui/container/container.cpp

**UI Controls** (all .cpp files):
- [x] button, checkbox, child, colorpicker
- [x] combo, esp_preview, keybind, listbox
- [x] multicombo, popup, slider, text
- [x] textinput, window

**UI Menu**:
- [x] src/ui/menu/menu.cpp

## Getting Help

If build fails:
1. Check Visual Studio Output window for specific errors
2. Verify all files in vcxproj are present
3. Clean solution and rebuild
4. Check Windows SDK version matches project
5. Refer to EVO_MENU_INTEGRATION.md for architecture details

## Additional Resources

- **ImGui Documentation**: https://github.com/ocornut/imgui
- **DirectX 11 Programming Guide**: https://docs.microsoft.com/en-us/windows/win32/direct3d11/
- **Original Menu Repo**: https://github.com/FaszosMazsi/menu

## Success Indicators

✅ Build succeeds with 0 errors
✅ DLL is ~2-5 MB in size
✅ No warnings about missing symbols
✅ Menu shows when INSERT is pressed
✅ Menu is interactive (can click, drag, type)
✅ ESP still renders correctly
✅ No crashes during normal use
✅ Menu can be toggled on/off
✅ All tabs are accessible

## Version Info

- **Menu System**: Evo Framework (adapted from @FaszosMazsi/menu)
- **DirectX Version**: 11
- **ImGui Version**: Included in project
- **Target Game**: Counter-Strike 2
- **Injection Method**: Compatible with both LoadLibrary and Manual Map
