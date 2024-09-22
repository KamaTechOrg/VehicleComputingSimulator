
#include <QtTest/QtTest>
#include "frames.h"
#include "log_handler.h"
// Mocking necessary components
class MockLogHandler : public LogHandler {
    // Implement mock methods as needed
};

// Test class for Frames
class TestFrames : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void testUniqueColors();
    void testInitialFramesMat();
    void testUpdateFrames();
    void testInvalidIndexHandling();

private:
    Frames *frames;
    MockLogHandler logHandler;
};

void TestFrames::init()
{
    frames = new Frames(logHandler);
}

void TestFrames::cleanup()
{
    delete frames;
}
void TestFrames::testInvalidIndexHandling()
{
    frames->initialFramesMat(2);
    frames->fillFramesMat();
    
    // Add a log entry with invalid IDs
    LogHandler::LogEntry logEntry;
    logEntry.srcId = -1; // Invalid ID
    logEntry.dstId = 10; // Invalid ID
    logEntry.timestamp = QDateTime::currentDateTime();

    frames->setActiveLogEntries({{logEntry.timestamp, logEntry}});
    frames->updateFrames();

    // No specific checks; just ensure no crash or assertion failure
}

void TestFrames::testUniqueColors()
{
    frames->fillFramesMat();
    QSet<QString> colors;
    for (const auto &row : frames->getFramesMat()) {
        for (const auto &frame : row) {
            QVERIFY(!colors.contains(frame.color));
            colors.insert(frame.color);
        }
    }
}


void TestFrames::testInitialFramesMat()
{
    frames->initialFramesMat(5);
    QCOMPARE(frames->getFramesMat().size(), 5); // לבדוק את גודל framesMat
}

void TestFrames::testUpdateFrames()
{
    frames->updateFrames();
}

QTEST_MAIN(TestFrames)
#include "test_frames.moc"