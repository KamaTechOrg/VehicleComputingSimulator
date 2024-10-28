#ifndef PROCESS_H
#define PROCESS_H

#include <QString>
#include "process_dialog.h"

class Process {
public:
    Process(int id, const QString &name, const QString &executionFile,
            const QString &qemuPlatform, 
            const QMap<Key_Permission, bool> &permissions);
    Process();
    Process(const Process &other);

    int getId() const;
    QString getName() const;
    QString getExecutionFile() const;
    QString getQEMUPlatform() const;
    bool getSecurityPermission(Key_Permission key) const;
    QMap<Key_Permission, bool> getSecurityPermissions() const;

    void setId(int newId);
    void setName(const QString &newName);
    void setExecutionFile(const QString &newExecutionFile);
    void setQEMUPlatform(const QString &newQEMUPlatform);
    void setSecurityPermission(Key_Permission key, bool value);

private:
    int id;
    QString name;
    QString executionFile;
    QString qemuPlatform;
    QMap<Key_Permission, bool> securityPermissions;
};

#endif  // PROCESS_H