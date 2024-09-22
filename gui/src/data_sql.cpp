#include "data_sql.h"
#include "main_window.h"
dataToSql::dataToSql(QObject *parent) : QObject(parent)
{
   if (!QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("database.db");
    } else {
        db = QSqlDatabase::database("qt_sql_default_connection");
    }
    
    if (!db.open()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,"Cannot open database:");
    }
    else{
        MainWindow::guiLogger.logMessage(logger::LogLevel::INFO ,"i suceeses open database:");
    }
}
QString dataToSql::readLogFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,"Failed to open log file:");
            return QString();
    }
    else{
        MainWindow::guiLogger.logMessage(logger::LogLevel::INFO ,"i sucess open log file");
    }
    QTextStream in(&file);
    QString logData = in.readAll();
    file.close();
    return logData;
}

QByteArray dataToSql::readBsonFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,"Failed to open bson file:");
        return QByteArray();
    }
    else{
        MainWindow::guiLogger.logMessage(logger::LogLevel::INFO ,"i sucess open bson file");
    }
    QByteArray bsonData = file.readAll();
    file.close();
    return bsonData;
}

bool dataToSql::insertDataToDatabase(const QString &inputString, const QByteArray &bsonData, const QString &logData)
{
    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO ,"Available SQL Drivers:");

    if (!db.isOpen()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,"Database is not open.");
        return false;
    }

    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS data_simulation (id INTEGER PRIMARY KEY AUTOINCREMENT, input_string TEXT, datetime TEXT, bson_data BLOB, log_data TEXT)")) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,"Failed to create table:" );
        return false;
    }

    query.prepare("INSERT INTO data_simulation (input_string, datetime, bson_data, log_data) VALUES (:input_string, :datetime, :bson_data, :log_data)");
    query.bindValue(":input_string", inputString);
    query.bindValue(":datetime", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":bson_data", bsonData);
    query.bindValue(":log_data", logData);

    if (!query.exec()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,"Failed to insert data:");
        return false;
    }
    else{
        MainWindow::guiLogger.logMessage(logger::LogLevel::INFO ,"sucessed to insert data:");
    }
    return true;
}
QList<QVariantMap> dataToSql::getAllDataSimulation() {
    QList<QVariantMap> simulationList;

    if (!db.isOpen()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,"Database is not open.");
        return simulationList;
    }

    QSqlQuery query;
    if (!query.exec("SELECT id, input_string, datetime, bson_data, log_data FROM data_simulation")) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR , "Failed to fetch data_simulation:");
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
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,"Database is not open.");
        return record;
    }

    QSqlQuery query;
    query.prepare("SELECT id, input_string, datetime, bson_data, log_data FROM data_simulation WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
        "dataToSql","getRecordById",
        ("Failed to fetch record by ID:" +query.lastError().text()).toStdString());
        return record;
    }

    if (query.next()) {
        record["id"] = query.value("id");
        record["input_string"] = query.value("input_string");
        record["datetime"] = query.value("datetime");
        record["bson_data"] = query.value("bson_data");
        record["log_data"] = query.value("log_data");
    } else {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,"dataToSql","getRecordById","not found Record with ID" +id);
    }

    return record;
}
bool dataToSql::insertDataDetails(int dataSimulationId, const QString &timeStamp, int idSrc, int idDst, const QString &message, const QString &sendReceived, const QString &successFailed) {
    QSqlQuery query;

    // Create the table if it doesn't exist
    if (!query.exec("CREATE TABLE IF NOT EXISTS data_details ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "data_simulation_id INTEGER, "
                    "timesTamp TEXT, "
                    "idSrc INTEGER, "
                    "idDst INTEGER, "
                    "message TEXT, "
                    "send_recieved TEXT, "
                    "success_failed TEXT, "
                    "FOREIGN KEY(data_simulation_id) REFERENCES data_simulation(id))")) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,
                                        "dataToSql",
                                        "insertDataDetails",
                                        ("Failed to create data_details table:" + query.lastError().text()).toStdString());
        return false;
    }

    // Insert a new row into the table
    query.prepare("INSERT INTO data_details (data_simulation_id, timesTamp, idSrc, idDst, message, send_recieved, success_failed) "
                  "VALUES (:data_simulation_id, :timesTamp, :idSrc, :idDst, :message, :send_recieved, :success_failed)");
    query.bindValue(":data_simulation_id", dataSimulationId);
    query.bindValue(":timesTamp", timeStamp);
    query.bindValue(":idSrc", idSrc);
    query.bindValue(":idDst", idDst);
    query.bindValue(":message", message);
    query.bindValue(":send_recieved", sendReceived);
    query.bindValue(":success_failed", successFailed);

    // Execute the insert query
    if (!query.exec()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,
                                "dataToSql",
                                "insertDataDetails",
                                ("Failed to insert data into data_details:"+query.lastError().text() ).toStdString());
        return false;
    }

    return true;
}
QList<QVariantMap> dataToSql::getAllDataDetails() {
    QList<QVariantMap> detailsList;

    if (!db.isOpen()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,"Database is not open.");
        return detailsList;
    }

    QSqlQuery query;
    if (!query.exec("SELECT id, data_simulation_id, timesTamp, idSrc, idDst, message, send_recieved, success_failed FROM data_details")) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,
                                        "dataToSql",
                                        "getAllDataDetails",
                                        ("Failed to fetch data_details:"+query.lastError().text()).toStdString());
         return detailsList;
    }

    while (query.next()) {
        QVariantMap record;
        record["id"] = query.value("id");
        record["data_simulation_id"] = query.value("data_simulation_id");
        record["timesTamp"] = query.value("timesTamp");
        record["idSrc"] = query.value("idSrc");
        record["idDst"] = query.value("idDst");
        record["message"] = query.value("message");
        record["send_recieved"] = query.value("send_recieved");
        record["success_failed"] = query.value("success_failed");

        detailsList.append(record);
    }

    return detailsList;
}
QList<QVariantMap> dataToSql::getDataDetailsBySimulationAndTime(int dataSimulationId, const QString &timeStamp) {
    QList<QVariantMap> detailsList;

    if (!db.isOpen()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,"Database is not open.");
        return detailsList;
    }

    QSqlQuery query;
    query.prepare("SELECT id, data_simulation_id, timesTamp, idSrc, idDst, message, send_recieved, success_failed "
                  "FROM data_details WHERE data_simulation_id = :data_simulation_id AND timesTamp = :timeStamp");
    query.bindValue(":data_simulation_id", dataSimulationId);
    query.bindValue(":timeStamp", timeStamp);

    if (!query.exec()) {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR ,
        "dataToSql",
        "getDataDetailsBySimulationAndTime",
        ("Failed to fetch data_details by data_simulation_id and timeStamp:"+ query.lastError().text()).toStdString());
        return detailsList;
    }

    while (query.next()) {
        QVariantMap record;
        record["id"] = query.value("id");
        record["data_simulation_id"] = query.value("data_simulation_id");
        record["timesTamp"] = query.value("timesTamp");
        record["idSrc"] = query.value("idSrc");
        record["idDst"] = query.value("idDst");
        record["message"] = query.value("message");
        record["send_recieved"] = query.value("send_recieved");
        record["success_failed"] = query.value("success_failed");

        detailsList.append(record);
    }

    return detailsList;
}