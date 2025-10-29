#include "testUtils.h"

TEST(MarkerThreads, SmallArray) {
    EXPECT_NO_THROW(RunMarkerThreadsTest(10, 3));
}

TEST(MarkerThreads, LargerArray) {
    EXPECT_NO_THROW(RunMarkerThreadsTest(100, 50));
}

