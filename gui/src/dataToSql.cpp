#include "../include/dataToSql.h"

dataToSql::dataToSql(QObject *parent) : QObject(parent)
{
    // אתחול בסיס הנתונים
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
    
    if (!db.open()) {
        qWarning() << "Cannot open database:" << db.lastError().text();
    }
}
QString dataToSql::readLogFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open log file:" << filePath;
        return QString();
    }

    QTextStream in(&file);
    QString logData = in.readAll();
    file.close();
    return logData;
}

// פונקציה לקריאת קובץ בינארי לקובץ QByteArray
QByteArray dataToSql::readBsonFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open BSON file:" << filePath;
        return QByteArray();
    }

    QByteArray bsonData = file.readAll();
    file.close();
    return bsonData;
}

bool dataToSql::insertDataToDatabase(const QString &inputString, const QByteArray &bsonData, const QString &logData)
{
    qDebug() << "Available SQL Drivers:" << QSqlDatabase::drivers();

    if (!db.isOpen()) {
        qWarning() << "Database is not open.";
        return false;
    }

    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS data_simulation (id INTEGER PRIMARY KEY AUTOINCREMENT, input_string TEXT, datetime TEXT, bson_data BLOB, log_data TEXT)")) {
        qWarning() << "Failed to create table:" << query.lastError().text();
        return false;
    }

    query.prepare("INSERT INTO data_simulation (input_string, datetime, bson_data, log_data) VALUES (:input_string, :datetime, :bson_data, :log_data)");
    query.bindValue(":input_string", inputString);
    query.bindValue(":datetime", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":bson_data", bsonData);
    query.bindValue(":log_data", logData);

    if (!query.exec()) {
        qWarning() << "Failed to insert data:" << query.lastError().text();
        return false;
    }

    return true;
}
QList<QVariantMap> dataToSql::getAllDataSimulation() {
    QList<QVariantMap> simulationList;

    if (!db.isOpen()) {
        qWarning() << "Database is not open.";
        return simulationList;
    }

    QSqlQuery query;
    if (!query.exec("SELECT id, input_string, datetime, bson_data, log_data FROM data_simulation")) {
        qWarning() << "Failed to fetch data_simulation:" << query.lastError().text();
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
QVariantMap dataToSql::getRecordById(int id) {
    QVariantMap record;

    if (!db.isOpen()) {
        qWarning() << "Database is not open.";
        return record;
    }

    QSqlQuery query;
    query.prepare("SELECT id, input_string, datetime, bson_data, log_data FROM data_simulation WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "Failed to fetch record by ID:" << query.lastError().text();
        return record;
    }

    if (query.next()) {
        record["id"] = query.value("id");
        record["input_string"] = query.value("input_string");
        record["datetime"] = query.value("datetime");
        record["bson_data"] = query.value("bson_data");
        record["log_data"] = query.value("log_data");
    } else {
        qWarning() << "Record with ID" << id << "not found.";
    }

    return record;
}
