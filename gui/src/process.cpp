#include "process.h"
#include "main_window.h"

Process::Process(int id, const QString &name, const QString &executionFile,
                 const QString &qemuPlatform,
                 const QMap<KeyPermission, bool> &securityPermissions)
    : id(id), name(name), executionFile(executionFile), 
      qemuPlatform(qemuPlatform), securityPermissions(securityPermissions)
{
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "Process created with ID: " + std::to_string(id));
}

// Copy constructor
Process::Process(const Process &other)
    : id(other.id),
      name(other.name),
      executionFile(other.executionFile),
      qemuPlatform(other.qemuPlatform),
      securityPermissions(other.securityPermissions) 
{
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "Process copied with ID: " + std::to_string(other.id));
}

Process::Process() 
    : id(-1), name(""), executionFile(""), 
      qemuPlatform(""), securityPermissions() 
{}

int Process::getId() const
{
    return id;
}

QString Process::getName() const
{
    return name;
}

QString Process::getExecutionFile() const
{
    return executionFile;
}

QString Process::getQEMUPlatform() const
{
    return qemuPlatform;
}

void Process::setId(int newId) 
{
    id = newId;
}

void Process::setName(const QString &newName) 
{
    name = newName;
}

void Process::setExecutionFile(const QString &newExecutionFile)
{
    executionFile = newExecutionFile;
}

void Process::setQEMUPlatform(const QString &newQEMUPlatform) 
{
    qemuPlatform = newQEMUPlatform;
}

bool Process::getSecurityPermission(KeyPermission key) const
{
    return securityPermissions.value(key, false);
}

void Process::setSecurityPermission(KeyPermission key, bool value)
{
    securityPermissions[key] = value;
}

QMap<KeyPermission, bool> Process::getSecurityPermissions() const
{
    return securityPermissions;
}