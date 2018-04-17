

// アプリケーション用 例外クラス


AppException::AppException(HRESULT hr, const char* file, long line, const char* message)
{
    std::stringstream ss;

    ss << "エラーが発生しました。\n\n";

    ss << file << "(" << line << ")\n\n";

    ss << message << "\n\n";

    LPVOID lpMsgBuf;

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
        (LPSTR)&lpMsgBuf,
        0,
        NULL
    );

    // 文字列をコピーする。
    if (lpMsgBuf != nullptr)
    {
        ss << (LPCSTR)lpMsgBuf;
    }

    // バッファを解放する。
    LocalFree(lpMsgBuf);

    m_Message = ss.str();
}


void AppException::ShowMessageBox(HWND hWnd)
{
    MessageBoxA(hWnd, m_Message.c_str(), "AppException", MB_ICONSTOP);
}


void ThrowIfFailedHelper(HRESULT hr, const char* file, long line, const char* message)
{
    if (FAILED(hr))
    {
        throw AppException(hr, file, line, message);
    }
}


