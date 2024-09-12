//#include "../gui/src/main_window.h"
#include "../src/manager.h"

int main()
{
    //const std::vector<uint32_t>& processIds = MainWindow::getProcessIds();
   // Manager* manager = Manager::getInstance(processIds);
   
    Manager* manager = Manager::getInstance();
    manager->startConnection();
    
    while(true);
    return 0;
}
