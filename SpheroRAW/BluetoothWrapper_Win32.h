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
#include <algorithm>

#include "AtlBase.h"
#include "AtlConv.h"

#include <winsock2.h>
#include <ws2bth.h>
#pragma comment(lib, "Ws2_32.lib")

#include "BluetoothAPIs.h"
#pragma comment(lib, "Bthprops.lib")

//======================================================================================================================

#define BLUETOOTH_SOCKET_INVALID INVALID_SOCKET
typedef SOCKET BluetoothSocket;

typedef unsigned char ubyte;

//======================================================================================================================

bool BluetoothInitialize() {
    WSADATA     WSAData = { 0 };
    return (WSAStartup(MAKEWORD(2, 2), &WSAData) == 0);
};

void BluetoothCleanup() {
    WSACleanup();
}

bool BluetoothAvailable() {
    bool adapterFound = false;

    HANDLE info;
    BLUETOOTH_FIND_RADIO_PARAMS params;
    params.dwSize = sizeof(params);

    HBLUETOOTH_RADIO_FIND find = BluetoothFindFirstRadio(&params, &info);

    if(find != 0) {
        adapterFound = true;
        CloseHandle(info);
    }

    BluetoothFindRadioClose(find);
    return adapterFound;
}

BLUETOOTH_ADDRESS BluetoothGetDeviceAddress(std::string name) {
    BLUETOOTH_ADDRESS returnAddress = { BLUETOOTH_NULL_ADDRESS }; // TODO: Check if this is valid
    BLUETOOTH_DEVICE_SEARCH_PARAMS params = {
        sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),
        TRUE, FALSE, FALSE, TRUE, FALSE, 0, NULL
    };

    BLUETOOTH_DEVICE_INFO info;
    info.dwSize = sizeof(info);
    HBLUETOOTH_DEVICE_FIND found = BluetoothFindFirstDevice(&params, &info);

    for(; found != NULL; BluetoothFindNextDevice(found, &info)) {
        if(strcmp(CW2A(info.szName), name.c_str()) == 0) {
            returnAddress = info.Address;
            break;
        }
    }

    BluetoothFindDeviceClose(found);
    return returnAddress;
}

bool BluetoothDevicePaired(std::string name) {
    return (BluetoothGetDeviceAddress(name).ullLong != BLUETOOTH_NULL_ADDRESS);
}

BluetoothSocket BluetoothConnect(std::string name) {
    BLUETOOTH_ADDRESS deviceAddress = BluetoothGetDeviceAddress(name);
    if(deviceAddress.ullLong == BLUETOOTH_NULL_ADDRESS) { 
        return BLUETOOTH_SOCKET_INVALID; 
    }

    SOCKADDR_BTH socketAddress;
    socketAddress.addressFamily = AF_BTH;
    socketAddress.btAddr = deviceAddress.ullLong;
    socketAddress.port = 0;
    socketAddress.serviceClassId = SerialPortServiceClass_UUID;

    SOCKET btSock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if(btSock == INVALID_SOCKET) { 
        return BLUETOOTH_SOCKET_INVALID; 
    }

    if(connect(btSock, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) == SOCKET_ERROR) { 
        return BLUETOOTH_SOCKET_INVALID; 
    }

    return btSock;
}

void BluetoothDisconnect(BluetoothSocket socket) {
    if(socket != BLUETOOTH_SOCKET_INVALID) {
        closesocket(socket);
    }
}

bool BluetoothSend(BluetoothSocket &socket, const std::vector<ubyte> data) {
    if(data.empty()) return true;

    SetLastError(0);
    int bytesSent = send(socket, reinterpret_cast<const char*>(data.data()), data.size(), 0);
    if(bytesSent == SOCKET_ERROR) {
        if(GetLastError() == WSAEWOULDBLOCK)
            return true;
        else
            return false;
    }

    return true;
}

bool BluetoothReceive(BluetoothSocket &socket, std::vector<ubyte>& data, bool bBlocking = false) {

    u_long blockingMode = bBlocking ? 0 : 1; // Non-zero means non-blocking
    if(ioctlsocket(socket, FIONBIO, &blockingMode) == SOCKET_ERROR) {
        return false;
    }

    data.reserve(2048);
    data.resize(2048);

    SetLastError(0);
    int bytesRead = recv(socket, reinterpret_cast<char*>(data.data()), data.capacity(), 0);
    if(bytesRead == 0) {
        data.resize(0);
        return false;
    }
    else if(bytesRead == SOCKET_ERROR) {
        data.resize(0);
        if(GetLastError() == WSAEWOULDBLOCK)
            return true;
        else
            return false;
    }

    data.resize(bytesRead);
    return true;
}
//======================================================================================================================

