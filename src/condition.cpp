#include "condition.h"

Condition::~Condition()
{
    // destructor implementation
}


QString Condition::getShowCondition()
{
    return showCondition;
}

void Condition::setupLogicalMembers()
{
    showCondition = "if (  )"; // Initial display condition string
    condition = "";  // Internal condition string
    ind = 5;  // Starting index for cursor position
    indCondition = 0;  // Starting index for condition string
    isCursorVisible = true;  // Tracks cursor visibility state

    // Initial type and index for current operation
    typeCurrent = { "if", -1 };
    layersStack = stack<pair<QString, int>>();
    layersStack.push(typeCurrent);

    // Fill sensor and operator lists
    sensorList = {{"Speed", 1}, {"Tire Pressure", 2}, {"Communication", 3}, {"Camera", 4}};
    operatorsMap = {{"AND", "&"}, {"OR", "|"}};
    operatorList = {"=", "!=", ">", "<", "<=", ">="};
    // Map sensor IDs to their respective JSON files
    fillSensorsFields({
        {1, "speed_sensor.json"},
        {2, "tire_pressure_sensor.json"},
        {3, "communication_sensor.json"},
        {4, "camera_sensor.json"}
    });
}

void Condition::setupUi()
{
    label = new QLabel("");
    QFont font("Arial", 25);  // מגדיר את סוג הכתב ואת גודלו
    label->setFont(font);

    label1 = new QLabel("");
    label1->setFont(font);

    // יצירת מסגרת למסך מחשב (לקבוצה הראשונה)
    QGroupBox *screenBox = new QGroupBox("Screen");
    QVBoxLayout *screenLayout = new QVBoxLayout;
    
    screenLayout->addWidget(label);
    screenLayout->addWidget(label1);
    screenBox->setLayout(screenLayout);


    // יצירת כפתורים עם טקסט
    andBtn = new QPushButton("AND");
    orBtn = new QPushButton("OR");
    skip = new QPushButton("->");
    reset = new QPushButton("reset");
    textBox = new QLineEdit();
    submit = new QPushButton("add to condition");
    
    sensors = new QComboBox();
    sensors->addItem("Sensors");  // Placeholder item
    sensors->setCurrentIndex(0);

    sensorsFields = new QComboBox();
    sensorsFields->addItem("sensors fields");
    sensorsFields->setCurrentIndex(0);

    operators = new QComboBox();
    operators->addItem("Operators");  // Placeholder item
    operators->setCurrentIndex(0);

    // יצירת מסגרת למקלדת (לקבוצה השנייה)
    QGroupBox *keyboardBox = new QGroupBox("Keyboard");
    QVBoxLayout *keyboardLayout = new QVBoxLayout;

    // הוספת הכפתורים למקלדת
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(andBtn);
    buttonLayout->addWidget(orBtn);
    buttonLayout->addWidget(skip);
    buttonLayout->addWidget(reset);
    keyboardLayout->addLayout(buttonLayout);

    keyboardLayout->addWidget(sensors);

    QGroupBox *basicConditionBox = new QGroupBox("---create basic condition---");
    QVBoxLayout *basicConditionLayout = new QVBoxLayout;

    
    // הוספת שאר הוויידג'טים למסגרת המקלדת
    QHBoxLayout *textLayout = new QHBoxLayout;
    textLayout->addWidget(sensorsFields);
    textLayout->addWidget(operators);
    textLayout->addWidget(textBox);
    basicConditionLayout->addLayout(textLayout);
    basicConditionLayout->addWidget(submit);
    basicConditionBox->setLayout(basicConditionLayout);
    keyboardLayout->addWidget(basicConditionBox);

    keyboardBox->setLayout(keyboardLayout);

    // הוספת ה-QGroupBoxים ל-layout הראשי
    this->addWidget(screenBox);    // מסך מחשב
    this->addWidget(keyboardBox);  // מקלדת

    andBtn->setStyleSheet("background-color: #3498db; color: white; font-size: 18px;");
    andBtn->setFixedSize(80, 40);
    orBtn->setStyleSheet("background-color: #3498db; color: white; font-size: 18px;");
    orBtn->setFixedSize(80, 40);
    skip->setStyleSheet("background-color: rgb(13, 206, 13); color: white; font-size: 18px;");
    skip->setFixedSize(85, 40);
    reset->setStyleSheet("background-color: rgb(13, 206, 13); color: white; font-size: 18px;");
    reset->setFixedSize(85, 40);
    textBox->setFixedHeight(30);
    submit->setFixedSize(200, 30);

    operators->setFixedSize(200, 30);
    sensorsFields->setFixedSize(200, 30);

    for (const auto &sensor : sensorList) {
        sensors->addItem(sensor.first);
    }
    for (const auto &op : operatorList) {
        operators->addItem(op);
    }

}

