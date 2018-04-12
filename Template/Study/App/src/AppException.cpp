

// アプリケーション用 例外クラス


AppException::AppException(HRESULT hr, const char* file, long line, const char* title)
{
    m_Message = title;
    m_Message += "\n";

    LPSTR lpMsgBuf;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&lpMsgBuf, 0, NULL);

    m_Message += lpMsgBuf;
}


void AppException::ShowMessageBox(HWND hWnd)
{
    MessageBoxA(hWnd, m_Message.c_str(), "AppException", MB_ICONSTOP);
}


void ThrowIfFailedHelper(HRESULT hr, const char* file, long line, const char* title)
{
    if (FAILED(hr))
    {
        throw AppException(hr, file, line, title);
    }
}


