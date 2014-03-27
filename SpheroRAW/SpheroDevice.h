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
#include "SpheroRAWItf.h"

#include "SpheroProtocol.h"
using namespace Sphero::Protocol;
#ifdef _WIN32
    #include "BluetoothWrapper_Win32.h"
#else
    #error
#endif

//======================================================================================================================

class SpheroDevice : public ISpheroDevice {
private:
    std::string _name;
    SpheroState _state;
    BluetoothSocket _socket = BLUETOOTH_SOCKET_INVALID;

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

public:
    virtual SpheroState state() {
        return _state;
    }

    virtual SpheroState connect() {
        _socket = BluetoothConnect(_name);

        if(_socket == BLUETOOTH_SOCKET_INVALID)
            _state = SpheroState_Error_ConnectionFailed;
        else
            _state = SpheroState_Connected;

#ifdef _DEBUG
        std::vector<ubyte> pingData = GenerateCommand(DeviceId_CORE, CoreCommandId_PING);
        BluetoothSend(_socket, pingData);

        std::vector<ubyte> colorData;
        colorData.push_back(0xFF);
        colorData.push_back(0x00);
        colorData.push_back(0xFF);
        colorData.push_back(0x00);
        std::vector<ubyte> setRGBLedData = GenerateCommand(DeviceId_SPHERO, SpheroCommandId_SET_RGB_LED, 0x00, colorData);
        BluetoothSend(_socket, setRGBLedData);
#endif

        return _state;
    }

    virtual SpheroState disconnect() {
        BluetoothDisconnect(_socket);
        _socket = BLUETOOTH_SOCKET_INVALID;
        _state = SpheroState_Disconnected;
        return _state;
    }

    virtual SpheroDataInput pull() {
        std::vector<ubyte> data;
        if(!BluetoothReceive(_socket, data))
            _state = SpheroState_Disconnected;

        return SpheroDataInput();
    }

    virtual void push(SpheroDataOutput data) {
        return;
    }
};

//======================================================================================================================


