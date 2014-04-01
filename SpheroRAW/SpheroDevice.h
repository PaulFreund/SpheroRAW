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

#ifdef _DEBUG
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
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId performLevel1Diagnostics() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId performLevel2Diagnostics() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId clearCouters() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId assignTimeValue() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId pollPacketTimes(const uint spheroRxTime, const uint spheroTxTime) {
		return INVALID_SEQUENCE_ID;
	}

    // Sphero commands
    virtual SequenceId setHeading(const ushort heading) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setStabilisation(const bool enable) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setRotationRate(const ubyte rate) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId getChassisId() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId selfLevel(const MessageData data) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setDataStreaming(const MessageData data) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId configureCollisionDetection(const MessageData data) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId configureLocator(const MessageData data) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setAccelerometerRange(const ubyte range) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId readLocator() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setRGBLedOutput(const ubyte red, const ubyte green, const ubyte blue, const bool persist) {
        std::vector<ubyte> colorData;
        colorData.push_back(red);
        colorData.push_back(green);
        colorData.push_back(blue);

        ubyte persistFlag = persist ? 0x01 : 0x00;
        colorData.push_back(persistFlag);

        return deviceSend(DeviceId_SPHERO, SpheroCommandId_SET_RGB_LED, colorData);
    }

    virtual SequenceId setBackLEDOutput(const ubyte intensity) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId getRGBLed() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId roll(const ubyte speed, const ushort heading, const ubyte state) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId boost(const bool enable) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setRAWMotorValues(const ubyte modeLeft, const ubyte powerLeft, const ubyte modeRight, const ubyte powerRight) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setMotionTimeout(const ushort msecTimeout) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setPermanentOptionFlags(const MessageData data) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId getPermanentOptionFlags() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setTemporaryOptionFlags(const MessageData data) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId getTemporaryOptionFlags() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId getConfigurationBlock(const ubyte id) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setSSBModifierBlock(const uint pwd, const MessageData data) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setDeviceMode(const bool enableHackMode) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setConfigurationBlock(const MessageData data) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId getDeviceMode() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId getSSB() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setSSB(const uint password, const MessageData data) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId refillBank(const ubyte type) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId buyConsumable(const ubyte id, const ubyte quantity) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId useConsumable(const ubyte id) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId grantCores(const uint password, const uint quantity, const ubyte flags) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId addXP(const uint password, const uint quantity) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId levelUpAttribute(const uint password, const ubyte attrId) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId getPasswordSeed() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId boolenableSSBAsyncMessages(const bool enable) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId runMacro(const ubyte id) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId saveTemporaryMacro(const MessageData macroData) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId saveMacro(const MessageData macroData) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId reinitMacroExecutive() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId abortMacro() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId getMacroStatus() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId setMacroParameter(const ubyte parameter, const ubyte valueOne, const ubyte value) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId appendMacroChunk(const MessageData macroData) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId eraseOrbBasicStorage(const ubyte area) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId appendOrbBasicFragment(const ubyte area, const MessageData fragment) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId executeOrbBasicFragment(const ubyte area) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId abortOrbBasicProgram() {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId submitValueToInputStatement(const uint value) {
		return INVALID_SEQUENCE_ID;
	}

    virtual SequenceId commitRAMProgramToFlash() {
		return INVALID_SEQUENCE_ID;
	}

    //------------------------------------------------------------------------------------------------------------------
private:
    SequenceId deviceSend(DeviceId device, ubyte command, std::vector<ubyte> data = std::vector<ubyte>()) {
        if(_state != SpheroState_Connected)
            return 0;

        SequenceId seqId = nextSeqId();
        std::vector<ubyte> commandData = generateCommand(device, command, seqId, data);

#ifdef _DEBUG
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

#ifdef _DEBUG
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


