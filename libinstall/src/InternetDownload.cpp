
#include "precompiled_headers.h"
#include "InternetDownload.h"
#include "libinstall/CancelToken.h"

InternetDownload::InternetDownload(const tstring& userAgent, const tstring& url, CancelToken cancelToken, boost::function<void(int)> progressFunction /* = NULL */) 
    : m_url(url),
      m_hInternet(NULL),
      m_hConnect(NULL),
      m_hHttp(NULL),
      m_error(0),
      m_cancelToken(cancelToken),
      m_progressFunction(progressFunction)
{
    m_requestComplete = ::CreateEvent(NULL, TRUE /*manualReset*/, FALSE /*initialState*/, NULL /*name*/);
    m_responseReceived = ::CreateEvent(NULL, TRUE /*manualReset*/, FALSE /*initialState*/, NULL /*name*/);

    m_hInternet = ::InternetOpen(userAgent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL /* proxy*/ , NULL /* proxy bypass */, INTERNET_FLAG_ASYNC /* dwflags */);
    
    INTERNET_STATUS_CALLBACK ourCallback = &InternetDownload::statusCallback;
    ::InternetSetStatusCallback(m_hInternet, ourCallback);
    DWORD timeout = 120000;
    ::InternetSetOption(m_hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(DWORD));
    ::InternetSetOption(m_hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(DWORD));
}

InternetDownload::~InternetDownload() 
{
    if (m_hHttp) {
        ::InternetCloseHandle(m_hHttp);
        m_hHttp = NULL;
    }

    if (m_hConnect) {
        ::InternetCloseHandle(m_hConnect);
        m_hConnect = NULL;
    }

    if (m_hInternet) {
        ::InternetCloseHandle(m_hInternet);
        m_hInternet = NULL;
    }
}

void InternetDownload::statusCallback( HINTERNET hInternet,
     DWORD_PTR dwContext,
     DWORD dwInternetStatus,
     LPVOID lpvStatusInformation,
     DWORD dwStatusInformationLength )
{
    InternetDownload *download = (InternetDownload*)dwContext;
    switch(dwInternetStatus) {
    case INTERNET_STATUS_RECEIVING_RESPONSE:
            download->receivingResponse();
            break;

    case INTERNET_STATUS_REQUEST_COMPLETE:
            download->responseReceived();
            break;
    case INTERNET_STATUS_HANDLE_CREATED:
            download->m_hHttp = (*(HANDLE*)lpvStatusInformation);
            break;

    default: 
        break;

    }


}

BOOL InternetDownload::request() {

    if (!m_hInternet) {
        return FALSE;
    }
    
    m_hHttp = InternetOpenUrl(m_hInternet, m_url.c_str(), NULL, 0, 0, reinterpret_cast<DWORD_PTR>(this));
    return TRUE;
}

BOOL InternetDownload::waitForHandle(HANDLE handle)
{
    HANDLE waitHandles[2];
    waitHandles[0] = m_cancelToken.getToken();
    waitHandles[1] = handle;
    int waitResponse = WaitForMultipleObjects(2, waitHandles, FALSE, 60000);

    switch(waitResponse) {
        case WAIT_OBJECT_0:
            {
                // cancelled
                return FALSE;
            }
            
        case WAIT_TIMEOUT:
            {
                // More than 60seconds for response
                return FALSE;
            }

        case WAIT_FAILED:
            {
                return FALSE;
            }
    }

    return TRUE;
}

BOOL InternetDownload::getData(writeData_t writeData, void *context) 
{

    if (m_error) {
        return FALSE;
    }

    if (m_cancelToken.isSignalled()) {
        return FALSE;
    }

    OutputDebugString(_T("Beginning getData - waiting for request completion\n"));
    DWORD bytesAvailable;

    if (!waitForHandle(m_responseReceived)) {
        return FALSE;
    }

    TCHAR headerBuffer[1024];
    DWORD headerIndex = 0;
    DWORD bufferLength = 1024;
    long contentLength = 0;
    BOOL contentLengthSuccess = HttpQueryInfo(m_hHttp, HTTP_QUERY_CONTENT_LENGTH, headerBuffer, &bufferLength, &headerIndex);
    if (contentLengthSuccess) {
        contentLength = _ttol(headerBuffer);
    }

    bufferLength = 1024;
    headerIndex = 0;
    BOOL contentTypeSuccess = HttpQueryInfo(m_hHttp, HTTP_QUERY_CONTENT_TYPE, headerBuffer, &bufferLength, &headerIndex);
    if (contentTypeSuccess) {
        m_contentType = headerBuffer;
    }

    long bytesWritten = 0;

    // Make point-at-which-we-receive-the-headers 5% of the total progress (arbitrarily chosen!)
    if (!m_progressFunction.empty()) {
        m_progressFunction(5);
    }


    DWORD bytesToRead = 16384;
    BYTE buffer[16384]; // InternetReadFile seems to give back 8k buffers, so an 8k buffer is optimal
    DWORD *bytesRead = new DWORD();
    do {
        receivingResponse();
        int readFileResponse = InternetReadFile(m_hHttp, buffer, bytesToRead, bytesRead);
        if (!readFileResponse && ERROR_IO_PENDING == GetLastError()) {
            if (!waitForHandle(m_responseReceived)) {
                return FALSE;
            }
        } else if (!readFileResponse) {
            return FALSE;
        }

        // Check the cancellation token, because otherwise we only check it if we get an ASYNC result from InternetReadFile
        // That tends to happen on slow(ish) connections, but isn't guaranteed. (From what I've seen)
        if (m_cancelToken.isSignalled()) {
            return FALSE;
        }

        (*this.*writeData)(buffer, *bytesRead, context);
        bytesWritten += *bytesRead;
        if (contentLength && !m_progressFunction.empty()) {
            int percent = static_cast<int>((static_cast<double>(bytesWritten) / static_cast<double>(contentLength)) * 95 + 5);
            m_progressFunction(percent);
        }

    } while (*bytesRead != 0);

    delete bytesRead;
    return TRUE;

}

BOOL InternetDownload::saveToFile(const tstring& filename) {
    if (request()) {
        FILE *fp = _tfopen(filename.c_str(), _T("wb"));
        BOOL success = getData(&InternetDownload::writeToFile, fp);
        fclose(fp);
        return success;
    }

    return FALSE;
}

std::string InternetDownload::getContent() {
    if (request()) {
        std::string result;
        BOOL success = getData(&InternetDownload::writeToString, &result);
    
        if (success) {
            return result;
        }
    }

    return "";
}

void InternetDownload::writeToFile(BYTE* buffer, DWORD bufferLength, void *context) {
    fwrite(buffer, bufferLength, 1, reinterpret_cast<FILE*>(context));
}

void InternetDownload::writeToString(BYTE* buffer, DWORD bufferLength, void* context) {
    reinterpret_cast<std::string*>(context)->append(reinterpret_cast<char*>(buffer), static_cast<size_t>(bufferLength));
}
