#ifndef PROCESS_H
#define PROCESS_H

#include <QString>

class Process {
public:
    Process(int id, const QString &name, const QString &executionFile,
            const QString &qemuPlatform, int busID);
    Process();
    Process(const Process &other);  // Copy constructor

    int getId() const;
    QString getName() const;
    QString getExecutionFile() const;
    QString getQEMUPlatform() const;
    int getBusID() const;

    void setId(int newId)
    {
        id = newId;
    }
    void setName(const QString &newName)
    {
        name = newName;
    }
    void setExecutionFile(const QString &newExecutionFile)
    {
        executionFile = newExecutionFile;
    }
    void setQEMUPlatform(const QString &newQEMUPlatform)
    {
        qemuPlatform = newQEMUPlatform;
    }
    void setBusesId(int newId)
    {
        busID = newId;
    }
private:
    int id;
    QString name;
    QString executionFile;
    QString qemuPlatform;
    int busID;
};

#endif  // PROCESS_H