#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "dataToSql.h"

class HistoryWindow : public QWidget {
    Q_OBJECT

public:
    HistoryWindow(dataToSql *dataHandler, QWidget *parent = nullptr);

private slots:
    void loadHistory();  // Function to load history data

private:
    dataToSql *dataHandler;  // Pointer to the dataToSql instance
    QTableWidget *tableWidget; // Table to display the history data
};

#endif // HISTORYWINDOW_H
