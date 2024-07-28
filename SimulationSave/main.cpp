#include <QApplication>
#include "SimulationDataManager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SimulationDataManager window;
    window.resize(400, 300);
    window.setWindowTitle("Simulation");
    window.show();
    return app.exec();
}
