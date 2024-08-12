

#ifndef TEST_BUILD
#include <QApplication>
#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setFixedSize(600, 600);  // Ensure that this line comes after the declaration of 'window'
    window.show();

    return app.exec();
}

#endif
