#ifndef TABBOOKMARK_H_
#define TABBOOKMARK_H_

#include "IAccessibleUtils.h"

HHOOK mouse_hook = NULL;

#define KEY_PRESSED 0x8000
bool IsPressed(int key)
{
    return key && (::GetKeyState(key) & KEY_PRESSED) != 0;
}

// 滾輪切換標籤頁
bool handleMouseWheel(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse)
{
    if (wParam != WM_MOUSEWHEEL)
    {
        return false;
    }

    HWND hwnd = WindowFromPoint(pmouse->pt);
    NodePtr TopContainerView = GetTopContainerView(hwnd);

    PMOUSEHOOKSTRUCTEX pwheel = (PMOUSEHOOKSTRUCTEX)lParam;
    int zDelta = GET_WHEEL_DELTA_WPARAM(pwheel->mouseData);

    // 是否啟用滑鼠停留在標籤欄時滾輪切換標籤
    if (WheelTab && IsOnTheTabBar(TopContainerView, pmouse->pt))
    {
        hwnd = GetTopWnd(hwnd);
        if (zDelta > 0)
        {
            ExecuteCommand(IDC_SELECT_PREVIOUS_TAB, hwnd);
        }
        else
        {
            ExecuteCommand(IDC_SELECT_NEXT_TAB, hwnd);
        }
        return true;
    }

    // 是否啟用在任何位置按住右鍵時滾輪切換標籤
    if (WheelTabWhenPressRButton && IsPressed(VK_RBUTTON))
    {
        hwnd = GetTopWnd(hwnd);
        if (zDelta > 0)
        {
            ExecuteCommand(IDC_SELECT_PREVIOUS_TAB, hwnd);
        }
        else
        {
            ExecuteCommand(IDC_SELECT_NEXT_TAB, hwnd);
        }
        return true;
    }

    return false;
}

// 雙擊關閉標籤頁
bool handleDblClk(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse)
{
    if (wParam != WM_LBUTTONDBLCLK || !EnableDoubleClickCloseTab)
    {
        return false;
    }

    HWND hwnd = WindowFromPoint(pmouse->pt);
    NodePtr TopContainerView = GetTopContainerView(hwnd);

    bool isOnOneTab = IsOnOneTab(TopContainerView, pmouse->pt);
    bool isOnlyOneTab = IsOnlyOneTab(TopContainerView);

    if (isOnOneTab)
    {
        if (isOnlyOneTab)
        {
            ExecuteCommand(IDC_NEW_TAB);
            ExecuteCommand(IDC_SELECT_PREVIOUS_TAB);
            ExecuteCommand(IDC_CLOSE_TAB);
        }
        else
        {
            ExecuteCommand(IDC_CLOSE_TAB);
        }
        return true;
    }

    return false;
}

// 右鍵關閉標籤頁
bool handleRightClick(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse)
{
    if (wParam != WM_RBUTTONUP || IsPressed(VK_SHIFT) || !EnableRightClickCloseTab)
    {
        return false;
    }

    HWND hwnd = WindowFromPoint(pmouse->pt);
    NodePtr TopContainerView = GetTopContainerView(hwnd);

    bool isOnOneTab = IsOnOneTab(TopContainerView, pmouse->pt);
    bool isOnlyOneTab = IsOnlyOneTab(TopContainerView);

    if (isOnOneTab)
    {
        if (isOnlyOneTab)
        {
            ExecuteCommand(IDC_NEW_TAB);
            ExecuteCommand(IDC_SELECT_PREVIOUS_TAB);
            ExecuteCommand(IDC_CLOSE_TAB);
        }
        else
        {
            SendKey(VK_MBUTTON);
        }
        return true;
    }

    return false;
}

// 保留最後標籤頁 - 中鍵
bool handleMiddleClick(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse)
{
    if (wParam != WM_MBUTTONUP)
    {
        return false;
    }

    HWND hwnd = WindowFromPoint(pmouse->pt);
    NodePtr TopContainerView = GetTopContainerView(hwnd);

    bool isOnOneTab = IsOnOneTab(TopContainerView, pmouse->pt);
    bool isOnlyOneTab = IsOnlyOneTab(TopContainerView);

    if (isOnOneTab && isOnlyOneTab)
    {
        ExecuteCommand(IDC_NEW_TAB);
        return true;
    }

    return false;
}

// 新標籤頁打開書籤
bool handleBookmark(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse)
{
    if (wParam != WM_LBUTTONUP || IsPressed(VK_CONTROL) || IsPressed(VK_SHIFT) || !IsBookmarkNewTab)
    {
        return false;
    }

    HWND hwnd = WindowFromPoint(pmouse->pt);
    NodePtr TopContainerView = GetTopContainerView(hwnd);

    bool isOnBookmark = IsOnBookmark(TopContainerView, pmouse->pt);
    bool isOnNewTab = IsOnNewTab(TopContainerView);

    if (TopContainerView && isOnBookmark && !isOnNewTab)
    {
        SendKey(VK_MBUTTON, VK_SHIFT);
        return true;
    }

    return false;
}

