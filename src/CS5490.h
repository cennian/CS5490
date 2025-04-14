#ifndef CS5490_H
#define CS5490_H

#include <Arduino.h>

#define CS5490_REG(addr, page, sign, frac) ((uint32_t)(addr) | ((uint32_t)(page) << 8) | ((uint32_t)(frac) << 16) | ((uint32_t)((sign) ? 1 : 0) << 24))

class CS5490 {
public:
    // Register constants (address, page, sign, fractional bits)
    static constexpr uint32_t CONFIG0     = CS5490_REG(0x00, 0, 0, 0);
    static constexpr uint32_t CONFIG1     = CS5490_REG(0x01, 0, 0, 0);
    static constexpr uint32_t MASK        = CS5490_REG(0x03, 0, 0, 0);
    static constexpr uint32_t PC         = CS5490_REG(0x05, 0, 0, 0);      // Phase Compensation
    static constexpr uint32_t SERIALCTRL  = CS5490_REG(0x07, 0, 0, 0);
    static constexpr uint32_t PULSEWIDTH  = CS5490_REG(0x08, 0, 0, 0);
    static constexpr uint32_t PULSECTRL   = CS5490_REG(0x09, 0, 0, 0);
    static constexpr uint32_t STATUS0     = CS5490_REG(0x17, 0, 0, 0);
    static constexpr uint32_t STATUS1     = CS5490_REG(0x18, 0, 0, 0);
    static constexpr uint32_t STATUS2     = CS5490_REG(0x19, 0, 0, 0);
    static constexpr uint32_t REGLOCK     = CS5490_REG(0x22, 0, 0, 0);
    static constexpr uint32_t V_PEAK      = CS5490_REG(0x24, 0, 1, 23);
    static constexpr uint32_t I_PEAK      = CS5490_REG(0x25, 0, 1, 23);
    static constexpr uint32_t PSDC        = CS5490_REG(0x30, 0, 0, 0);
    static constexpr uint32_t ZXNUM       = CS5490_REG(0x37, 0, 0, 0);
    // Page 16 (Software registers)
    static constexpr uint32_t CONFIG2     = CS5490_REG(0x00, 16, 0, 0);
    static constexpr uint32_t REGCHK      = CS5490_REG(0x01, 16, 0, 0);
    static constexpr uint32_t V_INST      = CS5490_REG(0x02, 16, 1, 23);
    static constexpr uint32_t I_INST      = CS5490_REG(0x03, 16, 1, 23);
    static constexpr uint32_t P_INST      = CS5490_REG(0x04, 16, 1, 23);
    static constexpr uint32_t P_AVG       = CS5490_REG(0x05, 16, 1, 23);
    static constexpr uint32_t I_RMS       = CS5490_REG(0x06, 16, 0, 24);
    static constexpr uint32_t V_RMS       = CS5490_REG(0x07, 16, 0, 24);
    static constexpr uint32_t Q_AVG       = CS5490_REG(0x0E, 16, 1, 23);
    static constexpr uint32_t Q_INST      = CS5490_REG(0x0F, 16, 1, 23);
    static constexpr uint32_t S           = CS5490_REG(0x14, 16, 1, 23);
    static constexpr uint32_t PF          = CS5490_REG(0x15, 16, 1, 23);
    static constexpr uint32_t TEMP        = CS5490_REG(0x1B, 16, 1, 16);
    static constexpr uint32_t P_SUM       = CS5490_REG(0x1D, 16, 1, 23);
    static constexpr uint32_t S_SUM       = CS5490_REG(0x1E, 16, 1, 23);
    static constexpr uint32_t Q_SUM       = CS5490_REG(0x1F, 16, 1, 23);
    static constexpr uint32_t I_DCOFF     = CS5490_REG(0x20, 16, 1, 23);
    static constexpr uint32_t I_GAIN      = CS5490_REG(0x21, 16, 0, 22);
    static constexpr uint32_t V_DCOFF     = CS5490_REG(0x22, 16, 1, 23);
    static constexpr uint32_t V_GAIN      = CS5490_REG(0x23, 16, 0, 22);
    static constexpr uint32_t P_OFF       = CS5490_REG(0x24, 16, 1, 23);
    static constexpr uint32_t I_ACOFF     = CS5490_REG(0x25, 16, 1, 23);
    static constexpr uint32_t Q_OFF       = CS5490_REG(0x26, 16, 1, 23);
    static constexpr uint32_t EPSILON     = CS5490_REG(0x31, 16, 1, 23);
    static constexpr uint32_t SAMPLECOUNT = CS5490_REG(0x33, 16, 0, 0);
    static constexpr uint32_t T_GAIN      = CS5490_REG(0x36, 16, 0, 16);
    static constexpr uint32_t T_OFF       = CS5490_REG(0x37, 16, 1, 16);
    static constexpr uint32_t T_SETTLE    = CS5490_REG(0x39, 16, 0, 0);
    static constexpr uint32_t SYS_GAIN    = CS5490_REG(0x3C, 16, 1, 22);
    static constexpr uint32_t SYS_TIME    = CS5490_REG(0x3D, 16, 0, 0);
    // Page 17 (Software registers continued)
    static constexpr uint32_t VSAG_DUR    = CS5490_REG(0x00, 17, 0, 0);
    static constexpr uint32_t VSAG_LEVEL  = CS5490_REG(0x01, 17, 1, 23);
    static constexpr uint32_t IOVER_DUR   = CS5490_REG(0x04, 17, 0, 0);
    static constexpr uint32_t IOVER_LEVEL = CS5490_REG(0x05, 17, 1, 23);
    // Page 18 (Software registers continued)
    static constexpr uint32_t IZX_LEVEL   = CS5490_REG(0x18, 18, 1, 23);
    static constexpr uint32_t PULSERATE   = CS5490_REG(0x1C, 18, 1, 23);
    static constexpr uint32_t INT_GAIN    = CS5490_REG(0x2B, 18, 1, 23);
    static constexpr uint32_t VSWELL_DUR  = CS5490_REG(0x2E, 18, 0, 0);
    static constexpr uint32_t VSWELL_LEVEL= CS5490_REG(0x2F, 18, 1, 23);
    static constexpr uint32_t VZX_LEVEL   = CS5490_REG(0x3A, 18, 1, 23);
    static constexpr uint32_t CYCLECOUNT  = CS5490_REG(0x3E, 18, 0, 0);
    static constexpr uint32_t SCALE       = CS5490_REG(0x3F, 18, 0, 23);

