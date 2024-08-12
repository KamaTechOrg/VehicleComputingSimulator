#include <QtTest/QtTest>
#include "../src/main_window.h"

class TestMainWindow : public QObject
{
    Q_OBJECT

private slots:

    //void testCreateNewProcess();
    //void testAddProcessSquare();
     void testIsUniqueId();
    //void testStartProcesses();
    //void testEndProcesses();
     //void testTimerTimeout();
     void testOpenImageDialog();
     void testDeleteSquare();
};
/*showTimerInput, editSquare,addId,compileBoxes,getExecutableName*/

// void TestMainWindow::testCreateNewProcess()
// {
//     MainWindow window;
//     QSignalSpy spy(&window, SIGNAL(createNewProcess()));
//     window.createNewProcess();
//     QCOMPARE(spy.count(), 1); // Check if the signal was emitted once
// }

// void TestMainWindow::testAddProcessSquare()
// {
//     MainWindow window;
//     Process process(5, "Test Process", "../src/dummy_program1", "QEMUPlatform");
//     window.addProcessSquare(process);
//     QCOMPARE(window.squares.size(), 1); // Check if square is added
// }

void TestMainWindow::testIsUniqueId()
{
    MainWindow window;
    window.addId(5);
    QCOMPARE(window.isUniqueId(5), false); // Check if the ID is unique
    QCOMPARE(window.isUniqueId(10), true); // Check if a different ID is unique
}

// // void TestMainWindow::testStartProcesses()
// // {
// //     MainWindow window;
// //     window.startProcesses();
// //     QVERIFY(!window.runningProcesses.isEmpty()); // Ensure processes are started
// // }


// void testStartProcesses() {
//     MainWindow mainWindow;

//     // Set up a mock environment if needed
//     mainWindow.timeInput->setText("5");  // Set a timer of 5 seconds

//     // Simulate clicking the start button
//     QTest::mouseClick(mainWindow.startButton, Qt::LeftButton);

//     // Verify that the timer started and processes were added
//     QVERIFY(mainWindow.timer->isActive());
//     QCOMPARE(mainWindow.logOutput->toPlainText().contains("Timer started for 5 seconds."), true);
// }

// void TestMainWindow::testEndProcesses()
// {
//     MainWindow window;
//     window.startProcesses();
//     window.endProcesses();
//     QVERIFY(window.runningProcesses.isEmpty()); // Ensure processes are stopped
// }

// void TestMainWindow::testTimerTimeout()
// {
//     MainWindow window;
//     window.startProcesses();
//     QSignalSpy spy(&window, SIGNAL(timerTimeout()));
//     window.timerTimeout();
//     QCOMPARE(spy.count(), 1); // Check if the signal was emitted once
// }

void TestMainWindow::testOpenImageDialog()
{
    MainWindow window;
    window.openImageDialog();
    QVERIFY(!window.currentImagePath.isEmpty()); // Ensure image path is set
}


void TestMainWindow::testDeleteSquare()
{
    MainWindow window;
    Process process(5, "Test Process", "../src/dummy_program1", "QEMUPlatform");
    window.addProcessSquare(process);

    window.deleteSquare(5);
    
    // Ensure that only the initial 4 squares remain
    QCOMPARE(window.squares.size(), 4);
    QVERIFY(!window.squarePositions.contains(5));
    QVERIFY(!window.usedIds.contains(5));
}


QTEST_MAIN(TestMainWindow)
#include "test_main_window.moc"
