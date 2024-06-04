#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS 1000
#define BASELINE_READINGS 100

PulseOximeter pox;
uint32_t tsLastReport = 0;
float baselineRed = 0;
float baselineIR = 0;

void onBeatDetected() {
    Serial.println("Beat!");
}

void calibrateBaseline() {
    Serial.println("Calibrating baseline...");
    long redSum = 0;
    long irSum = 0;

    for (int i = 0; i < BASELINE_READINGS; i++) {
        pox.update();
        redSum += pox.getRawIR();
        irSum += pox.getRawRed();
        delay(10);
    }

    baselineRed = redSum / BASELINE_READINGS;
    baselineIR = irSum / BASELINE_READINGS;

    Serial.print("Baseline Red: ");
    Serial.println(baselineRed);
    Serial.print("Baseline IR: ");
    Serial.println(baselineIR);
}

void setup() {
    Serial.begin(115200);
    Serial.print("Initializing pulse oximeter..");

    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

    pox.setOnBeatDetectedCallback(onBeatDetected);
    calibrateBaseline();
}

void loop() {
    pox.update();

    float currentRed = pox.getRawRed();
    float currentIR = pox.getRawIR();

    // Apply baseline calibration
    float calibratedRed = currentRed - baselineRed;
    float calibratedIR = currentIR - baselineIR;

    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2:");
        Serial.print(pox.getSpO2());
        Serial.print("% / Calibrated Red:");
        Serial.print(calibratedRed);
        Serial.print(" / Calibrated IR:");
        Serial.println(calibratedIR);

        tsLastReport = millis();
    }
}
