#include "db_manager.h"
#include "main_window.h"

DbManager::DbManager(QObject *parent) : QObject(parent)
{
    if (!QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("database.db");
    }
    else {
        db = QSqlDatabase::database("qt_sql_default_connection");
    }

    if (!db.open()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Cannot open database:");
    }
    else {
        MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                         "i suceeses open database:");
    }
}

QString DbManager::readLogFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Failed to open log file:");
        return QString();
    }
    else {
        MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                         "i sucess open log file");
    }
    QTextStream in(&file);
    QString logData = in.readAll();
    file.close();
    return logData;
}

QByteArray DbManager::readBsonFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Failed to open bson file:");
        return QByteArray();
    }
    else {
        MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                         "i sucess open bson file");
    }
    QByteArray bsonData = file.readAll();
    file.close();
    return bsonData;
}

bool DbManager::insertDataToDatabase(const QString &inputString,
                                     const QByteArray &bsonData,
                                     const QString &logData)
{
    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                     "Available SQL Drivers:");

    if (!db.isOpen()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Database is not open.");
        return false;
    }

    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS data_simulation (id INTEGER "
                    "PRIMARY KEY AUTOINCREMENT, input_string TEXT, datetime "
                    "TEXT, bson_data BLOB, log_data TEXT)")) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Failed to create table:");
        return false;
    }

    query.prepare(
        "INSERT INTO data_simulation (input_string, datetime, bson_data, "
        "log_data) VALUES (:input_string, :datetime, :bson_data, :log_data)");
    query.bindValue(":input_string", inputString);
    query.bindValue(":datetime",
                    QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":bson_data", bsonData);
    query.bindValue(":log_data", logData);

    if (!query.exec()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Failed to insert data:");
        return false;
    }
    else {
        MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                         "sucessed to insert data:");
    }
    return true;
}

QList<QVariantMap> DbManager::getAllDataSimulation()
{
    QList<QVariantMap> simulationList;

    if (!db.isOpen()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Database is not open.");
        return simulationList;
    }

    QSqlQuery query;
    if (!query.exec("SELECT id, input_string, datetime, bson_data, log_data "
                    "FROM data_simulation")) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Failed to fetch data_simulation:");
        return simulationList;
    }

    while (query.next()) {
        QVariantMap record;
        record["id"] = query.value("id");
        record["input_string"] = query.value("input_string");
        record["datetime"] = query.value("datetime");
        record["bson_data"] = query.value("bson_data");
        record["log_data"] = query.value("log_data");

        simulationList.append(record);
    }

    return simulationList;
}

QVariantMap DbManager::getRecordById(int id)
{
    QVariantMap record;

    if (!db.isOpen()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Database is not open.");
        return record;
    }

    QSqlQuery query;
    query.prepare(
        "SELECT id, input_string, datetime, bson_data, log_data FROM "
        "data_simulation WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::ERROR, "DbManager", "getRecordById",
            ("Failed to fetch record by ID:" + query.lastError().text())
                .toStdString());
        return record;
    }

    if (query.next()) {
        record["id"] = query.value("id");
        record["input_string"] = query.value("input_string");
        record["datetime"] = query.value("datetime");
        record["bson_data"] = query.value("bson_data");
        record["log_data"] = query.value("log_data");
    }
    else {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR, "DbManager",
                                         "getRecordById",
                                         "not found Record with ID" + id);
    }

    return record;
}
