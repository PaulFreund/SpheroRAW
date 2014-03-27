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
#include <vector>
#include <algorithm>
#include <numeric>
typedef unsigned char ubyte;

//======================================================================================================================

namespace Sphero {
    namespace Protocol {
        const ubyte SPHERO_SEQ1_MASK = 0xFF;
        const ubyte SPHERO_SEQ2_MASK = 0xFC;
        const ubyte SPHERO_SEQ2_MASK_RESET = 0x02;
        const ubyte SPHERO_SEQ2_MASK_ANSWER = 0x01;

        enum SpheroDeviceId {
            SpheroDeviceId_Core = 0x00
        };

        enum SpheroCommandId {
            SpheroCommandId_Ping = 0x01
        };

        enum SpheroResponseCode {
			SpheroResponseCode_ORBOTIX_RSP_CODE_OK			    = 0x00,
			SpheroResponseCode_ORBOTIX_RSP_CODE_EGEN			= 0x01,
			SpheroResponseCode_ORBOTIX_RSP_CODE_ECHKSUM			= 0x02,
			SpheroResponseCode_ORBOTIX_RSP_CODE_EFRAG			= 0x03,
			SpheroResponseCode_ORBOTIX_RSP_CODE_EBAD_CMD		= 0x04,
			SpheroResponseCode_ORBOTIX_RSP_CODE_EUNSUPP			= 0x05,
			SpheroResponseCode_ORBOTIX_RSP_CODE_EBAD_MSG		= 0x06,
			SpheroResponseCode_ORBOTIX_RSP_CODE_EPARAM			= 0x07,
			SpheroResponseCode_ORBOTIX_RSP_CODE_EEXEC			= 0x08,
			SpheroResponseCode_ORBOTIX_RSP_CODE_EBAD_DID		= 0x09,
			SpheroResponseCode_ORBOTIX_RSP_CODE_MEM_BUSY		= 0x0A,
			SpheroResponseCode_ORBOTIX_RSP_CODE_BAD_PASSWORD	= 0x0B,
			SpheroResponseCode_ORBOTIX_RSP_CODE_POWER_NOGOOD	= 0x31,
			SpheroResponseCode_ORBOTIX_RSP_CODE_PAGE_ILLEGAL	= 0x32,
			SpheroResponseCode_ORBOTIX_RSP_CODE_FLASH_FAIL		= 0x33,
			SpheroResponseCode_ORBOTIX_RSP_CODE_MA_CORRUPT		= 0x34,
			SpheroResponseCode_ORBOTIX_RSP_CODE_MSG_TIMEOUT		= 0x35
        };
        enum SpheroAsyncResponseIdCodes {
            SpheroAsyncResponseId_PowerNotification             = 0x01,
            SpheroAsyncResponseId_Level1DiagnosticResponse      = 0x02,
            SpheroAsyncResponseId_SensorDataStreaming           = 0x03,
            SpheroAsyncResponseId_ConfigBlockContents           = 0x04,
            SpheroAsyncResponseId_PreSleepWarning10Sec          = 0x05,
            SpheroAsyncResponseId_MacroMarkers                  = 0x06,
            SpheroAsyncResponseId_CollisionDetected             = 0x07,
            SpheroAsyncResponseId_orbBasicPrintMessage          = 0x08,
            SpheroAsyncResponseId_orbBasicErrorMessageASCII     = 0x09,
            SpheroAsyncResponseId_orbBasicErrorMessageBinary    = 0x0A,
            SpheroAsyncResponseId_SelfLevelResult               = 0x0B,
            SpheroAsyncResponseId_GyroAxisLimitExceeded         = 0x0C,
            SpheroAsyncResponseId_SpheroSoulData                = 0x0D, 
            SpheroAsyncResponseId_LevelUpNotification           = 0x0E,
            SpheroAsyncResponseId_ShieldDamageNotification      = 0x0F,
            SpheroAsyncResponseId_XPUpdateNotification          = 0x10,
            SpheroAsyncResponseId_BoostUpdateNotification       = 0x11
        };

        static std::vector<ubyte> GenerateClientCommand(ubyte device, ubyte command, ubyte sequence, std::vector<ubyte> data = std::vector<ubyte>()) {
            std::vector<ubyte> returnData;

            returnData.push_back(SPHERO_SEQ1_MASK);
            returnData.push_back(SPHERO_SEQ2_MASK | SPHERO_SEQ2_MASK_ANSWER);
            returnData.push_back(device);
            returnData.push_back(command);
            returnData.push_back(sequence);
            returnData.push_back(data.size()+1);

            returnData.insert(returnData.end(), data.begin(), data.end());

            unsigned long byteSum = std::accumulate(returnData.begin()+2, returnData.end(), 0);

            returnData.push_back(static_cast<ubyte>(~(byteSum%256)));

            return returnData;
        }

        struct RawClientCommandHeader {
            ubyte SOP1;             // SOP1     Start of Packet #1      Always FFh
            ubyte SOP2;             // SOP2     Start of Packet #2      F8 to FFh encoding 4 bits of per - message options(see below)
            ubyte DID;              // DID      Device ID               The virtual device this packet is intended for
            ubyte CID;              // CID      Command ID              The command code
            ubyte SEQ;              // SEQ      Sequence Number         This client field is echoed in the response for all synchronous commands(and ignored by Sphero when SOP2 has bit 0 clear)
            ubyte DLEN;             // DLEN     Data Length             The number of bytes following through the end of the packet
        };
        // Data block after the header: 
        // DATA     Data                    Optional data to accompany the Command
        // CHK      Checksum                The modulo 256 sum of all the bytes from the DID through the end of the data payload, bit inverted(1's complement)


        struct RawSpheroResponseHeader {
            ubyte SOP1;             // SOP1     Start of Packet #1      Always FFh
            ubyte SOP2;             // SOP2     Start of Packet #2      Set to FFh when this is an acknowledgement, FEh when this is an asynchronous message
            ubyte MRSP;             // MRSP     Message Response        This is generated by the message decoder of the virtual device (refer to the appropriate appendix for a list of values)
            ubyte SEQ;              // SEQ      Sequence Number         Echoed to the client when this is a direct message response (set to 00h when SOP2 = FEh)
            ubyte DLEN;             // DLEN     Data Length             The number of bytes following through the end of the packet 
        };
        // Data block after the header: 
        // DATA     Data                    Optional data in response to the Command or based on "streaming" data settings
        // CHK      Checksum                The modulo 256 sum of all the bytes from the DID through the end of the data payload, bit inverted(1's complement)

        struct RawSpheroAsyncResponseHeader {
            ubyte SOP1;             // SOP1     Start of Packet #1      Always FFh
            ubyte SOP2;             // SOP2     Start of Packet #2      FEh as this is an asynchronous message
            ubyte ID_CODE;          // ID CODE  Response ID             The ID identifies the type of data to be expecting
            ubyte DLEN_MSB;         // DLEN-MSB MSB part of DLEN        Echoed to the client when this is a direct message response (set to 00h when SOP2 = FEh)
            ubyte DLEN_LSB;         // DLEN-LSB LSB part of DLEN        The number of bytes following through the end of the packet 
        };
        // Data block after the header: 
        // DATA     Data                    Optional data in response to the Command or based on "streaming" data settings
        // CHK      Checksum                The modulo 256 sum of all the bytes from the DID through the end of the data payload, bit inverted(1's complement)

    }
}