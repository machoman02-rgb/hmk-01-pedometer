//there are two codes buried in this they both half work but i dont have time to fix it because
//i am bad at judging how long this will take

#include <Arduino.h>
#include <math.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include <AceButton.h>
using namespace ace_button;

void setReports();
#define BNO08X_RESET -1
int pinD1 = 1;
AceButton button(pinD1);

enum displayMode {
  MODE_Steps,
  MODE_distance,
  MODE_adjustment,
  MODE_accel,
  MODE_COUNT
};

displayMode curMode = MODE_Steps;

float StepDistance = 24.0;

volatile long prevChangeTime = 0;
volatile long prevChangeTimetwo = 0;

long debounceTime = 50;

float accel = 0;

volatile bool changeButtonFlag = false;

void IRAM_ATTR buttonToChangeThings() {
  long now = millis();

  if (now > prevChangeTime + debounceTime) {
    changeButtonFlag = true;
    prevChangeTime = now;
  }
}

void ChangeMode(AceButton* button,uint8_t eventType, uint8_t buttonState) {
  Serial.print(F("handleEvent(): eventType: "));
  Serial.print(AceButton::eventName(eventType));
  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);
  if (eventType == (uint8_t)AceButton::kEventDoubleClicked){
    curMode = (displayMode)((curMode +1 ) % displayMode::MODE_COUNT);
  }
}

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  pinMode(pinD1,INPUT_PULLDOWN);
  button.init(pinD1,LOW);

  pinMode(2, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(2), buttonToChangeThings, RISING);

  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(ChangeMode);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);

  Serial.println("Adafruit BNO08x test!");

  // Try to initialize!
  if (!bno08x.begin_I2C()) {
    // if (!bno08x.begin_UART(&Serial1)) {  // Requires a device with > 300 byte
    // UART buffer! if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
    Serial.println("Failed to find BNO08x chip");
    while (1) {
      delay(10);
    }
  }
  if (!bno08x.enableReport(SH2_STEP_COUNTER)) {
    Serial.println("Could not enable step counter");
  }

  Serial.println("BNO08x Found!");

  setReports();
}

void loop(){
  button.check();
  delay(10);

  if (bno08x.wasReset()) {
    Serial.print("sensor was reset ");
    setReports();
  }

  if (!bno08x.getSensorEvent(&sensorValue)) {
    return;
  }
  switch (sensorValue.sensorId) {

  /*
  Serial.print(" Accelerometer - x: ");
  Serial.print(x);
  Serial.print(" y: ");
  Serial.print(y);
  Serial.print(" z: ");
  Serial.println(z);
  */
 
 if (curMode == displayMode::MODE_Steps){
  case SH2_STEP_COUNTER:
    Serial.print("Step Counter - steps: ");
    Serial.print(sensorValue.un.stepCounter.steps);
    Serial.print(" latency: ");
    Serial.println(sensorValue.un.stepCounter.latency);
    break;
  }
  if (curMode== displayMode::MODE_distance){
    Serial.print(" distance travelled ");
    Serial.println(sensorValue.un.stepCounter.steps*StepDistance);
  }

  if (curMode== displayMode::MODE_adjustment){
    Serial.print(" adjustment of stride length ");
      if (changeButtonFlag){
         StepDistance += 6.0;
      }
      if (StepDistance > 48){
       StepDistance = StepDistance - 36;
      }
    Serial.print(StepDistance);
    Serial.print(" Inches ");
  }
  if (curMode== displayMode::MODE_accel){
  case SH2_RAW_ACCELEROMETER:
    Serial.print("Raw Accelerometer - x: ");
    Serial.print(sensorValue.un.rawAccelerometer.x);
    Serial.print(" y: ");
    Serial.print(sensorValue.un.rawAccelerometer.y);
    Serial.print(" z: ");
    Serial.println(sensorValue.un.rawAccelerometer.z);
    break;
  }
}


void setReports(void) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_ACCELEROMETER)) {
    Serial.println("Could not enable accelerometer");
  } else {
    Serial.println("Set accelerometer report... success!");
  }
}


#include <Arduino.h>
#include <math.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include <AceButton.h>

using namespace ace_button;

void setReports();

#define BNO08X_RESET -1

// -----------------------------
// BUTTON SETUP
// -----------------------------

int pinD1 = 1;
int pinD2 = 2;

AceButton button(pinD1);


// -----------------------------
// DISPLAY MODES
// -----------------------------

enum displayMode {
  MODE_Steps,
  MODE_distance,
  MODE_adjustment,
  MODE_accel,
  MODE_COUNT
};

displayMode curMode = MODE_Steps;


// -----------------------------
// VARIABLES
// -----------------------------

float StepDistance = 24.0;

volatile long prevChangeTime = 0;
volatile long prevChangeTimetwo = 0;

long debounceTime = 50;

float accel = 0;

volatile bool changeButtonFlag = false;


// -----------------------------
// D2 BUTTON INTERRUPT
// Changes stride distance
// -----------------------------

void IRAM_ATTR buttonToChangeThings() {

  long now = millis();

  if (now > prevChangeTime + debounceTime) {

    changeButtonFlag = true;

    prevChangeTime = now;
  }
}


// -----------------------------
// D1 BUTTON EVENT
// Double-click changes display mode
// -----------------------------

