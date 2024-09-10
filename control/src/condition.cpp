#include "condition.h"

#define BUTTON_SIZE  80, 40
#define BASE_CON_PART_HEIGHT 30
#define BASE_CON_PART_WIDTH 200

using namespace std;

Condition::Condition(MainWindow *mainWindow) : mainWindow(mainWindow)
{
    setupLogicalMembers();
    setupUi();
    connectSignals();
}

// Destructor for the Condition class: Handles cleanup and saving of conditions
Condition::~Condition()
{
    // Add the condition to the bson
    cout << "Add the condition to the bson" << endl;
    Output &output = Output::getInstance();
    output.addNewCondition(condition.toStdString());

    // Clear layout items
    QLayoutItem *item;
    while ((item = this->takeAt(0)) != nullptr) {
        QWidget *widget = item->widget();
        if (widget) {
            widget->setParent(nullptr);
        }
        delete item;
    }
    delete selectActions;
    mainWindow->saveCondition(showCondition);
}

// Sets up initial conditions and data structures
void Condition::setupLogicalMembers()
{
    // Initialize condition strings and cursor position
    showCondition = "if (  )";
    condition = "";
    ind = 5;
    indCondition = 0;
    isCursorVisible = true;

    // Initialize current operation type and stack
    typeCurrent = {"if", -1};
    layersStack = stack<pair<QString, int>>();
    layersStack.push(typeCurrent);

    // Initialize sensor and operator lists
    sensorList = {{"Speed", 1},
                  {"Tire Pressure", 2},
                  {"Communication", 3},
                  {"Camera", 4}};
    operatorsMap = {{"AND", "&"}, {"OR", "|"}};
    operatorList = {"=", "!=", ">", "<", "<=", ">="};

    // Map sensor IDs to JSON files
    fillSensorsFields({{1, "sensors_data/speed_sensor.json"},
                       {2, "sensors_data/tire_pressure_sensor.json"},
                       {3, "sensors_data/communication_sensor.json"},
                       {4, "sensors_data/camera_sensor.json"}});
}

