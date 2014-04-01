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

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>

//#ifdef _DEBUG
//    #define SPHERORAW_DEBUG_DEBUG
//#endif


#ifdef SPHERORAW_DEBUG_DEBUG
    #include <sstream>
    #include <iostream>
    #include <iomanip>
#endif

#include "SpheroRAWItf.h"

#ifdef _WIN32
    #include "BluetoothWrapper_Win32.h"
#else
    #error
#endif

//======================================================================================================================

class SpheroDevice : public ISpheroDevice {

    //------------------------------------------------------------------------------------------------------------------
private:
    std::string _name;
    SpheroState _state;
    BluetoothSocket _socket = BLUETOOTH_SOCKET_INVALID;

    SequenceId _currentSequence = 0;
    SequenceId nextSeqId() {
        if(_currentSequence == 255)
            _currentSequence = 0;

        return ++_currentSequence;
    }

    std::vector<ubyte> _receiveBuffer;
    std::vector<SpheroMessage> _messages;

    //------------------------------------------------------------------------------------------------------------------
public:
    SpheroDevice(std::string name) : _name(name) {
        if(!BluetoothInitialize()) {
            _state = SpheroState_Error_BluetoothError;
            return;
        }

        if(!BluetoothAvailable()) {
            _state = SpheroState_Error_BluetoothUnavailable;
            return;
        }

        if(!BluetoothDevicePaired(_name)) {
            _state = SpheroState_Error_NotPaired;
            return;
        }

        _state = SpheroState_Disconnected;
    }

    ~SpheroDevice() {
        BluetoothCleanup();
    }

    //------------------------------------------------------------------------------------------------------------------
public:
    virtual SpheroState state() {
        return _state;
    }

    virtual void connect() {
        _currentSequence = 0;
        _messages.clear();
        _receiveBuffer.clear();
        _socket = BluetoothConnect(_name);

        if(_socket == BLUETOOTH_SOCKET_INVALID)
            _state = SpheroState_Error_ConnectionFailed;
        else
            _state = SpheroState_Connected;
    }

    virtual void disconnect() {
        BluetoothDisconnect(_socket);
        _socket = BLUETOOTH_SOCKET_INVALID;
        _state = SpheroState_Disconnected;
    }

    virtual std::vector<SpheroMessage> receive() {
        _state = deviceReceive();

        std::vector<SpheroMessage> data = _messages;
        _messages.clear();
        return data;
    }

    // Core commands
    virtual SequenceId ping() {
        return deviceSend(DeviceId_CORE, CoreCommandId_PING);
    }

    virtual SequenceId getVersioning() {
        return deviceSend(DeviceId_CORE, CoreCommandId_VERSION);
	}

