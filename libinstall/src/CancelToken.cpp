
#include "precompiled_headers.h"
#include "libinstall/CancelToken.h"

CancelToken::CancelToken() 
    : m_refCount(new int(1))
{
    m_token = ::CreateEvent(NULL, TRUE /*manualReset*/, FALSE /*initialState*/, NULL /*name*/); 
}

CancelToken::CancelToken(const CancelToken& copy) 
{
    m_refCount = copy.m_refCount;
    (*m_refCount)++;
    m_token = copy.m_token;
}

CancelToken& CancelToken::operator=(CancelToken& other) 
{
    this->~CancelToken();
    m_refCount = other.m_refCount;
    (*m_refCount)++;
    m_token = other.m_token;

    return *this;
}

CancelToken::~CancelToken() 
{
    (*m_refCount)--;
    if (0 == *m_refCount) {
        ::CloseHandle(m_token);
        delete m_refCount;
        m_refCount = NULL;
    }
}

        
BOOL CancelToken::isSignalled() const
{
    return (WAIT_OBJECT_0 == WaitForSingleObject(m_token, 0));
}

void CancelToken::triggerCancel()
{
    SetEvent(m_token);
}