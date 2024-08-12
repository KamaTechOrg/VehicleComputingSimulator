#include <QtTest/QtTest>
#include "../src/draggable_square.h"  // Include your classes to test

class DraggableSquareTests : public QObject {
    Q_OBJECT

private slots:
    void testExample() {
        // Your test code here
        QVERIFY(true);  // Example test that always passes
    }
};

// Register the test
QTEST_MAIN(DraggableSquareTests)
#include "test_example.moc"
