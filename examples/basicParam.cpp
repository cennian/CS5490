#include <Arduino.h>
#include "CS5490.h"

// Create an instance of CS5490, using Serial1 for the CS5490 (ESP32-S3 has multiple UARTs).
// Adjust the serial port and pins as needed for your connection.
CS5490 energyMeter(Serial1);

void setup() {
    Serial.begin(115200);
    // Initialize CS5490 communication, switch to 19200 baud for faster communication.
    // Enable debug output for demonstration (optional).
    energyMeter.begin(19200, true);

    // Ensure the chip is awake (in case it was in standby)
    energyMeter.sendInstruction(CS5490::WAKEUP);
    // Start continuous conversion mode to continuously update measurement registers
    energyMeter.sendInstruction(CS5490::CONT_CONV);

    // (Optional: If calibration is needed, you would call calibration instructions here, 
    // for example: energyMeter.sendInstruction(CS5490::I_AC_CAL) after applying known calibration signals.)
}

void loop() {
    // Read various measurements from the CS5490
    float voltage_rms   = energyMeter.readRegister(CS5490::V_RMS);    // Voltage RMS (in volts)
    float current_rms   = energyMeter.readRegister(CS5490::I_RMS);    // Current RMS (in amps)
    float power_avg     = energyMeter.readRegister(CS5490::P_AVG);    // Active power (in watts)
    float power_factor  = energyMeter.readRegister(CS5490::PF);       // Power factor (dimensionless, -1.0 to 1.0)
    float temperature   = energyMeter.readRegister(CS5490::TEMP);     // Internal temperature (in °C, default scaling)
    // Compute line frequency using Epsilon register: lineFreq = Epsilon * 4000 (OWR)
    float epsilon       = energyMeter.readRegister(CS5490::EPSILON);  // Epsilon = lineFreq / 4000
    float line_freq_hz  = epsilon * 4000.0f;

    // Print the measured values to the serial monitor
    Serial.print("V_RMS: "); Serial.print(voltage_rms, 2); Serial.print(" V, ");
    Serial.print("I_RMS: "); Serial.print(current_rms, 3); Serial.print(" A, ");
    Serial.print("P_AVG: "); Serial.print(power_avg, 2); Serial.print(" W, ");
    Serial.print("PF: "); Serial.print(power_factor, 3);
    Serial.print(", Freq: "); Serial.print(line_freq_hz, 2); Serial.print(" Hz, ");
    Serial.print("Temp: "); Serial.print(temperature, 2); Serial.println(" C");

    delay(1000); // Read and print every 1 second
}

