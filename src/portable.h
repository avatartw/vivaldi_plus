#ifndef PORTABLE_H_
#define PORTABLE_H_

std::wstring QuoteSpaceIfNeeded(const std::wstring &str)
{
    if (str.find(L' ') == std::wstring::npos)
        return std::move(str);

    std::wstring escaped(L"\"");
    for (auto c : str)
    {
        if (c == L'"')
            escaped += L'"';
        escaped += c;
    }
    escaped += L'"';
    return std::move(escaped);
}

std::wstring JoinArgsString(std::vector<std::wstring> lines, const std::wstring &delimiter)
{
    std::wstring text;
    bool first = true;
    for (auto &line : lines)
    {
        if (!first)
            text += delimiter;
        else
            first = false;
        text += QuoteSpaceIfNeeded(line);
    }
    return text;
}

bool IsExistsPortable()
{
    std::wstring path = GetAppDir() + L"\\portable";
    if (PathFileExists(path.data()))
    {
        return true;
    }
    return false;
}

bool IsNeedPortable()
{
    return true;
    //    static bool need_portable = IsExistsPortable();
    //    return need_portable;
}

bool IsCustomIniExist()
{
    std::wstring path = GetAppDir() + L"\\config.ini";
    if (PathFileExists(path.data()))
    {
        return true;
    }
    return false;
}

// GetUserDataDir retrieves the user data directory path from the config file.
// It first tries to read the data dir from the "data" key in the "dir_setting" section.
// If that fails, it falls back to a default relative to the app dir.
// It expands any environment variables in the path.
std::wstring GetUserDataDir()
{
    std::wstring configFilePath = GetAppDir() + L"\\config.ini";
    if (!PathFileExists(configFilePath.c_str()))
    {
        return GetAppDir() + L"\\..\\Data";
    }

    TCHAR userDataBuffer[MAX_PATH];
    ::GetPrivateProfileStringW(L"dir_setting", L"data", L"", userDataBuffer, MAX_PATH, configFilePath.c_str());

    std::wstring expandedPath = ExpandEnvironmentPath(userDataBuffer);

    // Expand %app%
    ReplaceStringInPlace(expandedPath, L"%app%", GetAppDir());

    std::wstring dataDir;
    dataDir = GetAbsolutePath(expandedPath);

    wcscpy(userDataBuffer, dataDir.c_str());

    return std::wstring(userDataBuffer);
}

// GetDiskCacheDir retrieves the disk cache directory path from the config file.
// It first tries to read the cache dir from the "cache" key in the "dir_setting" section.
// If that fails, it falls back to a default relative to the app dir.
// It expands any environment variables in the path.
std::wstring GetDiskCacheDir()
{
    std::wstring configFilePath = GetAppDir() + L"\\config.ini";

    if (!PathFileExists(configFilePath.c_str()))
    {
        return GetAppDir() + L"\\..\\Cache";
    }

    TCHAR cacheDirBuffer[MAX_PATH];
    ::GetPrivateProfileStringW(L"dir_setting", L"cache", L"", cacheDirBuffer, MAX_PATH, configFilePath.c_str());

    std::wstring expandedPath = ExpandEnvironmentPath(cacheDirBuffer);

    // Expand %app%
    ReplaceStringInPlace(expandedPath, L"%app%", GetAppDir());

    std::wstring cacheDir;
    cacheDir = GetAbsolutePath(expandedPath);
    wcscpy(cacheDirBuffer, cacheDir.c_str());

    return std::wstring(cacheDirBuffer);
}

// 如果 ini 存在，從中讀取 CommandLine；如果 ini 不存在，或者存在，但是 CommandLine 為空，則返回空字串
std::wstring GetCrCommandLine()
{
    if (IsCustomIniExist())
    {
        std::wstring IniPath = GetAppDir() + L"\\config.ini";
        std::vector<TCHAR> CommandLineBuffer(64); // 初始大小為 64
        DWORD bytesRead = ::GetPrivateProfileStringW(L"General", L"CommandLine", L"", CommandLineBuffer.data(), CommandLineBuffer.size(), IniPath.c_str());

        // 如果讀取的字符數接近緩衝區的大小，可能需要更大的緩衝區
        while (bytesRead >= CommandLineBuffer.size() - 1)
        {
            CommandLineBuffer.resize(CommandLineBuffer.size() + 64);
            bytesRead = ::GetPrivateProfileStringW(L"General", L"CommandLine", L"", CommandLineBuffer.data(), CommandLineBuffer.size(), IniPath.c_str());
        }

        return std::wstring(CommandLineBuffer.data());
    }
    else
    {
        return std::wstring(L"");
    }
}

// 构造新命令行
// Parses the command line param into individual arguments and inserts
// additional arguments for portable mode.
//
// param: The command line passed to the application.
//
// Returns: The modified command line with additional args.
std::wstring GetCommand(LPWSTR param)
{
    std::vector<std::wstring> args;

    int argc;
    LPWSTR *argv = CommandLineToArgvW(param, &argc);

    int insert_pos = 0;
    for (int i = 0; i < argc; ++i) {
        if (std::wstring(argv[i]).find(L"--") != std::wstring::npos || std::wstring(argv[i]).find(L"--single-argument") != std::wstring::npos) {
            break;
        }
        insert_pos = i;
    }
    for (int i = 0; i < argc; ++i)
    {
        // 保留原来参数
        if (i)
            args.push_back(argv[i]);

        // 追加参数
        if (i == insert_pos)
        {
            args.push_back(L"--gopher");

            // args.push_back(L"--force-local-ntp");
            // args.push_back(L"--disable-background-networking");

            // args.push_back(L"--disable-features=RendererCodeIntegrity");

            // 獲取命令行，然後追加參數
            // 截取拆分每個--開頭的參數，然後多次 args.push_back
            // 重複上述過程，直到字串中不再存在 -- 號
            // 這樣就可以保證每個參數生效
            {
                auto cr_command_line = GetCrCommandLine();

                std::wstring temp = cr_command_line;
                while (true)
                {
                    auto pos = temp.find(L"--");
                    if (pos == std::wstring::npos)
                    {
                        break;
                    }
                    else
                    {
                        auto pos2 = temp.find(L" --", pos);
                        if (pos2 == std::wstring::npos)
                        {
                            args.push_back(temp);
                            break;
                        }
                        else
                        {
                            args.push_back(temp.substr(pos, pos2 - pos));
                            temp = temp.substr(0, pos) + temp.substr(pos2 + 1);
                        }
                    }
                }
            }

            // if (IsNeedPortable())
            {
                auto userdata = GetUserDataDir();
                auto diskcache = GetDiskCacheDir();

                args.push_back(L"--user-data-dir=" + userdata);
                if (diskcache != userdata)
                {
                    args.push_back(L"--disk-cache-dir=" + diskcache);
                }
            }
        }
    }
    LocalFree(argv);

    return JoinArgsString(args, L" ");
}

void Portable(LPWSTR param)
{
    wchar_t path[MAX_PATH];
    ::GetModuleFileName(nullptr, path, MAX_PATH);

    std::wstring args = GetCommand(param);

    SHELLEXECUTEINFO sei = {0};
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"open";
    sei.lpFile = path;
    sei.nShow = SW_SHOWNORMAL;

    sei.lpParameters = args.c_str();
    if (ShellExecuteEx(&sei))
    {
        ExitProcess(0);
    }
}

#endif // PORTABLE_H_
