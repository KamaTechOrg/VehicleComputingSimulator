#include "main_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow::guiLogger.initializeLogFile();

    MainWindow window;

    window.setFixedSize(1000, 800);
    window.show();

    MainWindow::guiLogger.cleanUp();

    return app.exec();
}