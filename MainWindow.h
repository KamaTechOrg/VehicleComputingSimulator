#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QProcess>
#include <QDir>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QTimer>
#include <QLabel>
#include <QFileDialog>
#include <QPixmap>

class ProcessManager : public QWidget {
    Q_OBJECT

public:
    ProcessManager(QWidget *parent = nullptr);
    ~ProcessManager();

public slots:
    void startProcesses();
    void endProcesses();
    void showTimerInput();
    void timerTimeout();
    void readProcess1Output();
    void readProcess2Output();
    void openImageDialog();

private:
    QPushButton *startButton;
    QPushButton *endButton;
    QPushButton *timerButton;
    QLineEdit *timeInput;
    QLabel *timeLabel;
    QTextEdit *logOutput;
    QProcess *process1;
    QProcess *process2;
    QTimer *timer;
    QLabel *imageLabel;
};

#endif // MAINWINDOW_H
