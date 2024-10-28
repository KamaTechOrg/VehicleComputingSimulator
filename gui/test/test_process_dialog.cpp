#include <QtTest/QtTest>
#include "process_dialog.h"

class ProcessDialogTests : public QObject {
    Q_OBJECT

private slots:
    void testGetId()
    {
        ProcessDialog dialog;
        dialog.setId(123);
        QCOMPARE(dialog.getId(), 123);
    }

    void testGetName()
    {
        ProcessDialog dialog;
        dialog.setName("Test Process");
        QCOMPARE(dialog.getName(), "Test Process");
    }

    void testGetExecutionFile()
    {
        ProcessDialog dialog;
        dialog.setExecutionFile("TestProject");
        QCOMPARE(dialog.getExecutionFile(), "TestProject");
    }

    void testGetQEMUPlatform()
    {
        ProcessDialog dialog;
        dialog.setQEMUPlatform("arm");
        QCOMPARE(dialog.getQEMUPlatform(), "arm");
    }

    void testIsValid()
    {
        ProcessDialog dialog;
        dialog.setId(123);
        dialog.setName("Test Process");
        dialog.setExecutionFile("TestProject");
        dialog.setQEMUPlatform("x86");
        
        QVERIFY(dialog.isValid());
    }

    void testValidateAndAccept()
    {
        ProcessDialog dialog;
        dialog.setId(123);
        dialog.setName("Test Process");
        dialog.setExecutionFile("TestProject");
        dialog.setQEMUPlatform("x86");

        QMap<Key_Permission, bool> permissions;
        permissions.insert(Key_Permission::VERIFY, true);
        permissions.insert(Key_Permission::SIGN, true);
        dialog.setSecurityPermissions(permissions);

        // Assuming true indicates validation success
        QVERIFY(dialog.validateAndAccept() == true);
    }
};

QTEST_MAIN(ProcessDialogTests)

#include "test_process_dialog.moc"