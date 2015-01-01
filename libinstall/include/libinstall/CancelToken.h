#pragma once

class CancelToken {
public:
    CancelToken();
    CancelToken(const CancelToken& copy);
    CancelToken& operator=(CancelToken& other);

    ~CancelToken();


    void triggerCancel();
    HANDLE getToken() const { return m_token; }

    BOOL isSignalled() const;

    int getRefCount() const { return *m_refCount; }

private:
    int *m_refCount;
    HANDLE m_token;

};