#include "precompiled_headers.h"

#include "gtest/gtest.h"
#include "libinstall/CancelToken.h"


class CancelTokenTest : public ::testing::Test {

};

TEST_F(CancelTokenTest, test_token_is_not_signalled_initially) 
{
    CancelToken cancelToken;

    EXPECT_EQ(cancelToken.isSignalled(), FALSE);
}

TEST_F(CancelTokenTest, test_token_signalled_after_trigger)
{
    CancelToken cancelToken;

    cancelToken.triggerCancel();

    EXPECT_EQ(cancelToken.isSignalled(), TRUE);
}

TEST_F(CancelTokenTest, test_copy_constructor)
{
    CancelToken cancelToken;

    CancelToken copyToken(cancelToken);

    EXPECT_EQ(copyToken.isSignalled(), FALSE);
    
    cancelToken.triggerCancel();

    EXPECT_EQ(copyToken.isSignalled(), TRUE);

}

TEST_F(CancelTokenTest, test_initial_refCount)
{
    CancelToken cancelToken;

    EXPECT_EQ(cancelToken.getRefCount(), 1);
}

TEST_F(CancelTokenTest, test_copy_constructor_refCount)
{
    CancelToken cancelToken;
    CancelToken copyToken(cancelToken);

    EXPECT_EQ(cancelToken.getRefCount(), 2);
    EXPECT_EQ(copyToken.getRefCount(), 2);
}

TEST_F(CancelTokenTest, test_assignement_operator_refCount)
{
    CancelToken cancelToken;
    CancelToken copyToken = cancelToken;

    EXPECT_EQ(cancelToken.getRefCount(), 2);
    EXPECT_EQ(copyToken.getRefCount(), 2);
}

TEST_F(CancelTokenTest, test_destructor) 
{
    CancelToken cancelToken;
    {
        CancelToken copyToken(cancelToken);
        EXPECT_EQ(copyToken.getRefCount(), 2);
    }
    EXPECT_EQ(cancelToken.getRefCount(), 1);
}