// Function that set up the UI components and layout for managing conditions and actions in the Condition class
void Condition::setupUi()
{
    // Create a read-only text label and set font
    label = new QTextEdit("");
    label->setReadOnly(true);
    QFont font("Arial", 25);
    label->setFont(font);

    // Create another label with the same font
    label1 = new QLabel("");
    label1->setFont(font);

    // Set up a text cursor and color formats
    cursor = new QTextCursor(label->document());
    formatRed.setForeground(Qt::red);
    formatGreen.setForeground(QColor("#00B521"));
    formatBlue.setForeground(Qt::blue);

    // Create a screen section (QGroupBox)
    QGroupBox *screenBox = new QGroupBox("");
    QVBoxLayout *screenLayout = new QVBoxLayout;
    screenLayout->addWidget(label);
    screenLayout->addWidget(label1);
    screenBox->setLayout(screenLayout);

    // Initialize buttons, text box, and combo boxes
    andBtn = new QPushButton("AND");
    orBtn = new QPushButton("OR");
    skip = new QPushButton("->");
    reset = new QPushButton("reset");
    textBox = new QLineEdit();
    submit = new QPushButton("add to condition");

    sensors = new QComboBox();
    sensors->addItem("Sensors");  // Placeholder
    sensors->setItemData(0, QVariant(0),
                         Qt::UserRole - 1);  // Disable placeholder
    sensors->setCurrentIndex(0);

    sensorsFields = new QComboBox();
    sensorsFields->addItem("sensors fields");  // Placeholder
    sensorsFields->setItemData(0, QVariant(0),
                               Qt::UserRole - 1);  // Disable placeholder
    sensorsFields->setCurrentIndex(0);

    operators = new QComboBox();
    operators->addItem("Operators");  // Placeholder
    operators->setItemData(0, QVariant(0),
                           Qt::UserRole - 1);  // Disable placeholder
    operators->setCurrentIndex(0);

    // Create a keyboard section (QGroupBox) with buttons
    QGroupBox *keyboardBox = new QGroupBox("");
    QVBoxLayout *keyboardLayout = new QVBoxLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(andBtn);
    buttonLayout->addWidget(orBtn);
    buttonLayout->addWidget(skip);
    buttonLayout->addWidget(reset);
    keyboardLayout->addLayout(buttonLayout);
    keyboardLayout->addWidget(sensors);

    // Create a condition input section (QGroupBox)
    QGroupBox *basicConditionBox =
        new QGroupBox("---create basic condition---");
    QVBoxLayout *basicConditionLayout = new QVBoxLayout;
    QHBoxLayout *textLayout = new QHBoxLayout;
    textLayout->addWidget(sensorsFields);
    textLayout->addWidget(operators);
    textLayout->addWidget(textBox);
    basicConditionLayout->addLayout(textLayout);
    basicConditionLayout->addWidget(submit);
    basicConditionBox->setLayout(basicConditionLayout);
    keyboardLayout->addWidget(basicConditionBox);

    keyboardBox->setLayout(keyboardLayout);

    // Add action button
    selectActions = new QPushButton("Pass to selection of actions");
    selectActions->setFixedSize(200, 50);

    // Add the screen and keyboard sections to the main layout
    this->addWidget(screenBox);
    this->addWidget(keyboardBox);
    this->addWidget(selectActions);

    // Style buttons and set sizes
    andBtn->setStyleSheet(
        "background-color: #3498db; color: white; font-size: 18px;");
    andBtn->setFixedSize(BUTTON_SIZE);
    orBtn->setStyleSheet(
        "background-color: #3498db; color: white; font-size: 18px;");
    orBtn->setFixedSize(BUTTON_SIZE);
    skip->setStyleSheet(
        "background-color: rgb(13, 206, 13); color: white; font-size: 18px;");
    skip->setFixedSize(BUTTON_SIZE);
    reset->setStyleSheet(
        "background-color: rgb(13, 206, 13); color: white; font-size: 18px;");
    reset->setFixedSize(BUTTON_SIZE);
    textBox->setFixedHeight(BASE_CON_PART_HEIGHT);
    submit->setFixedSize(BASE_CON_PART_WIDTH, BASE_CON_PART_HEIGHT);
    operators->setFixedSize(BASE_CON_PART_WIDTH, BASE_CON_PART_HEIGHT);
    sensorsFields->setFixedSize(BASE_CON_PART_WIDTH, BASE_CON_PART_HEIGHT);

    // Populate combo boxes with sensor and operator lists
    for (const auto &sensor : sensorList) {
        sensors->addItem(sensor.first);
    }
    for (const auto &op : operatorList) {
        operators->addItem(op);
    }
}

// Function that connect signals to their corresponding slots or handlers
void Condition::connectSignals()
{
    // Connect each button to its respective handler
    connect(andBtn, &QPushButton::clicked,
            [&]() { buttonClickHandler(andBtn); });
    connect(orBtn, &QPushButton::clicked, [&]() { buttonClickHandler(orBtn); });
    connect(skip, &QPushButton::clicked, this, &Condition::skipHandler);
    connect(reset, &QPushButton::clicked, this, &Condition::resetButtonState);
    connect(submit, &QPushButton::clicked, this, &Condition::submitHandler);

    // Connect combo boxes to selection handlers
    connect(sensors, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &Condition::sensorSelectionHandler);
    connect(operators, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Condition::operatorSelectionHandler);

    // Connect action button to the main window transition function
    connect(selectActions, &QPushButton::clicked, this,
            [this]() { mainWindow->goNext(); });

    // Timer to toggle cursor visibility and update the display
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]() {
        isCursorVisible = !isCursorVisible;  // Toggle cursor visibility
        updateDisplay();                     // Refresh display
    });
    timer->start(500);  // Timer set to 500 ms interval
}

