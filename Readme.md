# About

SpheroRAW is a low level interface to Sphero(2.0) requiring only a bluetooth stack and providing a easy to use library.

# Requirements

Currently Visual Studio 2013 on Windows is required to build and the binary will work from Windows 7 upwards. Linux support (using bluez instead of the WinAPI) can be implemented when required.

# Features

## Implemented

* Connect to a Sphero and communicate with it on Windows

## Planned

* Connect to Sphero
* Read Sphero inputs
* Write Sphero outputs

## Unsure

* Device programming (Macros/orbBasic)

# Documentation
The documentation will be extended once the interface is stable.

## SpheroRAW API

The interface is defined in [SpheroRAWItf.h](https://github.com/PaulFreund/SpheroRAW/blob/master/SpheroRAW/SpheroRAWItf.h)

## Sphero Device
The low level Sphero API is documented [here](https://github.com/orbotix/DeveloperResources/raw/master/docs/Sphero_API_1.50.pdf).

# License

SpheroRAW is released under the MIT license