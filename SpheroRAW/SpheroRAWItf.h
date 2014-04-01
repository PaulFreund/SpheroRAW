//######################################################################################################################
/*
    Copyright (c) since 2014 - Paul Freund

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/
//######################################################################################################################

#pragma once

#ifdef SPHERO_RAW_EXPORTS
    #define SPHERO_RAW_API extern "C" __declspec(dllexport) 
#else
    #define SPHERO_RAW_API extern "C" __declspec(dllimport) 
#endif

//======================================================================================================================

#include <vector>
#include <stdint.h>

//======================================================================================================================

typedef uint8_t ubyte;
typedef uint16_t ushort;
typedef uint32_t uint;

typedef ubyte SequenceId;
const ubyte INVALID_SEQUENCE_ID = 0;

typedef std::vector<ubyte> MessageData;
typedef std::vector<ubyte> CommandParameters;

//======================================================================================================================

const ubyte SPHERO_REQUEST_SOP1_MASK = 0xFF;
const ubyte SPHERO_REQUEST_SOP2_MASK = 0xFC;
const ubyte SPHERO_REQUEST_SOP2_MASK_RESET = 0x02;
const ubyte SPHERO_REQUEST_SOP2_MASK_ANSWER = 0x01;
const ubyte SPHERO_RESPONSE_SOP1_MASK = 0xFF;
const ubyte SPHERO_RESPONSE_SOP2_MASK_ACK = 0xFF;
const ubyte SPHERO_RESPONSE_SOP2_MASK_ASYNC = 0xFE;
const ubyte SPHERO_RESPONSE_HEADER_SIZE = 0x05;

enum DeviceId {
    DeviceId_CORE = 0x00,
    DeviceId_BOOTLOADER = 0x01,
    DeviceId_SPHERO = 0x02

};

enum CoreCommandId {
    CoreCommandId_PING = 0x01,
    CoreCommandId_VERSION = 0x02,
    CoreCommandId_CONTROL_UART_TX = 0x03,
    CoreCommandId_SET_BT_NAME = 0x10,
    CoreCommandId_GET_BT_NAME = 0x11,
    CoreCommandId_SET_AUTO_RECONNECT = 0x12,
    CoreCommandId_GET_AUTO_RECONNECT = 0x13,
    CoreCommandId_GET_PWR_STATE = 0x20,
    CoreCommandId_SET_PWR_NOTIFY = 0x21,
    CoreCommandId_SLEEP = 0x22,
    CoreCommandId_GET_POWER_TRIPS = 0x23,
    CoreCommandId_SET_POWER_TRIPS = 0x24,
    CoreCommandId_SET_INACTIVE_TIMER = 0x25,
    CoreCommandId_GOTO_BL = 0x30,
    CoreCommandId_RUN_L1_DIAGS = 0x40,
    CoreCommandId_RUN_L2_DIAGS = 0x41,
    CoreCommandId_CLEAR_COUNTERS = 0x42,
    CoreCommandId_ASSIGN_TIME = 0x50,
    CoreCommandId_POLL_TIMES = 0x51
};

enum BootloaderCommandId {
    BootloaderCommandId_BEGIN_REFLASH = 0x02,
    BootloaderCommandId_HERE_IS_PAGE = 0x03,
    BootloaderCommandId_LEAVE_BOOTLOADER = 0x04,
    BootloaderCommandId_IS_PAGE_BLANK = 0x05,
    BootloaderCommandId_CMD_ERASE_USER_CONFIG = 0x06
};

enum SpheroCommandId {
    SpheroCommandId_SET_CAL = 0x01,
    SpheroCommandId_SET_STABILIZ = 0x02,
    SpheroCommandId_SET_ROTATION_RATE = 0x03,
    SpheroCommandId_SET_CREATION_DATE = 0x04,
    SpheroCommandId_GET_BALL_REG_WEBSITE = 0x05,
    SpheroCommandId_REENABLE_DEMO = 0x06,
    SpheroCommandId_GET_CHASSIS_ID = 0x07,
    SpheroCommandId_SET_CHASSIS_ID = 0x08,
    SpheroCommandId_SELF_LEVEL = 0x09,
    SpheroCommandId_SET_VDL = 0x0A,
    SpheroCommandId_SET_DATA_STREAMING = 0x11,
    SpheroCommandId_SET_COLLISION_DET = 0x12,
    SpheroCommandId_LOCATOR = 0x13,
    SpheroCommandId_SET_ACCELERO = 0x14,
    SpheroCommandId_READ_LOCATOR = 0x15,
    SpheroCommandId_SET_RGB_LED = 0x20,
    SpheroCommandId_SET_BACK_LED = 0x21,
    SpheroCommandId_GET_RGB_LED = 0x22,
    SpheroCommandId_ROLL = 0x30,
    SpheroCommandId_BOOST = 0x31,
    SpheroCommandId_MOVE = 0x32,
    SpheroCommandId_SET_RAW_MOTORS = 0x33,
    SpheroCommandId_SET_MOTION_TO = 0x34,
    SpheroCommandId_SET_OPTIONS_FLAG = 0x35,
    SpheroCommandId_GET_OPTIONS_FLAG = 0x36,
    SpheroCommandId_SET_TEMP_OPTIONS_FLAG = 0x37,
    SpheroCommandId_GET_TEMP_OPTIONS_FLAG = 0x38,
    SpheroCommandId_GET_CONFIG_BLK = 0x40,
    SpheroCommandId_SET_SSB_PARAMS = 0x41,
    SpheroCommandId_SET_DEVICE_MODE = 0x42,
    SpheroCommandId_SET_CFG_BLOCK = 0x43,
    SpheroCommandId_GET_DEVICE_MODE = 0x44,
    SpheroCommandId_GET_SSB = 0x46,
    SpheroCommandId_SET_SSB = 0x47,
    SpheroCommandId_SSB_REFILL = 0x48,
    SpheroCommandId_SSB_BUY = 0x49,
    SpheroCommandId_SSB_USE_CONSUMEABLE = 0x4A,
    SpheroCommandId_SSB_GRANT_CORES = 0x4B,
    SpheroCommandId_SSB_ADD_XP = 0x4C,
    SpheroCommandId_SSB_LEVEL_UP_ATTR = 0x4D,
    SpheroCommandId_GET_PW_SEED = 0x4E,
    SpheroCommandId_SSB_ENABLE_ASYNC = 0x4F,
    SpheroCommandId_RUN_MACRO = 0x50,
    SpheroCommandId_SAVE_TEMP_MACRO = 0x51,
    SpheroCommandId_SAVE_MACRO = 0x52,
    SpheroCommandId_REINIT_MACRO_EXECUTIVE = 0x54,
    SpheroCommandId_ABORT_MACRO = 0x55,
    SpheroCommandId_GET_MACRO_STATUS = 0x56,
    SpheroCommandId_SET_MACRO_PARAMETER = 0x57,
    SpheroCommandId_APPEND_MACRO_CHUNK = 0x58,
    SpheroCommandId_ERASE_ORBBASIC_STORAGE = 0x60,
    SpheroCommandId_APPEND_ORBBASIC_FRAGMENT = 0x61,
    SpheroCommandId_EXECUTE_ORBBASIC_PROGRAM = 0x62,
    SpheroCommandId_ABORT_ORBBASIC_PROGRAM = 0x63,
    SpheroCommandId_SUBMIT_VALUE_TO_INPUT_STATEMENT = 0x64,
    SpheroCommandId_COMMIT_RAM_PROGRAM_TO_FLASH = 0x65
};

enum ResponseCode {
    ResponseCode_OK = 0x00,
    ResponseCode_EGEN = 0x01,
    ResponseCode_ECHKSUM = 0x02,
    ResponseCode_EFRAG = 0x03,
    ResponseCode_EBAD_CMD = 0x04,
    ResponseCode_EUNSUPP = 0x05,
    ResponseCode_EBAD_MSG = 0x06,
    ResponseCode_EPARAM = 0x07,
    ResponseCode_EEXEC = 0x08,
    ResponseCode_EBAD_DID = 0x09,
    ResponseCode_MEM_BUSY = 0x0A,
    ResponseCode_BAD_PASSWORD = 0x0B,
    ResponseCode_POWER_NOGOOD = 0x31,
    ResponseCode_PAGE_ILLEGAL = 0x32,
    ResponseCode_FLASH_FAIL = 0x33,
    ResponseCode_MA_CORRUPT = 0x34,
    ResponseCode_MSG_TIMEOUT = 0x35
};

enum AsyncResponseIdCode {
    AsyncResponseId_Invalid = 0x00,
    AsyncResponseId_PowerNotification = 0x01,
    AsyncResponseId_Level1DiagnosticResponse = 0x02,
    AsyncResponseId_SensorDataStreaming = 0x03,
    AsyncResponseId_ConfigBlockContents = 0x04,
    AsyncResponseId_PreSleepWarning10Sec = 0x05,
    AsyncResponseId_MacroMarkers = 0x06,
    AsyncResponseId_CollisionDetected = 0x07,
    AsyncResponseId_orbBasicPrintMessage = 0x08,
    AsyncResponseId_orbBasicErrorMessageASCII = 0x09,
    AsyncResponseId_orbBasicErrorMessageBinary = 0x0A,
    AsyncResponseId_SelfLevelResult = 0x0B,
    AsyncResponseId_GyroAxisLimitExceeded = 0x0C,
    AsyncResponseId_SpheroSoulData = 0x0D,
    AsyncResponseId_LevelUpNotification = 0x0E,
    AsyncResponseId_ShieldDamageNotification = 0x0F,
    AsyncResponseId_XPUpdateNotification = 0x10,
    AsyncResponseId_BoostUpdateNotification = 0x11
};

enum SpheroState {
    SpheroState_None = 0,
    SpheroState_Error_BluetoothError = 1,
    SpheroState_Error_BluetoothUnavailable = 2,
    SpheroState_Error_NotPaired = 3,
    SpheroState_Error_ConnectionFailed = 4,
    SpheroState_Disconnected = 5,
    SpheroState_Connected = 6
};

//======================================================================================================================

struct SpheroMessage {
    SequenceId sequenceId = INVALID_SEQUENCE_ID;
    AsyncResponseIdCode idCode = AsyncResponseId_Invalid;
    ResponseCode responseCode = ResponseCode_OK;

    MessageData data;
};

//======================================================================================================================

class ISpheroDevice {
public:
    virtual SpheroState state() = 0;
    virtual void connect() = 0;
    virtual void disconnect() = 0;

    virtual std::vector<SpheroMessage> receive() = 0;

    // Core commands
    virtual SequenceId ping() = 0;
    virtual SequenceId getVersioning() = 0;
    virtual SequenceId setUARTTxLine(const bool enable = true) = 0;
    virtual SequenceId setDeviceName(const std::string name) = 0;
    virtual SequenceId getBluetoothInfo() = 0;
    virtual SequenceId setAutoReconnect(const bool enable = true, const ubyte secTimeout = 30) = 0;
    virtual SequenceId getAutoReconnect() = 0;
    virtual SequenceId getPowerState() = 0;
    virtual SequenceId setPowerNotification(const bool enable = true) = 0;
    virtual SequenceId sleep(const ushort secSleepDuration = 0xFFFF, const ubyte wakeupMacro = 0, const ushort wakeupOrbBasicMacro = 0) = 0;
    virtual SequenceId getVoltageTripPoints() = 0;
    virtual SequenceId setVoltageTripPoints(const ushort voltLow = 700, const ushort voltCritical = 650) = 0;
    virtual SequenceId setInactivityTimeout(const ushort secInactivityTimeout = 600) = 0;
    virtual SequenceId jumpToBootloader() = 0;
    virtual SequenceId performLevel1Diagnostics() = 0;
    virtual SequenceId performLevel2Diagnostics() = 0;
    virtual SequenceId clearCouters() = 0;
    virtual SequenceId assignTimeValue(const uint timeValue = 0) = 0;
    virtual SequenceId pollPacketTimes(const uint spheroRxTime, const uint spheroTxTime) = 0;

    // Sphero commands
    virtual SequenceId setHeading(const ushort heading) = 0;
    virtual SequenceId setStabilisation(const bool enable) = 0;
    virtual SequenceId setRotationRate(const ubyte rate) = 0;
    virtual SequenceId getChassisId() = 0;
    virtual SequenceId selfLevel(const CommandParameters data) = 0;
    virtual SequenceId setDataStreaming(const CommandParameters data) = 0;
    virtual SequenceId configureCollisionDetection(const CommandParameters data) = 0;
    virtual SequenceId configureLocator(const CommandParameters data) = 0;
    virtual SequenceId setAccelerometerRange(const ubyte range) = 0;
    virtual SequenceId readLocator() = 0;
    virtual SequenceId setRGBLedOutput(const ubyte red, const ubyte green, const ubyte blue, const bool persist) = 0;
    virtual SequenceId setBackLEDOutput(const ubyte intensity) = 0;
    virtual SequenceId getRGBLed() = 0;
    virtual SequenceId roll(const ubyte speed, const ushort heading, const ubyte state) = 0;
    virtual SequenceId boost(const bool enable = true) = 0;
    virtual SequenceId setRAWMotorValues(const ubyte modeLeft, const ubyte powerLeft, const ubyte modeRight, const ubyte powerRight) = 0;
    virtual SequenceId setMotionTimeout(const ushort msecTimeout) = 0;
    virtual SequenceId setPermanentOptionFlags(const CommandParameters data) = 0;
    virtual SequenceId getPermanentOptionFlags() = 0;
    virtual SequenceId setTemporaryOptionFlags(const CommandParameters data) = 0;
    virtual SequenceId getTemporaryOptionFlags() = 0;
    virtual SequenceId getConfigurationBlock(const ubyte id) = 0;
    virtual SequenceId setSSBModifierBlock(const uint pwd, const CommandParameters data) = 0;
    virtual SequenceId setDeviceMode(const bool enableHackMode) = 0;
    virtual SequenceId setConfigurationBlock(const CommandParameters data) = 0;
    virtual SequenceId getDeviceMode() = 0;
    virtual SequenceId getSSB() = 0;
    virtual SequenceId setSSB(const uint password, const CommandParameters data) = 0;
    virtual SequenceId refillBank(const ubyte type) = 0;
    virtual SequenceId buyConsumable(const ubyte id, const ubyte quantity) = 0;
    virtual SequenceId useConsumable(const ubyte id) = 0;
    virtual SequenceId grantCores(const uint password, const uint quantity, const ubyte flags) = 0;
    virtual SequenceId addXP(const uint password, const uint quantity) = 0;
    virtual SequenceId levelUpAttribute(const uint password, const ubyte attrId) = 0;
    virtual SequenceId getPasswordSeed() = 0;
    virtual SequenceId enableSSBAsyncMessages(const bool enable = true) = 0;
    virtual SequenceId runMacro(const ubyte id) = 0;
    virtual SequenceId saveTemporaryMacro(const CommandParameters macroData) = 0;
    virtual SequenceId saveMacro(const CommandParameters macroData) = 0;
    virtual SequenceId reinitMacroExecutive() = 0;
    virtual SequenceId abortMacro() = 0;
    virtual SequenceId getMacroStatus() = 0;
    virtual SequenceId setMacroParameter(const ubyte parameter, const ubyte valueOne, const ubyte value) = 0;
    virtual SequenceId appendMacroChunk(const CommandParameters macroData) = 0;
    virtual SequenceId eraseOrbBasicStorage(const ubyte area) = 0;
    virtual SequenceId appendOrbBasicFragment(const ubyte area, const CommandParameters fragment) = 0;
    virtual SequenceId executeOrbBasicProgram(const ubyte area, const ushort startLine) = 0;
    virtual SequenceId abortOrbBasicProgram() = 0;
    virtual SequenceId submitValueToInputStatement(const uint value) = 0;
    virtual SequenceId commitRAMProgramToFlash() = 0;
};

//======================================================================================================================

SPHERO_RAW_API ISpheroDevice* SpheroRAW_Create(const char* nameBuffer);
SPHERO_RAW_API void SpheroRAW_Destroy(ISpheroDevice* deviceHandle);

//======================================================================================================================



