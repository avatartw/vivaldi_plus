//
// Created by czyt on 2023/10/12.
//

// This file is used to control the features of the browser.
// whether to open the new tab page when clicking the right mouse button.

bool EnableDoubleClickCloseTab = false;
bool EnableRightClickCloseTab = false;
bool KeepLastTab = false;
bool WheelTab = false;
bool WheelTabWhenPressRButton = false;

void ParseFeatureFlags()
{
    std::wstring configFilePath = GetAppDir() + L"\\config.ini";

    if (PathFileExists(configFilePath.c_str()))
    {
        // Read the config file.
        EnableDoubleClickCloseTab = GetPrivateProfileIntW(L"features", L"double_click_close_tab", 1, configFilePath.c_str()) == 1;
        EnableRightClickCloseTab = GetPrivateProfileIntW(L"features", L"right_click_close_tab", 1, configFilePath.c_str()) == 1;
        KeepLastTab = GetPrivateProfileIntW(L"features", L"keep_last_tab", 1, configFilePath.c_str()) == 1;
        WheelTab = GetPrivateProfileIntW(L"features", L"wheel_tab", 1, configFilePath.c_str()) == 1;
        WheelTabWhenPressRButton = GetPrivateProfileIntW(L"features", L"wheel_tab_when_press_rbutton", 1, configFilePath.c_str()) == 1;
    }
}
