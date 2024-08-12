#include <QTest>
#include "SquareTests.h"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    SquareTests tests;
    return QTest::qExec(&tests, argc, argv);
}