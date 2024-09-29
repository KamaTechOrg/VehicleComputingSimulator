#ifndef __COMMUNICATION_DATA_DIALOG_H__
#define __COMMUNICATION_DATA_DIALOG_H__

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStringList>
#include <QListWidget>
#include "src/packet_parser.h"

class CommunicationDataDialog : public QDialog {
    Q_OBJECT

public:
    CommunicationDataDialog(const QStringList &buffers,
                            int srcId, QWidget *parent = nullptr);
    static std::map<int, PacketParser*> parserMap;

private slots:
    void
    updateBufferDisplay();
    void changeViewFormat(
        int index);
    void bufferSelected();

private:
    QStringList buffers;
    int currentIndex;
    QTextEdit *bufferView;
    QComboBox *formatSelector;
    QListWidget *bufferList;
    int srcId;

    QString hexToBinary(const QString &hex);
    QString hexToText(const QString &hex,const int srcId);
};

#endif  // __COMMUNICATION_DATA_DIALOG_H__