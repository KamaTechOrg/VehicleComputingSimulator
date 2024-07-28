// #include <QApplication>
// #include <QPushButton>
// #include <QProcess>
// #include <QDir>
// #include <QVBoxLayout>
// #include <QWidget>
// #include <QTextEdit>
// #include <QLineEdit>
// #include <QTimer>
// #include <QLabel>
// ////
// #include <QFileDialog>
// #include <QPixmap>

// class ProcessManager : public QWidget {
//     Q_OBJECT

// public:
//     ProcessManager(QWidget *parent = nullptr) : QWidget(parent) {
//         QVBoxLayout *layout = new QVBoxLayout(this);

//         startButton = new QPushButton("Start", this);
//         endButton = new QPushButton("End", this);
//         timerButton = new QPushButton("Set Timer", this);
//         timeInput = new QLineEdit(this);
//         timeLabel = new QLabel("Enter time in seconds:", this);
//         logOutput = new QTextEdit(this);
//         logOutput->setReadOnly(true);

//         layout->addWidget(startButton);
//         layout->addWidget(endButton);
//         layout->addWidget(timerButton);
//         layout->addWidget(timeLabel);
//         layout->addWidget(timeInput);
//         layout->addWidget(logOutput);

//         timeLabel->hide();
//         timeInput->hide();

//         connect(startButton, &QPushButton::clicked, this, &ProcessManager::startProcesses);
//         connect(endButton, &QPushButton::clicked, this, &ProcessManager::endProcesses);
//         connect(timerButton, &QPushButton::clicked, this, &ProcessManager::showTimerInput);
//         ///chani
//         QPushButton *chooseButton = new QPushButton("choose img", this);
//         //onclick go to openImageDialog
//         connect(chooseButton, &QPushButton::clicked, this, &ProcessManager::openImageDialog);
//         //to show img
//         imageLabel = new QLabel(this);
//         //places
//         imageLabel->setAlignment(Qt::AlignCenter);
//         imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//         layout->addWidget(chooseButton);
//         layout->addWidget(imageLabel);
//         setLayout(layout);
//         ///chani

    
//     }

// public slots:
//     void startProcesses() {
//         if (process1 && process1->state() != QProcess::NotRunning) {
//             logOutput->append("Process1 is already running.");
//             return;
//         }

//         if (process2 && process2->state() != QProcess::NotRunning) {
//             logOutput->append("Process2 is already running.");
//             return;
//         }

//         process1 = new QProcess(this);
//         process2 = new QProcess(this);

//         QDir dir1("/home/tamar/QtCMakeProject/dummy_program1/build");
//         QDir dir2("/home/tamar/QtCMakeProject/dummy_program2/build");

//         connect(process1, &QProcess::readyReadStandardOutput, this, &ProcessManager::readProcess1Output);
//         connect(process2, &QProcess::readyReadStandardOutput, this, &ProcessManager::readProcess2Output);

//         logOutput->append("Starting process1 from: " + dir1.absoluteFilePath("dummy_program"));
//         logOutput->append("Starting process2 from: " + dir2.absoluteFilePath("dummy_program"));

//         process1->start(dir1.absoluteFilePath("dummy_program"));
//         process2->start(dir2.absoluteFilePath("dummy_program"));

//         if (!process1->waitForStarted() || !process2->waitForStarted()) {
//             logOutput->append("Failed to start one or both processes.");
//             delete process1;
//             delete process2;
//             process1 = nullptr;
//             process2 = nullptr;
//             return;
//         }

//         logOutput->append("Both processes started successfully.");

//         // Start the timer if a valid time is set
//         bool ok;
//         int time = timeInput->text().toInt(&ok);
//         if (ok && time > 0) {
//             logOutput->append("Timer started for " + QString::number(time) + " seconds.");

//             // Stop and delete existing timer if any
//             if (timer) {
//                 timer->stop();
//                 delete timer;
//             }

//             timer = new QTimer(this);
//             connect(timer, &QTimer::timeout, this, &ProcessManager::timerTimeout);

