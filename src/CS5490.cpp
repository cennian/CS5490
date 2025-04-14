#include "CS5490.h"

// Structure to hold register info for debug (name and default raw value)
struct RegInfo { uint32_t reg; const char* name; uint32_t defaultVal; };
static const RegInfo regTable[] = {
    // Hardware Page 0
    { CS5490::CONFIG0,    "config0",    0xC02000 },
    { CS5490::CONFIG1,    "config1",    0x00EEEE },
    { CS5490::MASK,       "mask",       0x000000 },
    { CS5490::PC,         "phaseComp",  0x000000 },
    { CS5490::SERIALCTRL, "serialctrl", 0x02004D },
    { CS5490::PULSEWIDTH, "pulsewidth", 0x000001 },
    { CS5490::PULSECTRL,  "pulsectrl",  0x000000 },
    { CS5490::STATUS0,    "status0",    0x800000 },
    { CS5490::STATUS1,    "status1",    0x801800 },
    { CS5490::STATUS2,    "status2",    0x000000 },
    { CS5490::REGLOCK,    "reglock",    0x000000 },
    { CS5490::V_PEAK,     "v_peak",     0x000000 },
    { CS5490::I_PEAK,     "i_peak",     0x000000 },
    { CS5490::PSDC,       "psdc",       0x000000 },
    { CS5490::ZXNUM,      "zxnum",      0x000064 },
    // Software Page 16
    { CS5490::CONFIG2,    "config2",    0x100200 },
    { CS5490::REGCHK,     "regchk",     0x000000 },
    { CS5490::V_INST,     "v_inst",     0x000000 },
    { CS5490::I_INST,     "i_inst",     0x000000 },
    { CS5490::P_INST,     "p_inst",     0x000000 },
    { CS5490::P_AVG,      "p_avg",      0x000000 },
    { CS5490::I_RMS,      "i_rms",      0x000000 },
    { CS5490::V_RMS,      "v_rms",      0x000000 },
    { CS5490::Q_AVG,      "q_avg",      0x000000 },
    { CS5490::Q_INST,     "q_inst",     0x000000 },
    { CS5490::S,          "s",          0x000000 },
    { CS5490::PF,         "pf",         0x000000 },
    { CS5490::EPSILON,    "epsilon",    0x01999A },
    { CS5490::TEMP,       "temp",       0x000000 },
    { CS5490::P_SUM,      "p_sum",      0x000000 },
    { CS5490::S_SUM,      "s_sum",      0x000000 },
    { CS5490::Q_SUM,      "q_sum",      0x000000 },
    { CS5490::I_DCOFF,    "i_dcoff",    0x000000 },
    { CS5490::I_GAIN,     "i_gain",     0x400000 },
    { CS5490::V_DCOFF,    "v_dcoff",    0x000000 },
    { CS5490::V_GAIN,     "v_gain",     0x400000 },
    { CS5490::P_OFF,      "p_off",      0x000000 },
    { CS5490::I_ACOFF,    "i_acoff",    0x000000 },
    { CS5490::Q_OFF,      "q_off",      0x000000 },
    { CS5490::SAMPLECOUNT,"samplecount",0x000FA0 },
    { CS5490::T_GAIN,     "t_gain",     0x06B716 },
    { CS5490::T_OFF,      "t_off",      0x000000 },
    { CS5490::T_SETTLE,   "t_settle",   0x000000 },
    { CS5490::SYS_GAIN,   "sys_gain",   0x500000 },
    { CS5490::SYS_TIME,   "sys_time",   0x000000 },
    // Software Page 17
    { CS5490::VSAG_DUR,   "vsag_dur",   0x000000 },
    { CS5490::VSAG_LEVEL, "vsag_level", 0x000000 },
    { CS5490::IOVER_DUR,  "iover_dur",  0x000000 },
    { CS5490::IOVER_LEVEL,"iover_level",0x7FFFFF },
    // Software Page 18
    { CS5490::IZX_LEVEL,  "izx_level",  0x100000 },
    { CS5490::PULSERATE,  "pulserate",  0x800000 },
    { CS5490::INT_GAIN,   "int_gain",   0x143958 },
    { CS5490::VSWELL_DUR, "vswell_dur", 0x000000 },
    { CS5490::VSWELL_LEVEL,"vswell_level",0x7FFFFF },
    { CS5490::VZX_LEVEL,  "vzx_level",  0x100000 },
    { CS5490::CYCLECOUNT, "cyclecount", 0x000064 },
    { CS5490::SCALE,      "scale",      0x4CCCCD }
};
static const size_t REG_COUNT = sizeof(regTable)/sizeof(regTable[0]);