void ChangeMode(
  AceButton* button,
  uint8_t eventType,
  uint8_t buttonState
) {

  Serial.print(F("handleEvent(): eventType: "));
  Serial.print(AceButton::eventName(eventType));

  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);

  if (eventType == (uint8_t)AceButton::kEventDoubleClicked) {

    curMode = (displayMode)(
      (curMode + 1) % displayMode::MODE_COUNT
    );

    Serial.print("Display mode changed to: ");
    Serial.println((int)curMode);
  }
}


// -----------------------------
// BNO08x SETUP
// -----------------------------

Adafruit_BNO08x bno08x(BNO08X_RESET);

sh2_SensorValue_t sensorValue;


// -----------------------------
// SETUP
// -----------------------------

void setup(void) {

  Serial.begin(115200);

  while (!Serial) {
    delay(10);
  }

  Serial.println();
  Serial.println("BNO08x Step Counter Program");
  Serial.println("---------------------------");


  // ---------------------------
  // D1 BUTTON
  // ---------------------------

  pinMode(pinD1, INPUT_PULLDOWN);

  button.init(pinD1, LOW);

  ButtonConfig* buttonConfig = button.getButtonConfig();

  buttonConfig->setEventHandler(ChangeMode);

  buttonConfig->setFeature(
    ButtonConfig::kFeatureDoubleClick
  );


  // ---------------------------
  // D2 BUTTON
  // ---------------------------

  pinMode(pinD2, INPUT_PULLDOWN);

  attachInterrupt(
    digitalPinToInterrupt(pinD2),
    buttonToChangeThings,
    RISING
  );


  // ---------------------------
  // BNO08x
  // ---------------------------

  Serial.println("Initializing BNO08x...");

  if (!bno08x.begin_I2C()) {

    Serial.println("Failed to find BNO08x chip");

    while (1) {
      delay(10);
    }
  }

  Serial.println("BNO08x Found!");


  // ---------------------------
  // ENABLE SENSOR REPORTS
  // ---------------------------

  setReports();
}


// -----------------------------
// MAIN LOOP
// -----------------------------

void loop() {

  // Check D1 button
  button.check();

  delay(10);


  // Check if BNO08x reset
  if (bno08x.wasReset()) {

    Serial.println("Sensor was reset");

    setReports();
  }


  // Get sensor data
  if (!bno08x.getSensorEvent(&sensorValue)) {
    return;
  }


  // ---------------------------
  // DETERMINE SENSOR DATA TYPE
  // ---------------------------

  switch (sensorValue.sensorId) {


    // =================================
    // STEP COUNTER
    // =================================

    case SH2_STEP_COUNTER:

      // -------------------------------
      // STEP MODE
      // -------------------------------

      if (curMode == displayMode::MODE_Steps) {

        Serial.print("Step Counter - steps: ");

        Serial.print(
          sensorValue.un.stepCounter.steps
        );

        Serial.print("   latency: ");

        Serial.println(
          sensorValue.un.stepCounter.latency
        );
      }


      // -------------------------------
      // DISTANCE MODE
      // -------------------------------

      if (curMode == displayMode::MODE_distance) {

        float distance =
          sensorValue.un.stepCounter.steps
          * StepDistance;

        Serial.print("Distance travelled: ");

        Serial.print(distance);

        Serial.println(" inches");
      }


      // -------------------------------
      // STRIDE ADJUSTMENT MODE
      // -------------------------------

      if (curMode == displayMode::MODE_adjustment) {

        // D2 was pressed
        if (changeButtonFlag) {

          StepDistance += 6.0;

          changeButtonFlag = false;


          // Reset to 12 inches
          // after going above 48
          if (StepDistance > 48.0) {

            StepDistance -= 36.0;
          }
        }


        Serial.print("Stride length: ");

        Serial.print(StepDistance);

        Serial.println(" inches");

        Serial.println(
          "Press D2 to increase stride length"
        );
      }

      break;


    // =================================
    // RAW ACCELEROMETER
    // =================================

    case SH2_RAW_ACCELEROMETER:

      if (curMode == displayMode::MODE_accel) {

        Serial.print("Raw Accelerometer - X: ");

        Serial.print(
          sensorValue.un.rawAccelerometer.x
        );

        Serial.print("   Y: ");

        Serial.print(
          sensorValue.un.rawAccelerometer.y
        );

        Serial.print("   Z: ");

        Serial.println(
          sensorValue.un.rawAccelerometer.z
        );
      }

      break;
  }
}


// -----------------------------
// ENABLE BNO08x REPORTS
// -----------------------------

void setReports(void) {

  Serial.println("Setting desired reports");


  // ---------------------------
  // STEP COUNTER
  // ---------------------------

  if (!bno08x.enableReport(SH2_STEP_COUNTER)) {

    Serial.println(
      "Could not enable step counter"
    );

  } else {

    Serial.println(
      "Set step counter report... success!"
    );
  }


  // ---------------------------
  // RAW ACCELEROMETER
  // ---------------------------

  if (!bno08x.enableReport(SH2_RAW_ACCELEROMETER)) {

    Serial.println(
      "Could not enable raw accelerometer"
    );

  } else {

    Serial.println(
      "Set raw accelerometer report... success!"
    );
  }
}