#include "HistoryWindow.h"
#include <QMessageBox>
#include <QHeaderView>

HistoryWindow::HistoryWindow(dataToSql *dataHandler, QWidget *parent)
    : QWidget(parent), dataHandler(dataHandler) {
    // Set window title
    setWindowTitle("הסטוריית סימולציות");

    // Initialize tableWidget
    tableWidget = new QTableWidget(this);

    // Set the size policies to expanding
    tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Enable column stretching for better fitting
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Set up layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tableWidget);

    // Set a default window size
    setMinimumSize(600, 400);

    // Connect the button to load history data
    loadHistory();

}

void HistoryWindow::loadHistory() {
    // Get data from the database
    QList<QVariantMap> simulationData = dataHandler->getAllDataSimulation();

    // Set up table columns
    tableWidget->setRowCount(simulationData.size());
    tableWidget->setColumnCount(4);  // ID, Input String, Datetime, Rerun Button

    QStringList headers;
    headers << "ID" << "Input String" << "Datetime" << "הרצה נוספת";
    tableWidget->setHorizontalHeaderLabels(headers);
    // if (!query.exec("CREATE TABLE IF NOT EXISTS data_simulation (id INTEGER PRIMARY KEY AUTOINCREMENT, input_string TEXT, datetime TEXT, bson_data BLOB, log_data TEXT)")) {
    if (simulationData.size()==0)
        qDebug()<<"the table is empty";
    // Fill table with data
    for (int i = 0; i < simulationData.size(); ++i) {
        // Set ID, Input String, and Datetime
        tableWidget->setItem(i, 0, new QTableWidgetItem(simulationData[i]["id"].toString()));
        tableWidget->setItem(i, 1, new QTableWidgetItem(simulationData[i]["input_string"].toString()));
        tableWidget->setItem(i, 2, new QTableWidgetItem(simulationData[i]["datetime"].toString()));

        // Create "הרצה נוספת" button
        QPushButton *rerunButton = new QPushButton("הרצה נוספת", this);
        tableWidget->setCellWidget(i, 3, rerunButton);

        // Connect the rerun button to display a message
        connect(rerunButton, &QPushButton::clicked, [=]() {
            QMessageBox::information(this, "הרצה נוספת", "תודה שלחצת על הרצה נוספת.");
        });
    }
}
