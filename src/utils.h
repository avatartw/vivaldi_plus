#ifndef UTILS_H_
#define UTILS_H_

#include <map>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <functional>

#include "FastSearch.h"

std::wstring Format(const wchar_t *format, va_list args)
{
    std::vector<wchar_t> buffer;

    size_t length = _vscwprintf(format, args);

    buffer.resize((length + 1) * sizeof(wchar_t));

    _vsnwprintf_s(&buffer[0], length + 1, length, format, args);

    return std::wstring(&buffer[0]);
}

std::wstring Format(const wchar_t *format, ...)
{
    va_list args;

    va_start(args, format);
    auto str = Format(format, args);
    va_end(args);

    return str;
}

// 搜索内存
uint8_t *memmem(uint8_t *src, int n, const uint8_t *sub, int m)
{
    return (uint8_t *)FastSearch(src, n, sub, m);
}

uint8_t *SearchModuleRaw(HMODULE module, const uint8_t *sub, int m)
{
    uint8_t *buffer = (uint8_t *)module;

    PIMAGE_NT_HEADERS nt_header = (PIMAGE_NT_HEADERS)(buffer + ((PIMAGE_DOS_HEADER)buffer)->e_lfanew);
    PIMAGE_SECTION_HEADER section = (PIMAGE_SECTION_HEADER)((char *)nt_header + sizeof(DWORD) +
                                                            sizeof(IMAGE_FILE_HEADER) + nt_header->FileHeader.SizeOfOptionalHeader);

    for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++)
    {
        if (strcmp((const char *)section[i].Name, ".text") == 0)
        {
            return memmem(buffer + section[i].PointerToRawData, section[i].SizeOfRawData, sub, m);
            break;
        }
    }
    return NULL;
}

uint8_t *SearchModuleRaw2(HMODULE module, const uint8_t *sub, int m)
{
    uint8_t *buffer = (uint8_t *)module;

    PIMAGE_NT_HEADERS nt_header = (PIMAGE_NT_HEADERS)(buffer + ((PIMAGE_DOS_HEADER)buffer)->e_lfanew);
    PIMAGE_SECTION_HEADER section = (PIMAGE_SECTION_HEADER)((char *)nt_header + sizeof(DWORD) +
                                                            sizeof(IMAGE_FILE_HEADER) + nt_header->FileHeader.SizeOfOptionalHeader);

    for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++)
    {
        if (strcmp((const char *)section[i].Name, ".rdata") == 0)
        {
            return memmem(buffer + section[i].PointerToRawData, section[i].SizeOfRawData, sub, m);
            break;
        }
    }
    return NULL;
}
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

// 获得程序所在文件夹
std::wstring GetAppDir()
{
    wchar_t path[MAX_PATH];
    ::GetModuleFileName(NULL, path, MAX_PATH);
    ::PathRemoveFileSpec(path);
    return path;
}

void DebugLog(const wchar_t *format, ...)
{
//    va_list args;
//
//    va_start(args, format);
//    auto str = Format(format, args);
//    va_end(args);
//
//    str = Format(L"[vivaldi++]%s\n", str.c_str());
//
//    OutputDebugStringW(str.c_str());
}

// https://source.chromium.org/chromium/chromium/src/+/main:chrome/app/chrome_command_ids.h?q=chrome_command_ids.h&ss=chromium%2Fchromium%2Fsrc
#define IDC_NEW_TAB 34014
#define IDC_CLOSE_TAB 34015
#define IDC_SELECT_NEXT_TAB 34016
#define IDC_SELECT_PREVIOUS_TAB 34017
#define IDC_SELECT_TAB_0 34018
#define IDC_SELECT_TAB_1 34019
#define IDC_SELECT_TAB_2 34020
#define IDC_SELECT_TAB_3 34021
#define IDC_SELECT_TAB_4 34022
#define IDC_SELECT_TAB_5 34023
#define IDC_SELECT_TAB_6 34024
#define IDC_SELECT_TAB_7 34025
#define IDC_SELECT_LAST_TAB 34026
#define IDC_SHOW_TRANSLATE 35009

#define IDC_UPGRADE_DIALOG 40024

HWND GetTopWnd(HWND hwnd)
{
    while (::GetParent(hwnd) && ::IsWindowVisible(::GetParent(hwnd)))
    {
        hwnd = ::GetParent(hwnd);
    }
    return hwnd;
}

void ExecuteCommand(int id, HWND hwnd = 0)
{
    if (hwnd == 0)
        hwnd = GetForegroundWindow();
    // hwnd = GetTopWnd(hwnd);
    // hwnd = GetForegroundWindow();
    // PostMessage(hwnd, WM_SYSCOMMAND, id, 0);
    ::SendMessageTimeoutW(hwnd, WM_SYSCOMMAND, id, 0, 0, 1000, 0);
}

