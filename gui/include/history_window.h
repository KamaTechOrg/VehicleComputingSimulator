#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "data_sql.h"
#include <QHeaderView>
#include <QDesktopWidget>
#include <QApplication>
#include <QDialog>

class HistoryWindow : public QDialog {
    Q_OBJECT

public:
    HistoryWindow(dataToSql *dataHandler, QWidget *parent = nullptr);
    QTableWidget *tableWidget; // Table to display the history data
    dataToSql *dataHandler;  // Pointer to the dataToSql instance

private slots:
    void loadHistory();  // Function to load history data
};

#endif // HISTORYWINDOW_H
