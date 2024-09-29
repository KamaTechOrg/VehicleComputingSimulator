
#include <opencv2/opencv.hpp>
#include "manager.h"

using namespace std;
using namespace cv;

int main()
{
    Manager manager;
    manager.init();
    manager.mainDemo();
    return 0;
}