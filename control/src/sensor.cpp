#include "sensor.h"
using namespace std;

// Function that running until the timeout is reached and activate the update.
void Sensor::delayedFunction()
{
    while (timerCounter > 0) {
        this_thread::sleep_for(chrono::milliseconds(1000)); // Sleep for 1 second
        timerCounter.fetch_sub(1); // Atomically decrement the timer
        cout << "Time left: " << timerCounter.load() << " seconds" << endl;
    }
    updateDefualtValues();
}

// Function that activate or reset the timer
void Sensor::startOrResetTimer()
{
    lock_guard<mutex> lock(mtx);
    if (timerCounter == 0) {
        // Timer is not active, set the timer to 10 seconds and start it
        timerCounter = timeForUpdate;
        cout << "Starting new timer..." << endl;
        timerThread = thread(&Sensor::delayedFunction, this);
        timerThread.detach(); // Detach the thread so it can run independently
    }
    else {
        // Timer is already running, just reset it to 10 seconds
        timerCounter = timeForUpdate;
        cout << "Timer is active, resetting to 10 seconds..." << endl;
    }
}

// Function that update the fields to be the default values
void Sensor::updateDefualtValues()
{
    for (auto field : fieldsMap) {
        updateTrueRoots(field.second.name, field.second.defaultValue, parser->getFieldType(field.second.type));
    }
    
    // TODO: check if the default value have to send a message
    GlobalProperties &instanceGP = GlobalProperties::getInstance();
    for (int cId : instanceGP.trueConditions)
        instanceGP.conditions[cId]->activateActions();
}

// C-tor initializes the id member variable.
Sensor::Sensor(int id, string name, string jsonFilePath) : id(id), name(name)
{
    timeForUpdate = 10;
    timerCounter = 0;
    isUsingHSM = true;

    parser = new PacketParser(jsonFilePath);
    vector<Field> tempFields = parser->getFields();

    for (auto field : tempFields) {
        if (field.type == "bit_field") 
            for (auto subField : parser->getBitFieldFields(field.name)) {
                GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, subField.name + " : " + subField.type);
                fieldsMap[subField.name] = subField;
            }
        else {
            GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, field.name + " : " + field.type);
            fieldsMap[field.name] = field;
        } 
        msgLength += field.size;
    }
}

void Sensor::handleMessage(void *msg)
{
    startOrResetTimer(); // Starts the timer

    parser->setBuffer(msg);

    for (auto field : fieldsMap) {
        string fieldName = field.first;
        // cout<<get<float>(parser->getFieldValue(fieldName));

        updateTrueRoots(fieldName, parser->getFieldValue(fieldName),
                        parser->getFieldType(field.second.type));
    }
}

//Updates the condition status according to the received field and returns the  list of the full conditions whose root is true
void Sensor::updateTrueRoots(string field, FieldValue value, FieldType type)
{
    
    GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, "Processing field: " + field);
cout << field << ": ";
    // Update the field value in the sensor
    this->fields[field].first = value;

    // Evaluate each condition related to the field
    for (BasicCondition *bc : this->fields[field].second) {
        bool flag = false, prevStatus = bc->status;
        FieldValue bcValue = bc->value;
        OperatorTypes op = bc->operatorType;

        switch (type) {
            case FieldType::UNSIGNED_INT: {
                flag =
                    applyComparison(get<unsigned int>(value),
                                    get<unsigned int>(bcValue), op);
                break;
            }
            case FieldType::SIGNED_INT: {
                flag = applyComparison(get<int>(value),
                                       get<int>(bcValue), op);
                break;
            }
            case FieldType::CHAR_ARRAY: {
                flag = applyComparison(get<string>(value),
                                       get<string>(bcValue), op);
                break;
            }
            case FieldType::FLOAT_FIXED: {  
                flag = applyComparison(get<float>(value),
                                       get<float>(bcValue), op);
                break;
            }
            case FieldType::FLOAT_MANTISSA: {
                flag = applyComparison(get<float>(value),
                                       get<float>(bcValue), op);
                break;
            }
            case FieldType::BOOLEAN: {
                flag = applyComparison(get<bool>(value),
                                       get<bool>(bcValue), op);
                break;
            }
            case FieldType::DOUBLE: {
                flag = applyComparison(get<double>(value),
                                       get<double>(bcValue), op);
            }
            default: {
                GlobalProperties::controlLogger.logMessage(logger::LogLevel::ERROR, "Invalid FieldType encountered");
                break;
            }
        }

        // Update the condition's status
        bc->status = flag;

        // If the condition's status has changed
        if (flag != prevStatus) {
            GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, "Condition status changed for field: " + field);

            // Update parent conditions and check if the root condition is true
            for (Node *parent : bc->parents) {
                (bc->status) ? parent->countTrueConditions++
                             : parent->countTrueConditions--;
                parent->updateTree();
                GlobalProperties::controlLogger.logMessage(logger::LogLevel::INFO, "Updated parent tree for field: " + field);
            }
        }
    }
}

// Function to apply the correct operator
template <typename T>
bool Sensor::applyComparison(T a, T b, OperatorTypes op)
{
    GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, "applyComparison");
    cout << a << endl;
    switch (op) {
        case OperatorTypes::e:
            return a == b;
        case OperatorTypes::ne:
            return a != b;
        case OperatorTypes::l:
            return a < b;
        case OperatorTypes::b:
            return a > b;
        case OperatorTypes::le:
            return a <= b;
        case OperatorTypes::be:
            return a >= b;
    }  
    return false;
}