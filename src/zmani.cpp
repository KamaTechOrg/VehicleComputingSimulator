//  QFile bsonFile("../");
//     QByteArray bsonData;
//     if (bsonFile.open(QIODevice::ReadOnly)) {
//         bsonData = bsonFile.readAll();
//         bsonFile.close();
//     } else {
//         qWarning() << "Failed to open BSON file";
//         bsonData.clear(); // או טיפול בבעיה אחרת
//     }

//     // קריאת תוכן קובץ ה-log
//     QFile logFile("../build/log_file.log"); // נניח ששם הקובץ הוא log_output.txt
//     QString logData;
//     if (logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
//         logData = logFile.readAll();
//         logFile.close();
//     } else {
//         qWarning() << "Failed to open log file";
//         logData.clear(); // או טיפול בבעיה אחרת
//     }

//     // הוספת נתונים לבסיס הנתונים
//     dataManager->insertDataToDatabase("NAME_ZMANI", bsonData, logData);