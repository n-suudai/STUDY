#pragma once


// アプリケーション用 例外クラス


class AppException
{
public:
    AppException(HRESULT hr, const char* file, long line, const char* message);

    AppException(const char* file, long line, const char* message);

    AppException(GLenum error, const char* file, long line, const char* message);

    void ShowMessageBox(HWND hWnd);

private:
    std::string m_Message;
};


void ThrowIfFailedHelper(HRESULT hr, const char* file, long line, const char* message);

void ThrowIfFalseHelper(bool x, const char* file, long line, const char* message);

void ThrowIfErrorHelper(const char* file, long line, const char* message);


#define ThrowIfFailed(hr) ThrowIfFailedHelper(hr, __FILE__, __LINE__, #hr)

#define ThrowIfFalse(x) ThrowIfFalseHelper(x, __FILE__, __LINE__, #x)

#define ThrowIfFalseMessage(x, msg) ThrowIfFalseHelper(x, __FILE__, __LINE__, msg)

#define ThrowIfGLError(msg) ThrowIfErrorHelper(__FILE__, __LINE__, msg)