void Condition::connectSignals()
{
    // חיבור כל כפתור לפונקציה
    connect(andBtn, &QPushButton::clicked,
                     [&]() { buttonClickHandler(andBtn); });
    connect(orBtn, &QPushButton::clicked,
                     [&]() { buttonClickHandler(orBtn); });
    connect(skip, &QPushButton::clicked, [&](){ skipHandler(); });
    connect(reset, &QPushButton::clicked, [&]() { resetButtonState(); });
    connect(submit, &QPushButton::clicked, [&](){ submitHandler(); });
    connect(sensors, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &Condition::sensorSelectionHandler);

    connect(operators, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &Condition::operatorSelectionHandler);


    // טיימר לעדכון הצגת הסמן
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]() {
        isCursorVisible = !isCursorVisible;  // החלפת מצב הסימון
        updateDisplay();                     // עדכון התצוגה
    });
    timer->start(500);
}

// פונקציה לעדכון ה-QLabel עם הסימון "|"
void Condition::updateDisplay()
{
    updateSensorComboBoxState();  // Update sensor combo box state
    updateSkipButtonState(); // Update skip button visibility

    QString displayedShowCondition = showCondition;  // Get the current display string
    if (isCursorVisible) {
        // Insert cursor at the current index
        displayedShowCondition.insert(ind, "|");
    }
    else {
        // Insert space at the current index
        displayedShowCondition.insert(ind, " ");
    }
    label->setText(displayedShowCondition); // Update the QLabel with the new text

    label1->setText(condition);  // Update the QLabel with the internal condition
}


// פונקציה כללית שמופעלת על כל כפתור שנלחץ
void Condition::buttonClickHandler(QPushButton *button)
{
    QString buttonText = "";
    int tempInd = ind;  // Store current index
    if (typeCurrent.first == "if" || typeCurrent.first == "") {
        // Insert new condition block
        buttonText += "(  )";
        ind += 2;
        condition.insert(indCondition,
                        operatorsMap[button->text()] + "(,)");
        indCondition += 2;
        layersStack.push({button->text(), layersStack.top().second});
    }
    else {
        ind += button->text().length() + 4;
        condition.insert(indCondition, ",");
        indCondition++;
        layersStack.push(typeCurrent);
    }
    if(typeCurrent.second == -1)
        typeCurrent.second = layersStack.top().second;

    typeCurrent.first = "";
    buttonText += " " + button->text() + " (  )";  // Get button text
    showCondition.insert(tempInd, buttonText);
    updateDisplay();  // Update display after button click
    updateButtonVisible();  // Update button visibility
}

