# About

SpheroRAW is a low level interface to Sphero(2.0) requiring only a bluetooth stack and providing a easy to use library.

# Requirements

Currently Visual Studio 2013 on Windows is required to build and the binary will work from Windows 7 upwards. Linux support (using bluez instead of the WinAPI) can be implemented when required. BluetoothWrapper_Win32.h should then be copied to BluetoothWrapper_$(Platform) and included in SpheroDevice.h.

# Features

## Implemented

* Connect to a Sphero and communicate with it on Windows
* All of spheros commands, most with clearly defined parameters

## Unsure

* Make interface C-Style with POD-types only
* Implement more complex command parameters
* Return message interpretation

# Documentation

Creating and destroying device handle:
	ISpheroDevice* device = SpheroRAW_Create("Sphero-GRB");
	...
	SpheroRAW_Destroy(device);

Connect and disconnect from device:
	device->connect();
	...
	device->disconnect();
	
The API does not throw exceptions. After every command the connection status should be checked:
	if(device->state() == SpheroState_Connected) {
		...
	}
	
Every active command returns a SequenceId which cycles from 1 to 255 and is reflected in the belonging response:
	SequenceId pingId = device->ping();

Response packets and asynchroneus packets have to be periodically polled from the device:
	std::vector<SpheroMessage> newMessages = device->receive();
	
A SpheroMessage can either contain a response packet which is indicated by a nonzero sequenceId or a asynchroneus packet which is indicated by a nonzero idCode.

## SpheroRAW API

The interface is defined in [SpheroRAWItf.h](https://github.com/PaulFreund/SpheroRAW/blob/master/SpheroRAW/SpheroRAWItf.h)

## Sphero Device
The low level Sphero API is documented [here](https://github.com/orbotix/DeveloperResources/raw/master/docs/Sphero_API_1.50.pdf).

# License

SpheroRAW is released under the MIT license