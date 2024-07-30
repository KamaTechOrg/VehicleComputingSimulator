#include <QMessageBox>
#include <QHBoxLayout>
#include "process.h"
#include "main_window.h"

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent), process1(nullptr), process2(nullptr), timer(nullptr) 
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QWidget *toolbox = new QWidget(this);
    toolboxLayout = new QVBoxLayout(toolbox);
    startButton = new QPushButton("Start", this);
    endButton = new QPushButton("End", this);
    timerButton = new QPushButton("Set Timer", this);
    timeInput = new QLineEdit(this);
    timeLabel = new QLabel("Enter time in seconds:", this);
    logOutput = new QTextEdit(this);
    QPushButton *chooseButton = new QPushButton("choose img", this);

    timeLabel->hide();
    timeInput->hide();
    logOutput->setReadOnly(true);

    mainLayout->addWidget(toolbox);

    QPushButton *addProcessButton = new QPushButton("Add Process", toolbox);
    toolboxLayout->addWidget(addProcessButton);
    toolboxLayout->addStretch();
    connect(addProcessButton, &QPushButton::clicked, this, &MainWindow::createNewProcess);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startProcesses);
    connect(endButton, &QPushButton::clicked, this, &MainWindow::endProcesses);
    connect(timerButton, &QPushButton::clicked, this, &MainWindow::showTimerInput);
    connect(chooseButton, &QPushButton::clicked, this, &MainWindow::openImageDialog);

    toolbox->setMaximumWidth(100);
    toolbox->setMinimumWidth(100);
    toolboxLayout->addWidget(startButton);
    toolboxLayout->addWidget(endButton);
    toolboxLayout->addWidget(timerButton);
    toolboxLayout->addWidget(timeLabel);
    toolboxLayout->addWidget(timeInput);
    toolboxLayout->addWidget(logOutput);
    toolboxLayout->addWidget(chooseButton);

    workspace = new QWidget(this);
    workspace->setStyleSheet("background-color: white;");

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    mainLayout->addWidget(workspace);
    centralWidget->setLayout(mainLayout);

    int i = 0;
    Process mainProcess(i, "Main", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(mainProcess, i);
    addId(i++);
    Process hsmProcess(i, "HSM", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(hsmProcess, i);
    addId(i++);
    Process logsDbProcess(i, "LogsDb", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(logsDbProcess, i);
    addId(i++);
    Process busManagerProcess(i, "Bus_Manager", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(busManagerProcess, i);
    addId(i++);
}

MainWindow::~MainWindow() 
{
    qDeleteAll(squares);
    if (process1) delete process1;
    if (process2) delete process2;
    if (timer) delete timer;
}

void MainWindow::createNewProcess() 
{
    ProcessDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted && dialog.isValid()) {
        int id = dialog.getId();
        if (id <= 10) {
            QMessageBox::warning(this, "Invalid ID", "The ID must be greater than 10.");
            return;
        }
        if (!isUniqueId(id)) {
            QMessageBox::warning(this, "Non-unique ID", "The ID entered is already in use. Please choose a different ID.");
            return;
        }
        Process newProcess(id, dialog.getName(), dialog.getCMakeProject(), dialog.getQEMUPlatform());
        addProcessSquare(newProcess);
        addId(id);
    }
}

void MainWindow::addProcessSquare(const Process& process) 
{
    DraggableSquare *square = new DraggableSquare(workspace);
    square->setProcess(process);

    QPoint pos = squarePositions.value(process.getId(), QPoint(0, 0));
    square->move(pos);
    square->show();

    squarePositions[process.getId()] = pos;
}

void MainWindow::addProcessSquare(const Process& process, int index) 
{
    DraggableSquare *square = new DraggableSquare(workspace);
    square->setProcess(process);

    int x = (index % 2) * (square->width() + 10);
    int y = (index / 2) * (square->height() + 10);
    QPoint pos = squarePositions.value(process.getId(), QPoint(x, y));
    square->move(pos);
    square->show();

    squarePositions[process.getId()] = pos;
}

bool MainWindow::isUniqueId(int id) 
{
    return !usedIds.contains(id);
}

void MainWindow::addId(int id) 
{
    usedIds.insert(id);
}

void MainWindow::resizeEvent(QResizeEvent *event) 
{
    QMainWindow::resizeEvent(event);
    resizeSquares(originalSize, event->size());
    originalSize = event->size();
}

void MainWindow::resizeSquares(const QSize& oldSize, const QSize& newSize) 
{
    float widthRatio = static_cast<float>(newSize.width()) / oldSize.width();
    float heightRatio = static_cast<float>(newSize.height()) / oldSize.height();

    for (DraggableSquare* square : squares) {
        QRect currentRect = square->geometry();
        QSize currentSize = currentRect.size();
        QPoint currentPos = currentRect.topLeft();

        QSize newSize = QSize(currentSize.width() * widthRatio, currentSize.height() * heightRatio);
        QPoint newPos = QPoint(currentPos.x() * widthRatio, currentPos.y() * heightRatio);

        square->setGeometry(QRect(newPos, newSize));
    }
}

void MainWindow::startProcesses() 
{
    QString inputText = timeInput->text();
    bool ok = true;
    int time = 0;

    if (!inputText.isEmpty()) {
        time = inputText.toInt(&ok);
    }

    if (!ok || (time <= 0 && !inputText.isEmpty())) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid number of seconds.");
        timeInput->clear();
        return;
    }

    if (time > 0) {
        logOutput->append("Timer started for " + QString::number(time) + " seconds.");

        if (timer) {
            timer->stop();
            delete timer;
        }

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::timerTimeout);

        timer->start(time * 1000);
        timeLabel->hide();
        timeInput->hide();
    }

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

    QDir dir1("../src/dummy_program1/build");
    QDir dir2("../src/dummy_program2/build");

    connect(process1, &QProcess::readyReadStandardOutput, this, &MainWindow::readProcess1Output);
    connect(process2, &QProcess::readyReadStandardOutput, this, &MainWindow::readProcess2Output);

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
}

void MainWindow::endProcesses() 
{
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

void MainWindow::showTimerInput() 
{
    timeLabel->show();
    timeInput->show();
}

void MainWindow::timerTimeout() 
{
    logOutput->append("Timer timeout reached.");
    endProcesses();
}

void MainWindow::readProcess1Output() 
{
    logOutput->append("Process1: " + process1->readAllStandardOutput());
}

void MainWindow::readProcess2Output() 
{
    logOutput->append("Process2: " + process2->readAllStandardOutput());
}

void MainWindow::openImageDialog() 
{
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Select Image"), "", tr("Image Files (*.png *.jpg *.jpeg)"));
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
#include "moc_main_window.cpp"