    virtual SequenceId setUARTTxLine(const bool enable = true) {
        ubyte flag_enable = enable ? 0x01 : 0x00;
        return deviceSend(DeviceId_CORE, CoreCommandId_CONTROL_UART_TX, { flag_enable });
        return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setDeviceName(const std::string name) {
        return deviceSend(DeviceId_CORE, CoreCommandId_SET_BT_NAME, std::vector<ubyte>(name.begin(), name.end()));
	}

    virtual SequenceId getBluetoothInfo() {
        return deviceSend(DeviceId_CORE, CoreCommandId_GET_BT_NAME);
	}

    virtual SequenceId setAutoReconnect(const bool enable = true, const ubyte secTimeout = 30) {
        ubyte flag_enable = enable ? 0x01 : 0x00;
        std::vector<ubyte> data;
        data.push_back(flag_enable);
        data.push_back(secTimeout);
        return deviceSend(DeviceId_CORE, CoreCommandId_SET_AUTO_RECONNECT, data);
	}

    virtual SequenceId getAutoReconnect() {
        return deviceSend(DeviceId_CORE, CoreCommandId_GET_AUTO_RECONNECT);
	}

    virtual SequenceId getPowerState() {
        return deviceSend(DeviceId_CORE, CoreCommandId_GET_PWR_STATE);
	}

    virtual SequenceId setPowerNotification(const bool enable = true) {
        ubyte flag_enable = enable ? 0x01 : 0x00;
        return deviceSend(DeviceId_CORE, CoreCommandId_SET_PWR_NOTIFY, { flag_enable });
	}

    virtual SequenceId sleep(const ushort secSleepDuration = 0xFFFF, const ubyte wakeupMacro = 0, const ushort wakeupOrbBasicMacro = 0) {
        std::vector<ubyte> data;
        ushort net_secSleepDuration = htons(secSleepDuration);
        ushort net_wakeupOrbBasicMacro = htons(wakeupOrbBasicMacro);
        data.push_back((net_secSleepDuration >> (8 * 0)) & 0xff);
        data.push_back((net_secSleepDuration >> (8 * 1)) & 0xff);
        data.push_back(wakeupMacro);
        data.push_back((net_wakeupOrbBasicMacro >> (8 * 0)) & 0xff);
        data.push_back((net_wakeupOrbBasicMacro >> (8 * 1)) & 0xff);
        return deviceSend(DeviceId_CORE, CoreCommandId_SLEEP, data);
	}

    virtual SequenceId getVoltageTripPoints() {
        return deviceSend(DeviceId_CORE, CoreCommandId_GET_POWER_TRIPS);
	}

    virtual SequenceId setVoltageTripPoints(const ushort voltLow = 700, const ushort voltCritical = 650) {
        std::vector<ubyte> data;
        ushort net_voltLow = htons(voltLow);
        ushort net_voltCritical = htons(voltCritical);
        data.push_back((net_voltLow >> (8 * 0)) & 0xff);
        data.push_back((net_voltLow >> (8 * 1)) & 0xff);
        data.push_back((net_voltCritical >> (8 * 0)) & 0xff);
        data.push_back((net_voltCritical >> (8 * 1)) & 0xff);
        return deviceSend(DeviceId_CORE, CoreCommandId_SET_POWER_TRIPS, data);
	}

    virtual SequenceId setInactivityTimeout(const ushort secInactivityTimeout = 600) {
        std::vector<ubyte> data;
        ushort net_secInactivityTimeout = htons(secInactivityTimeout);
        data.push_back((net_secInactivityTimeout >> (8 * 0)) & 0xff);
        data.push_back((net_secInactivityTimeout >> (8 * 1)) & 0xff);
        return deviceSend(DeviceId_CORE, CoreCommandId_SET_INACTIVE_TIMER, data);
	}

    virtual SequenceId jumpToBootloader() {
        return deviceSend(DeviceId_CORE, CoreCommandId_GOTO_BL);
	}

    virtual SequenceId performLevel1Diagnostics() {
        return deviceSend(DeviceId_CORE, CoreCommandId_RUN_L1_DIAGS);
	}

    virtual SequenceId performLevel2Diagnostics() {
        return deviceSend(DeviceId_CORE, CoreCommandId_RUN_L2_DIAGS);
	}

    virtual SequenceId clearCouters() {
        return deviceSend(DeviceId_CORE, CoreCommandId_CLEAR_COUNTERS);
	}

    virtual SequenceId assignTimeValue(const uint timeValue = 0) {
        std::vector<ubyte> data;
        uint net_timeValue = htons(timeValue);
        data.push_back((net_timeValue >> (8 * 0)) & 0xff);
        data.push_back((net_timeValue >> (8 * 1)) & 0xff);
        data.push_back((net_timeValue >> (8 * 2)) & 0xff);
        data.push_back((net_timeValue >> (8 * 3)) & 0xff);
        return deviceSend(DeviceId_CORE, CoreCommandId_ASSIGN_TIME, data);
	}

    virtual SequenceId pollPacketTimes(const uint spheroRxTime, const uint spheroTxTime) {
        std::vector<ubyte> data;
        uint net_spheroRxTime = htons(spheroRxTime);
        uint net_spheroTxTime = htons(spheroTxTime);
        data.push_back((net_spheroRxTime >> (8 * 0)) & 0xff);
        data.push_back((net_spheroRxTime >> (8 * 1)) & 0xff);
        data.push_back((net_spheroRxTime >> (8 * 2)) & 0xff);
        data.push_back((net_spheroRxTime >> (8 * 3)) & 0xff);
        data.push_back((net_spheroTxTime >> (8 * 0)) & 0xff);
        data.push_back((net_spheroTxTime >> (8 * 1)) & 0xff);
        data.push_back((net_spheroTxTime >> (8 * 2)) & 0xff);
        data.push_back((net_spheroTxTime >> (8 * 3)) & 0xff);
        return deviceSend(DeviceId_CORE, CoreCommandId_POLL_TIMES, data);
	}

    // Sphero commands
    virtual SequenceId setHeading(const ushort heading) {
        std::vector<ubyte> data;
        ushort net_heading = htons(heading);
        data.push_back((net_heading >> (8 * 0)) & 0xff);
        data.push_back((net_heading >> (8 * 1)) & 0xff);
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_CAL, data);
	}

