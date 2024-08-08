#include "process_dialog.h"
#include <QIntValidator>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QComboBox>

ProcessDialog::ProcessDialog(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *idLabel = new QLabel("ID:");
    idEdit = new QLineEdit(this);
    idEdit->setValidator(new QIntValidator(11, 10000, this));
    layout->addWidget(idLabel);
    layout->addWidget(idEdit);

    QLabel *nameLabel = new QLabel("Name:");
    nameEdit = new QLineEdit(this);
    layout->addWidget(nameLabel);
    layout->addWidget(nameEdit);

    QLabel *cmakeProjectLabel = new QLabel("CMake Project:");
    cmakeProjectEdit = new QLineEdit(this);
    layout->addWidget(cmakeProjectLabel);
    layout->addWidget(cmakeProjectEdit);

    QLabel *qemuPlatformLabel = new QLabel("QEMU Platform:");
    qemuPlatformCombo = new QComboBox(this);
    qemuPlatformCombo->addItems({"x86_64", "arm", "aarch64"});
    layout->addWidget(qemuPlatformLabel);
    layout->addWidget(qemuPlatformCombo);

    QRegExp regex("[a-zA-Z0-9]*"); // Allows only English letters and numbers
    QRegExpValidator *validator = new QRegExpValidator(regex, this);

    QRegExp cmakeProjectRegex("[\\x20-\\x7E]*"); // Allows any printable ASCII character
    QRegExpValidator *cmakeProjectValidator = new QRegExpValidator(cmakeProjectRegex, this);

    nameEdit->setValidator(validator);
    cmakeProjectEdit->setValidator(cmakeProjectValidator);
    // qemuPlatformEdit->setValidator(validator);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &ProcessDialog::validateAndAccept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    setLayout(layout);
}

int ProcessDialog::getId() const
{
    return idEdit->text().toInt();
}

QString ProcessDialog::getName() const
{
    return nameEdit->text();
}

QString ProcessDialog::getCMakeProject() const
{
    return cmakeProjectEdit->text();
}

QString ProcessDialog::getQEMUPlatform() const
{
    return qemuPlatformCombo->currentText();
}

bool ProcessDialog::isValid() const
{
    return !idEdit->text().isEmpty() && !nameEdit->text().isEmpty() &&
           !cmakeProjectEdit->text().isEmpty() && !qemuPlatformCombo->currentText().isEmpty();
}

void ProcessDialog::validateAndAccept()
{
    if (isValid()) {
        accept();
    }
    else {
        QMessageBox::warning(this, "Input Error", "Please fill in all fields correctly.");
    }
}