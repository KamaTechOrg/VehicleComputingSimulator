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