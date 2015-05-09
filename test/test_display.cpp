//
// Created by Cyril Leroux on 23/04/2015.
//

#include "gtest/gtest.h"
#include "../src/display_manager.h"

TEST(DisplayManager, CountMonitors)
{
    const auto monCount = DisplayManager::MonitorCount();
    std::cout << "Monitor count: " << monCount << std::endl;
    ASSERT_TRUE(monCount > 0) << "There should be at least one monitor";

//    ASSERT_TRUE(DisplayManager::GetProcessList());
    ASSERT_TRUE(DisplayManager::GetWindowList());
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}