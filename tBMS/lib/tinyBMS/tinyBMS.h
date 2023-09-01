#ifndef tinyBMS_h
#define tinyBMS_h

#include "Arduino.h"
#include "string.h"

#define BUFFER 64
#define AMOUNT_OF_CELLS 16

struct tinyBMSdata {
  double BPV;
  double MINCV;
  double MAXCV;
  double BPC;
  double BMST;
  double SOC;
  double EXT1;
  double EXT2;
  struct STATUS {
    int OS;
    bool LST;
    bool LSS;
    bool CST;
    bool CSS;
    struct BALANCING {
      bool balancing;
    } cel[AMOUNT_OF_CELLS];
  } status;
};

class tinyBMSdevice
{
    private:
        char buffer[BUFFER];
        char label[BUFFER];
        int charCounter = 0;
        unsigned long lastUpdated;
        bool valid = false;

        void assignValue(char* label, char* value);

    public:
        struct tinyBMSdata data;

        void encodeTinyBMSSerial(char c);
};

#endif