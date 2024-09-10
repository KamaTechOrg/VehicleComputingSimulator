#include "dataToSql.h"
dataToSql::dataToSql(QObject *parent) : QObject(parent)
{
    // אתחול בסיס הנתונים
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
    
    if (!db.open()) {
        qWarning() << "Cannot open database:" << db.lastError().text();
    }
}

bool dataToSql::insertDataToDatabase(const QString &inputString, const QByteArray &bsonData, const QString &logData)
{
    qDebug() << "Available SQL Drivers:" << QSqlDatabase::drivers();

    if (!db.isOpen()) {
        qWarning() << "Database is not open.";
        return false;
    }

    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS records (id INTEGER PRIMARY KEY AUTOINCREMENT, input_string TEXT, datetime TEXT, bson_data BLOB, log_data TEXT)")) {
        qWarning() << "Failed to create table:" << query.lastError().text();
        return false;
    }

    query.prepare("INSERT INTO records (input_string, datetime, bson_data, log_data) VALUES (:input_string, :datetime, :bson_data, :log_data)");
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