    // Instruction code constants (use with sendInstruction)
    static constexpr uint8_t SOFT_RESET  = 0x01;
    static constexpr uint8_t STANDBY     = 0x02;
    static constexpr uint8_t WAKEUP      = 0x03;
    static constexpr uint8_t SINGLE_CONV = 0x14;
    static constexpr uint8_t CONT_CONV   = 0x15;
    static constexpr uint8_t HALT_CONV   = 0x18;
    // Calibration instructions
    static constexpr uint8_t I_AC_CAL    = 0x31;
    static constexpr uint8_t V_AC_CAL    = 0x32;
    static constexpr uint8_t IV_AC_CAL   = 0x36;
    static constexpr uint8_t I_DC_CAL    = 0x21;
    static constexpr uint8_t V_DC_CAL    = 0x22;
    static constexpr uint8_t IV_DC_CAL   = 0x26;
    static constexpr uint8_t I_GAIN_CAL  = 0x39;
    static constexpr uint8_t V_GAIN_CAL  = 0x3A;
    static constexpr uint8_t IV_GAIN_CAL = 0x3E;

    // Constructor: pass the UART interface and optional reset pin
    CS5490(HardwareSerial &uart, int resetPin = -1);

    // Initialize communication with the CS5490.
    // Opens the serial at 600 baud (chip default), performs a hardware reset (if reset pin provided),
    // then switches to the specified baud rate (default 600 if not changing).
    // Optionally enable debug output.
    bool begin(long baudRate = 600, bool debugEnable = false);

    // Read a register value (automatically handles fixed-point conversion to float if applicable).
    float readRegister(uint32_t reg);
    // Write a value to a register (automatically converts from float to fixed-point if needed).
    void writeRegister(uint32_t reg, float value);
    // Read a raw 24-bit register value (unconverted).
    uint32_t readRawRegister(uint32_t reg);
    // Write a raw 24-bit value to a register.
    void writeRawRegister(uint32_t reg, uint32_t rawValue);

    // Send an instruction command (e.g., SOFT_RESET, CONT_CONV, calibration commands).
    void sendInstruction(uint8_t instrCode);

    // Enable or disable debug printout of register operations.
    void setDebug(bool enable);

    // Check if the last read operation succeeded (true if 3 bytes were received).
    bool lastReadSuccess() const { return _readSuccess; }

    // Perform a software reset via instruction and reinitialize serial at 600 baud.
    bool softReset();

private:
    HardwareSerial *_serial;
    int _resetPin;
    bool _debug;
    int _currentPage;
    bool _readSuccess;
    long _currentBaud;  // track current UART baud

    void _switchBaud(long baud);
    void _printDebugRead(uint32_t reg, uint32_t raw, float value);
    void _printDebugWrite(uint32_t reg, float value, uint32_t raw);
    const char* _regName(uint32_t reg);
    uint32_t    _regDefault(uint32_t reg);
};

#endif // CS5490_H