void Condition::skipHandler()
{
    // Check if the cursor is at the end of the string
    if (ind == showCondition.length() - 2)
        return;

    // Find the nearest closing parenthesis
    ind = showCondition.toStdString().find(')', ind);

    if (ind != showCondition.length() - 2 && showCondition[ind + 2].isLetter()) {
        typeCurrent = {"", layersStack.top().second};
        ind = showCondition.toStdString().find('(', ind);
        ind++;
    }
    else {
        if(typeCurrent.first != "" ) {
            typeCurrent = layersStack.top();
            layersStack.pop();
        }
    }
    
    ind++;

    if (showCondition[ind - 1] == ')') {
        indCondition = condition.toStdString().find(')', indCondition - 1) - 1;
    } else {
        indCondition = std::max(static_cast<int>(
                        condition.toStdString().find(',', indCondition)) + 1, indCondition);
    }
    updateButtonVisible();
    updateSensorComboBoxState();
    updateSkipButtonState();
    updateDisplay();
}

void Condition::resetButtonState()
{
    setupLogicalMembers();
    sensorSelectionHandler(0);
    updateSensorComboBoxState();
    updateSkipButtonState();
    updateDisplay();
}

void Condition::sensorSelectionHandler(int index)
{
    if (index > 0) {
        QString selectedSensor = sensors->currentText();
        typeCurrent.second = sensorList[selectedSensor];
        showCondition.insert(
            ind, "[ " + QString::number(sensorList[selectedSensor]) + " ]");
        ind += 4 + QString::number(sensorList[selectedSensor]).length();
        condition.insert(indCondition,
            "[" + QString::number(sensorList[selectedSensor]) + "]");
        indCondition +=
            2 + QString::number(sensorList[selectedSensor]).length();
        sensorsFields->clear();
        sensorsFields->addItem(QString("sensor %1 fields").arg(typeCurrent.second));
        sensorsFields->setCurrentIndex(0);
        for (const auto &sf : sensorsFieldsList[typeCurrent.second]) {
            sensorsFields->addItem(sf);
        }
        updateSensorComboBoxState();
        updateDisplay();
    }
}

void Condition::operatorSelectionHandler(int index)
{
    if (index > 0)
        updateDisplay();
    updateSensorComboBoxState();
}

void Condition::submitHandler()
{
    QString currentField = sensorsFields->currentText();

    QString currentOperator = operators->currentText();

    QString enteredText = textBox->text();
    textBox->setText("");

    QString finalCondition = currentField + " " + currentOperator + " " + enteredText;
    showCondition.insert(ind, finalCondition);
    ind += finalCondition.length() + 1;

    finalCondition = currentOperator + "(" + currentField + "," + enteredText + ")";
    condition.insert(indCondition, finalCondition);
    indCondition += finalCondition.length();
    typeCurrent.first = "Basic";
    skipHandler();
}

void Condition::updateSensorComboBoxState()
{
    sensors->setEnabled(typeCurrent.second == -1);
}

void Condition::updateSkipButtonState()
{
    skip->setVisible(!typeCurrent.first.isEmpty() || ind == showCondition.length() - 2);
}

void Condition::updateButtonVisible()
{
    andBtn->show();
    orBtn->show();
    if (typeCurrent.first == "AND") 
        orBtn->hide();
    else if (typeCurrent.first == "OR") 
        andBtn->hide();
}

void Condition::fillSensorsFields(map<int, string> pathesToJsonFiles)
{
    for(auto sensor : pathesToJsonFiles)
        sensorsFieldsList[sensor.first] = getFieldsOfSensor(sensor.second);
}

vector<QString> Condition::getFieldsOfSensor(string psthToSensorJson)
{
     // Read the json file
    ifstream f(psthToSensorJson);

    // Check if the input is correct
    if (!f.is_open()) {
        cerr << "Failed to open " << psthToSensorJson << endl;
    }
    json *data = NULL;
    // Try parse to json type and return it
    try {
        data = new json(json::parse(f));
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    catch (...) {
        cout << "My Unknown Exception" << endl;
    }

    vector<QString> fields;

    for(auto field : (*data)["fields"])
        fields.push_back(QString::fromStdString(field["name"]));

    return fields;
}