// Constructor
CS5490::CS5490(HardwareSerial &uart, int resetPin) 
    : _serial(&uart), _resetPin(resetPin), _debug(false), _currentPage(-1), _readSuccess(true), _currentBaud(600) {}

// Initialize communication
bool CS5490::begin(long baudRate, bool debugEnable) {
    _debug = debugEnable;
    _currentPage = -1;
    _readSuccess = true;
    // Set up hardware reset pin if provided
    if (_resetPin >= 0) {
        pinMode(_resetPin, OUTPUT);
        // Perform hardware reset: pull reset low then high
        digitalWrite(_resetPin, LOW);
        delay(10);
        digitalWrite(_resetPin, HIGH);
        // Wait for chip to initialize
        delay(300);
    }
    // Start serial at default 600 baud
    _serial->begin(600);
    _currentBaud = 600;
    // Flush any initial data from reset
    while (_serial->available()) { _serial->read(); }
    // If we need to switch to a new baud
    if (baudRate != 600) {
        _switchBaud(baudRate);
    }
    return true;
}

// Set debug mode
void CS5490::setDebug(bool enable) {
    _debug = enable;
}

// Internal: calculate and switch the CS5490 and host to a new baud rate
void CS5490::_switchBaud(long baud) {
    // If already at desired baud, no change
    if (baud == _currentBaud) return;
    // Calculate baud rate register value: BR = round(baud * (524288 / 4096000))
    // (Assuming MCLK = 4.096 MHz)
    float br_f = (float)baud * (524288.0f / 4096000.0f);
    uint32_t br = (uint32_t)lround(br_f);
    if (br > 0xFFFF) br = 0xFFFF;
    // Write new baud value to SerialCtrl register (bits BR15:0 plus enable bit 17)
    uint32_t serialCtrlVal = 0x020000 | (br & 0xFFFF);
    writeRawRegister(CS5490::SERIALCTRL, serialCtrlVal);
    if (_debug) {
        Serial.print("Baud switch: writing SerialCtrl = 0x");
        Serial.print(serialCtrlVal, HEX);
        Serial.print(" for target baud ");
        Serial.println(baud);
    }
    delay(100); // let chip apply baud rate
    // Reconfigure host UART to new baud
    _serial->end();
    _serial->begin(baud);
    delay(50);
    _currentBaud = baud;
    // Clear any data in buffer
    while (_serial->available()) { _serial->read(); }
    // Reset page tracking since communication was reset
    _currentPage = -1;
}

// Read raw register (24-bit value)
uint32_t CS5490::readRawRegister(uint32_t reg) {
    // Determine page and address from combined register constant
    uint8_t page = (reg >> 8) & 0x3F;
    uint8_t addr = reg & 0x3F;
    // Switch page if needed
    if (_currentPage != (int)page) {
        uint8_t pageCmd = 0x80 | page;
        _serial->write(pageCmd);
        _currentPage = page;
    }
    // Send address to read (bit6=0, bit7=0 means read command)
    _serial->write(addr);
    // Wait for 3 data bytes from chip
    unsigned long start = millis();
    while (_serial->available() < 3 && (millis() - start < 500)) {
        // wait up to 500ms
    }
    uint32_t rawValue = 0;
    if (_serial->available() >= 3) {
        // Read 3 bytes (LSB first as CS5490 outputs LSB first)
        uint8_t b0 = _serial->read();
        uint8_t b1 = _serial->read();
        uint8_t b2 = _serial->read();
        // Combine into 24-bit little-endian value
        rawValue = (uint32_t)b0 | ((uint32_t)b1 << 8) | ((uint32_t)b2 << 16);
        _readSuccess = true;
    } else {
        // Timeout or not enough data
        _readSuccess = false;
    }
    // Flush any extra bytes if present
    while (_serial->available()) { _serial->read(); }
    return rawValue & 0x00FFFFFF; // 24-bit mask
}