// 新標籤頁打開書籤資料夾中的書籤
bool handleBookmarkMenu(WPARAM wParam, LPARAM lParam, PMOUSEHOOKSTRUCT pmouse)
{
    if (wParam != WM_LBUTTONUP || IsPressed(VK_CONTROL) || IsPressed(VK_SHIFT) || !IsBookmarkNewTab)
    {
        return false;
    }

    HWND hwnd_p = WindowFromPoint(pmouse->pt);
    HWND hwnd_k = GetFocus();
    NodePtr TopContainerView = GetTopContainerView(hwnd_k);
    NodePtr MenuBarPane = GetMenuBarPane(hwnd_p);

    bool isOnMenuBookmark = IsOnMenuBookmark(MenuBarPane, pmouse->pt);
    bool isOnNewTab = IsOnNewTab(TopContainerView);

    if (TopContainerView && MenuBarPane && isOnMenuBookmark && !isOnNewTab)
    {
        SendKey(VK_MBUTTON, VK_SHIFT);
        return true;
    }

    return false;
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    static bool wheel_tab_ing = false;
    static bool double_click_ing = false;

    if (nCode != HC_ACTION)
    {
        return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
    }

    if (nCode == HC_ACTION)
    {
        PMOUSEHOOKSTRUCT pmouse = (PMOUSEHOOKSTRUCT)lParam;

        if (wParam == WM_MOUSEMOVE || wParam == WM_NCMOUSEMOVE)
        {
            return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
        }

        // Defining a `dwExtraInfo` value to prevent hook the message sent by
        // Chrome++ itself.
        if (pmouse->dwExtraInfo == MAGIC_CODE)
        {
            // DebugLog(L"MAGIC_CODE %x", wParam);
            goto next;
        }

        if (wParam == WM_RBUTTONUP && wheel_tab_ing)
        {
            // DebugLog(L"wheel_tab_ing");
            wheel_tab_ing = false;
            return 1;
        }

        // if (wParam == WM_MBUTTONDOWN)
        //{
        //     //DebugLog(L"wheel_tab_ing");
        //     return 1;
        // }
        // if (wParam == WM_LBUTTONUP && double_click_ing)
        //{
        //     //DebugLog(L"double_click_ing");
        //     double_click_ing = false;
        //     return 1;
        // }

        if (handleMouseWheel(wParam, lParam, pmouse))
        {
            return 1;
        }

        if (handleDblClk(wParam, lParam, pmouse))
        {
        }

        if (handleRightClick(wParam, lParam, pmouse))
        {
            return 1;
        }

        if (handleMiddleClick(wParam, lParam, pmouse))
        {
        }

        if (handleBookmark(wParam, lParam, pmouse))
        {
            return 1;
        }
        if (handleBookmarkMenu(wParam, lParam, pmouse))
        {
            return 1;
        }
    }
next:
    // DebugLog(L"CallNextHookEx %X", wParam);
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}

bool IsNeedKeep()
{
    bool keep_tab = false;

    NodePtr TopContainerView = GetTopContainerView(GetForegroundWindow());
    if (IsOnlyOneTab(TopContainerView))
    {
        keep_tab = true;
    }

    if (TopContainerView)
    {
    }

    return keep_tab;
}

bool IsNeedOpenUrlInNewTab()
{
    bool open_url_ing = false;

    NodePtr TopContainerView = GetTopContainerView(GetForegroundWindow());
    if (IsOmniboxFocus(TopContainerView))
    {
        if(!IsOnNewTab(TopContainerView))
        {
            open_url_ing = true;
        }

    }

    if (TopContainerView)
    {
    }

    return open_url_ing;
}

HHOOK keyboard_hook = NULL;
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && !(lParam & 0x80000000)) // pressed
    {
        bool keep_tab = false;

        if (wParam == 'W' && IsPressed(VK_CONTROL) && !IsPressed(VK_SHIFT))
        {
            keep_tab = IsNeedKeep();
        }
        if (wParam == VK_F4 && IsPressed(VK_CONTROL))
        {
            keep_tab = IsNeedKeep();
        }

        if (keep_tab)
        {
            ExecuteCommand(IDC_NEW_TAB);
            ExecuteCommand(IDC_SELECT_PREVIOUS_TAB);
            ExecuteCommand(IDC_CLOSE_TAB);
            return 1;
        }

        bool open_url_ing = false;

        if (IsOpenUrlNewTab && wParam == VK_RETURN && !IsPressed(VK_MENU))
        {
            open_url_ing = IsNeedOpenUrlInNewTab();
        }

        if (open_url_ing)
        {
            SendKey(VK_MENU, VK_RETURN);
            return 1;
        }
    }
    return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
}

void TabBookmark()
{
    mouse_hook = SetWindowsHookEx(WH_MOUSE, MouseProc, hInstance, GetCurrentThreadId());
    keyboard_hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInstance, GetCurrentThreadId());
}

#endif // TABBOOKMARK_H_
