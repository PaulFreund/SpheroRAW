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

#include <vector>
// #include "SpheroProtocol.h"

//======================================================================================================================

enum SpheroState {
    SpheroState_None = 0,
    SpheroState_Error_BluetoothError = 1,
    SpheroState_Error_BluetoothUnavailable = 2,
    SpheroState_Error_NotPaired = 3,
    SpheroState_Error_ConnectionFailed = 4,
    SpheroState_Disconnected = 5,
    SpheroState_Connected = 6
};

typedef unsigned char ubyte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef std::vector<ubyte> UndefinedData;

struct SpheroNotification {
    //Sphero::Protocol::AsyncResponseIdCodes type;
    UndefinedData data;
};

/*
enum OptionFlag {

};
*/

class ISpheroDevice {
public:
    virtual SpheroState state() = 0;
    virtual SpheroState connect() = 0;
    virtual SpheroState disconnect() = 0;

    virtual std::vector<SpheroNotification> receive() = 0;
/*
    // Core commands
    virtual bool ping() = 0;
    virtual bool getVersioning(UndefinedData& versioning) = 0;
    virtual bool setUARTTxLine(const bool enable) = 0;
    virtual bool setDeviceName(const std::string name) = 0;
    virtual bool getBluetoothInfo(UndefinedData& bluetoothInfo) = 0;
    virtual bool setAutoReconnect(const bool enable, const ubyte timeout) = 0;
    virtual bool getAutoReconnect(bool& enabled, ubyte& timeout) = 0;
    virtual bool getPowerState(UndefinedData& powerState) = 0;
    virtual bool setPowerNotification(const bool enable) = 0;
    virtual bool sleep(const short secSleepDuration, const ushort wakeupMacro, const ushort wakeupOrbBasicMacro) = 0;
    virtual bool getVoltageTripPoints(ushort& voltLow, ushort& voltCritical) = 0;
    virtual bool setVoltageTripPoints(const ushort voltLow, const ushort voltCritical) = 0;
    virtual bool setInactivityTimeout(const ushort secInactivityTimeout) = 0;
    virtual bool jumpToBootloader() = 0;
    virtual bool performLevel1Diagnostics(UndefinedData& report) = 0;
    virtual bool performLevel2Diagnostics(UndefinedData& report) = 0;
    virtual bool clearCouters() = 0;
    virtual bool assignTimeValue(uint& timeValue) = 0;
    virtual bool pollPacketTimes(uint& clientTxTime, const uint spheroRxTime, const uint spheroTxTime) = 0;

    // Sphero commands
    virtual bool setHeading(const ushort heading) = 0;
    virtual bool setStabilisation(const bool enable) = 0;
    virtual bool setRotationRate(const ubyte rate) = 0;
    virtual bool getChassisId(ushort& id) = 0;
    virtual bool selfLevel(const UndefinedData data) = 0;
    virtual bool setDataStreaming(const UndefinedData data) = 0;
    virtual bool configureCollisionDetection(const UndefinedData data) = 0;
    virtual bool configureLocator(const UndefinedData data) = 0;
    virtual bool setAccelerometerRange(const ubyte range) = 0;
    virtual bool readLocator(UndefinedData& data) = 0;
    virtual bool setRGBLedOutput(const ubyte red, const ubyte green, const ubyte blue, const bool persist) = 0;
    virtual bool setBackLEDOutput(const ubyte intensity) = 0;
    virtual bool getRGBLed(ubyte& red, ubyte& green, ubyte& blue) = 0;
    virtual bool roll(const ubyte speed, const ushort heading, const ubyte state) = 0;
    virtual bool boost(const bool enable) = 0;
    virtual bool setRAWMotorValues(const ubyte modeLeft, const ubyte powerLeft, const ubyte modeRight, const ubyte powerRight) = 0;
    virtual bool setMotionTimeout(const ushort msecTimeout) = 0;
    virtual bool setPermanentOptionFlags(const UndefinedData data) = 0;
    virtual bool getPermanentOptionFlags(UndefinedData& data) = 0;
    virtual bool setTemporaryOptionFlags(const UndefinedData data) = 0;
    virtual bool getTemporaryOptionFlags(UndefinedData& data) = 0;
    virtual bool getConfigurationBlock(const ubyte id, ubyte& data) = 0;
    virtual bool setSSBModifierBlock(const uint pwd, const UndefinedData data) = 0;
    virtual bool setDeviceMode(const bool enableHackMode) = 0;
    virtual bool setConfigurationBlock(const UndefinedData data) = 0;
    virtual bool getDeviceMode(bool& hackModeEnabled) = 0;
    virtual bool getSSB() = 0;
    virtual bool setSSB(const uint password, const UndefinedData data) = 0;
    virtual bool refillBank(const ubyte type, uint& coresRemaining) = 0;
    virtual bool buyConsumable(const ubyte id, const ubyte quantity, ubyte& quantityRemaining, uint& coresRemaining) = 0;
    virtual bool useConsumable(const ubyte id, ubyte& quantityRemaining) = 0;
    virtual bool grantCores(const uint password, const uint quantity, const ubyte flags, uint& newCoresCount) = 0;
    virtual bool addXP(const uint password, const uint quantity, uint& xpPercentToNextLevel) = 0;
    virtual bool levelUpAttribute(const uint password, const ubyte attrId, ubyte& attrLevel, ushort& attrPtsRemaining) = 0;
    virtual bool getPasswordSeed(uint& seed) = 0;
    virtual bool boolenableSSBAsyncMessages(const bool enable) = 0;
    virtual bool runMacro(const ubyte id) = 0;
    virtual bool saveTemporaryMacro(const UndefinedData macroData) = 0;
    virtual bool saveMacro(const UndefinedData macroData) = 0;
    virtual bool reinitMacroExecutive() = 0;
    virtual bool abortMacro(ubyte& id, ushort& cmdNum) = 0;
    virtual bool getMacroStatus(ubyte& id, ushort& cmdNum) = 0;
    virtual bool setMacroParameter(const ubyte parameter, const ubyte valueOne, const ubyte value) = 0;
    virtual bool appendMacroChunk(const UndefinedData macroData) = 0;
    virtual bool eraseOrbBasicStorage(const ubyte area) = 0;
    virtual bool appendOrbBasicFragment(const ubyte area, const UndefinedData fragment) = 0;
    virtual bool executeOrbBasicFragment(const ubyte area, ushort&startLine) = 0;
    virtual bool abortOrbBasicProgram() = 0;
    virtual bool submitValueToInputStatement(const uint value) = 0;
    virtual bool commitRAMProgramToFlash() = 0;
*/
};

//======================================================================================================================

SPHERO_RAW_API ISpheroDevice* SpheroRAW_Create(const char* nameBuffer);
SPHERO_RAW_API void SpheroRAW_Destroy(ISpheroDevice* deviceHandle);

//======================================================================================================================



