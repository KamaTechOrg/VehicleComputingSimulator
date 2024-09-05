#ifndef SIMULATION_DATA_MANAGER_H
#define SIMULATION_DATA_MANAGER_H

#include <QJsonObject>
#include <QPushButton>
#include <QString>
#include <QVector>
#include <QWidget>
#include <bson/bson.h>
#include "draggable_square.h"

class SimulationDataManager : public QWidget {
    Q_OBJECT

public:
    SimulationDataManager(QWidget *parent = nullptr);
    virtual ~SimulationDataManager() = default;
    void saveSimulationData(const std::string &fileName,
                            QVector<DraggableSquare *> squares, QString img);
    QJsonObject loadSimulationData(const std::string &fileName);
    bool SimulationDataManager::insertDataToDatabase( const QString &inputString, const QByteArray &bsonData, const QString &logData) ; 

    void printJson(QJsonObject jsonObject);

private:
    struct User {
        int id;
        QString name;
        QString img;
    };

    struct SimulationData {
        QVector<DraggableSquare *> processes;
        User user;
    };

    SimulationData data;

    QJsonObject bsonToJsonObject(const bson_t *document);
    void readSimulationData(QVector<DraggableSquare *> squares, QString img);
};

#endif  // SIMULATION_DATA_MANAGER_H
