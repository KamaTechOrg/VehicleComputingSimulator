#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ProcessManager manager;
    manager.resize(1000, 800);
    manager.show();

    return app.exec();
}