template<typename String, typename Char, typename Function>
void StringSplit(String *str, Char delim, Function f)
{
    String *ptr = str;
    while (*str)
    {
        if (*str == delim)
        {
            *str = 0;           // 截断字符串

            if (str - ptr)      // 非空字符串
            {
                f(ptr);
            }

            *str = delim;       // 还原字符串
            ptr = str + 1;      // 移动下次结果指针
        }
        str++;
    }

    if (str - ptr)  // 非空字符串
    {
        f(ptr);
    }
}

// 分别模拟按键按下和释放，适用于模拟单个按键操作
// template <typename... T>
// void SendKey(T... keys)
// {
//     std::vector<INPUT> inputs;
//     std::vector<int> keys_ = {keys...};
//     for (auto &key : keys_)
//     {
//         INPUT input = {0};
//         input.type = INPUT_KEYBOARD;
//         input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
//         input.ki.wVk = (WORD)key;
//         input.ki.dwExtraInfo = MAGIC_CODE;
//
//         // 修正鼠标消息
//         switch (input.ki.wVk)
//         {
//         case VK_RBUTTON:
//             input.type = INPUT_MOUSE;
//             input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
//             input.mi.dwExtraInfo = MAGIC_CODE;
//             break;
//         case VK_LBUTTON:
//             input.type = INPUT_MOUSE;
//             input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
//             input.mi.dwExtraInfo = MAGIC_CODE;
//             break;
//         case VK_MBUTTON:
//             input.type = INPUT_MOUSE;
//             input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
//             input.mi.dwExtraInfo = MAGIC_CODE;
//             break;
//         }
//
//         inputs.push_back(input);
//     }
//     std::reverse(keys_.begin(), keys_.end());
//     for (auto &key : keys_)
//     {
//         INPUT input = {0};
//         input.type = INPUT_KEYBOARD;
//         input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
//         input.ki.wVk = (WORD)key;
//         input.ki.dwExtraInfo = MAGIC_CODE;
//
//         // 修正鼠标消息
//         switch (input.ki.wVk)
//         {
//         case VK_RBUTTON:
//             input.type = INPUT_MOUSE;
//             input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
//             input.mi.dwExtraInfo = MAGIC_CODE;
//             break;
//         case VK_LBUTTON:
//             input.type = INPUT_MOUSE;
//             input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;
//             input.mi.dwExtraInfo = MAGIC_CODE;
//             break;
//         case VK_MBUTTON:
//             input.type = INPUT_MOUSE;
//             input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
//             input.mi.dwExtraInfo = MAGIC_CODE;
//             break;
//         }
//
//         inputs.push_back(input);
//     }
//     // for (auto & key : inputs)
//     //{
//     //     DebugLog(L"%X %X", key.ki.wVk, key.mi.dwFlags);
//     // }
//
//     ::SendInput((UINT)inputs.size(), &inputs[0], sizeof(INPUT));
// }

// 发送组合按键操作
// class SendKeys
// {
//   public:
//     template <typename... T>
//     SendKeys(T... keys)
//     {
//         std::vector<int> keys_ = {keys...};
//         for (auto &key : keys_)
//         {
//             INPUT input = {0};
//             input.type = INPUT_KEYBOARD;
//             input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
//             input.ki.wVk = key;

//             // 修正鼠标消息
//             switch (key)
//             {
//             case VK_MBUTTON:
//                 input.type = INPUT_MOUSE;
//                 input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
//                 break;
//             }

//             inputs_.push_back(input);
//         }

//         SendInput((UINT)inputs_.size(), &inputs_[0], sizeof(INPUT));
//     }
//     ~SendKeys()
//     {
//         for (auto &input : inputs_)
//         {
//             input.ki.dwFlags |= KEYEVENTF_KEYUP;

//             // 修正鼠标消息
//             switch (input.ki.wVk)
//             {
//             case VK_MBUTTON:
//                 input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
//                 break;
//             }
//         }

//         SendInput((UINT)inputs_.size(), &inputs_[0], sizeof(INPUT));
//     }

//   private:
//     std::vector<INPUT> inputs_;
// };

