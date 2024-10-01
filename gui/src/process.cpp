#include "process.h"
#include "main_window.h"

Process::Process(int id, const QString &name, const QString &executionFile,
                 const QString &qemuPlatform)
    : id(id), name(name), executionFile(executionFile),
      qemuPlatform(qemuPlatform),constructorType(ParameterizedConstructor1)

{
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "Process created with ID: " + std::to_string(id));
}

Process::Process(int id, const QString &name, const QString &executionFile,
            const QString &qemuPlatform, const QString &pluginsEdit)
    :id(id), name(name), executionFile(executionFile), qemuPlatform(qemuPlatform), 
     pluginsEdit(pluginsEdit),constructorType(ParameterizedConstructor2)

{
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "Process and with FLAG created with ID: " + std::to_string(id));
}
// Copy constructor
Process::Process(const Process &other)
    : id(other.id),
      name(other.name),
      executionFile(other.executionFile),
      qemuPlatform(other.qemuPlatform)
{
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "Process copied with ID: " + std::to_string(other.id));
}

Process::Process() : id(-1), name(""), executionFile(""), qemuPlatform("") {}

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

QString Process::getPluginsEdit() const
{
    return pluginsEdit;
}
Process::ConstructorType Process::getConstructorType() const
{
    return constructorType;
}