// Read register with conversion
float CS5490::readRegister(uint32_t reg) {
    uint32_t raw = readRawRegister(reg);
    // Determine if fixed-point conversion needed
    uint8_t fracBits = (uint8_t)((reg >> 16) & 0xFF);
    bool isSigned = (reg & 0x01000000) != 0;
    float value;
    if (fracBits > 0) {
        // Fixed-point number: convert to float
        // If signed and MSB (bit23) is set, extend sign to 32-bit
        int32_t signedVal = raw;
        if (isSigned) {
            if (raw & 0x800000) { // if sign bit set in 24-bit value
                signedVal |= 0xFF000000; // extend sign through 32-bit
            }
        }
        // Convert to float by dividing by 2^fracBits
        value = (float)signedVal / (float)(1 << fracBits);
    } else {
        // Not a fixed-point number, just use as unsigned integer (0..FFFFFF)
        value = (float)raw;
    }
    if (_debug) {
        _printDebugRead(reg, raw, value);
    }
    return value;
}

// Write raw 24-bit value to register
void CS5490::writeRawRegister(uint32_t reg, uint32_t rawValue) {
    uint8_t page = (reg >> 8) & 0x3F;
    uint8_t addr = reg & 0x3F;
    // Switch page if needed
    if (_currentPage != (int)page) {
        uint8_t pageCmd = 0x80 | page;
        _serial->write(pageCmd);
        _currentPage = page;
    }
    // Send write command (0x40 | address)
    uint8_t writeCmd = 0x40 | addr;
    _serial->write(writeCmd);
    // Send 3 bytes (LSB first) of the 24-bit value
    uint8_t b0 = rawValue & 0xFF;
    uint8_t b1 = (rawValue >> 8) & 0xFF;
    uint8_t b2 = (rawValue >> 16) & 0xFF;
    _serial->write(b0);
    _serial->write(b1);
    _serial->write(b2);
}

// Write register with value conversion
void CS5490::writeRegister(uint32_t reg, float value) {
    uint8_t fracBits = (uint8_t)((reg >> 16) & 0xFF);
    bool isSigned = (reg & 0x01000000) != 0;
    uint32_t raw;
    if (fracBits > 0) {
        // Convert float to fixed-point (signed or unsigned as appropriate)
        // Multiply by 2^fracBits and round to nearest integer
        float scaled = value * (float)(1 << fracBits);
        if (isSigned) {
            // For signed registers, cast to int
            int32_t sv = (int32_t)lround(scaled);
            raw = (uint32_t)(sv & 0x00FFFFFF);
        } else {
            if (value < 0) scaled = 0.0f; // clamp negative to 0 for unsigned
            raw = (uint32_t)lround(scaled) & 0x00FFFFFF;
        }
    } else {
        // Not a fixed-point register: interpret value as a raw integer
        if (value < 0) value = 0;
        raw = ((uint32_t)lround(value)) & 0x00FFFFFF;
    }
    if (_debug) {
        _printDebugWrite(reg, value, raw);
    }
    writeRawRegister(reg, raw);
}

// Send an instruction command (via CMD_INST prefix 0xC0)
void CS5490::sendInstruction(uint8_t instrCode) {
    uint8_t cmdByte = 0xC0 | (instrCode & 0x3F);
    _serial->write(cmdByte);
    if (_debug) {
        Serial.print("Instruction 0x");
        Serial.print(instrCode, HEX);
        Serial.println(" sent");
    }
}

