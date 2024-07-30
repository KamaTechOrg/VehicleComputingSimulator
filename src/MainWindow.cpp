#include "MainWindow.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include "Process.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
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

    // Set toolbox width
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
    mainLayout->addWidget(imageLabel);

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    // No layout is set to workspace to allow free positioning of squares
    mainLayout->addWidget(workspace);

    centralWidget->setLayout(mainLayout);


    ///add 4 Processes 
    int i=0;
    Process Main(i, "Main", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(Main,i);
    addId(i++);
    Process HSM(i, "HSM", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(HSM,i);
    addId(i++);
    Process LogsDb(i, "LogsDb", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(LogsDb,i);
    addId(i++);
    Process Bus_Manager(i, "Bus_Manager", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(Bus_Manager,i);
    addId(i++);
}



MainWindow::~MainWindow() 
{
    qDeleteAll(squares);
    if (process1) delete process1;
    if (process2) delete process2;
    if (timer) delete timer;
}

void MainWindow::createNewProcess() {
     ProcessDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted && dialog.isValid()) {
        int id = dialog.getId();
        if (id <= 10) {
            QMessageBox::warning(this, "Invalid ID", "The ID must be greater than 10.");
            return;
        }
        if (!isUniqueId(id)) {
            QMessageBox::warning(this, "Non-unique ID",
                                 "The ID entered is already in use. Please choose a different ID.");
            return;
        }
        Process newProcess(id, dialog.getName(), dialog.getCMakeProject(), dialog.getQEMUPlatform());
        addProcessSquare(newProcess);
        addId(id);
    }
}

void MainWindow::addProcessSquare(const Process& process) {
    DraggableSquare *square = new DraggableSquare(workspace);
    square->setProcess(process);

    // Retrieve the position for the square
    QPoint pos = squarePositions.value(process.getId(), QPoint(0, 0));

    // Move the square to the specified position
    square->move(pos);

    // Ensure the square is visible
    square->show();

    // Store the position of the new square
    squarePositions[process.getId()] = pos;
}



void MainWindow::addProcessSquare(const Process& process,int index) {
    DraggableSquare *square = new DraggableSquare(workspace);
    square->setProcess(process);

    // Retrieve the position for the square
    
    int x = (index % 2) * (square->width() + 10);
    int y = (index / 2) * (square->height() + 10);
    
    QPoint pos = squarePositions.value(process.getId(),QPoint(x, y));
    // Move the square to the specified position
    square->move(pos);

    // Ensure the square is visible
    square->show();

    // Store the position of the new square
    squarePositions[process.getId()] = pos;
}


// Function to check if an ID is unique
bool MainWindow::isUniqueId(int id) {
    return !usedIds.contains(id);
}

// Function to add a new ID to the set
void MainWindow::addId(int id) {
    usedIds.insert(id);
}


void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    resizeSquares(originalSize, event->size());
    originalSize = event->size(); // Update originalSize to the new size
}

void MainWindow::resizeSquares(const QSize& oldSize, const QSize& newSize) {
    float widthRatio = static_cast<float>(newSize.width()) / oldSize.width();
    float heightRatio = static_cast<float>(newSize.height()) / oldSize.height();

    for (DraggableSquare* square : squares) {
        // Get current size and position
        QRect currentRect = square->geometry();
        QSize currentSize = currentRect.size();
        QPoint currentPos = currentRect.topLeft();

        // Calculate new size
        QSize newSize = QSize(currentSize.width() * widthRatio, currentSize.height() * heightRatio);

        // Calculate new position
        QPoint newPos = QPoint(currentPos.x() * widthRatio, currentPos.y() * heightRatio);

        // Resize and move square
        square->setGeometry(QRect(newPos, newSize));
    }
}

void MainWindow::startProcesses() {
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

    bool ok;
    int time = timeInput->text().toInt(&ok);
    if (ok && time > 0) {
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
}

void MainWindow::endProcesses() {
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

void MainWindow::showTimerInput() {
    timeLabel->show();
    timeInput->show();
}

void MainWindow::timerTimeout() {
    logOutput->append("Timer timeout reached.");
    endProcesses();
}

void MainWindow::readProcess1Output() {
    logOutput->append("Process1: " + process1->readAllStandardOutput());
}

void MainWindow::readProcess2Output() {
    logOutput->append("Process2: " + process2->readAllStandardOutput());
}

void MainWindow::openImageDialog() {
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