//             timer->start(time * 1000); // Convert seconds to milliseconds

//             // Hide the input field after setting the timer
//             timeLabel->hide();
//             timeInput->hide();
//         }
//     }

//     void endProcesses() {
//         logOutput->append("Ending processes...");

//         if (process1 && process1->state() != QProcess::NotRunning) {
//             logOutput->append("Ending process1...");
//             process1->terminate(); // Use terminate for a cleaner shutdown
//             process1->waitForFinished();
//             delete process1;
//             process1 = nullptr;
//         }

//         if (process2 && process2->state() != QProcess::NotRunning) {
//             logOutput->append("Ending process2...");
//             process2->terminate(); // Use terminate for a cleaner shutdown
//             process2->waitForFinished();
//             delete process2;
//             process2 = nullptr;
//         }

//         logOutput->append("All processes ended.");

//         if (timer) {
//             timer->stop(); // Stop the timer if it's running
//             delete timer;
//             timer = nullptr;
//         }

//         // Make time input visible again for re-setting the timer
//         timeInput->show();
//         timeLabel->show();
//         timeInput->clear();
//     }

//     void showTimerInput() {
//         timeLabel->show();
//         timeInput->show();
//     }

//     void timerTimeout() {
//         logOutput->append("Timer timeout reached.");
//         endProcesses();
//     }

//     void readProcess1Output() {
//         logOutput->append("Process1: " + process1->readAllStandardOutput());
//     }

//     void readProcess2Output() {
//         logOutput->append("Process2: " + process2->readAllStandardOutput());
//     }
//     ///chani

//     void openImageDialog()
//     {
//         QString imagePath = QFileDialog::getOpenFileName(this, tr("בחר תמונה"), "", tr("קבצי תמונה (*.png *.jpg *.jpeg)"));
//         if (!imagePath.isEmpty())
//         {
//             QPixmap pixmap(imagePath);
//             if (!pixmap.isNull())
//             {
//                 imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio));
//             }
//         }
//     }

// private:
//     QPushButton *startButton;
//     QPushButton *endButton;
//     QPushButton *timerButton;
//     QLineEdit *timeInput;
//     QLabel *timeLabel;
//     QTextEdit *logOutput;
//     QProcess *process1 = nullptr;
//     QProcess *process2 = nullptr;
//     QTimer *timer = nullptr;
//     int timerDuration = 0; // Duration in seconds
//     QLabel *imageLabel;///chani


// };

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);

//     ProcessManager manager;
//     manager.resize(1000, 800);
//     manager.show();

//     return app.exec();
// }

// #include "main.moc"

#include <QApplication>
#include <QPushButton>
#include <QProcess>
#include <QDir>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QTimer>
#include <QLabel>
#include <QFileDialog>
#include <QPixmap>
#include <QPalette>

class ProcessManager : public QWidget {
    Q_OBJECT

public:
    ProcessManager(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);

        startButton = new QPushButton("Start", this);
        endButton = new QPushButton("End", this);
        timerButton = new QPushButton("Set Timer", this);
        timeInput = new QLineEdit(this);
        timeLabel = new QLabel("Enter time in seconds:", this);
        logOutput = new QTextEdit(this);
        logOutput->setReadOnly(true);

        layout->addWidget(startButton);
        layout->addWidget(endButton);
        layout->addWidget(timerButton);
        layout->addWidget(timeLabel);
        layout->addWidget(timeInput);
        layout->addWidget(logOutput);

        timeLabel->hide();
        timeInput->hide();

        connect(startButton, &QPushButton::clicked, this, &ProcessManager::startProcesses);
        connect(endButton, &QPushButton::clicked, this, &ProcessManager::endProcesses);
        connect(timerButton, &QPushButton::clicked, this, &ProcessManager::showTimerInput);

        QPushButton *chooseButton = new QPushButton("choose img", this);
        connect(chooseButton, &QPushButton::clicked, this, &ProcessManager::openImageDialog);

        layout->addWidget(chooseButton);

