#include <QtTest/QtTest>
#include "../src/log_handler.h"
#include "../src/simulation_data_manager.h"
#include "../src/draggable_square.h"
#include "../src/process.h"
#include <QCoreApplication>
#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>

class LogHandlerTests : public QObject {
    Q_OBJECT
    SimulationDataManager manager;

private slots:
    void testReadLogFile();
    void testSortLogEntries();
    void testFindProcessCoordinatesById();
    void testGetProcessSquares();
    void testBsonToJsonObject();
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

void LogHandlerTests::testFindProcessCoordinatesById()
{
    LogHandler logHandler;
    
     // Create some sample processes
    Process process1(1, "Process1", "CMakeProject1", "QEMUPlatform1");
    Process process2(2, "Process2", "CMakeProject2", "QEMUPlatform2");

    // Create some draggable squares with the processes
    DraggableSquare square1;
    square1.setProcess(&process1);
    square1.setDragStartPosition(QPoint(10, 20));
    
    DraggableSquare square2;
    square2.setProcess(&process2);
    square2.setDragStartPosition(QPoint(30, 40));

    QVector<DraggableSquare *> squares;
    squares.append(&square1);
    squares.append(&square2);

    QString img = "path/to/image.png";

    // Create a SimulationDataManager instance
    SimulationDataManager dataManager;

    // Save the simulation data
    QString fileName = "simulation_data.bson";
    dataManager.saveSimulationData(fileName.toStdString(), squares, img);

    QVector<int> result1 = logHandler.findProcessCoordinatesById(1, fileName);
    QVector<int> v1 = {10, 20};
    if (result1.size() >= 2 && v1.size() >= 2) {
    QCOMPARE(result1[0], v1[0]);
    QCOMPARE(result1[1], v1[1]);
    } else {
         QFAIL("Vectors do not contain enough elements for comparison");
    }
    QVector<int> result2 = logHandler.findProcessCoordinatesById(2, fileName);
    QVector<int> v2 = {30, 40};
    QCOMPARE(result2, v2);
    // Check coordinate lookup for non-existent ID
    QVector<int> resultNotFound = logHandler.findProcessCoordinatesById(3, fileName);
    QVector<int> v3 = {-1, -1};
    QCOMPARE(resultNotFound, v3);
}

void LogHandlerTests::testGetProcessSquares() {
    LogHandler logHandler;

    // Create objects to test
    Process process1(1, "Process1", "CMakeProject1", "QEMUPlatform1");
    DraggableSquare square1;
    square1.setProcess(&process1);
    // Let's say you want to check that the DraggableSquare has been added to the QMap.
    // Add the DraggableSquare to the map
    QMap<int, DraggableSquare *> &processSquares = const_cast<QMap<int, DraggableSquare *> &>(logHandler.getProcessSquares());
    processSquares.insert(1, &square1);

   // Now the contents of the map can be checked
    const QMap<int, DraggableSquare *> &squares = logHandler.getProcessSquares();
    QVERIFY(squares.contains(1));
    QCOMPARE(squares[1]->getProcess()->getId(), 1);
}

void LogHandlerTests::testBsonToJsonObject() {
    LogHandler logHandler;

    // Create a dummy BSON
    bson_t bson;
    bson_init(&bson);
    BSON_APPEND_INT32(&bson, "test", 123);

    // Get a JSON object from BSON
    QJsonObject jsonObject = logHandler.bsonToJsonObject(&bson);
    QVERIFY(jsonObject.contains("test"));
    QCOMPARE(jsonObject.value("test").toInt(), 123);

    bson_destroy(&bson);
}

QTEST_MAIN(LogHandlerTests)
#include "test_log_handler.moc"