// Function for update the cursor and the text
void Condition::updateDisplay()
{
    updateSensorComboBoxState();  // Update sensor combo box state

    updateSkipButtonState();      // Update skip button visibility

    QString displayedShowCondition =
        showCondition;  // Get the current display string
    if (isCursorVisible) {
        // Show cursor at the current index
        displayedShowCondition.insert(ind, "|");
    }
    else {
        // Show space at the current index
        displayedShowCondition.insert(ind, " ");
    }
    label->setPlainText(
        displayedShowCondition);  // Update the label with modified text
    updateColors();               // Update color formatting
    label->show();                // Display updated label

    label1->setText(
        condition);  // Update the secondary label with the condition text

    // Control visibility of selectActions button based on condition length and cursor position
    bool enableSelectActions =
        !condition.isEmpty() && ind >= showCondition.length() - 3 &&
        (showCondition[ind - 1] == ')' || typeCurrent.first == "Basic");
    
    selectActions->setVisible(enableSelectActions);  // Show/Hide action button

    label->setTextInteractionFlags(
        Qt::NoTextInteraction);  // Disable text interaction
}

// General function triggered when the Or/And button is clicked
void Condition::buttonClickHandler(QPushButton *button)
{
    QString buttonText = "";  // Temporary string for the button text
    int tempInd = ind;        // Save the current index

    // If it’s the first click in the current condition block
    if (typeCurrent.first == "if" || typeCurrent.first == "") {
        QTextCharFormat formatRed;
        formatRed.setForeground(Qt::red);
        // Insert a new condition block and adjust index
        buttonText += "(  )";
        ind += 2;
        condition.insert(indCondition, operatorsMap[button->text()] + "(,)");
        indCondition += 2;
        layersStack.push(
            {button->text(), typeCurrent.second});  // Save to layers stack
    }
    else {
        ind += button->text().length() + 4;   // Update index for button text
        condition.insert(indCondition, ",");  // Insert separator in condition
        indCondition++;
        layersStack.push(typeCurrent);  // Push current type onto stack
    }

    if (typeCurrent.second == -1)
        typeCurrent.second =
            layersStack.top().second;  // Update layer depth if needed

    typeCurrent.first = "";  // Reset current type
    buttonText += " " + button->text() +
                  " (  )";  // Add button text and condition brackets
    showCondition.insert(
        tempInd, buttonText);  // Insert the new text into the display string
    updateDisplay();           // Refresh the display
    updateButtonVisible();     // Adjust button visibility based on new state
}

// Move the cursor to the next relevant position in the condition string and updates the UI accordingly
void Condition::skipHandler()
{
    // Check if the cursor is at the end of the string
    if (ind >= showCondition.length() - 2) {
        ind = showCondition.length() - 2;
        return;
    }

    // Find the nearest closing parenthesis
    ind = showCondition.toStdString().find(')', ind);

    // If a letter follows the closing parenthesis, update type and move to the next open parenthesis
    if (ind != showCondition.length() - 2 &&
        showCondition[ind + 2].isLetter()) {
        typeCurrent = {"", layersStack.top().second};
        ind = showCondition.toStdString().find('(', ind);
        ind++;
    }
    else {
        // Update current type based on the top of the stack if applicable
        if (typeCurrent.first != "") {
            typeCurrent = layersStack.top();
            layersStack.pop();
        }
    }

    ind++;  // Move cursor one position forward

    // Adjust indCondition if not at a closing parenthesis
    if (showCondition[ind - 1] != ')')
        indCondition = std::max(
            static_cast<int>(condition.toStdString().find(',', indCondition)) +
                1,
            indCondition);

    // Update UI components
    updateButtonVisible();
    updateSensorComboBoxState();
    updateSkipButtonState();
    updateDisplay();
}

