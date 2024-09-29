#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include "draggable_square.h"
#include <QDateTime>
#include <QDockWidget>
#include <QJsonObject>
#include <QMainWindow>
#include <QString>
#include <QTime>
#include <QVector>

class LogHandler {
    friend class TestLogHandler;
public:
    struct LogEntry {
        QDateTime timestamp;
        int srcId;
        int dstId;

        bool operator<(const LogEntry &other) const
        {
            return timestamp < other.timestamp;
        }
    };

    void readLogFile(const QString &fileName);
    void sortLogEntries();
    void analyzeLogEntries(QMainWindow *mainWindow, const QString &jsonFileName,
                           bool realTime = false);
    QVector<LogHandler::LogEntry> getLogEntries();
    const QMap<int, DraggableSquare *> &getProcessSquares() const;

private:
    QVector<LogEntry> logEntries;
    QMap<int, DraggableSquare *>
        processSquares;
    QMap<int, QMap<int, int>>
        communicationCounts;
    LogEntry parseLogLine(const QString &line);
};

#endif  // LOGHANDLER_H