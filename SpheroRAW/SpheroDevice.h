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
#include "BluetoothWrapper.h"

//======================================================================================================================

class SpheroDevice : ISpheroDevice {
private:
    SpheroState _state;

public:
    SpheroDevice(std::string name) : _state(SpheroState_None) {
        if(!BluetoothAdapterAvailable()) {
            _state = SpheroState_Error_AdapterMissing;
            return;
        }

        if(!BluetoothDevicePaired(name)) {
            _state = SpheroState_Error_NotPaired;
            return;
        }
    }

    ~SpheroDevice() {

    }

public:
    virtual SpheroState state() {
        return SpheroState_None;
    }

    virtual SpheroState connect() {
        return SpheroState_None;
    }

    virtual SpheroState disconnect() {
        return SpheroState_None;
    }

    virtual SpheroDataInput pull() {
        return SpheroDataInput();
    }

    virtual void push(SpheroDataOutput data) {
        return;
    }
};

//======================================================================================================================


