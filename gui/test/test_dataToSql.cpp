#include <QtTest/QtTest>
#include <QFile>
#include <QTemporaryFile>
#include <QVariantMap>
#include "../include/dataToSql.h"

class dataToSqlTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();  // Setup before any test runs
    void cleanupTestCase();  // Cleanup after all tests run
    void testReadLogFile();
    void testReadBsonFile();
    void testInsertDataToDatabase();
    void testGetAllDataSimulation();
    void testGetRecordById();
    void testInsertDataDetails() ;
    void testGetAllDataDetails() ;
    void testGetDataDetailsBySimulationAndTime() ;

};

void dataToSqlTest::initTestCase() {
    // Any initial setup if needed
    qDebug() << "Starting dataToSql tests...";
}

void dataToSqlTest::cleanupTestCase() {
    // Cleanup operations after all tests
    qDebug() << "Cleaning up...";
}

void dataToSqlTest::testReadLogFile() {
    // Create a temporary log file
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());  // Check if file opened successfully
    QTextStream out(&tempFile);
    out << "This is a log file.\n";
    tempFile.close();

    dataToSql dataHandler;
    QString logData = dataHandler.readLogFile(tempFile.fileName());

    QCOMPARE(logData, QString("This is a log file.\n"));  // Check content
}

void dataToSqlTest::testReadBsonFile() {
    // Create a temporary BSON file with some binary data
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QByteArray testData("Test BSON data");
    tempFile.write(testData);
    tempFile.close();

    dataToSql dataHandler;
    QByteArray bsonData = dataHandler.readBsonFile(tempFile.fileName());

    QCOMPARE(bsonData, testData);  // Check if BSON data matches
}

void dataToSqlTest::testInsertDataToDatabase() {
    dataToSql dataHandler;
    QByteArray bsonData("Test BSON data");
    QString logData("Test log data");

    bool result = dataHandler.insertDataToDatabase("TestInput", bsonData, logData);
    QVERIFY(result);  // Ensure the insertion was successful
}

void dataToSqlTest::testGetAllDataSimulation() {
    dataToSql dataHandler;
    QList<QVariantMap> dataList = dataHandler.getAllDataSimulation();
    QVERIFY(!dataList.isEmpty());  // Check that data was retrieved
}

void dataToSqlTest::testGetRecordById() {
    dataToSql dataHandler;
    QVariantMap record = dataHandler.getRecordById(1);  // Assume there's at least one record
    QVERIFY(!record.isEmpty());  // Check if the record is valid
    QCOMPARE(record["id"].toInt(), 1);  // Verify the ID
}
////
void dataToSqlTest::testInsertDataDetails() {
    dataToSql dbHandler;
    // Assuming that dbHandler is properly initialized and connected to the SQLite database

    bool result = dbHandler.insertDataDetails(1, "2024-09-15 12:00:00", 100, 101, "Test Message", "sent", "success");
    QVERIFY(result); // Ensure the insertion was successful

    QList<QVariantMap> details = dbHandler.getAllDataDetails();
    QVERIFY(!details.isEmpty()); // Ensure data was inserted

    QVariantMap lastRecord = details.last();
    QCOMPARE(lastRecord["data_simulation_id"].toInt(), 1);
    QCOMPARE(lastRecord["timesTamp"].toString(), "2024-09-15 12:00:00");
    QCOMPARE(lastRecord["idSrc"].toInt(), 100);
    QCOMPARE(lastRecord["idDst"].toInt(), 101);
    QCOMPARE(lastRecord["message"].toString(), "Test Message");
    QCOMPARE(lastRecord["send_recieved"].toString(), "sent");
    QCOMPARE(lastRecord["success_failed"].toString(), "success");
}
void dataToSqlTest::testGetAllDataDetails() {
    dataToSql dbHandler;
    // Assuming dbHandler is initialized and has data
    QList<QVariantMap> details = dbHandler.getAllDataDetails();
    QVERIFY(!details.isEmpty()); // Ensure there is data in the database

    // Check some known data
    QVariantMap firstRecord = details.first();
    QCOMPARE(firstRecord["data_simulation_id"].toInt(), 1); // Modify as needed
    QCOMPARE(firstRecord["timesTamp"].toString(), "2024-09-15 12:00:00"); // Example timestamp
}
void dataToSqlTest::testGetDataDetailsBySimulationAndTime() {
    dataToSql dbHandler;
    // Insert test data first, ensuring there's data to fetch
    dbHandler.insertDataDetails(1, "2024-09-15 12:00:00", 100, 101, "Test Message", "sent", "success");

    // Test fetching the inserted data
    QList<QVariantMap> details = dbHandler.getDataDetailsBySimulationAndTime(1, "2024-09-15 12:00:00");
    QVERIFY(!details.isEmpty()); // Ensure the query returns data

    QVariantMap record = details.first();
    QCOMPARE(record["data_simulation_id"].toInt(), 1);
    QCOMPARE(record["timesTamp"].toString(), "2024-09-15 12:00:00");
    QCOMPARE(record["idSrc"].toInt(), 100);
    QCOMPARE(record["idDst"].toInt(), 101);
    QCOMPARE(record["message"].toString(), "Test Message");
    QCOMPARE(record["send_recieved"].toString(), "sent");
    QCOMPARE(record["success_failed"].toString(), "success");
}

QTEST_MAIN(dataToSqlTest)
#include "test_dataToSql.moc"
