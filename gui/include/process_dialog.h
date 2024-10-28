#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QMap>

enum class Key_Permission {
        VERIFY,
        SIGN,
        ENCRYPT,
        DECRYPT,
        EXPORTABLE
    };

class ProcessDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProcessDialog(QWidget *parent = nullptr);

    int getId() const;
    QString getName() const;
    QString getExecutionFile() const;
    QString getQEMUPlatform() const;
    QMap<Key_Permission, bool> getSelectedPermissionsMap() const;
    static bool addingNewProcess;
    
    void setId(int id);
    void setName(const QString &name);
    void setExecutionFile(const QString &executableFile);
    void setQEMUPlatform(const QString &qemuPlatform);
    void setCMakeProject(const QString &cmakeProject);
    void setSecurityPermissions(const QMap<Key_Permission, bool>& permissions);

    bool isValid() const;
    QString permToString(Key_Permission perm) const;

private slots:
    bool validateAndAccept();
    void selectExecutableFile();

private:
    void setupPermissionCheckboxes();

    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QLineEdit *executionFile;
    QComboBox *qemuPlatformCombo;
    QMap<Key_Permission, QCheckBox*> permissionCheckboxes;
    QLineEdit *cmakeProjectEdit;            
    QPushButton *selectExecutableFileButton; 
    QMap<Key_Permission, QString> permMap;

    friend class ProcessDialogTests;
};

#endif  // PROCESSDIALOG_H