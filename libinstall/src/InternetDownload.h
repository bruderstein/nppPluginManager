#pragma once

class InternetDownload {
public:
    InternetDownload(const tstring& userAgent, const tstring& url);
        
    ~InternetDownload();

    int saveToFile(const tstring& filename);

    tstring getContent();
    

private:

    BOOL request();
    void writeToFile(BYTE* buffer, DWORD bufferLength, void* context);
    void writeToString(BYTE* buffer, DWORD bufferLength, void* context);

    typedef void (InternetDownload::*writeData_t)(BYTE* buffer, DWORD bufferLength, void* context);

    BOOL getData(writeData_t writeData, void* context);

    tstring m_url;
    tstring m_userAgent;

    HINTERNET m_hInternet;
    HINTERNET m_hConnect;
    HINTERNET m_hHttp;

    DWORD m_error;
    
};