// Perform a software reset instruction and reinitialize to 600 baud
bool CS5490::softReset() {
    // Send software reset instruction
    sendInstruction(CS5490::SOFT_RESET);
    // Small delay for reset to complete
    delay(100);
    // Reconfigure host serial back to 600 baud, since chip defaults to 600 after reset
    _serial->end();
    _serial->begin(600);
    delay(50);
    _currentBaud = 600;
    _currentPage = -1;
    // Flush any data after reset
    while (_serial->available()) { _serial->read(); }
    return true;
}

// Debug helper: find register name
const char* CS5490::_regName(uint32_t reg) {
    for (size_t i = 0; i < REG_COUNT; ++i) {
        if (regTable[i].reg == reg) {
            return regTable[i].name;
        }
    }
    return "UNKNOWN";
}
// Debug helper: get default value of register
uint32_t CS5490::_regDefault(uint32_t reg) {
    for (size_t i = 0; i < REG_COUNT; ++i) {
        if (regTable[i].reg == reg) {
            return regTable[i].defaultVal;
        }
    }
    return 0;
}

// Debug print for read operations
void CS5490::_printDebugRead(uint32_t reg, uint32_t raw, float value) {
    const char* name = _regName(reg);
    uint32_t defRaw = _regDefault(reg);
    uint8_t fracBits = (uint8_t)((reg >> 16) & 0xFF);
    bool isSigned = (reg & 0x01000000) != 0;
    Serial.print("Read ");
    // Align name to 10 characters for neat output
    int nameLen = strlen(name);
    if (nameLen < 10) {
        for (int i = 0; i < 10 - nameLen; ++i) Serial.print(" ");
    }
    Serial.print(name);
    Serial.print(" = ");
    Serial.print(raw, HEX);
    // Pad raw to 6 hex digits
    int rawHexLen = 0;
    uint32_t temp = raw;
    do { rawHexLen++; temp >>= 4; } while(temp);
    for (int i = rawHexLen; i < 6; ++i) Serial.print("0");
    // (We already printed raw in HEX without leading zeros via print; pad manually if needed)
    // Actually, simpler: use print with formatting:
    // We'll redo printing raw properly:
    Serial.print("\b\b\b\b\b\b"); // remove the previously printed raw (this is a minor hack for formatting)
    Serial.print(String(raw & 0xFFFFFF, HEX)); // print as 6-digit hex (String will not pad, so handle manually if needed)
    rawHexLen = String(raw & 0xFFFFFF, HEX).length();
    for(int i = rawHexLen; i < 6; ++i) Serial.print("0"); // pad if necessary (this approach might not perfectly align, but attempt)
    // Continue output
    Serial.print(" ");
    if (fracBits > 0) {
        // Print the converted float value
        Serial.print(value, 6);  // print fixed-point value (6 decimal places for detail)
        // Compute default converted value for comparison
        float defVal;
        if (isSigned) {
            int32_t defSigned = defRaw;
            if (defRaw & 0x800000) defSigned |= 0xFF000000;
            defVal = (float)defSigned / (float)(1 << fracBits);
        } else {
            defVal = (float)(defRaw & 0x00FFFFFF) / (float)(1 << fracBits);
        }
        Serial.print(" [");
        Serial.print(defVal, 6);
        Serial.print("]");
    } else {
        // Print current and default as hex
        Serial.print("0x");
        Serial.print(raw, HEX);
        Serial.print(" [0x");
        Serial.print(defRaw, HEX);
        Serial.print("]");
    }
    Serial.println();
}

// Debug print for write operations
void CS5490::_printDebugWrite(uint32_t reg, float value, uint32_t raw) {
    const char* name = _regName(reg);
    Serial.print("Write ");
    int nameLen = strlen(name);
    if (nameLen < 10) {
        for (int i = 0; i < 10 - nameLen; ++i) Serial.print(" ");
    }
    Serial.print(name);
    Serial.print(" = ");
    // Print the value in human-readable form
    Serial.print(value);
    Serial.print(" (0x");
    // Print raw hex value zero-padded to 6 digits
    Serial.print(raw, HEX);
    int rawHexLen = String(raw & 0xFFFFFF, HEX).length();
    for(int i = rawHexLen; i < 6; ++i) Serial.print("0");
    Serial.println(")");
}

