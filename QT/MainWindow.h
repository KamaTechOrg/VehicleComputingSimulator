#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <vector>
#include <QSet>
#include <QMap>
#include <QGridLayout>
#include "Process.h"
#include "DraggableSquare.h"
#include "ProcessDialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void createNewProcess();
    
private:
    void addProcessSquare(const Process &process);
    bool isUniqueId(int id);
    void addId(int id);
    void resizeSquares(const QSize& oldSize, const QSize& newSize);

    // QVBoxLayout *toolboxLayout;
    // QGridLayout *workspaceLayout;
    // QWidget *workspace;
    // std::vector<DraggableSquare*> squares;
    // QMap<int, QPoint> squarePositions;  // Map to store the positions of squares by their ID
    // QSet<int> usedIds;
    // QSize originalSize;  // Store the original size of the window



    QVBoxLayout *toolboxLayout;
    QWidget *workspace;
    std::vector<DraggableSquare*> squares;
    QMap<int, QPoint> squarePositions;  // Map to store the positions of squares by their ID
    QSet<int> usedIds;
    QSize originalSize;  // Store the original size of the window

    
    
};

#endif // MAINWINDOW_H
