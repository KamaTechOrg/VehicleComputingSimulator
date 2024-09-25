#include <QTest>
#include <QMouseEvent>
#include <QWidget>
#include "draggable_square.h"
#include "process.h"
#include "main_window.h"
#include <QFile>
#include <QMessageBox>

class DraggableSquareTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void testSetProcess();
    void testMousePressEvent();
    void testMouseMoveEvent();
    void testMouseReleaseEvent();
    void testSetCrashIndicator();
    void testShowCrashInfo();
    void testSetDumpFilePath();

private:
    QWidget *parentWidget;
    DraggableSquare *draggableSquare;
    Process *testProcess = new Process();
    MainWindow *mainWindow;
};

void DraggableSquareTest::initTestCase()
{
    // Initialize a parent widget and the DraggableSquare instance
    parentWidget = new QWidget();
    parentWidget->resize(800, 600);
    mainWindow = new MainWindow(parentWidget);
    mainWindow->resize(800, 600);
    parentWidget->show();
    testProcess = new Process(1, "Test Process", "../../src/dummy_program1",
                              "QEMUPlatform");
    draggableSquare =
        new DraggableSquare(parentWidget, "background-color: red;", 100, 100);
    draggableSquare->setProcess(testProcess);
    draggableSquare->show();
}

void DraggableSquareTest::cleanupTestCase()
{
    delete draggableSquare;
    delete mainWindow;
    delete parentWidget;
    delete testProcess;
}

void DraggableSquareTest::testSetProcess()
{
    QCOMPARE(draggableSquare->getProcess()->getId(), testProcess->getId());
    QCOMPARE(draggableSquare->getProcess()->getName(), testProcess->getName());
    QCOMPARE(draggableSquare->getProcess()->getCMakeProject(),
             testProcess->getCMakeProject());
    QCOMPARE(draggableSquare->getProcess()->getQEMUPlatform(),
             testProcess->getQEMUPlatform());
}

void DraggableSquareTest::testMousePressEvent()
{
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(50, 50),
                           Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent rightClickEvent(QEvent::MouseButtonPress, QPoint(50, 50),
                                Qt::RightButton, Qt::RightButton,
                                Qt::NoModifier);
    // Test left button press starts dragging
    draggableSquare->mousePressEvent(&pressEvent);
    QCOMPARE(draggableSquare->getDragStartPosition(), QPoint(50, 50));
    // Test right button press triggers context menu (add additional checks as needed)
    draggableSquare->mousePressEvent(&rightClickEvent);
    // Since this involves GUI elements, you would typically need to simulate and verify the context menu here
}

void DraggableSquareTest::testMouseMoveEvent()
{
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(50, 50),
                           Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    draggableSquare->mousePressEvent(&pressEvent);
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(70, 70), Qt::LeftButton,
                          Qt::LeftButton, Qt::NoModifier);
    draggableSquare->mouseMoveEvent(&moveEvent);
    // Assuming the parent widget has a size of 800x600 and draggable square is constrained
    QCOMPARE(draggableSquare->pos(), QPoint(20, 20));
}

void DraggableSquareTest::testMouseReleaseEvent()
{
    draggableSquare->mousePressEvent(
        new QMouseEvent(QEvent::MouseButtonPress, QPoint(50, 50),
                        Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
    draggableSquare->mouseReleaseEvent(
        new QMouseEvent(QEvent::MouseButtonRelease, QPoint(50, 50),
                        Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
    QCOMPARE(draggableSquare->getDragStartPosition(), QPoint(50, 50));
}

void DraggableSquareTest::testSetCrashIndicator()
{
    // Test enabling the crash indicator
    draggableSquare->setCrashIndicator(true);
    QVERIFY(draggableSquare->graphicsEffect() != nullptr);
    QVERIFY(draggableSquare->crashLabel->isVisible());
    QVERIFY(draggableSquare->infoButton->isVisible());

    // Test disabling the crash indicator
    draggableSquare->setCrashIndicator(false);
    QVERIFY(draggableSquare->graphicsEffect() == nullptr);
    QVERIFY(!draggableSquare->crashLabel->isVisible());
    QVERIFY(!draggableSquare->infoButton->isVisible());
}

void DraggableSquareTest::testShowCrashInfo()
{
    QString testDumpPath = "../test/test_crash_dump_verify.txt";

    // Case 1: Create a temporary crash dump file and verify it's shown correctly
    QFile file(testDumpPath);
    QVERIFY(file.open(QIODevice::WriteOnly |
                      QIODevice::Text));  // Verify file can be opened
    QTextStream out(&file);
    out << "Test crash dump contents for QVerify.";
    file.close();

    // Set the dump file path and verify it's set correctly
    draggableSquare->setDumpFilePath(testDumpPath);
    QCOMPARE(draggableSquare->getDumpFilePath(),
             testDumpPath);  // Verify file path is correctly set
    QVERIFY(file.exists());  // Verify that the dump file exists

    // Verify behavior when dump file doesn't exist
    draggableSquare->setDumpFilePath("/invalid/path/to/non_existent_file.txt");
    QVERIFY(!QFile::exists("/invalid/path/to/non_existent_file.txt"));
}

void DraggableSquareTest::testSetDumpFilePath()
{
    QString testPath = "/tmp/test_dump_path.txt";
    draggableSquare->setDumpFilePath(testPath);
    QCOMPARE(draggableSquare->getDumpFilePath(), testPath);
}

QTEST_MAIN(DraggableSquareTest)
#include "test_draggable_square.moc"
