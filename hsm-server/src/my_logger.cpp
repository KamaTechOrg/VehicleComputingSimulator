#include "../include/my_logger.h"
// מגדיר נתיב מוחלט לקובץ הלוג
const std::string logFilePath =
    "../../HSM_Communication.log";  // יש לשנות לנתיב המתאים

// מוודא שהקובץ נפתח פעם אחת בלבד
std::ofstream logFile(logFilePath, std::ios::app);

// יוצר mutex עבור סינכרון
std::mutex logMutex;

std::string getTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                      now.time_since_epoch())
                      .count();
    return std::to_string(now_ns);
}

std::string dataToHex(const unsigned char *data, size_t size)
{
    std::ostringstream oss;
    oss << "0x";
    for (size_t i = 0; i < size; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(data[i]);
    }
    return oss.str();
}

void log(logger::LogLevel loglevel, const std::string &hsm_id,
         const std::string &user_id, const std::string &message)
{
    std::cout << "Im here in log" << std::endl;
    std::string levelStr;
    switch (loglevel) {
        case logger::LogLevel::INFO:
            levelStr = "INFO";
            break;
        case logger::LogLevel::ERROR:
            levelStr = "ERROR";
            break;
    }

    std::string logMessage = "[" + getTimestamp() + "ns] [" + levelStr +
                             "] SRC " + hsm_id + " DST " + user_id + " " +
                             message;

    // מבצע סינכרון לגישה לקובץ הלוג
    std::lock_guard<std::mutex> guard(logMutex);
    logFile << logMessage << std::endl;
}

int getId()
{
    static int counter = 0;
    return ++counter;
}