#ifndef DATATOSQL_H
#define DATATOSQL_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDateTime>
#include <QDebug>

class dataToSql : public QObject
{
    Q_OBJECT

public:
    explicit dataToSql(QObject *parent = nullptr);
    bool insertDataToDatabase(const QString &inputString, const QByteArray &bsonData, const QString &logData);
    QList<QVariantMap> getAllDataSimulation() ;
    QVariantMap getRecordById(int id) ;

private:
    QSqlDatabase db;
};

#endif // DATATOSQL_H
