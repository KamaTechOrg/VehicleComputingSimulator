#ifndef PROCESS_H
#define PROCESS_H

#include <QString>

class Process {
public:
    enum ConstructorType {
        ParameterizedConstructor1,
        ParameterizedConstructor2
    };
    Process(int id, const QString &name, const QString &executionFile,
            const QString &qemuPlatform);
    Process(int id, const QString &name, const QString &executionFile,
            const QString &qemuPlatform, const QString &pluginsEdit);
    Process();
    Process(const Process &other);  // Copy constructor

    int getId() const;
    QString getName() const;
    QString getExecutionFile() const;
    QString getQEMUPlatform() const;
    QString getPluginsEdit() const;
    ConstructorType getConstructorType()const;
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
    void setPluginsEdit(const QString &newPluginsEdit)
    {
        pluginsEdit = newPluginsEdit;
    }

private:
    int id;
    QString name;
    QString executionFile;
    QString qemuPlatform;
    QString pluginsEdit;
    ConstructorType constructorType;

};

#endif  // PROCESS_H