#include "../include/main_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qDebug() << QDir::currentPath();

    MainWindow window;
    window.setFixedSize(600, 600);
    window.show();

    return app.exec();
}