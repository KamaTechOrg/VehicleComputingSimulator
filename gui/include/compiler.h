#ifndef COMPILER_H
#define COMPILER_H

#include <QThread>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QStringList>
#include <QString>

class Compiler : public QThread {
    Q_OBJECT
public:
    Compiler(QString cmakePath, bool *compileSuccessful,
             bool plug,QObject *parent = nullptr);
    void setUserDefines(QString defines);
    QString getUserDefines();

protected:
    void run() override;

signals:
    void logMessage(const QString &message);

private:
    QString cmakePath;
    QString userDefines;
    bool *compileSuccessful;
    bool plug;
};

#endif  // COMPILER_H