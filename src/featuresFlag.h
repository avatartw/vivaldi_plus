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
bool IsOpenUrlNewTab = false;
bool IsBookmarkNewTab = false;
bool IsNewTabDisable = false;

// 尝试读取 ini 文件
bool IsIniExist()
{
    std::wstring path = GetAppDir() + L"\\config.ini";
    if (PathFileExists(path.data()))
    {
        return true;
    }
    return false;
}

void ParseFeatureFlags()
{
    if (IsIniExist())
    {
        // Read the config file.
        std::wstring IniPath = GetAppDir() + L"\\config.ini";
        EnableDoubleClickCloseTab = GetPrivateProfileIntW(L"features", L"double_click_close_tab", 1, IniPath.c_str()) == 1;
        EnableRightClickCloseTab = GetPrivateProfileIntW(L"features", L"right_click_close_tab", 1, IniPath.c_str()) == 1;
        KeepLastTab = GetPrivateProfileIntW(L"features", L"keep_last_tab", 1, IniPath.c_str()) == 1;
        WheelTab = GetPrivateProfileIntW(L"features", L"wheel_tab", 1, IniPath.c_str()) == 1;
        WheelTabWhenPressRButton = GetPrivateProfileIntW(L"features", L"wheel_tab_when_press_rbutton", 1, IniPath.c_str()) == 1;
        IsOpenUrlNewTab = GetPrivateProfileIntW(L"features", L"open_url_new_tab", 1, IniPath.c_str()) == 1;
        IsBookmarkNewTab = GetPrivateProfileIntW(L"features", L"open_bookmark_new_tab", 1, IniPath.c_str()) == 1;
        IsNewTabDisable = GetPrivateProfileIntW(L"features", L"new_tab_disable", 1, IniPath.c_str()) == 1;
    }
}

// 如果启用老板键，则读取 ini 文件中的老板键设置；如果 ini 不存在或者该值为空，则返回空字符串
std::wstring GetBosskey()
{
    if (IsIniExist())
    {
        std::wstring IniPath = GetAppDir() + L"\\config.ini";
        TCHAR BosskeyBuffer[100];
        ::GetPrivateProfileStringW(L"General", L"Bosskey", L"", BosskeyBuffer, 100, IniPath.c_str());
        return std::wstring(BosskeyBuffer);
    }
    else
    {
        return std::wstring(L"");
    }
}

// 定义翻译快捷键
std::wstring GetTranslateKey()
{
    if (IsIniExist())
    {
        std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
        TCHAR TranslateKeyBuffer[100];
        ::GetPrivateProfileStringW(L"General", L"TranslateKey", L"", TranslateKeyBuffer, 100, IniPath.c_str());
        return std::wstring(TranslateKeyBuffer);
    }
    else
    {
        return std::wstring(L"");
    }
}