template <typename... T>
void SendKey(T&&... keys) {
  std::vector<typename std::common_type<T...>::type> keys_ = {
      std::forward<T>(keys)...};
  std::vector<INPUT> inputs{};
  inputs.reserve(keys_.size() * 2);
  for (auto& key : keys_) {
    INPUT input = {0};
    // 修正鼠标消息
    switch (key) {
      case VK_RBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE
                               ? MOUSEEVENTF_LEFTDOWN
                               : MOUSEEVENTF_RIGHTDOWN;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      case VK_LBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE
                               ? MOUSEEVENTF_RIGHTDOWN
                               : MOUSEEVENTF_LEFTDOWN;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      case VK_MBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      default:
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = (WORD)key;
        input.ki.dwExtraInfo = MAGIC_CODE;
        break;
    }
    input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
    inputs.emplace_back(std::move(input));
  }
  for (auto& key = keys_.begin(); key != keys_.end(); ++key) {
    INPUT input = {0};
    // 修正鼠标消息
    switch (*key) {
      case VK_RBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE
                               ? MOUSEEVENTF_LEFTUP
                               : MOUSEEVENTF_RIGHTUP;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      case VK_LBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE
                               ? MOUSEEVENTF_RIGHTUP
                               : MOUSEEVENTF_LEFTUP;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      case VK_MBUTTON:
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        input.mi.dwExtraInfo = MAGIC_CODE;
        break;
      default:
        input.type = INPUT_KEYBOARD;
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        input.ki.wVk = (WORD)(*key);
        input.ki.dwExtraInfo = MAGIC_CODE;
        break;
    }
    input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
    inputs.emplace_back(std::move(input));
  }
  ::SendInput((UINT)inputs.size(), &inputs[0], sizeof(INPUT));
}

// 发送鼠标消息
void SendOneMouse(int mouse)
{
    // 交换左右键
    if (::GetSystemMetrics(SM_SWAPBUTTON) == TRUE)
    {
        if (mouse == MOUSEEVENTF_RIGHTDOWN)
            mouse = MOUSEEVENTF_LEFTDOWN;
        else if (mouse == MOUSEEVENTF_RIGHTUP)
            mouse = MOUSEEVENTF_LEFTUP;
    }

    INPUT input[1];
    memset(input, 0, sizeof(input));

    input[0].type = INPUT_MOUSE;

    input[0].mi.dwFlags = mouse;
    input[0].mi.dwExtraInfo = MAGIC_CODE;
    ::SendInput(1, input, sizeof(INPUT));
}

bool isEndWith(const wchar_t *s, const wchar_t *sub)
{
    if (!s || !sub)
        return false;
    size_t len1 = wcslen(s);
    size_t len2 = wcslen(sub);
    if (len2 > len1)
        return false;
    return !_memicmp(s + len1 - len2, sub, len2 * sizeof(wchar_t));
}

// 获得指定路径的绝对路径，如 "data/../Cache"
std::wstring GetAbsolutePath(const std::wstring &path)
{
    wchar_t buffer[MAX_PATH];
    ::GetFullPathNameW(path.c_str(), MAX_PATH, buffer, NULL);
    return buffer;
}

// 展开环境路径比如 %windir%
std::wstring ExpandEnvironmentPath(const std::wstring &path)
{
    std::vector<wchar_t> buffer(MAX_PATH);
    size_t expandedLength = ::ExpandEnvironmentStrings(path.c_str(), &buffer[0], (DWORD)buffer.size());
    if (expandedLength > buffer.size())
    {
        buffer.resize(expandedLength);
        expandedLength = ::ExpandEnvironmentStrings(path.c_str(), &buffer[0], (DWORD)buffer.size());
    }
    return std::wstring(&buffer[0], 0, expandedLength);
}
// 替换字符串
void ReplaceStringInPlace(std::wstring &subject, const std::wstring &search, const std::wstring &replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::wstring::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

// 压缩HTML
std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
    return s;
}
std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
    return s;
}

std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}

std::vector<std::string> split(const std::string &text, char sep)
{
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos)
    {
        std::string temp = text.substr(start, end - start);
        tokens.push_back(temp);
        start = end + 1;
    }
    std::string temp = text.substr(start);
    tokens.push_back(temp);
    return tokens;
}

void compression_html(std::string &html)
{
    auto lines = split(html, '\n');
    html.clear();
    for (auto &line : lines)
    {
        html += "\n";
        html += trim(line);
    }
}

// 替换字符串
bool ReplaceStringInPlace(std::string &subject, const std::string &search, const std::string &replace)
{
    bool find = false;
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
        find = true;
    }
    return find;
}
// bool WriteMemory(PBYTE BaseAddress, PBYTE Buffer, DWORD nSize)
//{
//     DWORD ProtectFlag = 0;
//     if (VirtualProtectEx(GetCurrentProcess(), BaseAddress, nSize, PAGE_EXECUTE_READWRITE, &ProtectFlag))
//     {
//         memcpy(BaseAddress, Buffer, nSize);
//         FlushInstructionCache(GetCurrentProcess(), BaseAddress, nSize);
//         VirtualProtectEx(GetCurrentProcess(), BaseAddress, nSize, ProtectFlag, &ProtectFlag);
//         return true;
//     }
//     return false;
// }

#endif // UTILS_H_
