#define DEBUG_MODE true  // Set to false when exporting to Excel

#define NumOfBatteries 3
#define CapChargeTime 100
#define BatChargeTime 10000
#define HighestValue 1027
#define FlybackThreshold 5

// To Store the battery reading
int BatteryReadingValues[NumOfBatteries];

// These Arrays contain the pins of every catogary
int transistor[NumOfBatteries] = {2,3,4};
int opto1[NumOfBatteries] = {5,6,7};
int opto2[NumOfBatteries] ={8,9,10};

// Analog and its Enable that controls the reading the battery values 
int BatteryReadingPin = A0;
int EnableAnalogRead = 11;

// Flyback Control
int FlybackEnable = 12;
int FlybackReadingPin = A1;

// Variable that help in finding the Lowest battey 
int LowestIndex = 0;
int LowestCharge = HighestValue;

void DebugLog(String message) {
  if (DEBUG_MODE) {
    Serial.println(message);
  }
}

void TurnOffAllOpto(){
  for(int i=0; i < NumOfBatteries;i++){
    digitalWrite(opto1[i], LOW);
    digitalWrite(opto2[i], LOW);
    //digitalWrite(transistor[i], LOW);
  }
}

void TurnOffAllTransistors(){
  for(int i=0; i < NumOfBatteries;i++){
    digitalWrite(transistor[i], LOW);
  }
}

void ReadBattery(int index){
  // First Turn Off all optocouplers to make sure it is turned off
  TurnOffAllOpto();
  // Open the Optocouplers of each battery
  digitalWrite(opto1[index],HIGH);
  digitalWrite(opto2[index],HIGH);
  // Delay to make some time to the capacitor to charge
  delay(CapChargeTime);
  // Disable the optocoupler that is connected to the batteries
  digitalWrite(opto2[index],LOW);
  // Enable the opto that insulate the Analog pin
  digitalWrite(EnableAnalogRead,HIGH);
  // Reading the voltage of the selected Capacitor and Store the it
  BatteryReadingValues[index] = analogRead(BatteryReadingPin);
  // Disable the opto that insulate the Analog pin
  digitalWrite(EnableAnalogRead,LOW);
  digitalWrite(opto1[index],LOW);
}

void ReadAll(){
  for (int index = 0; index < NumOfBatteries; index++) {
  ReadBattery(index);
  }
}

void CheckLowestBattery(){
  // Reset the LowestCharge
  LowestCharge = HighestValue;

  // Find the lowest charge battery's Value and Index
  for (int index = 0; index < NumOfBatteries; index++) {
    if (BatteryReadingValues[index] < LowestCharge) {
    LowestCharge = BatteryReadingValues[index];
    LowestIndex = index;
    }
  }

  // Turn Off all transistors
  TurnOffAllTransistors();
  // Turn on the transistor of the lowest battery to charge it
  digitalWrite(transistor[LowestIndex], HIGH);
}

void FlybackControl() {
  // Read the flyback output voltage (convert analog to voltage)
  int flybackRawValue = analogRead(FlybackReadingPin);
  float flybackVoltage = (flybackRawValue / 1023.0) * 5.0; // Assuming a 5V reference

  // Debugging: Print the flyback voltage
  DebugLog("Flyback Voltage: ");
  DebugLog(flybackVoltage);

  // Check if the voltage is below the threshold
  if (flybackVoltage < FlybackThreshold) {
    // Turn ON the flyback
    digitalWrite(FlybackEnable, HIGH);
    DebugLog("Flyback ON");
  } else {
    // Turn OFF the flyback
    digitalWrite(FlybackEnable, LOW);
    DebugLog("Flyback OFF");
  }
}


void setup() {
  for (int index = 0; index < NumOfBatteries; index++) {
    pinMode(transistor[index], OUTPUT);
    pinMode(opto1[index], OUTPUT);
    pinMode(opto2[index], OUTPUT);
  }

  pinMode(EnableAnalogRead, OUTPUT);
  pinMode(FlybackEnable, OUTPUT);
  
  pinMode(BatteryReadingPin, INPUT);
  pinMode(FlybackReadingPin, INPUT);
}

void ExportToExcel() {
  // Print battery readings
  Serial.print("Battery Readings: ");
  for (int i = 0; i < NumOfBatteries; i++) {
    Serial.print(BatteryReadingValues[i]);
    if (i < NumOfBatteries - 1) Serial.print(", ");
  }

  // Print flyback voltage
  int flybackRawValue = analogRead(FlybackReadingPin);
  float flybackVoltage = (flybackRawValue / 1023.0) * 5.0;
  Serial.print(", Flyback Voltage: ");
  Serial.println(flybackVoltage);
}

void loop() {
  // Reading and processing logic
  ReadAll();
  CheckLowestBattery();
  FlybackControl();

  // Debugging or Export Logic
  if (DEBUG_MODE) {
    DebugLog("Debug: Battery reading completed.");
  } else {
    ExportToExcel();  // Export data to Excel-compatible format
  }

  delay(1000);  // Delay for readability
}