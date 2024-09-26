#include <QtTest/QtTest>
#include "main_window.h"

class TestMainWindow : public QObject {
    Q_OBJECT

private slots:
    void init();     // Setup method, runs before each test
    void cleanup();  // Cleanup method, runs after each test

    void testCreateNewProcess();
    void testAddProcessSquare();
    void testIsUniqueId();
    void testStartProcesses();
    void testEndProcesses();
    void testDeleteSquare();
    // void testSetCoreDumpLimit();
    // void testCrashAndCoreDump();
    // void testCreateBacktrace();
    void testShowTimerInput();
    void addId(int id);

private:
    MainWindow *window;  // Pointer to MainWindow to allow reuse in each test
    QString crashedProcessRelativePath =
        "../test/crashProgram";  // Path to the crashing process
    QString crashedProcessAbsolutePath;
};

void TestMainWindow::init()
{
    // Initialize the MainWindow before each test
    window = new MainWindow();
}

void TestMainWindow::cleanup()
{
    // Clean up any resources used during the test
    delete window;
    window = nullptr;
}

void TestMainWindow::testCreateNewProcess()
{
    int newProcessId = 6;
    QString processName = "NewProcess";
    QString cmakeProject = "../dummy_program1";
    QString qemuPlatform = "QEMUPlatform";

    Process *newProcess =
        new Process(newProcessId, processName, cmakeProject, qemuPlatform);
    window->addProcessSquare(newProcess);
    window->addId(newProcessId);

    Process *retrievedProcess = window->getProcessById(newProcessId);
    QVERIFY(retrievedProcess != nullptr);
    QCOMPARE(retrievedProcess->getName(), processName);
    QCOMPARE(retrievedProcess->getExecutionFile(), cmakeProject);
    QCOMPARE(retrievedProcess->getQEMUPlatform(), qemuPlatform);

    // Cleanup for this specific test
    delete newProcess;
}

void TestMainWindow::testAddProcessSquare()
{
    Process *newProcess =
        new Process(5, "Test Process", "../dummy_program1", "QEMUPlatform");
    window->addProcessSquare(newProcess);
    QCOMPARE(window->squares.size(), 5);  // Check if square is added

    delete newProcess;  // Ensure we clean up the process
}

void TestMainWindow::testIsUniqueId()
{
    window->addId(5);
    QCOMPARE(window->isUniqueId(5), false);  // Check if the ID is unique
    QCOMPARE(window->isUniqueId(10),
             true);  // Check if a different ID is unique
}

void TestMainWindow::testStartProcesses()
{
    window->compileProjects();
    window->runProjects();
    QVERIFY(
        !window->runningProcesses.isEmpty());  // Ensure processes are started
    window->endProcesses();
}

void TestMainWindow::testEndProcesses()
{
    window->compileProjects();
    window->runProjects();
    window->endProcesses();
    QVERIFY(
        window->runningProcesses.isEmpty());  // Ensure processes are stopped
}

void TestMainWindow::testDeleteSquare()
{
    QString cmakeProject = "../dummy_program1";
    Process *process =
        new Process(5, "Test Process", cmakeProject, "QEMUPlatform");
    window->addProcessSquare(process);

    window->deleteSquare(5);

    // Ensure that only the initial 4 squares remain
    QCOMPARE(window->squares.size(), 4);
    QVERIFY(!window->squarePositions.contains(5));
    QVERIFY(!window->usedIds.contains(5));

    delete process;  // Cleanup after test
}

void TestMainWindow::addId(int id)
{
    window->addId(77);
    QVERIFY(window->usedIds.contains(77));
}

void TestMainWindow::testShowTimerInput()
{
    // Initially, the time input and label should be hidden
    QVERIFY(!window->timeInput->isVisible());
    QVERIFY(!window->timeLabel->isVisible());

    window->show();
    // Trigger the showTimerInput function
    window->showTimerInput();

    // Now, the time input and label should be visible
    QVERIFY(window->timeInput->isVisible());
    QVERIFY(window->timeLabel->isVisible());
}

// void TestMainWindow::testSetCoreDumpLimit()
// {
//     window->compileProjects();
//     struct rlimit core_limit;
//     getrlimit(RLIMIT_CORE, &core_limit);

//     QCOMPARE(core_limit.rlim_cur, RLIM_INFINITY);
//     QCOMPARE(core_limit.rlim_max, RLIM_INFINITY);
// }

// void TestMainWindow::testCrashAndCoreDump()
// {
//     // Add a process that crashes
//     Process *crashedProcess = new Process(
//         7, "CrashedProcess", crashedProcessRelativePath, "QEMUPlatform");
//     window->addProcessSquare(crashedProcess);
//     window->compileProjects();
//     // Start the process (this will crash with division by zero)
//     window->runProjects();

//     // Check if core dump and crash indicator are set for the crashed process
//     for (DraggableSquare *square : window->squares) {
//         Process *process = square->getProcess();
//         QFileInfo processFileInfo(process->getCMakeProject());
//         QString processAbsolutePath = processFileInfo.absoluteFilePath();

//         if (processAbsolutePath == crashedProcessAbsolutePath) {
//             QVERIFY(!square->getDumpFilePath()
//                          .isEmpty());  // Verify dump file path is set

//             // Check if the core dump file exists
//             QString coreDumpPath = square->getDumpFilePath();
//             QFile coreDumpFile(coreDumpPath);
//             QVERIFY(coreDumpFile.exists());  // Core dump should exist
//         }
//     }
//     window->endProcesses();
//     delete crashedProcess;
// }

// void TestMainWindow::testCreateBacktrace()
// {
//     // Add a process that crashes
//     Process *crashedProcess = new Process(
//         7, "CrashedProcess", crashedProcessRelativePath, "QEMUPlatform");
//     window->addProcessSquare(crashedProcess);
//     window->compileProjects();
//     // Simulate the process crash and check for the backtrace
//     window->runProjects();

//     // Check for backtrace creation
//     for (DraggableSquare *square : window->squares) {
//         Process *process = square->getProcess();
//         QFileInfo processFileInfo(process->getCMakeProject());
//         QString processAbsolutePath = processFileInfo.absoluteFilePath();

//         if (processAbsolutePath == crashedProcessAbsolutePath) {
//             QString coreDumpPath = square->getDumpFilePath();
//             QString backtraceFilePath =
//                 coreDumpPath.replace("core.", "backtrace_");

//             // Check if the backtrace file exists
//             QFile backtraceFile(backtraceFilePath);
//             QVERIFY(backtraceFile.exists());  // Backtrace should exist
//         }
//     }
//     window->endProcesses();
//     delete crashedProcess;
// }

QTEST_MAIN(TestMainWindow)
#include "test_main_window.moc"