    virtual SequenceId setStabilisation(const bool enable = true) {
        ubyte flag_enable = enable ? 0x01 : 0x00;
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_STABILIZ, { flag_enable });
	}

    virtual SequenceId setRotationRate(const ubyte rate) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_ROTATION_RATE, { rate });
	}

    virtual SequenceId getChassisId() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_GET_CHASSIS_ID);
	}

    virtual SequenceId selfLevel(const CommandParameters data) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SELF_LEVEL, data);
	}

    virtual SequenceId setDataStreaming(const CommandParameters data) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_DATA_STREAMING, data);
	}

    virtual SequenceId configureCollisionDetection(const CommandParameters data) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_COLLISION_DET, data);
	}

    virtual SequenceId configureLocator(const CommandParameters data) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_LOCATOR, data);
	}

    virtual SequenceId setAccelerometerRange(const ubyte range) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_ACCELERO, { range });
	}

    virtual SequenceId readLocator() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_READ_LOCATOR);
	}

    virtual SequenceId setRGBLedOutput(const ubyte red, const ubyte green, const ubyte blue, const bool persist) {
        ubyte flag_persist = persist ? 0x01 : 0x00;
        std::vector<ubyte> data;
        data.push_back(red);
        data.push_back(green);
        data.push_back(blue);
        data.push_back(flag_persist);
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_RGB_LED, data);
    }

    virtual SequenceId setBackLEDOutput(const ubyte intensity) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_BACK_LED, { intensity });
	}

    virtual SequenceId getRGBLed() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_GET_RGB_LED);
	}

    virtual SequenceId roll(const ubyte speed, const ushort heading, const ubyte state) {
        std::vector<ubyte> data;
        ushort net_heading = htons(heading);
        data.push_back(speed);
        data.push_back((net_heading >> (8 * 0)) & 0xff);
        data.push_back((net_heading >> (8 * 1)) & 0xff);
        data.push_back(state);
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_ROLL, data);
	}

    virtual SequenceId boost(const bool enable = true) {
        ubyte flag_enable = enable ? 0x01 : 0x00;
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_BOOST, { flag_enable });
	}

    virtual SequenceId setRAWMotorValues(const ubyte modeLeft, const ubyte powerLeft, const ubyte modeRight, const ubyte powerRight) {
        std::vector<ubyte> data;
        data.push_back(modeLeft);
        data.push_back(powerLeft);
        data.push_back(modeRight);
        data.push_back(powerRight);
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_RAW_MOTORS, data);
	}

    virtual SequenceId setMotionTimeout(const ushort msecTimeout) {
        std::vector<ubyte> data;
        ushort net_msecTimeout = htons(msecTimeout);
        data.push_back((net_msecTimeout >> (8 * 0)) & 0xff);
        data.push_back((net_msecTimeout >> (8 * 1)) & 0xff);
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_MOTION_TO, data);
	}

    virtual SequenceId setPermanentOptionFlags(const CommandParameters data) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_OPTIONS_FLAG, data);
	}

    virtual SequenceId getPermanentOptionFlags() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_GET_OPTIONS_FLAG);
	}

    virtual SequenceId setTemporaryOptionFlags(const CommandParameters data) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_TEMP_OPTIONS_FLAG, data);
	}

    virtual SequenceId getTemporaryOptionFlags() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_GET_TEMP_OPTIONS_FLAG);
	}

    virtual SequenceId getConfigurationBlock(const ubyte id) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_GET_CONFIG_BLK, { id });
	}

    virtual SequenceId setSSBModifierBlock(const uint pwd, const CommandParameters data) {
        std::vector<ubyte> commandData;
        uint net_pwd = htons(pwd);
        commandData.push_back((net_pwd >> (8 * 0)) & 0xff);
        commandData.push_back((net_pwd >> (8 * 1)) & 0xff);
        commandData.push_back((net_pwd >> (8 * 2)) & 0xff);
        commandData.push_back((net_pwd >> (8 * 3)) & 0xff);
        commandData.insert(commandData.end(), data.begin(), data.end());
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_SSB_PARAMS, commandData);
	}

    virtual SequenceId setDeviceMode(const bool enableHackMode) {
        ubyte flag_enableHackMode = enableHackMode ? 0x01 : 0x00;
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_DEVICE_MODE, { flag_enableHackMode });
	}

    virtual SequenceId setConfigurationBlock(const CommandParameters data) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_CFG_BLOCK, data);
	}

    virtual SequenceId getDeviceMode() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_GET_DEVICE_MODE);
	}

    virtual SequenceId getSSB() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_GET_SSB);
    }

    virtual SequenceId setSSB(const uint password, const CommandParameters data) {
        std::vector<ubyte> commandData;
        uint net_password = htons(password);
        commandData.push_back((net_password >> (8 * 0)) & 0xff);
        commandData.push_back((net_password >> (8 * 1)) & 0xff);
        commandData.push_back((net_password >> (8 * 2)) & 0xff);
        commandData.push_back((net_password >> (8 * 3)) & 0xff);
        commandData.insert(commandData.end(), data.begin(), data.end());
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_SSB, commandData);
	}

    virtual SequenceId refillBank(const ubyte type) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SSB_REFILL, { type });
	}

    virtual SequenceId buyConsumable(const ubyte id, const ubyte quantity) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SSB_BUY, { id, quantity });
	}

    virtual SequenceId useConsumable(const ubyte id) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SSB_USE_CONSUMEABLE, { id });
	}

    virtual SequenceId grantCores(const uint password, const uint quantity, const ubyte flags) {
        std::vector<ubyte> data;
        uint net_password = htons(password);
        uint net_quantity = htons(quantity);
        data.push_back((net_password >> (8 * 0)) & 0xff);
        data.push_back((net_password >> (8 * 1)) & 0xff);
        data.push_back((net_password >> (8 * 2)) & 0xff);
        data.push_back((net_password >> (8 * 3)) & 0xff);
        data.push_back((net_quantity >> (8 * 0)) & 0xff);
        data.push_back((net_quantity >> (8 * 1)) & 0xff);
        data.push_back((net_quantity >> (8 * 2)) & 0xff);
        data.push_back((net_quantity >> (8 * 3)) & 0xff);
        data.push_back(flags);
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SSB_GRANT_CORES, data);
	}

    virtual SequenceId addXP(const uint password, const uint quantity) {
        std::vector<ubyte> data;
        uint net_password = htons(password);
        uint net_quantity = htons(quantity);
        data.push_back((net_password >> (8 * 0)) & 0xff);
        data.push_back((net_password >> (8 * 1)) & 0xff);
        data.push_back((net_password >> (8 * 2)) & 0xff);
        data.push_back((net_password >> (8 * 3)) & 0xff);
        data.push_back((net_quantity >> (8 * 0)) & 0xff);
        data.push_back((net_quantity >> (8 * 1)) & 0xff);
        data.push_back((net_quantity >> (8 * 2)) & 0xff);
        data.push_back((net_quantity >> (8 * 3)) & 0xff);
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SSB_ADD_XP, data);
	}

    virtual SequenceId levelUpAttribute(const uint password, const ubyte attrId) {
        std::vector<ubyte> data;
        uint net_password = htons(password);
        data.push_back((net_password >> (8 * 0)) & 0xff);
        data.push_back((net_password >> (8 * 1)) & 0xff);
        data.push_back((net_password >> (8 * 2)) & 0xff);
        data.push_back((net_password >> (8 * 3)) & 0xff);
        data.push_back(attrId);
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SSB_LEVEL_UP_ATTR, data);
	}

    virtual SequenceId getPasswordSeed() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_GET_PW_SEED);
	}

    virtual SequenceId enableSSBAsyncMessages(const bool enable = true) {
        ubyte flag_enable = enable ? 0x01 : 0x00;
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SSB_ENABLE_ASYNC, { flag_enable });
	}

    virtual SequenceId runMacro(const ubyte id) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_RUN_MACRO, { id });
	}

    virtual SequenceId saveTemporaryMacro(const CommandParameters macroData) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SAVE_TEMP_MACRO, macroData);
	}

    virtual SequenceId saveMacro(const CommandParameters macroData) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SAVE_MACRO, macroData);
	}

    virtual SequenceId reinitMacroExecutive() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_REINIT_MACRO_EXECUTIVE);
	}

    virtual SequenceId abortMacro() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_ABORT_MACRO);
	}

    virtual SequenceId getMacroStatus() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_GET_MACRO_STATUS);
	}

    virtual SequenceId setMacroParameter(const ubyte parameter, const ubyte valueOne, const ubyte value) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_MACRO_PARAMETER, { parameter, valueOne, value });
	}

    virtual SequenceId appendMacroChunk(const CommandParameters macroData) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_APPEND_MACRO_CHUNK, macroData);
	}

    virtual SequenceId eraseOrbBasicStorage(const ubyte area) {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_ERASE_ORBBASIC_STORAGE, { area });
	}

    virtual SequenceId appendOrbBasicFragment(const ubyte area, const CommandParameters fragment) {
        std::vector<ubyte> data;
        data.push_back(area);
        data.insert(data.end(), fragment.begin(), fragment.end());
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_APPEND_ORBBASIC_FRAGMENT, data);
    }

    virtual SequenceId executeOrbBasicProgram(const ubyte area, const ushort startLine) {
        std::vector<ubyte> data;
        ushort net_startLine = htons(startLine);
        data.push_back(area);
        data.push_back((net_startLine >> (8 * 0)) & 0xff);
        data.push_back((net_startLine >> (8 * 1)) & 0xff);
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_EXECUTE_ORBBASIC_PROGRAM, data);
    }

    virtual SequenceId abortOrbBasicProgram() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_ABORT_ORBBASIC_PROGRAM);
    }

    virtual SequenceId submitValueToInputStatement(const uint value) {
        std::vector<ubyte> data;
        uint net_value = htons(value);
        data.push_back((net_value >> (8 * 0)) & 0xff);
        data.push_back((net_value >> (8 * 1)) & 0xff);
        data.push_back((net_value >> (8 * 2)) & 0xff);
        data.push_back((net_value >> (8 * 3)) & 0xff);
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SUBMIT_VALUE_TO_INPUT_STATEMENT, data);
    }

    virtual SequenceId commitRAMProgramToFlash() {
        return deviceSend(DeviceId_SPHERO, SpheroCommandId_COMMIT_RAM_PROGRAM_TO_FLASH);
    }

    //------------------------------------------------------------------------------------------------------------------
