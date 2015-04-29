//
// Created by Cyril Leroux on 23/04/2015.
//

#include "gtest/gtest.h"
#include "../src/display_manager.h"

TEST(DisplayManager, CountMonitors )
{
    ASSERT_TRUE(DisplayManager::MonitorCount() > 0) << "There should be at least one monitor";
}


int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}