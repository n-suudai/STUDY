#pragma once


// アプリケーション用 例外クラス


class AppException
{
public:
    AppException(HRESULT hr, const char* file, long line, const char* message);

    void ShowMessageBox(HWND hWnd);

private:
    std::string m_Message;
};


void ThrowIfFailedHelper(HRESULT hr, const char* file, long line, const char* message);


#define ThrowIfFailed(hr) ThrowIfFailedHelper(hr, __FILE__, __LINE__, #hr)

