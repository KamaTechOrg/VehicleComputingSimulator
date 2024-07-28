#ifndef DRAGGABLESQUARE_H
#define DRAGGABLESQUARE_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include "Process.h"

class DraggableSquare : public QWidget
{
    Q_OBJECT

public:
    explicit DraggableSquare(QWidget *parent = nullptr);
    void setProcess(const Process &process);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint dragStartPosition;
    QPoint initialPosition;
    QLabel *label;
    Process process;
};

#endif // DRAGGABLESQUARE_H
