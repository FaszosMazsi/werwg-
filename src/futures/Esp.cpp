#include "Esp.h"
#include "Misc.h"

//extern Misc misc;

uintptr_t Esp::client = (uintptr_t)GetModuleHandle(L"client.dll");

void Esp::drawSettings()
{
    ImGui::Checkbox("Esp", &enabled);

    if (enabled) {
        ImGui::Checkbox("Debug Menu", &showDebugMenu);
        ImGui::Checkbox("Név", &showName);
        ImGui::Checkbox("Élet", &showHealth);
        ImGui::Checkbox("Armor", &showArmor);
        //ImGui::Checkbox("Head ESP", &showHead); // <-- ÚJ GOMB!
    }
}

void Esp::drawESP()
{
    if (!enabled)
        return;

    float(*ViewMatrix)[4][4] = (float(*)[4][4])(client + Offsets::dwViewMatrix);

    auto localPawn = *(uintptr_t*)(client + Offsets::dwLocalPlayerPawn);
    if (!localPawn)
        return;

    auto entityList = *(uintptr_t*)(client + Offsets::dwEntityList);

    for (int i = 1; i < 64; i++) {
        uintptr_t list_entry1 = *(uintptr_t*)(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
        if (!list_entry1)
            continue;

        uintptr_t playerController = *(uintptr_t*)(list_entry1 + 120 * (i & 0x1FF));
        if (!playerController)
            continue;

        uint32_t playerPawn = *(uint32_t*)(playerController + Offsets::m_hPlayerPawn);
        if (!playerPawn)
            continue;

        uintptr_t list_entry2 = *(uintptr_t*)(entityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
        if (!list_entry2)
            continue;

        uintptr_t pCSPlayerPawnPtr = *(uintptr_t*)(list_entry2 + 120 * (playerPawn & 0x1FF));
        if (!pCSPlayerPawnPtr)
            continue;

        if (pCSPlayerPawnPtr == localPawn)
            continue;

        auto localTeam = *(int*)(localPawn + Offsets::m_iTeamNum);

        int playerTeam = *(int*)(pCSPlayerPawnPtr + Offsets::m_iTeamNum);
        if (playerTeam == localTeam)
            continue;

        // Name
        char* namePtr = *(char**)(playerController + Offsets::m_sSanitizedPlayerName);
        std::string entityName = namePtr ? std::string(namePtr) : "Unknown";

        // health
        int health = *(int*)(pCSPlayerPawnPtr + Offsets::m_iHealth);
        if (!health || health > 100)
            continue;

        // armor
        int armor = *(int*)(pCSPlayerPawnPtr + Offsets::m_ArmorValue);

        // pos
        Vec3 feetpos = *(Vec3*)(pCSPlayerPawnPtr + Offsets::m_vOldOrigin);
        Vec3 headpos = { feetpos.x, feetpos.y, feetpos.z + 73.0f };

        Vec2 feet, head;

        if (feetpos.WorldToScreen(feet, ViewMatrix) && headpos.WorldToScreen(head, ViewMatrix)) {
            float height = fabsf(feet.y - head.y);
            float width = height / 2.0f;

            ImGui::GetForegroundDrawList()->AddRect(ImVec2(feet.x - width / 2, head.y), ImVec2(feet.x + width / 2, feet.y), ImColor(255, 255, 255));

            // Középre igazított név, outline-nal
            if (showName) {
                ImFont* defaultFont = ImGui::GetFont();
                float origFontSize = ImGui::GetFontSize();

                float nameFontSize = 13.0f; // Fix méret
                ImGui::PushFont(defaultFont);
                ImGui::SetWindowFontScale(nameFontSize / origFontSize);

                ImVec2 textSize = ImGui::CalcTextSize(entityName.c_str());
                ImVec2 namePos = ImVec2(feet.x - (textSize.x / 2), head.y - textSize.y - 2);

                ImU32 outlineColor = IM_COL32(0, 0, 0, 220);
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (dx != 0 || dy != 0)
                            ImGui::GetForegroundDrawList()->AddText(ImVec2(namePos.x + dx, namePos.y + dy), outlineColor, entityName.c_str());
                    }
                }
                ImGui::GetForegroundDrawList()->AddText(namePos, IM_COL32(255, 255, 255, 255), entityName.c_str());

                ImGui::SetWindowFontScale(1.0f);
                ImGui::PopFont();
            }

            float barPadding = 2.0f;

            float minBarWidth = 3.0f;
            float maxBarWidth = 7.0f;
            float barWidth = minBarWidth;
            float minH = 15.0f, maxH = 70.0f; // totalHeight tartomány

            if (height <= minH)
                barWidth = minBarWidth;
            else if (height >= maxH)
                barWidth = maxBarWidth;
            else
                barWidth = minBarWidth + (maxBarWidth - minBarWidth) * ((height - minH) / (maxH - minH));

            if (showHealth) {
                Esp::drawVerticalBar(
                    feet.x - width / 2 - barWidth, // barX
                    feet.y,
                    height,
                    barWidth,
                    health, 100,
                    ImColor(255, 0, 0),
                    ImColor(0, 255, 0),
                    ImColor(50, 50, 50, 200),
                    ImColor(100, 100, 100),
                    ImColor(255, 255, 255),
                    barPadding
                );
            }

            if (showArmor) {
                Esp::drawVerticalBar(
                    feet.x - width / 2 - barWidth - barWidth - barPadding, // barX (next to health)
                    feet.y,
                    height,
                    barWidth,
                    armor, 100,
                    ImColor(196, 76, 2),
                    ImColor(0, 0, 255),
                    ImColor(50, 50, 50, 200),
                    ImColor(100, 100, 100),
                    ImColor(0, 200, 255),
                    barPadding
                );
            }


        }
    }

    misc.runBhop();
}

void Esp::drawVerticalBar(float barX, float barBottomY, float totalHeight, float barWidth, int value, int maxValue,
    ImColor topColor, ImColor bottomColor, ImColor bgColor, ImColor outlineColor, ImColor textColor, float padding)
{
    barX += padding;

    int steps = (int)totalHeight;
    for (int i = 0; i < steps; i++) {
        float t = (float)i / steps;
        ImU32 col = ImColor(
            (int)((1.0f - t) * topColor.Value.x * 255 + t * bottomColor.Value.x * 255),
            (int)((1.0f - t) * topColor.Value.y * 255 + t * bottomColor.Value.y * 255),
            (int)((1.0f - t) * topColor.Value.z * 255 + t * bottomColor.Value.z * 255)
        );
        ImGui::GetForegroundDrawList()->AddRectFilled(
            ImVec2(barX, barBottomY - i - 1),
            ImVec2(barX + barWidth, barBottomY - i),
            col
        );
    }

    float percent = value / (float)maxValue;
    float filledHeight = totalHeight * percent;
    ImGui::GetForegroundDrawList()->AddRectFilled(
        ImVec2(barX, barBottomY - totalHeight),
        ImVec2(barX + barWidth, barBottomY - filledHeight),
        bgColor
    );

    ImGui::GetForegroundDrawList()->AddRect(
        ImVec2(barX, barBottomY - totalHeight),
        ImVec2(barX + barWidth, barBottomY),
        outlineColor
    );

    ImGui::GetForegroundDrawList()->AddRect(
        ImVec2(barX, barBottomY - filledHeight),
        ImVec2(barX + barWidth, barBottomY),
        textColor
    );
}