#include <gtest/gtest.h>
#include "global_properties.h"
#include "test_helper.h"
#include "full_condition.h"


// Check build sensors
TEST(SensorsBuild, SensorsTest) {

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   EXPECT_EQ(instanceGP.sensors.size(), 2);
}

#pragma region Basic Condition Test

// Insert BasicCondition into instanceGP.conditions
TEST(EqualCondition, BasicConditionTest) {

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   testCondition("[5]=(code,500)", 5, "code", "500");

   EXPECT_EQ(instanceGP.conditions.size(), 1);
}

// Check the update status
TEST(SimpleUpdateStatus, UpdateTest) {

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check the update status with BasicConditionNotEqual
TEST(NotEqualCondition, BasicConditionTest) {

   GlobalProperties::resetInstance();
   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   testCondition("[5]!=(code,500)", 5, "code", "800");

   EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check the update status with BasicConditionLetter
TEST(LetterCondition, BasicConditionTest) {

   GlobalProperties::resetInstance();
   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   testCondition("[5]<(code,500)", 5, "code", "500");

   EXPECT_EQ(instanceGP.trueConditions.size(), 0);
}

// Check the update status with BasicConditionBigger
TEST(BiggerCondition, BasicConditionTest) {

   GlobalProperties::resetInstance();
   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   testCondition("[5]>(code,500)", 5, "code", "800");

   EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

#pragma endregion

#pragma region Complex Condition Test

// Check building Or operator
// TEST(OrCondition, ComplexConditionTest) {

//    GlobalProperties::resetInstance();
//    GlobalProperties& instanceGP = GlobalProperties::getInstance();
   
//    testCondition("[5]|(=(code,500),=(msg,aaa))", 5, "code", "500");

//    // EXPECT_EQ(instanceGP.conditions.size(), 1);
//    EXPECT_EQ(instanceGP.trueConditions.size(), 1);
// }

// Check building And operator
TEST(AndCondition, ComplexConditionTest) {

   GlobalProperties::resetInstance();
   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   testCondition("[5]&(=(code,500),!=(msg,aaa))", 5, "msg", "bbb");

   EXPECT_EQ(instanceGP.conditions.size(), 1);
   EXPECT_EQ(instanceGP.trueConditions.size(), 0);
}

#pragma endregion

// int main(int argc, char** argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
// }