#include <QtTest/QtTest>
#include "log_handler.h"
#include "draggable_square.h"

class TestLogHandler : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testReadLogFile();
    void testParseLogLine();
    void testSortLogEntries();

private:
    LogHandler logHandler;
    QString testDataDir;
};

void TestLogHandler::initTestCase()
{
    // Setup code: Create a temporary directory and test files
    testDataDir = QDir::temp().filePath(
        "loghandler_test_" +
        QString::number(QDateTime::currentMSecsSinceEpoch()));
    QDir().mkpath(testDataDir);

    // Create a sample log file
    QFile logFile(testDataDir + "/test_log.txt");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << "[1623456789000000000ns] [INFO] SRC 1 DST 2 Packet number: 1 "
               "messageId: 1 Data: 0xf1010 Success\n";
        out << "[1623456789100000000ns] [INFO] SRC 2 DST 1 Packet number: 2 "
               "messageId: 2 Data: 0x010ab received\n";
        out << "\n";  // Adding an empty line to simulate real-world logs
        logFile.close();
    }

    // Create a sample JSON file
    QFile jsonFile(testDataDir + "/test_state.json");
    if (jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&jsonFile);
        out << R"({"squares":[{"id":1,"name":"Process1","CMakeProject":"Project1","QEMUPlatform":"Platform1","position":{"x":10,"y":20},"width":50,"height":50}]})";
        jsonFile.close();
    }
}

void TestLogHandler::cleanupTestCase()
{
    // Clean up: Remove the temporary directory and its contents
    QDir(testDataDir).removeRecursively();
}

void TestLogHandler::testReadLogFile()
{
    logHandler.readLogFile(testDataDir + "/test_log.txt");
    QVERIFY(!logHandler.getLogEntries().isEmpty());
}

void TestLogHandler::testParseLogLine()
{
    QString line =
        "[1623456789000000000ns] [INFO] SRC 1 DST 2 Packet number: 1 "
        "messageId: 1 Data: 0xf105485 Success";
    LogHandler::LogEntry entry = logHandler.parseLogLine(line);
    QCOMPARE(entry.srcId, 1);
    QCOMPARE(entry.dstId, 2);
}

void TestLogHandler::testSortLogEntries()
{
    LogHandler logHandler;
    logHandler.readLogFile("../log_file.log");

    logHandler.sortLogEntries();
    QVector<LogHandler::LogEntry> logEntries = logHandler.getLogEntries();

    for (int i = 1; i < logEntries.size(); ++i) {
        QVERIFY(logEntries[i - 1] < logEntries[i]);
    }
}

QTEST_MAIN(TestLogHandler)
#include "test_log_handler.moc"