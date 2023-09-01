#include <tinyBMS.h>

// TODO: mogelijk nog controle of de data correct is.

void tinyBMSdevice::assignValue(char* label, char* value)
{
  if(!strncmp(label, "BPC", 4)) this->data.BPC = atof(value);
  else if(!strncmp(label, "BPV", 4)) this->data.BPV = atof(value);
  else if(!strncmp(label, "MINCV", 6)) this->data.MINCV = atof(value);
  else if(!strncmp(label, "MAXCV", 6)) this->data.MAXCV = atof(value);
  else if(!strncmp(label, "BMST", 5)) this->data.BMST = atof(value);
  else if(!strncmp(label, "SOC", 4)) this->data.SOC = atof(value);
  else if(!strncmp(label, "EXT1", 5)) this->data.EXT1 = atof(value);
  else if(!strncmp(label, "EXT2", 5)) this->data.EXT2 = atof(value);
  else if(!strncmp(label, "OS", 3)) this->data.status.OS = atoi(value);
  else if(!strncmp(label, "LSS", 3)) this->data.status.LSS = atoi(value);
  else if(!strncmp(label, "LST", 3)) this->data.status.LST = atoi(value);
  else if(!strncmp(label, "CSS", 3)) this->data.status.CSS = atoi(value);
  else if(!strncmp(label, "CST", 3)) this->data.status.CST = atoi(value);
  else if(!strncmp(label, "CB", 3)) {
    for(int i = 0; i < AMOUNT_OF_CELLS; i++){
      this->data.status.cel[i].balancing = (value[i] - '0'); // Je zou maar niet begrijpen wat hier staat
    }
  }
}

void tinyBMSdevice::encodeTinyBMSSerial(char c)
{
    switch(c){
        case '\n':
          break;
        case '\r':
          break;
        case ':':
          buffer[charCounter] = '\0';
          strncpy(label, buffer, charCounter); // TODO: dit klopt niet voor strings die groter of kleiner zijn dan vier karakters.
          charCounter = 0;
          break;
        case '\t':
          buffer[charCounter] = '\0';
          assignValue(label, buffer);
          if(millis() - lastUpdated > 5000)
          {
            valid = false;
          } else {
            valid = true;
          }
          charCounter = 0;
          break;
        default:
          buffer[charCounter] = c;
          ++charCounter;
          break;
    }
}