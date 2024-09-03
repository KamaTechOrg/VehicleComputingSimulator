#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "logger.h"

using ::testing::Return;

class MockLogger : public logger {
public:
    MOCK_METHOD(void, logMessage, (LogLevel level, const std::string &message), (override));
};

class LoggerTest : public ::testing::Test {
protected:
    LoggerTest() {
        logger::initializeLogFile();
    }

    void SetUp() override {
        std::remove("test.log"); // Ensure the file does not exist before the test
    }

    void TearDown() override {
        std::remove("test.log"); // Clean up after the test
    }
};

TEST_F(LoggerTest, InitializeLogFile) {
    std::ifstream logFile(logger::getLogFileName());
    EXPECT_TRUE(logFile.good()) << "Log file was not created: " << logger::getLogFileName();
    logFile.close();
}

TEST_F(LoggerTest, LogMessage) {
    //logger::logMessage(logger::LogLevel::INFO, "Test INFO message");
    //logger::logMessage(logger::LogLevel::ERROR, "Test ERROR message");

    std::ifstream logFile(logger::getLogFileName());
    std::string content((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    EXPECT_NE(content.find("Test INFO message"), std::string::npos);
    EXPECT_NE(content.find("Test ERROR message"), std::string::npos);
}

TEST_F(LoggerTest, LogLevelFiltering) {
    //logger::logMessage(logger::LogLevel::ERROR, "Test ERROR message");
    //logger::logMessage(logger::LogLevel::INFO, "Test INFO message");

    std::ifstream logFile(logger::getLogFileName());
    std::string content((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    EXPECT_NE(content.find("Test ERROR message"), std::string::npos);
    EXPECT_EQ(content.find("Test INFO message"), std::string::npos) << "INFO message should not be logged with ERROR level";
}