// Reset all logical members and UI components to their default state
void Condition::resetButtonState()
{
    // Reset all logical members and UI components to default states
    setupLogicalMembers();              // Reset condition logic
    sensors->setCurrentIndex(0);        // Reset sensor selection
    sensorsFields->setCurrentIndex(0);  // Reset sensor fields
    operators->setCurrentIndex(0);      // Reset operator selection
    textBox->clear();                   // Clear input field
    sensorSelectionHandler(0);          // Reset sensor handler
    updateButtonVisible();              // Update button visibility
    updateSensorComboBoxState();        // Update sensor combo box state
    updateSkipButtonState();            // Update skip button state
    updateDisplay();                    // Refresh the display
}

// Handle sensor selection, updating the condition display and related UI elements based on the selected sensor
void Condition::sensorSelectionHandler(int index)
{
    if (index > 0) {
        // Handle sensor selection and update condition display
        QString selectedSensor = sensors->currentText();
        typeCurrent.second = sensorList[selectedSensor];  // Update sensor type
        showCondition.insert(
            ind, "[ " + QString::number(sensorList[selectedSensor]) + " ]");
        ind += 4 + QString::number(sensorList[selectedSensor])
                       .length();  // Update index

        // Update condition string
        condition.insert(
            indCondition,
            "[" + QString::number(sensorList[selectedSensor]) + "]");
        indCondition +=
            2 + QString::number(sensorList[selectedSensor]).length();

        // Update sensor fields for the selected sensor
        sensorsFields->clear();
        sensorsFields->addItem(
            QString("sensor %1 fields").arg(typeCurrent.second));
        sensorsFields->setItemData(0, QVariant(0),
                                   Qt::UserRole - 1);  // Disable placeholder

        sensorsFields->setCurrentIndex(0);  // Reset fields to default
        for (const auto &sf : sensorsFieldsList[typeCurrent.second]) {
            sensorsFields->addItem(sf);  // Add sensor fields to combo box
        }

        operators->setCurrentIndex(0);  // Reset operators to placeholder

        // Update UI state and display
        updateSensorComboBoxState();
        updateDisplay();
    }
}

// Update the display and sensor combo box state when an operator is selected
void Condition::operatorSelectionHandler(int index)
{
    // If a valid operator is selected, update the display
    if (index > 0)
        updateDisplay();
    updateSensorComboBoxState();  // Update the state of the sensor combo box
}

// Processe and submit the current condition based on selected field, operator, and entered text, then resets UI inputs
void Condition::submitHandler()
{
    // Get the current field, operator, and entered text
    QString currentField = sensorsFields->currentText();
    QString currentOperator = operators->currentText();
    QString enteredText = textBox->text();

    // Form the final condition string and update the display condition
    QString finalCondition =
        currentField + " " + currentOperator + " " + enteredText;
    showCondition.insert(ind, finalCondition);  // Insert into display string
    ind += finalCondition.length() + 1;         // Move the cursor forward

    // Update the internal condition string
    finalCondition =
        currentOperator + "(" + currentField + "," + enteredText + ")";
    condition.insert(indCondition,
                     finalCondition);         // Insert into internal condition
    indCondition += finalCondition.length();  // Move the internal index forward
    typeCurrent.first = "Basic";              // Set the current type
    // Reset UI for next input
    sensorsFields->setCurrentIndex(0);
    operators->setCurrentIndex(0);
    textBox->clear();

    updateButtonVisible();  // Update button visibility
    skipHandler();          // Handle the skip button
}

// Update the state of sensor and condition-related combo boxes and buttons based on the current condition context
void Condition::updateSensorComboBoxState()
{
    // Enable basic condition widgets if conditions are met
    bool enableBasicCondition =
        (typeCurrent.second != -1 && typeCurrent.first != "AND" &&
         typeCurrent.first != "OR") &&
        !(layersStack.size() <= 1 && typeCurrent.first == "Basic");

    // Enable the submit button if all conditions for submission are met
    bool enableSUbmit =
        typeCurrent.second != -1 && sensorsFields->currentIndex() != 0 &&
        operators->currentIndex() != 0 && !textBox->text().isEmpty();

    sensors->setEnabled(typeCurrent.second ==
                        -1);  // Enable sensors if applicable

    // Enable or disable basic condition inputs
    sensorsFields->setEnabled(enableBasicCondition);
    operators->setEnabled(enableBasicCondition);
    textBox->setEnabled(enableBasicCondition);
    submit->setEnabled(enableSUbmit);  // Enable submit button

    // Reset sensors combo box to default if enabled
    if (sensors->isEnabled()) {
        sensors->setCurrentIndex(0);  // Set to placeholder item
    }
}

