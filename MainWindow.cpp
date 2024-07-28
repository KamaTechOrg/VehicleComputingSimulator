#include "MainWindow.h"

ProcessManager::ProcessManager(QWidget *parent) : QWidget(parent), process1(nullptr), process2(nullptr), timer(nullptr) {
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

    layout->addWidget(imageLabel);
    setLayout(layout);
}

ProcessManager::~ProcessManager() {
    if (process1) delete process1;
    if (process2) delete process2;
    if (timer) delete timer;
}

void ProcessManager::startProcesses() {
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

    process1->start(dir1.absoluteFilePath("dummy_program"), QStringList());
    process2->start(dir2.absoluteFilePath("dummy_program"), QStringList());

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

void ProcessManager::endProcesses() {
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

void ProcessManager::showTimerInput() {
    timeLabel->show();
    timeInput->show();
}

void ProcessManager::timerTimeout() {
    logOutput->append("Timer timeout reached.");
    endProcesses();
}

void ProcessManager::readProcess1Output() {
    logOutput->append("Process1: " + process1->readAllStandardOutput());
}

void ProcessManager::readProcess2Output() {
    logOutput->append("Process2: " + process2->readAllStandardOutput());
}

void ProcessManager::openImageDialog() {
    QString imagePath = QFileDialog::getOpenFileName(this, tr("בחר תמונה"), "", tr("קבצי תמונה (*.png *.jpg *.jpeg)"));
    if (!imagePath.isEmpty()) {
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull()) {
            QPalette palette;
            palette.setBrush(this->backgroundRole(), QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
            this->setPalette(palette);
            this->setAutoFillBackground(true);
        }
    }
}

// Include the generated moc file
#include "moc_MainWindow.cpp"
