

// アプリケーション用 例外クラス


std::string ToString(GLenum error)
{
    switch (error)
    {
    case GL_NO_ERROR: return "GL_NO_ERROR";
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
#if defined(GL_VERSION_4_3) && !defined(GL_VERSION_3_2)
    case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
#endif
    }
    std::stringstream ss;
    ss << "ErrorCode '" << (error) << "'";
    return ss.str();
}


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


AppException::AppException(const char* file, long line, const char* message)
{
    std::stringstream ss;

    ss << "エラーが発生しました。\n\n";

    ss << file << "(" << line << ")\n\n";

    ss << message << "\n\n";

    m_Message = ss.str();
}


AppException::AppException(GLenum error, const char* file, long line, const char* message)
{
    std::stringstream ss;

    ss << "エラーが発生しました。\n\n";

    ss << file << "(" << line << ")\n\n";

    ss << message << "\n\n";

    ss << ToString(error) << "\n\n";

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


void ThrowIfFalseHelper(bool x, const char* file, long line, const char* message)
{
    if (!x)
    {
        throw AppException(file, line, message);
    }
}


void ThrowIfErrorHelper(const char* file, long line, const char* message)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        throw AppException(error, file, line, message);
    }
}

