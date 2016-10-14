#pragma once

#include "libinstall/CancelToken.h"

enum DOWNLOAD_STATUS {
    DOWNLOAD_STATUS_SUCCESS,
    DOWNLOAD_STATUS_FAIL,
    DOWNLOAD_STATUS_CANCELLED,
    DOWNLOAD_STATUS_FORCE_RETRY
};

class InternetDownload {
public:
    InternetDownload(HWND parentHwnd, const tstring& userAgent, const tstring& url, CancelToken cancelToken, std::function<void(int)> progressFunction = NULL);
        
    ~InternetDownload();

    void disableCache();

    BOOL saveToFile(const tstring& filename);

    std::string getContent();
    
    const tstring& getContentType() const { return m_contentType; }

private:

    BOOL request();
    void writeToFile(BYTE* buffer, DWORD bufferLength, void* context);
    void writeToString(BYTE* buffer, DWORD bufferLength, void* context);
    BOOL waitForHandle(HANDLE handle);

    static void __stdcall statusCallback(HINTERNET hInternet,
         DWORD_PTR dwContext,
         DWORD dwInternetStatus,
         LPVOID lpvStatusInformation,
         DWORD dwStatusInformationLength);

    void responseReceived() { ::SetEvent(m_responseReceived); }
    void receivingResponse() { ::ResetEvent(m_responseReceived); }
    void requestComplete() { ::SetEvent(m_requestComplete); }

    typedef void (InternetDownload::*writeData_t)(BYTE* buffer, DWORD bufferLength, void* context);

    DOWNLOAD_STATUS getData(writeData_t writeData, void* context);

    std::function<void(int)> m_progressFunction;

    tstring m_url;
    tstring m_userAgent;
    tstring m_contentType;

    HINTERNET m_hInternet;
    HINTERNET m_hConnect;
    HINTERNET m_hHttp;
    HANDLE m_responseReceived;
    HANDLE m_requestComplete;
    CancelToken m_cancelToken;
    HWND m_parentHwnd;

    // TODO: this is pretty horrible
    DWORD m_receivedBytes;

    DWORD m_error;
    DWORD m_flags;
    
};