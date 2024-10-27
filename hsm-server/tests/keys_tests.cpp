// #include <gtest/gtest.h>
// #include <fstream>
// #include <filesystem>
// #include <nlohmann/json.hpp>
// #include "../include/hsm.h" // כלול את הקובץ שבו נמצאת הפונקציה generateKey
// // using json = nlohmann::json;



// // פונקציה מדומה ליצירת מפתח
// std::string mockEncryption(int userId) {
//     return "mockedKeyForUser" + std::to_string(userId);
// }

// // בדיקה לפונקציה generateKey
// TEST(GenerateKeyTest, GeneratesCorrectJsonFile) {
//     int userId = 123;
//     KeyPermission permission = KeyPermission::READ;
//     Hsm h;
//     // קריאה לפונקציה כדי ליצור את קובץ המפתח
//     h.generateKey(userId, permission, mockEncryption);
    
//     // יצירת שם הקובץ כדי לבדוק אם נוצר כהלכה
//     std::string fileName = "../keys/key_" + std::to_string(userId) + ".json";
    
//     // בדיקה שהקובץ נוצר
//     ASSERT_TRUE(std::filesystem::exists(fileName)) << "Key file was not created!";
    
//     // קריאת תוכן הקובץ
//     std::ifstream file(fileName);
//     ASSERT_TRUE(file.is_open()) << "Failed to open the key file!";
    
//     // קריאת תוכן הקובץ ל-json
//     nlohmann::json jsonData;
//     file >> jsonData;
//     file.close();
    
//     // בדיקה שהמפתח, ההרשאה וה-userId נכונים
//     EXPECT_EQ(jsonData["userId"], userId);
//     EXPECT_EQ(jsonData["permission"], "Read");
//     EXPECT_EQ(jsonData["key"], "mockedKeyForUser123");

// }

// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }