#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDebug>
#include <vector>
#include <QSet>
#include <QMap>
#include <QWidget>
#include <QProcess>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMap>
#include <QPixmap>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QVector>
#include <memory>
#include <QCoreApplication>
#include <QDebug>
#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include "process.h"
#include "draggable_square.h"
#include "frames.h"
#include "log_handler.h"
#include "process.h"
#include "process_dialog.h"
#include "simulation_state_manager.h"
#include "db_manager.h"
#include "history_window.h"
#include "../../logger/logger.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    bool createBacktrace(const std::string &executablePath,
                     const std::string &coreDumpPath,
                     const std::string &outputFilePath);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    std::string getCoreDumpPath(qint64 pid, const std::string &executableName);
    void setCoreDumpLimit();
    void updateTimer();
    void endProcesses();
    void stopProcess(int deleteId);
    void showTimerInput();
    void timerTimeout();
    void openImageDialog();
    void createProcessConfigFile(int id, const QString &processPath);
    void disableButtonsExceptEnd();  // Disable all buttons except the "End" button
    void enableAllButtons();         // Re-enable all buttons
    void showLoadingIndicator();     // Show loading animation (spinner)
    void hideLoadingIndicator();     // Hide loading animation (spinner)
    QLineEdit *getTimeInput() const
    {
        return timeInput;
    }
    QPushButton *getStartButton() const
    {
        return runButton;
    }
    QTimer *getTimer() const
    {
        return timer;
    }
    QTextEdit *getLogOutput() const
    {
        return logOutput;
    }
    QString getCurrentImagePath() const
    {
        return currentImagePath;
    }
    static logger guiLogger;

private slots:
    void showSimulation();
    void loadSimulation();

public slots:
    void createNewProcess();
    void editSquare(int id);
    void deleteSquare(int id);
    void openHistoryWindow();  
    
protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    friend class TestMainWindow;
    friend class DraggableSquareTest;
    friend class UserInteractionTests;
    
    void runProjects();
    void logSquareProcessing(DraggableSquare *square, const QString &executionFilePath);
    void runBashScript(DraggableSquare *square, const QString &executionFilePath);
    void runCMakeProject(DraggableSquare *square, const QString &executionFilePath);
    void connectProcessSignals(QProcess *process);
    void handleProcessCrash(QProcess *process, const QString &executionPath, DraggableSquare *square, const QString &dumpDirectory);
    void logProcessStartFailure(const QString &filePath);
    void stopSimulationDueToCrash(DraggableSquare *square);
    void logCrashDetails(DraggableSquare *square, const std::string &backtraceFilePath);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void addProcessSquare(Process *&process);
    bool isUniqueId(int id);
    void addId(int id);
    void addProcessSquare(Process *process, QPoint position, int width,
                          int height, const QString &color);
    void compileProjects();
    QString getExecutableName(const QString &buildDirPath);
    Process *getProcessById(int id);
    void rotateImage();     // Function to handle rotation
    void openSecondProject();  // Function that handles launching the second project
    void setDefaultBackgroundImage();
    void openDialog();   
    QPushButton *openSecondProjectButton;  // Button to open the second project
    QVBoxLayout *toolboxLayout;
    QWidget *workspace;
    QVector<DraggableSquare *> squares;
    QMap<int, QPoint> squarePositions;
    QSet<int> usedIds;
    QPushButton *compileButton;
    QPushButton *runButton;
    QPushButton *endButton;
    QPushButton *timerButton;
    QLineEdit *timeInput;
    QLabel *timeLabel;
    QTextEdit *logOutput;
    QTimer *timer;
    QLabel *imageLabel;
    QVector<QPair<QProcess *, int>> runningProcesses;
    QLabel *loadingLabel;
    QPixmap loadingPixmap;  // Store the original pixmap
    QTimer *rotationTimer;  // Timer for rotation
    int rotationAngle;      // Store the current rotation angle
    QString currentImagePath;
    SimulationStateManager *stateManager;
    LogHandler logHandler;
    DbManager *sqlDataManager;
    DbManager *dataHandler;   // Pointer to DbManager
    HistoryWindow *historyWindow; // Pointer to history window
};

#endif  // MAIN_WINDOW_H