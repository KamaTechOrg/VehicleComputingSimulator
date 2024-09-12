
#include <QtTest/QtTest>
#include "../include/frames.h"
#include "../include/log_handler.h"
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
    void testFillFramesMat();
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


void TestFrames::testFillFramesMat()
{
    frames->initialFramesMat(5);
    frames->fillFramesMat();

    // Ensure that each frame in framesMat has a unique color
    QSet<QString> usedColors;
    bool allColorsUnique = true;
    for (const auto &row : frames->getFramesMat()) {
        for (const auto &frame : row) {
            if (usedColors.contains(frame.color)) {
                allColorsUnique = false;
                break;
            }
            usedColors.insert(frame.color);
        }
        if (!allColorsUnique) {
            break;
        }
    }
    QVERIFY(allColorsUnique);
    
    // Ensure each color is a valid hexadecimal color code
    for (const QString &color : usedColors) {
        QVERIFY(color.startsWith("#") && color.size() == 7);
    }
}


void TestFrames::testUpdateFrames()
{
    frames->updateFrames();
    // הוסף בדיקות לוודא שה-framesMat ו-activeLogEntries עודכנים כראוי
}

QTEST_MAIN(TestFrames)
#include "test_frames.moc"
