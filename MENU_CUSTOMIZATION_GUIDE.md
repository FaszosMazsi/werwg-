# Menu Customization Quick Reference

## Adding Your Own Features to the Menu

### 1. Define Your Variables
First, add static variables in `src/ui/menu/menu.cpp` at the top of the `init()` function:

```cpp
void evo::menu_t::init() {
    // Your variables
    static bool enable_feature = false;
    static float feature_value = 50.0f;
    static int feature_mode = 0;
    static evo::col_t feature_color;
    static float feature_color_hue = 0.0f;
    static int keybind[2]{ 0 }; // key, mode
```

### 2. Create a Child Window

```cpp
auto child = new evo::child_t(
    "Window Title",              // Title
    0,                          // Tab index (which main tab this appears on)
    evo::child_pos_t::_top_left, // Position: _top_left, _top_right, _bottom_left, _bottom_right
    evo::child_size_t::_full,   // Size: _full, _half_width, _half_height
    0                           // Advanced positioning
);
{
    window->make_child(child);
    
    // Add controls here...
    
} delete child;
```

### 3. Add Controls

#### Checkbox
```cpp
child->make_checkbox("Enable Feature", &enable_feature);
```

#### Checkbox with Colorpicker
```cpp
child->make_checkbox_colorpicker("Enable with Color", &enable_feature, &feature_color, &feature_color_hue);
```

#### Slider (Float)
```cpp
child->make_slider_float(
    "Value",           // Label
    &feature_value,    // Variable
    0,                 // Min
    100,               // Max
    "units"           // Suffix
);
```

#### Slider (Integer)
```cpp
static int int_value = 50;
child->make_slider_int("Integer Value", &int_value, 0, 100, "");
```

#### Dropdown/Combo
```cpp
static int selected_mode = 0;
child->make_dropdown("Mode", &selected_mode, {
    "Option 1",
    "Option 2",
    "Option 3"
});
```

#### Multi-select Dropdown
```cpp
static bool options[5]{ false };
auto multi = new evo::multi_dropdown_t("Select Multiple");
for (int i = 0; i < 5; i++) {
    multi->insert_item("Option " + std::to_string(i), &options[i]);
}
child->obj(multi);
```

#### Button
```cpp
child->make_button("Click Me", [&]() {
    // Code to execute on button click
    // Can access your variables here
});
```

#### Text Label
```cpp
child->make_text("This is informational text");
```

#### Text Input
```cpp
static std::string user_input;
child->make_textbox("Enter Text", &user_input);
```

#### Keybind
```cpp
child->make_text("Feature Keybind:");
child->make_keybind(&keybind[0], &keybind[1]);
// keybind[0] = key code (VK_*)
// keybind[1] = mode (0=always, 1=hold, 2=toggle)
```

#### Popup (Advanced settings)
```cpp
auto popup = new evo::popup_t(0, "Advanced Settings");
popup->bind_checkbox("Sub Option 1", &sub_option1);
popup->bind_checkbox("Sub Option 2", &sub_option2);
popup->bind_slider_int("Sub Slider", &sub_value, 0, 100, "");
popup->bind_text("Info text in popup");
child->obj(popup);
```

### 4. Enable Scrolling (Optional)

If you have many controls in a child window:

```cpp
child->enable_scrolling();
```

### 5. Complete Example