// Update the visibility of the skip button depending on the cursor position and current condition
void Condition::updateSkipButtonState()
{
    // Disable skip button if no condition or at the end of the string
    bool disableSkip =
        (typeCurrent.first.isEmpty() || ind >= showCondition.length() - 2);
    skip->setVisible(!disableSkip);  // Show or hide skip button
}

// Toggle visibility of AND/OR buttons based on the current condition context
void Condition::updateButtonVisible()
{
    // Hide AND/OR buttons if only a basic condition is present
    if (layersStack.size() <= 1 && typeCurrent.first == "Basic") {
        andBtn->hide();
        orBtn->hide();
    }
    else {
        andBtn->show();
        orBtn->show();
        // Hide the AND button if current type is OR and vice versa
        if (typeCurrent.first == "AND")
            orBtn->hide();
        else if (typeCurrent.first == "OR")
            andBtn->hide();
    }
}

// Apply color formatting to the text based on cursor position and condition elements
void Condition::updateColors()
{
    // Color the cursor- in ind position
    cursor->setPosition(ind);
    cursor->setPosition(ind + 1, QTextCursor::KeepAnchor);
    cursor->setCharFormat(formatBlue);

    // Color the closing bracket
    cursor->setPosition(ind + 2);
    cursor->setPosition(ind + 3, QTextCursor::KeepAnchor);
    cursor->setCharFormat(formatRed);

    // Color the opening bracket and the sensor
    int tempIndex = ind - 1, count = 0;
    bool current = true;
    while (tempIndex != 0) {
        if (showCondition[tempIndex] == ')')
            count++;
        else if (showCondition[tempIndex] == '(') {
            // Color the opening bracket
            if (!count && current) {
                current = false;
                cursor->setPosition(tempIndex);
                cursor->setPosition(tempIndex + 1, QTextCursor::KeepAnchor);
                cursor->setCharFormat(formatRed);
            }
            else
                count--;
        }
        // Color the current sensor
        else if (showCondition[tempIndex] == ']' && count <= 0) {
            cursor->setPosition(tempIndex - 5);
            cursor->setPosition(tempIndex + 1, QTextCursor::KeepAnchor);
            cursor->setCharFormat(formatGreen);
            tempIndex -= 5;
            if (!current)
                break;
        }
        // Color the operator
        else if (current && !count && typeCurrent.first != "Basic") {
            cursor->setPosition(tempIndex);
            cursor->setPosition(tempIndex + 1, QTextCursor::KeepAnchor);
            cursor->setCharFormat(formatRed);
        }
        tempIndex--;
    }
}

// Populate sensor fields list based on JSON file paths
void Condition::fillSensorsFields(map<int, string> pathesToJsonFiles)
{
    for (auto sensor : pathesToJsonFiles)
        sensorsFieldsList[sensor.first] = getFieldsOfSensor(sensor.second);
}

// Read fields from a sensor's JSON file
vector<QString> Condition::getFieldsOfSensor(string psthToSensorJson)
{
    // Read the json file
    ifstream f(psthToSensorJson);

    // Check if the input is correct
    if (!f.is_open())
        cerr << "Failed to open " << psthToSensorJson << endl;

    json *data = NULL;

    // Try parse to json type
    try {
        data = new json(json::parse(f));
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    catch (...) {
        cout << "My Unknown Exception" << endl;
    }

    // Read the fields and return them
    vector<QString> fields;

    for (auto field : (*data)["fields"])
        fields.push_back(QString::fromStdString(field["name"]));

    return fields;
}