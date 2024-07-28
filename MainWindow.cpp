
/// constractor
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ///////////////


    //////////////
    ///add 4 Processes 
    int i=0;
    Process Main(i, "Main", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(Main,i);
    addId(i++);
    Process HSM(i, "HSM", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(HSM,i);
    addId(i++);
    Process LogsDb(i, "LogsDb", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(LogsDb,i);
    addId(i++);
    Process Bus_Manager(i, "Bus_Manager", "./CMakeProject", "QEMUPlatform");
    addProcessSquare(Bus_Manager,i);
    addId(i++);
}

///////     overloading addProcessSquare   //////////
void MainWindow::addProcessSquare(const Process& process,int index) {
    DraggableSquare *square = new DraggableSquare(workspace);
    square->setProcess(process);

    // Retrieve the position for the square
    
    int x = (index % 2) * (square->width() + 10);
    int y = (index / 2) * (square->height() + 10);
    
    QPoint pos = squarePositions.value(process.getId(),QPoint(x, y));
    // Move the square to the specified position
    square->move(pos);

    // Ensure the square is visible
    square->show();

    // Store the position of the new square
    squarePositions[process.getId()] = pos;
}