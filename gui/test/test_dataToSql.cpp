#include <QtTest/QtTest>
#include <QFile>
#include <QTemporaryFile>
#include <QVariantMap>
#include "../src/dataToSql.h"

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

QTEST_MAIN(dataToSqlTest)
#include "test_dataToSql.moc"
