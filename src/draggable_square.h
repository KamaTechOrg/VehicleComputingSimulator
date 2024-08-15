#ifndef __DRAGGABLE_SQUARE_H__
#define __DRAGGABLE_SQUARE_H__

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include "process.h"

class DraggableSquare : public QWidget {
    Q_OBJECT

   public:
   friend class DraggableSquareTest;
    explicit DraggableSquare(QWidget *parent = nullptr,
                             const QString &color = "background-color: green;",
                             int width = 100, int height = 100);

    void setProcess(Process *process);
    Process *getProcess() const;
    const QPoint getDragStartPosition() const;
    int getId() const
    {
        return id;
    }
    void setId(int _id)
    {
        id = _id;
    }
    ~DraggableSquare() override;

   protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

   private:
   
    QPoint dragStartPosition;
    QPoint initialPosition;
    QLabel *label;
    Process *process;
    int id;
    bool dragging;

   private slots:
    void editSquare(int id);
    void deleteSquare(int id);
};

#endif  // __DRAGGABLE_SQUARE_H__