```cpp
void evo::menu_t::init() {
    std::vector<std::string> tabs = { "Aimbot", "Visuals", "Misc" };
    static bool aimbot_enabled = false;
    static float aimbot_fov = 90.0f;
    static int aimbot_target = 0;
    static evo::col_t esp_color(255, 0, 0);
    static float esp_color_hue = 0.0f;
    
    auto window = new evo::window_t(
        "My Cheat Menu", 
        &evo::theme::menu_spawn, 
        evo::theme::menu_size, 
        tabs, 
        &evo::theme::selected_tab
    );
    {
        // TAB 0: Aimbot
        {
            auto child = new evo::child_t("Aimbot Settings", 0, 
                evo::child_pos_t::_top_left, evo::child_size_t::_full, 0);
            {
                window->make_child(child);
                
                child->make_checkbox("Enable Aimbot", &aimbot_enabled);
                child->make_slider_float("FOV", &aimbot_fov, 1, 180, "°");
                child->make_dropdown("Target", &aimbot_target, {
                    "Head", "Chest", "Nearest"
                });
                
                child->enable_scrolling();
            } delete child;
        }
        
        // TAB 1: Visuals
        {
            auto child = new evo::child_t("ESP Settings", 1, 
                evo::child_pos_t::_top_left, evo::child_size_t::_full, 0);
            {
                window->make_child(child);
                
                child->make_checkbox_colorpicker("Box ESP", 
                    &esp_box_enabled, &esp_color, &esp_color_hue);
                
                child->enable_scrolling();
            } delete child;
        }
    }
    delete window;
    
    // Required: Begin drawing
    evo::externals::_ext_b->begin();
    evo::externals::_ext_b_p->begin_popup();
}
```

## Child Window Positioning

### Position Options:
- `evo::child_pos_t::_top_left` - Top left of the tab area
- `evo::child_pos_t::_top_right` - Top right of the tab area
- `evo::child_pos_t::_bottom_left` - Bottom left of the tab area
- `evo::child_pos_t::_bottom_right` - Bottom right of the tab area

### Size Options:
- `evo::child_size_t::_full` - Full height of available space
- `evo::child_size_t::_half_width` - Half the width
- `evo::child_size_t::_half_height` - Half the height

## Using Subtabs

```cpp
std::vector<std::string> subtabs = { "Sub 1", "Sub 2", "Sub 3" };
static int current_subtab = 0;

auto child = new evo::child_t(subtabs, 1, 
    evo::child_pos_t::_top_left, evo::child_size_t::_full, 
    0, &current_subtab);
{
    window->make_child(child);
    
    // Different controls based on current_subtab
    // The child will automatically show tab buttons
}
delete child;
```

## Accessing Variables from Game Code

Your menu variables are static in menu.cpp. To use them in game code:

### Option 1: Move to Header (Recommended)
Create a config header:

```cpp
// src/Config.h
#pragma once

namespace Config {
    inline bool aimbot_enabled = false;
    inline float aimbot_fov = 90.0f;
    // etc...
}
```

Then in menu.cpp, reference these instead of static variables:
```cpp
child->make_checkbox("Aimbot", &Config::aimbot_enabled);
```

### Option 2: Getter Functions
In menu.hpp:
```cpp
namespace evo {
    class menu_t {
    public:
        void init();
        bool get_aimbot_enabled();
        float get_aimbot_fov();
    };
}
```

## Styling Tips

### Colors
```cpp
// Create colors
evo::col_t red(255, 0, 0);
evo::col_t semi_transparent = red.modify_alpha(128);

// From hex
evo::col_t blue = evo::col_t().hex_to_rgb("#0000FF");

// HSB
evo::col_t rainbow = evo::col_t::from_hsb(hue, 1.0f, 1.0f);
```

### Conditional Display
```cpp
child->make_checkbox("Enable Feature", &enabled);

if (enabled) {
    child->make_slider_float("Feature Value", &value, 0, 100, "");
}
```

## Testing Your Changes

1. Build the project in Visual Studio
2. Inject the DLL into CS2
3. Press INSERT to open menu
4. Navigate to your tab
5. Test all controls work correctly

## Common Issues

### Control Not Showing
- Check that child is on correct tab index
- Ensure window->make_child(child) is called
- Verify correct positioning parameters

### Variable Not Updating
- Ensure you pass address (&variable) not value
- Check variable scope (must be static or global)
- Verify pointer lifetime

### Crash on Menu Open
- Check all pointers are valid
- Ensure all `new` have corresponding `delete`
- Verify window object is created correctly

## Need Help?

Refer to existing controls in `src/ui/menu/menu.cpp` for more examples.
