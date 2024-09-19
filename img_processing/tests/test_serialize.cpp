#include <gtest/gtest.h>
#include "../include/alert.h"
#include "../include/object_type_enum.h"
#include "manager.h"
#include "../include/alerter.h"

using namespace std;

TEST(DetectionAlert, simpleAlert)
{
    Alert da(false, 1, ObjectType::CAR, 100.0);
    vector<char> v = da.serialize();
    Alert afterDeserialization;
    afterDeserialization.deserialize(v.data());
    EXPECT_TRUE(da.getMessageType() == afterDeserialization.getMessageType() &&
                da.getLevel() == afterDeserialization.getLevel() &&
                da.getObjectType() == afterDeserialization.getObjectType() &&
                da.getObjectDistance() ==
                    afterDeserialization.getObjectDistance() &&
                da.getCarSpeed() == afterDeserialization.getCarSpeed() &&
                da.getObjectSpeed() == afterDeserialization.getObjectSpeed());
}

TEST(DetectionAlert, smartAlert)
{
    Alert da(true, 1, ObjectType::CAR, 100.0, 2, 2);
    vector<char> v = da.serialize();
    Alert afterDeserialization;
    afterDeserialization.deserialize(v.data());
    EXPECT_TRUE(da.getMessageType() == afterDeserialization.getMessageType() &&
                da.getLevel() == afterDeserialization.getLevel() &&
                da.getObjectType() == afterDeserialization.getObjectType() &&
                da.getObjectDistance() ==
                    afterDeserialization.getObjectDistance() &&
                da.getCarSpeed() == afterDeserialization.getCarSpeed() &&
                da.getObjectSpeed() == afterDeserialization.getObjectSpeed());
}

TEST(DetectionAlert, create_file_json)
{
    Alerter alerter;
    alerter.makeFileJSON();
}