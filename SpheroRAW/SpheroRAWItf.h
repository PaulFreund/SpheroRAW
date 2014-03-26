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

struct SpheroDataInput {
/*
    Raw Inputs 
    * Three axis rotation rate gyro 
    * Three axis accelerometer 
    * Approximate ground speed 
    * Data from radio link 
    * Battery voltage 
*/
};

struct SpheroDataOutput {
/*
    Raw Outputs 
    * Power to left and right drive wheels 
    * RGB LED color value 
    * Back LED intensity 
    * Data to radio link 
*/
};

enum SpheroState {
    SpheroState_None = 0,
    SpheroState_Error_AdapterMissing = 1,
    SpheroState_Error_NotPaired = 2,
    SpheroState_Disconnected = 3,
    SpheroState_Connected = 4
};

class ISpheroDevice {
public:
    virtual SpheroState state() = 0;

    virtual SpheroState connect() = 0;
    virtual SpheroState disconnect() = 0;

    virtual SpheroDataInput pull() = 0;
    virtual void push(SpheroDataOutput data) = 0;
};

//======================================================================================================================

SPHERO_RAW_API ISpheroDevice* SpheroRAW_Create(const char* nameBuffer);
SPHERO_RAW_API void SpheroRAW_Destroy(ISpheroDevice* deviceHandle);

//======================================================================================================================



