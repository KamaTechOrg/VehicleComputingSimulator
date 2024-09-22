#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDateTime>
#include <QDebug>
#include <QByteArray>
#include <QFile>
#include <QTextStream>

class DbManager : public QObject {
    Q_OBJECT

public:
    explicit DbManager(QObject *parent = nullptr);
    QByteArray readBsonFile(const QString &filePath);
    QString readLogFile(const QString &filePath);
    bool insertDataToDatabase(const QString &inputString,
                              const QByteArray &bsonData,
                              const QString &logData);
    QList<QVariantMap> getAllDataSimulation();
    QVariantMap getRecordById(int id);

private:
    QSqlDatabase db;
};

#endif  // DB_MANAGER_H