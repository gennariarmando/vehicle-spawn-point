#include "plugin.h"
#include "VehicleGenerator.h"

#include "CFont.h"

using namespace plugin;

bool bInitialised = false;

void Init() {
#ifdef DEBUG
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);

    Events::drawHudEvent += [] {
        if (!FindPlayerPed())
            return;

        char buff[64];
        CVector pos = FindPlayerCoors();
        sprintf(buff, "%.f, %.f, %.f", pos.x, pos.y, pos.z);

        CFont::SetPropOn();
        CFont::SetBackgroundOff();
        CFont::SetCentreOff();
        CFont::SetRightJustifyOff();
        CFont::SetFontStyle(0);
        CFont::SetColor(CRGBA(255, 255, 255, 255));
        CFont::SetDropColor(CRGBA(0, 0, 0, 255));
        CFont::SetDropShadowPosition(1);
        CFont::SetScale(SCREEN_MULTIPLIER(1.0f), SCREEN_MULTIPLIER(1.0f));
        CFont::PrintString(SCREEN_COORD(10.0f), SCREEN_COORD(10.0f), buff);
    };
#endif

    Events::initGameEvent += [] {
        if (!bInitialised) {
            std::ifstream file(PLUGIN_PATH("VehicleSpawnPoint.dat"));

            if (file.is_open()) {
                for (std::string line; getline(file, line);) {
                    int index;
                    float x, y, z;
                    float a;
                    int timer;
                    int locked;

                    if (!line[0] || line[0] == '\t' || line[0] == ' ' || line[0] == '#' || line[0] == '[')
                        continue;

                    sscanf(line.c_str(), "%d, %f, %f, %f, %f, %d, %d", &index, &x, &y, &z, &a, &timer, &locked);
                    CVehicleGenerator::AddSpawnPoint(index, CVector(x, y, z), a, timer, locked);
                }
                file.close();
            }
            bInitialised = true;
        }
    };

    Events::gameProcessEvent += [] {
        for (auto& iter : CVehicleGenerator::ListOfVehicles) {
            if (iter) {
                iter->Process();
            }
        }
    };

    Events::shutdownRwEvent += [] {
        if (bInitialised) {
            CVehicleGenerator::DeleteAllSpawnPoints();
            bInitialised = false;
        }
    };
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        Init();
    }
    return TRUE;
}
