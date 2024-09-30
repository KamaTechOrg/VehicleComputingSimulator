#include "../include/general.h"

void log(logger::LogLevel level, const std::string &message) {
    static logger logInstance("HSM");
    logInstance.logMessage(level, message);
}
