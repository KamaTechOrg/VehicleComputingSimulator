#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <vector>
#include <QSet>
#include <QMap>
#include <QGridLayout>
#include "Process.h"
#include "DraggableSquare.h"
#include "ProcessDialog.h"
#include <QWidget>
#include <QProcess>
#include <QDir>
#include <QTextEdit>
#include <QLineEdit>
#include <QTimer>
#include <QLabel>
#include <QFileDialog>
#include <QPixmap>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void startProcesses();
    void endProcesses();
    void showTimerInput();
    void timerTimeout();
    void readProcess1Output();
    void readProcess2Output();
    void openImageDialog();
    
protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void createNewProcess();
    
private:
    void addProcessSquare(const Process &process);
    bool isUniqueId(int id);
    void addId(int id);
    void resizeSquares(const QSize& oldSize, const QSize& newSize);
    void addProcessSquare(const Process& process,int index);
    QVBoxLayout *toolboxLayout;
    QWidget *workspace;
    std::vector<DraggableSquare*> squares;
    QMap<int, QPoint> squarePositions;  // Map to store the positions of squares by their ID
    QSet<int> usedIds;
    QSize originalSize;  // Store the original size of the window
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