        imageLabel = new QLabel(this);
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

        setLayout(layout);
    }

public slots:
    void startProcesses() {
        if (process1 && process1->state() != QProcess::NotRunning) {
            logOutput->append("Process1 is already running.");
            return;
        }

        if (process2 && process2->state() != QProcess::NotRunning) {
            logOutput->append("Process2 is already running.");
            return;
        }

        process1 = new QProcess(this);
        process2 = new QProcess(this);

        QDir dir1("/home/tamar/QtCMakeProject/dummy_program1/build");
        QDir dir2("/home/tamar/QtCMakeProject/dummy_program2/build");

        connect(process1, &QProcess::readyReadStandardOutput, this, &ProcessManager::readProcess1Output);
        connect(process2, &QProcess::readyReadStandardOutput, this, &ProcessManager::readProcess2Output);

        logOutput->append("Starting process1 from: " + dir1.absoluteFilePath("dummy_program"));
        logOutput->append("Starting process2 from: " + dir2.absoluteFilePath("dummy_program"));

        process1->start(dir1.absoluteFilePath("dummy_program"));
        process2->start(dir2.absoluteFilePath("dummy_program"));

        if (!process1->waitForStarted() || !process2->waitForStarted()) {
            logOutput->append("Failed to start one or both processes.");
            delete process1;
            delete process2;
            process1 = nullptr;
            process2 = nullptr;
            return;
        }

        logOutput->append("Both processes started successfully.");

        bool ok;
        int time = timeInput->text().toInt(&ok);
        if (ok && time > 0) {
            logOutput->append("Timer started for " + QString::number(time) + " seconds.");

            if (timer) {
                timer->stop();
                delete timer;
            }

            timer = new QTimer(this);
            connect(timer, &QTimer::timeout, this, &ProcessManager::timerTimeout);

            timer->start(time * 1000);

            timeLabel->hide();
            timeInput->hide();
        }
    }

    void endProcesses() {
        logOutput->append("Ending processes...");

        if (process1 && process1->state() != QProcess::NotRunning) {
            logOutput->append("Ending process1...");
            process1->terminate();
            process1->waitForFinished();
            delete process1;
            process1 = nullptr;
        }

        if (process2 && process2->state() != QProcess::NotRunning) {
            logOutput->append("Ending process2...");
            process2->terminate();
            process2->waitForFinished();
            delete process2;
            process2 = nullptr;
        }

        logOutput->append("All processes ended.");

        if (timer) {
            timer->stop();
            delete timer;
            timer = nullptr;
        }

        timeInput->show();
        timeLabel->show();
        timeInput->clear();
    }

    void showTimerInput() {
        timeLabel->show();
        timeInput->show();
    }

    void timerTimeout() {
        logOutput->append("Timer timeout reached.");
        endProcesses();
    }

    void readProcess1Output() {
        logOutput->append("Process1: " + process1->readAllStandardOutput());
    }

    void readProcess2Output() {
        logOutput->append("Process2: " + process2->readAllStandardOutput());
    }

    void openImageDialog() {
        QString imagePath = QFileDialog::getOpenFileName(this, tr("בחר תמונה"), "", tr("קבצי תמונה (*.png *.jpg *.jpeg)"));
        if (!imagePath.isEmpty()) {
            QPixmap pixmap(imagePath);
            if (!pixmap.isNull()) {
                // Set the pixmap as the background
                QPalette palette;
                palette.setBrush(this->backgroundRole(), QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
                this->setPalette(palette);
                this->setAutoFillBackground(true);
            }
        }
    }

private:
    QPushButton *startButton;
    QPushButton *endButton;
    QPushButton *timerButton;
    QLineEdit *timeInput;
    QLabel *timeLabel;
    QTextEdit *logOutput;
    QProcess *process1 = nullptr;
    QProcess *process2 = nullptr;
    QTimer *timer = nullptr;
    QLabel *imageLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ProcessManager manager;
    manager.resize(1000, 800);
    manager.show();

    return app.exec();
}

#include "main.moc"
