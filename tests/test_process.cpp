/**
 * @file test_process.cpp
 * @brief Unit tests for process execution
 */

#include "souncdown/process.hpp"
#include <gtest/gtest.h>

using namespace souncdown;

TEST(ProcessTest, ExecutableExists) {
    // Test common executables that should exist
    EXPECT_TRUE(Process::executable_exists("ls"));
    EXPECT_TRUE(Process::executable_exists("echo"));
    
    // Test non-existent executable
    EXPECT_FALSE(Process::executable_exists("this_should_not_exist_12345"));
}

TEST(ProcessTest, FindExecutable) {
    auto ls_path = Process::find_executable("ls");
    ASSERT_TRUE(ls_path.has_value());
    EXPECT_TRUE(ls_path->string().find("ls") != std::string::npos);
    
    auto invalid = Process::find_executable("invalid_executable_name");
    EXPECT_FALSE(invalid.has_value());
}

TEST(ProcessTest, ExecuteSimpleCommand) {
    auto result = Process::execute("echo", {"Hello, World!"});
    
    EXPECT_TRUE(result.success());
    EXPECT_EQ(result.exit_code, 0);
    EXPECT_TRUE(result.stdout_output.find("Hello, World!") != std::string::npos);
}

TEST(ProcessTest, ExecuteCommandWithFailure) {
    auto result = Process::execute("ls", {"/this/path/should/not/exist"});
    
    EXPECT_FALSE(result.success());
    EXPECT_NE(result.exit_code, 0);
}

// Add more tests as needed
