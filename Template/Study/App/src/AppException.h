#pragma once


// アプリケーション用 例外クラス


class AppException
{
public:
    AppException(HRESULT hr, const char* file, long line, const char* title);

    void ShowMessageBox(HWND hWnd);

private:
    std::string m_Message;
};


void ThrowIfFailedHelper(HRESULT hr, const char* file, long line, const char* title);


#define ThrowIFFailed(hr, title) ThrowIfFailedHelper(hr, __FILE__, __LINE__, title)

