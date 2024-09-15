#include <QBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenu>
#include "main_window.h"
#include "draggable_square.h"

void DraggableSquare::print() const
{
    std::stringstream ss;
    ss << "DraggableSquare:\n"
       << "  Process ID: " << process->getId() << "\n"
       << "  Drag Start Position: (" << dragStartPosition.x() << ", "
       << dragStartPosition.y() << ")\n"
       << "  Initial Position: (" << initialPosition.x() << ", "
       << initialPosition.y() << ")\n"
       << "  Color: " << label->styleSheet().toStdString() << "\n"
       << "  Size: (" << this->width() << ", " << this->height() << ")";

    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO, ss.str());
}

void DraggableSquare::setSquareColor(const QString &color)
{
    setStyleSheet(color);
    stopButton->setStyleSheet(QString("background-color: %1; border: none;  "
                                      "color: white; font-size: 12px;")
                                  .arg(color));
}
// constructor
DraggableSquare::DraggableSquare(QWidget *parent, const QString &color,
                                 int width, int height)
    : QWidget(parent),
      label(new QLabel(this)),
      stopButton(new QPushButton("STOP", this))
{
    setFixedSize(width, height);
    setStyleSheet(color);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(stopButton);
    setLayout(layout);
    stopButton->hide();
    connect(stopButton, &QPushButton::clicked, this,
            &DraggableSquare::handleStopButtonClicked);
}

// Copy constructor
DraggableSquare::DraggableSquare(const DraggableSquare &other)
    : QWidget(other.parentWidget()),
      dragStartPosition(other.dragStartPosition),
      initialPosition(other.initialPosition),
      label(new QLabel(other.label->text(), this)),
      process(other.process)  // Copy QLabel's text
{
    MainWindow::guiLogger.logMessage(logger::LogLevel::DEBUG,
                                     "Copying DraggableSquare");
    setFixedSize(other.width(), other.height());
    setStyleSheet(other.styleSheet());
}

// Copy assignment operator
DraggableSquare &DraggableSquare::operator=(const DraggableSquare &other)
{
    MainWindow::guiLogger.logMessage(logger::LogLevel::DEBUG,
                                     "Assigning DraggableSquare");
    if (this == &other) {
        return *this;
    }

    dragStartPosition = other.dragStartPosition;
    initialPosition = other.initialPosition;
    delete label;
    label = new QLabel(other.label->text(), this);  // Copy QLabel's text
    process = other.process;

    setFixedSize(other.width(), other.height());
    setStyleSheet(other.styleSheet());

    return *this;
}

void DraggableSquare::setProcess(Process *proc)
{
    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                     "Setting process for DraggableSquare");
    process = proc;
    if (process) {
        this->id = process->getId();
        label->setText(QString("ID: %1\nName: %2\nCMake: %3\nQEMU: %4")
                           .arg(process->getId())
                           .arg(process->getName())
                           .arg(process->getCMakeProject())
                           .arg(process->getQEMUPlatform()));
    }
}

Process *DraggableSquare::getProcess() const
{
    return process;
}

const QPoint DraggableSquare::getDragStartPosition() const
{
    return dragStartPosition;
}

void DraggableSquare::setDragStartPosition(QPoint dragStartPosition)
{
    this->dragStartPosition = dragStartPosition;
}

DraggableSquare::~DraggableSquare()
{
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "Destroying DraggableSquare with ID: " + std::to_string(id));
    if (label) {
        delete label;
        label = nullptr;
    }
}

void DraggableSquare::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        if (id < 0 || id > 4) {  // Prevent menu for IDs 1 to 4
            MainWindow::guiLogger.logMessage(
                logger::LogLevel::DEBUG,
                "Right-click on DraggableSquare with ID: " +
                    std::to_string(id));
            QMenu contextMenu(this);

            QAction *editAction = contextMenu.addAction("Edit");
            QAction *deleteAction = contextMenu.addAction("Delete");

            QAction *selectedAction = contextMenu.exec(event->globalPos());

            if (selectedAction == editAction) {
                editSquare(id);
            }
            else if (selectedAction == deleteAction) {
                deleteSquare(id);
            }
        }
    }
    else if (event->button() == Qt::LeftButton) {
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::DEBUG,
            "Left-click on DraggableSquare with ID: " + std::to_string(id));
        dragStartPosition = event->pos();
        dragging = true;
    }
    else {
        QWidget::mousePressEvent(event);
    }
}

void DraggableSquare::mouseMoveEvent(QMouseEvent *event)
{
    if (!dragging) {
        return;
    }

    QPoint newPos = mapToParent(event->pos() - dragStartPosition);
    newPos.setX(qMax(0, qMin(newPos.x(), parentWidget()->width() - width())));
    newPos.setY(qMax(0, qMin(newPos.y(), parentWidget()->height() - height())));

    move(newPos);
    dragStartPosition = newPos;
    MainWindow::guiLogger.logMessage(logger::LogLevel::DEBUG,
                                     "DraggableSquare moved to: (" +
                                         std::to_string(newPos.x()) + ", " +
                                         std::to_string(newPos.y()) + ")");
}

void DraggableSquare::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
        MainWindow::guiLogger.logMessage(logger::LogLevel::DEBUG,
                                         "DraggableSquare drag ended");
    }

    QWidget::mouseReleaseEvent(event);
}

void DraggableSquare::editSquare(int id)
{
    MainWindow *mainWindow =
        qobject_cast<MainWindow *>(parentWidget()->window());
    if (mainWindow) {
        mainWindow->editSquare(id);
    }
}

void DraggableSquare::deleteSquare(int id)
{
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "Editing DraggableSquare with ID: " + std::to_string(id));
    MainWindow *mainWindow =
        qobject_cast<MainWindow *>(parentWidget()->window());
    if (mainWindow) {
        mainWindow->deleteSquare(id);
    }
}
void DraggableSquare::setStopButtonVisible(bool visible)
{
    if (process->getId() > 3) {
        if (visible) {
            stopButton->show();
        }
        else {
            stopButton->hide();
        }
    }
}
void DraggableSquare::handleStopButtonClicked()
{
    if (process) {
        MainWindow *mainWindow =
            qobject_cast<MainWindow *>(parentWidget()->window());
        if (mainWindow) {
            mainWindow->stopProcess(
                process->getId());  // Pass the process ID to stopProcess
            stopButton->hide();
        }
    }
}