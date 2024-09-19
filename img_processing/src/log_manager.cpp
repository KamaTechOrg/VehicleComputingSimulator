#include "log_manager.h"
using namespace std;

logger LogManager::imgLogger("img_processing");

template <typename T>
string LogManager::enumTypeToString(T enumType)
{
    if constexpr (is_same_v<T, ErrorType>) {
        switch (enumType) {
            case ErrorType::IMAGE_ERROR:
                return "IMAGE_ERROR";
            case ErrorType::VIDEO_ERROR:
                return "VIDEO_ERROR";
            case ErrorType::FILE_ERROR:
                return "FILE_ERROR";
            case ErrorType::DETECTION_ERROR:
                return "DETECTION_ERROR";
            case ErrorType::TRACKING_ERROR:
                return "TRACKING_ERROR";
            case ErrorType::MODEL_ERROR:
                return "MODEL_ERROR";
            default:
                return "UNKNOWN_ERROR";
        }
    }
    else if constexpr (is_same_v<T, InfoType>) {
        switch (enumType) {
            case InfoType::MEDIA_FINISH:
                return "MEDIA_FINISH";
            case InfoType::ALERT_SENT:
                return "ALERT_SENT";
            case InfoType::CHANGED:
                return "CHANGED";
            case InfoType::UNION:
                return "UNION";
            case InfoType::NUM_OF_DIFFERENCE:
                return "NUM_OF_DIFFERENCE";
            case InfoType::EXECUTION_TIME:
                return "EXECUTION_TIME";
            case InfoType::IOU:
                return "IOU";
            case InfoType::DETECTION:
                return "DETECTION";
            case InfoType::TRACKING:
                return "TRACKING";
            case InfoType::DISTANCE:
                return "DISTANCE";
            case InfoType::MODE:
                return "MODE";
            default:
                return "UNKNOWN_TYPE";
        }
    }
    else if constexpr (is_same_v<T, DebugType>) {
        switch (enumType) {
            case DebugType::PRINT:
                return "PRINT";
        }
        return "UNKNOWN";
    }
    else {
        return "UNKNOWN";
    }
}

template <typename T>
void LogManager::logErrorMessage(T errorType, const string &details)
{
    string messageText = enumTypeToString(errorType);
    if (!details.empty()) {
        messageText += ": " + details;
    }
    LogManager::imgLogger.logMessage(logger::LogLevel::ERROR, messageText);
}

template <typename T>
void LogManager::logInfoMessage(T infoType, const string &details)
{
    string messageText = enumTypeToString(infoType);
    if (!details.empty()) {
        messageText += ": " + details;
    }
    LogManager::imgLogger.logMessage(logger::LogLevel::INFO, messageText);
}

template <typename T>
void LogManager::logDebugMessage(T debugType, const string &details)
{
    string messageText = enumTypeToString(debugType);
    if (!details.empty()) {
        messageText += ": " + details;
    }
    LogManager::imgLogger.logMessage(logger::LogLevel::DEBUG, messageText);
}

// Explicit instantiation for ErrorType
template void LogManager::logErrorMessage<ErrorType>(ErrorType errorType, const string &details);

// Explicit instantiation for InfoType
template void LogManager::logInfoMessage<InfoType>(InfoType infoType, const string &details);

// Explicit instantiation for DebugType
template void LogManager::logDebugMessage<DebugType>(DebugType debugType, const string &details);


