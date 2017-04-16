
#include "precompiled_headers.h"
#include "InternetDownload.h"
#include "libinstall/CancelToken.h"

InternetDownload::InternetDownload(HWND parentHwnd, const tstring& userAgent, const tstring& url, CancelToken cancelToken, std::function<void(int)> progressFunction /* = NULL */)
    : m_progressFunction(progressFunction),
      m_url(url),
      m_hInternet(NULL),
      m_hConnect(NULL),
      m_hHttp(NULL),
      m_cancelToken(cancelToken),
      m_parentHwnd(parentHwnd),
      m_receivedBytes(0),
      m_error(0),
      m_flags(0)
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

    if (m_responseReceived) {
        ::CloseHandle(m_responseReceived);
        m_responseReceived = NULL;
    }

    if (m_requestComplete) {
        ::CloseHandle(m_requestComplete);
        m_requestComplete = NULL;
    }
}

void InternetDownload::statusCallback( HINTERNET /* hInternet */,
     DWORD_PTR dwContext,
     DWORD dwInternetStatus,
     LPVOID lpvStatusInformation,
     DWORD /* dwStatusInformationLength */)
{
    InternetDownload *download = reinterpret_cast<InternetDownload*>(dwContext);
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
void InternetDownload::disableCache() {
    m_flags = INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RESYNCHRONIZE;
}

BOOL InternetDownload::request() {

    if (!m_hInternet) {
        return FALSE;
    }

    m_hHttp = InternetOpenUrl(m_hInternet, m_url.c_str(), NULL, 0, m_flags, reinterpret_cast<DWORD_PTR>(this));
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

DOWNLOAD_STATUS InternetDownload::getData(writeData_t writeData, void *context)
{

    if (m_error) {
        return DOWNLOAD_STATUS_FAIL;
    }

    if (m_cancelToken.isSignalled()) {
        return DOWNLOAD_STATUS_FAIL;
    }

    OutputDebugString(_T("Beginning getData - waiting for request completion\n"));

    if (!waitForHandle(m_responseReceived)) {
        return DOWNLOAD_STATUS_FAIL;
    }

    TCHAR headerBuffer[1024] = { 0 };
    DWORD headerIndex = 0;
    DWORD bufferLength = 1024;

    DWORD statusCode = 0;
    BOOL statusCodeSuccess = HttpQueryInfo(m_hHttp, HTTP_QUERY_STATUS_CODE, headerBuffer, &bufferLength, &headerIndex);
    if (statusCodeSuccess) {
        statusCode = _ttol(headerBuffer);
    }

    if (HTTP_STATUS_PROXY_AUTH_REQ == statusCode) {
        LPVOID errorBuffer = 0;
        DWORD dwFlags = FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS;
        DWORD errorResult = InternetErrorDlg(m_parentHwnd, m_hHttp, ERROR_INTERNET_INCORRECT_PASSWORD, dwFlags, &errorBuffer);
        if (errorResult == ERROR_INTERNET_FORCE_RETRY) {
            return DOWNLOAD_STATUS_FORCE_RETRY;
        }

    }

    bufferLength = 1024;
    headerIndex = 0;
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
    if (m_progressFunction != nullptr) {
        m_progressFunction(5);
    }


    DWORD bytesToRead = 16384;
    BYTE buffer[16384]; // InternetReadFile seems to give back 8k buffers, so an 8k buffer is optimal
    DWORD bytesRead = 0;
    do {
        receivingResponse();
        int readFileResponse = InternetReadFile(m_hHttp, buffer, bytesToRead, &bytesRead);
        if (!readFileResponse && ERROR_IO_PENDING == GetLastError()) {
            if (!waitForHandle(m_responseReceived)) {
                return DOWNLOAD_STATUS_CANCELLED;
            }
        } else if (!readFileResponse) {
            return DOWNLOAD_STATUS_FAIL;
        }

        // Check the cancellation token, because otherwise we only check it if we get an ASYNC result from InternetReadFile
        // That tends to happen on slow(ish) connections, but isn't guaranteed. (From what I've seen)
        if (m_cancelToken.isSignalled()) {
            return DOWNLOAD_STATUS_CANCELLED;
        }

        (*this.*writeData)(buffer, bytesRead, context);
        bytesWritten += bytesRead;
        if (contentLength && m_progressFunction != nullptr) {
            int percent = static_cast<int>((static_cast<double>(bytesWritten) / static_cast<double>(contentLength)) * 95 + 5);
            m_progressFunction(percent);
        }

    } while (bytesRead != 0);

    return DOWNLOAD_STATUS_SUCCESS;

}

BOOL InternetDownload::saveToFile(const tstring& filename) {
    if (request()) {
        FILE *fp = NULL;
        if (_tfopen_s(&fp, filename.c_str(), _T("wb")) == 0) {
            DOWNLOAD_STATUS status = getData(&InternetDownload::writeToFile, fp);
            if (fp)
            {
                fclose(fp);
            }
            if (status == DOWNLOAD_STATUS_FORCE_RETRY) {
                // This is where we should have used a second class for the actual request.
                ::InternetCloseHandle(m_hHttp);
                m_hHttp = NULL;
                return saveToFile(filename);
            }
            return DOWNLOAD_STATUS_SUCCESS == status;
        }
    }

    return FALSE;
}

std::string InternetDownload::getContent() {
    if (request()) {
        std::string result;
        DOWNLOAD_STATUS status = getData(&InternetDownload::writeToString, &result);

        if (DOWNLOAD_STATUS_FORCE_RETRY == status) {
            ::InternetCloseHandle(m_hHttp);
            m_hHttp = NULL;
            return getContent();
        }

        if (DOWNLOAD_STATUS_SUCCESS == status) {
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
