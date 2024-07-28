#include "MainWindow.h"
#include <QMessageBox>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QWidget *toolbox = new QWidget(this);
    toolboxLayout = new QVBoxLayout(toolbox);
    mainLayout->addWidget(toolbox);

    QPushButton *addProcessButton = new QPushButton("Add Process", toolbox);
    toolboxLayout->addWidget(addProcessButton);
    toolboxLayout->addStretch();
    connect(addProcessButton, &QPushButton::clicked, this, &MainWindow::createNewProcess);

    // Set toolbox width
    toolbox->setMaximumWidth(100);
    toolbox->setMinimumWidth(100);

    workspace = new QWidget(this);
    workspace->setStyleSheet("background-color: white;");
    // No layout is set to workspace to allow free positioning of squares
    mainLayout->addWidget(workspace);

    centralWidget->setLayout(mainLayout);
}

MainWindow::~MainWindow() 
{
    qDeleteAll(squares);
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


































// #include "MainWindow.h"
// #include <QMessageBox>
// #include <QResizeEvent>

// #include <QGridLayout>

// MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), originalSize(size()) {
//     QWidget *centralWidget = new QWidget(this);
//     setCentralWidget(centralWidget);

//     QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

//     QWidget *toolbox = new QWidget(this);
//     toolboxLayout = new QVBoxLayout(toolbox);
//     mainLayout->addWidget(toolbox);

//     QPushButton *addProcessButton = new QPushButton("Add Process", toolbox);
//     toolboxLayout->addWidget(addProcessButton);
//     toolboxLayout->addStretch();
//     connect(addProcessButton, &QPushButton::clicked, this, &MainWindow::createNewProcess);

//     // Set toolbox width
//     toolbox->setMaximumWidth(100);
//     toolbox->setMinimumWidth(100);

//     workspace = new QWidget(this);
//     workspace->setStyleSheet("background-color: white;");
//     workspaceLayout = new QGridLayout(workspace); // Changed from QVBoxLayout to QGridLayout
//     mainLayout->addWidget(workspace);

//     centralWidget->setLayout(mainLayout);
// }

// MainWindow::~MainWindow() 
// {
//     qDeleteAll(squares);
// }

// void MainWindow::resizeEvent(QResizeEvent *event) {
//     QMainWindow::resizeEvent(event);
//     resizeSquares(originalSize, event->size());
//     originalSize = event->size(); // Update originalSize to the new size
// }

// void MainWindow::resizeSquares(const QSize& oldSize, const QSize& newSize) {
//     float widthRatio = static_cast<float>(newSize.width()) / oldSize.width();
//     float heightRatio = static_cast<float>(newSize.height()) / oldSize.height();

//     for (DraggableSquare* square : squares) {
//         // Get current size and position
//         QRect currentRect = square->geometry();
//         QSize currentSize = currentRect.size();
//         QPoint currentPos = currentRect.topLeft();

//         // Calculate new size
//         QSize newSize = QSize(currentSize.width() * widthRatio, currentSize.height() * heightRatio);

//         // Calculate new position
//         QPoint newPos = QPoint(currentPos.x() * widthRatio, currentPos.y() * heightRatio);

//         // Resize and move square
//         square->setGeometry(QRect(newPos, newSize));
//     }
// }

// void MainWindow::createNewProcess() {
//     ProcessDialog dialog(this);

//     if (dialog.exec() == QDialog::Accepted && dialog.isValid()) {
//         if(!isUniqueId(dialog.getId()))
//             QMessageBox::warning(this, "Non-unique ID",
//                              "The ID entered is already in use. Please choose a different ID.");
//         else {
//             Process newProcess(dialog.getId(), dialog.getName(), dialog.getCMakeProject(), dialog.getQEMUPlatform());
//             addProcessSquare(newProcess);
//             addId(dialog.getId());
//         }
//     }
// }

// void MainWindow::addProcessSquare(const Process& process) {
//     DraggableSquare *square = new DraggableSquare(workspace);
//     square->setProcess(process);

//     // Set initial size and position (adjust according to your layout needs)
//     QSize initialSize(100, 100); // Example size
//     QPoint initialPos(0, 0); // Example position

//     square->setGeometry(QRect(initialPos, initialSize));
//     workspaceLayout->addWidget(square); // For QGridLayout, you may need to use addWidget with row and column
//     squares.push_back(square);
//     squarePositions[process.getId()] = initialPos;
// }

// bool MainWindow::isUniqueId(int id) {
//     return !usedIds.contains(id);
// }

// void MainWindow::addId(int id) {
//     usedIds.insert(id);
// }
