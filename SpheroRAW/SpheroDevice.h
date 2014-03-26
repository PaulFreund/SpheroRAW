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

//======================================================================================================================

#include <winsock2.h>
#include <ws2bth.h>
#include "BluetoothAPIs.h"
#pragma comment(lib, "Bthprops.lib")

#include <iostream>

//======================================================================================================================

class SpheroDevice : ISpheroDevice {
public:
    SpheroDevice(std::string name) {
        HBLUETOOTH_DEVICE_FIND founded_device;

        BLUETOOTH_DEVICE_INFO device_info;
        device_info.dwSize = sizeof(device_info);

        BLUETOOTH_DEVICE_SEARCH_PARAMS search_criteria;
        search_criteria.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
        search_criteria.fReturnAuthenticated = TRUE;
        search_criteria.fReturnRemembered = FALSE;
        search_criteria.fReturnConnected = TRUE;
        search_criteria.fReturnUnknown = FALSE;
        search_criteria.fIssueInquiry = FALSE;
        search_criteria.cTimeoutMultiplier = 0;
        founded_device = BluetoothFindFirstDevice(&search_criteria, &device_info);



        if(founded_device == NULL) {
        }

        do {
            std::cout << "founded device: " << device_info.szName << std::endl;

        }
        while(BluetoothFindNextDevice(founded_device, &device_info));
    }

    ~SpheroDevice() {

    }

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