private:
    SequenceId deviceSend(DeviceId device, ubyte command, std::vector<ubyte> data = std::vector<ubyte>()) {
        if(_state != SpheroState_Connected)
            return 0;

        SequenceId seqId = nextSeqId();
        std::vector<ubyte> commandData = generateCommand(device, command, seqId, data);

        #ifdef SPHERORAW_DEBUG
                std::stringstream mss;
                mss << "[SEND][" << std::setw(3) << commandData.size() << "B] ";
                for(size_t idx = 0; idx < commandData.size(); ++idx) {
                    mss << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(commandData[idx]) << " ";
                }
                std::cout << mss.str() << std::endl;
        #endif

        if(!BluetoothSend(_socket, commandData))
            return 0;

        return seqId;
    }

    SpheroState deviceReceive() {
        if(_state != SpheroState_Connected)
            return _state;

        bool receiveState = false;
        std::vector<ubyte> receiveData;

        // Receive all pending data
        while(true) {
            receiveData.clear();
            receiveState = BluetoothReceive(_socket, receiveData);
            
            if(receiveData.size() <= 0 || receiveState != true)
                break;
            
            _receiveBuffer.insert(_receiveBuffer.end(), receiveData.begin(), receiveData.end());
        }

        // Parse data to messages
        while(_receiveBuffer.size() > SPHERO_RESPONSE_HEADER_SIZE) {
            if(_receiveBuffer[0] != SPHERO_RESPONSE_SOP1_MASK)
                return SpheroState_Disconnected;

            SpheroMessage newMessage;
            size_t dataLength = 0;
            if(_receiveBuffer[1] == SPHERO_RESPONSE_SOP2_MASK_ACK) {
                newMessage.responseCode = (ResponseCode)_receiveBuffer[2];
                newMessage.sequenceId = _receiveBuffer[3];
                dataLength = _receiveBuffer[4];
            }
            else if(_receiveBuffer[1] == SPHERO_RESPONSE_SOP2_MASK_ASYNC) {
                newMessage.idCode = (AsyncResponseIdCode)_receiveBuffer[2];
                dataLength = (_receiveBuffer[3] << 8 | _receiveBuffer[4]);
            }
            else {
                return SpheroState_Disconnected;
            }
            
            // Check if message can be complete
            if(_receiveBuffer.size() < (dataLength + SPHERO_RESPONSE_HEADER_SIZE))
                break;

            size_t messageLength = SPHERO_RESPONSE_HEADER_SIZE + dataLength;

            // Check message checksum
            unsigned long byteSum = std::accumulate(_receiveBuffer.begin() + 2, _receiveBuffer.begin() + (messageLength - 1), 0);
            ubyte checkSum = static_cast<ubyte>(~(byteSum % 256));
            if(_receiveBuffer[dataLength+4] != checkSum)
                return SpheroState_Disconnected;

            #ifdef SPHERORAW_DEBUG
                        std::stringstream mss;
                        mss << "[RECV][" << std::setw(3) << messageLength << "B] ";
                        for(size_t idx = 0; idx < messageLength; ++idx) {
                            mss << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(_receiveBuffer[idx]) << " ";
                        }
                        std::cout << mss.str() << std::endl;
            #endif

            newMessage.data.insert(newMessage.data.end(), _receiveBuffer.begin() + SPHERO_RESPONSE_HEADER_SIZE, _receiveBuffer.begin() + (messageLength - 1));
            _receiveBuffer.erase(_receiveBuffer.begin(), _receiveBuffer.begin() + messageLength);

            _messages.push_back(newMessage);
        }

        // Check for errors
        if(receiveState == false)
            return SpheroState_Disconnected;

        return SpheroState_Connected;
    }

    std::vector<ubyte> generateCommand(DeviceId device, ubyte command, ubyte sequence = 0x00, std::vector<ubyte> data = std::vector<ubyte>()) {

        std::vector<ubyte> returnData;

        returnData.push_back(SPHERO_REQUEST_SOP1_MASK);
        returnData.push_back(SPHERO_REQUEST_SOP2_MASK | SPHERO_REQUEST_SOP2_MASK_ANSWER);
        returnData.push_back(device);
        returnData.push_back(command);
        returnData.push_back(sequence);
        returnData.push_back(data.size() + 1);

        returnData.insert(returnData.end(), data.begin(), data.end());

        unsigned long byteSum = std::accumulate(returnData.begin() + 2, returnData.end(), 0);

        returnData.push_back(static_cast<ubyte>(~(byteSum % 256)));

        return returnData;
    }
};

//======================================================================================================================


