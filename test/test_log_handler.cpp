#include <QtTest/QtTest>
#include "../src/log_handler.h"

class LogHandlerTests : public QObject {
    Q_OBJECT

private slots:
    void testReadLogFile();
    void testSortLogEntries();
    void testFindProcessCoordinatesById();
};

void LogHandlerTests::testReadLogFile() {
    LogHandler logHandler;
    logHandler.readLogFile("../log_file.log");

    QVERIFY(!logHandler.getLogEntries().isEmpty());
}

void LogHandlerTests::testSortLogEntries() {
    LogHandler logHandler;
    logHandler.readLogFile("../log_file.log");

    logHandler.sortLogEntries();
    QVector<LogHandler::LogEntry> logEntries = logHandler.getLogEntries();

    for (int i = 1; i < logEntries.size(); ++i) {
        QVERIFY(logEntries[i - 1] < logEntries[i]);
    }
}

void LogHandlerTests::testFindProcessCoordinatesById() {
    LogHandler logHandler;
    QVector<int> coordinates = logHandler.findProcessCoordinatesById(1, "some_json_file.json");

    QVERIFY(!coordinates.isEmpty());
    QCOMPARE(coordinates.size(), 2);
}

QTEST_MAIN(LogHandlerTests)
#include "test_log_handler.moc"
