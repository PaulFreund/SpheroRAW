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
        const ubyte SPHERO_SEQ1_MASK        = 0xFF;
        const ubyte SPHERO_SEQ2_MASK        = 0xFC;
        const ubyte SPHERO_SEQ2_MASK_RESET  = 0x02;
        const ubyte SPHERO_SEQ2_MASK_ANSWER = 0x01;

        enum DeviceId {
            DeviceId_CORE       = 0x00,
            DeviceId_BOOTLOADER = 0x01,
            DeviceId_SPHERO     = 0x02
            
        };

        enum CoreCommandId {
            CoreCommandId_PING                  = 0x01,
            CoreCommandId_VERSION               = 0x02,
            CoreCommandId_CONTROL_UART_TX       = 0x03,
            CoreCommandId_SET_BT_NAME           = 0x10,
            CoreCommandId_GET_BT_NAME           = 0x11,
            CoreCommandId_SET_AUTO_RECONNECT    = 0x12,
            CoreCommandId_GET_AUTO_RECONNECT    = 0x13,
            CoreCommandId_GET_PWR_STATE         = 0x20,
            CoreCommandId_SET_PWR_NOTIFY        = 0x21,
            CoreCommandId_SLEEP                 = 0x22,
            CoreCommandId_GET_POWER_TRIPS       = 0x23,
            CoreCommandId_SET_POWER_TRIPS       = 0x24,
            CoreCommandId_SET_INACTIVE_TIMER    = 0x25,
            CoreCommandId_GOTO_BL               = 0x30,
            CoreCommandId_RUN_L1_DIAGS          = 0x40,
            CoreCommandId_RUN_L2_DIAGS          = 0x41,
            CoreCommandId_CLEAR_COUNTERS        = 0x42,
            CoreCommandId_ASSIGN_TIME           = 0x50,
            CoreCommandId_POLL_TIMES            = 0x51
        };

        enum BootloaderCommandId {
            BootloaderCommandId_BEGIN_REFLASH           = 0x02,
            BootloaderCommandId_HERE_IS_PAGE            = 0x03,
            BootloaderCommandId_LEAVE_BOOTLOADER        = 0x04,
            BootloaderCommandId_IS_PAGE_BLANK           = 0x05,
            BootloaderCommandId_CMD_ERASE_USER_CONFIG   = 0x06
        };

        enum SpheroCommandId {
            SpheroCommandId_SET_CAL                 = 0x01,
            SpheroCommandId_SET_STABILIZ            = 0x02,
            SpheroCommandId_SET_ROTATION_RATE       = 0x03,
            SpheroCommandId_SET_CREATION_DATE       = 0x04,
            SpheroCommandId_GET_BALL_REG_WEBSITE    = 0x05,
            SpheroCommandId_REENABLE_DEMO           = 0x06,
            SpheroCommandId_GET_CHASSIS_ID          = 0x07,
            SpheroCommandId_SET_CHASSIS_ID          = 0x08,
            SpheroCommandId_SELF_LEVEL              = 0x09,
            SpheroCommandId_SET_VDL                 = 0x0A,
            SpheroCommandId_SET_DATA_STREAMING      = 0x11,
            SpheroCommandId_SET_COLLISION_DET       = 0x12,
            SpheroCommandId_LOCATOR                 = 0x13,
            SpheroCommandId_SET_ACCELERO            = 0x14,
            SpheroCommandId_READ_LOCATOR            = 0x15,
            SpheroCommandId_SET_RGB_LED             = 0x20,
            SpheroCommandId_SET_BACK_LED            = 0x21,
            SpheroCommandId_GET_RGB_LED             = 0x22,
            SpheroCommandId_ROLL                    = 0x30,
            SpheroCommandId_BOOST                   = 0x31,
            SpheroCommandId_MOVE                    = 0x32,
            SpheroCommandId_SET_RAW_MOTORS          = 0x33,
            SpheroCommandId_SET_MOTION_TO           = 0x34,
            SpheroCommandId_SET_OPTIONS_FLAG        = 0x35,
            SpheroCommandId_GET_OPTIONS_FLAG        = 0x36,
            SpheroCommandId_SET_TEMP_OPTIONS_FLAG   = 0x37,
            SpheroCommandId_GET_TEMP_OPTIONS_FLAG   = 0x38,
            SpheroCommandId_GET_CONFIG_BLK          = 0x40,
            SpheroCommandId_SET_SSB_PARAMS          = 0x41,
            SpheroCommandId_SET_DEVICE_MODE         = 0x42,
            SpheroCommandId_SET_CFG_BLOCK           = 0x43,
            SpheroCommandId_GET_DEVICE_MODE         = 0x44,
            SpheroCommandId_GET_SSB                 = 0x46,
            SpheroCommandId_SET_SSB                 = 0x47,
            SpheroCommandId_SSB_REFILL              = 0x48,
            SpheroCommandId_SSB_BUY                 = 0x49,
            SpheroCommandId_SSB_USE_CONSUMEABLE     = 0x4A,
            SpheroCommandId_SSB_GRANT_CORES         = 0x4B,
            SpheroCommandId_SSB_ADD_XP              = 0x4C,
            SpheroCommandId_SSB_LEVEL_UP_ATTR       = 0x4D,
            SpheroCommandId_GET_PW_SEED             = 0x4E,
            SpheroCommandId_SSB_ENABLE_ASYNC        = 0x4F,
            SpheroCommandId_RUN_MACRO               = 0x50,
            SpheroCommandId_SAVE_TEMP_MACRO         = 0x51
        };

        enum ResponseCode {
			ResponseCode_OK			    = 0x00,
			ResponseCode_EGEN			= 0x01,
			ResponseCode_ECHKSUM		= 0x02,
			ResponseCode_EFRAG			= 0x03,
			ResponseCode_EBAD_CMD		= 0x04,
			ResponseCode_EUNSUPP		= 0x05,
			ResponseCode_EBAD_MSG		= 0x06,
			ResponseCode_EPARAM		    = 0x07,
			ResponseCode_EEXEC			= 0x08,
			ResponseCode_EBAD_DID		= 0x09,
			ResponseCode_MEM_BUSY		= 0x0A,
			ResponseCode_BAD_PASSWORD	= 0x0B,
			ResponseCode_POWER_NOGOOD	= 0x31,
			ResponseCode_PAGE_ILLEGAL	= 0x32,
			ResponseCode_FLASH_FAIL	    = 0x33,
			ResponseCode_MA_CORRUPT	    = 0x34,
			ResponseCode_MSG_TIMEOUT	= 0x35
        };
        enum AsyncResponseIdCodes {
            AsyncResponseId_PowerNotification           = 0x01,
            AsyncResponseId_Level1DiagnosticResponse    = 0x02,
            AsyncResponseId_SensorDataStreaming         = 0x03,
            AsyncResponseId_ConfigBlockContents         = 0x04,
            AsyncResponseId_PreSleepWarning10Sec        = 0x05,
            AsyncResponseId_MacroMarkers                = 0x06,
            AsyncResponseId_CollisionDetected           = 0x07,
            AsyncResponseId_orbBasicPrintMessage        = 0x08,
            AsyncResponseId_orbBasicErrorMessageASCII   = 0x09,
            AsyncResponseId_orbBasicErrorMessageBinary  = 0x0A,
            AsyncResponseId_SelfLevelResult             = 0x0B,
            AsyncResponseId_GyroAxisLimitExceeded       = 0x0C,
            AsyncResponseId_SpheroSoulData              = 0x0D, 
            AsyncResponseId_LevelUpNotification         = 0x0E,
            AsyncResponseId_ShieldDamageNotification    = 0x0F,
            AsyncResponseId_XPUpdateNotification        = 0x10,
            AsyncResponseId_BoostUpdateNotification     = 0x11
        };

        static std::vector<ubyte> GenerateCommand(DeviceId device, ubyte command, ubyte sequence = 0x00, std::vector<ubyte> data = std::vector<ubyte>()) {

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