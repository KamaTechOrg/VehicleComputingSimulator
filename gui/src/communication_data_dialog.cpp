#include <QVBoxLayout>
#include <QHBoxLayout>
#include "communication_data_dialog.h"
#include "src/packet_parser.h"

std::map<int, PacketParser*> CommunicationDataDialog::parserMap;

// Constructor
CommunicationDataDialog::CommunicationDataDialog(const QStringList &buffers,
                                                 int srcId, QWidget *parent)
    : QDialog(parent), buffers(buffers), currentIndex(0), srcId(srcId)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    bufferList = new QListWidget(this);
    for (const QString &buffer : buffers) {
        bufferList->addItem(buffer.left(20));  // Show first 20 characters
    }
    layout->addWidget(bufferList);

    bufferView = new QTextEdit(this);
    bufferView->setReadOnly(true);
    layout->addWidget(bufferView);

    formatSelector = new QComboBox(this);
    formatSelector->addItem("Hexadecimal");
    formatSelector->addItem("Binary");
    formatSelector->addItem("Decoded Text");
    layout->addWidget(formatSelector);

    connect(bufferList, &QListWidget::currentRowChanged, this,
            &CommunicationDataDialog::bufferSelected);

    connect(formatSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommunicationDataDialog::changeViewFormat);

    bufferList->setCurrentRow(0);
    updateBufferDisplay();
}

// Slot to update buffer display when a buffer is selected
void CommunicationDataDialog::bufferSelected()
{
    currentIndex = bufferList->currentRow();
    updateBufferDisplay();
}

// Update buffer display based on current index and format
void CommunicationDataDialog::updateBufferDisplay()
{
    if (currentIndex < 0 || currentIndex >= buffers.size())
        return;

    QString buffer = buffers[currentIndex];
    int format = formatSelector->currentIndex();
    QString displayText;

    if (format == 0) {  // Hexadecimal
        displayText = buffer;
    }
    else if (format == 1) {  // Binary
        displayText = hexToBinary(buffer);
    }
    else if (format == 2) {  // Decoded text
        displayText = hexToText(buffer,srcId);
    }

    bufferView->setText(displayText);
}

// Change format for displaying the buffer
void CommunicationDataDialog::changeViewFormat(int index)
{
    Q_UNUSED(index);
    updateBufferDisplay();
}

// Convert hexadecimal string to binary string
QString CommunicationDataDialog::hexToBinary(const QString &hex)
{
    bool ok;
    QByteArray byteArray = QByteArray::fromHex(hex.toLatin1());
    QString binary;
    for (auto byte : byteArray) {
        binary += QString::number(byte, 2).rightJustified(8, '0') + " ";
    }
    return binary.trimmed();
}

// Convert hexadecimal string to decoded text
QString CommunicationDataDialog::hexToText(const QString &hex, const int srctId)
{
    QString cleanHex = hex;
    if (cleanHex.startsWith("0x", Qt::CaseInsensitive)) {
        cleanHex.remove(0, 2);
    }

    QByteArray byteArray = QByteArray::fromHex(cleanHex.toLatin1());

    if (parserMap.find(srctId) == parserMap.end()) {
        return "Error: Invalid srctId";
    }

    PacketParser *parser = parserMap[srctId];

    parser->setBuffer(byteArray.data());

    std::map<std::string, FieldValue> fieldValues = parser->getAllFieldValues();

    QString output;

    for (const auto &pair : fieldValues) {
        output += QString::fromStdString(pair.first) + ": ";

        if (std::holds_alternative<unsigned int>(pair.second)) {
            output += QString::number(std::get<unsigned int>(pair.second));
        }
        else if (std::holds_alternative<std::string>(pair.second)) {
            output +=
                QString::fromStdString(std::get<std::string>(pair.second));
        }
        else if (std::holds_alternative<float>(pair.second)) {
            output += QString::number(std::get<float>(pair.second), 'f', 6);
        }
        else if (std::holds_alternative<double>(pair.second)) {
            output += QString::number(std::get<double>(pair.second), 'f', 6);
        }
        else if (std::holds_alternative<int>(pair.second)) {
            output += QString::number(std::get<int>(pair.second));
        }
        else if (std::holds_alternative<bool>(pair.second)) {
            output += std::get<bool>(pair.second) ? "true" : "false";
        }
        output += "\n";
    }

    return output;
}