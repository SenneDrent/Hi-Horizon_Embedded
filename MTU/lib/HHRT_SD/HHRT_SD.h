#ifndef HHRT_SD_h
#define HHRT_SD_h

    #include "Arduino.h"
    #include "buffer.h"
    #include "SdFat.h"
    #include "DataFrame.h"

    #define SD_CONFIG  SdioConfig(FIFO_SDIO)

    class HHRT_SD {
        public:
            unsigned long lastUpdateMillis= 0;

            char* generalLogFileName = (char*) "ALLdata.txt";
            String generalColumnTitles =
                "unixTime[uint32],"
                "millis[uint32],"
                "pZon[uint16],"
                "batteryVoltage(mppt)[float32],"
                "distance[float32],"
                "speed[float32],"
                "batteryVoltage(motorcontroller)[float32],"
                "batteryCurrent(motorcontroller)[float32],"
                "PowerToMotor[float32],"
                "Power[float32],"
                "tempMotor[uint8],"
                "rpm[float32],"
                "cellTemp[float32],"
                "accuEnvTemp[float32],";
            char* errorFileName = (char*) "errorLog.txt";
            String errorColumnTitles = 
                "part,"
                "code,"
                "time,";

            uint8_t init();
            uint8_t writeRow(char* fileName, String row);
            uint8_t writeHeader(char* fileName, String columnTitles);
            uint8_t writeDataLogRow(DataFrame* data);
            uint8_t triggerErrorLog(DataFrame* data, String type, int code);
        private:
            FsFile file;
            uint8_t writeBuffer[512];
            int32_t index = 0;
    };

#endif 