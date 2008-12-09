/* AVR Butterfly temperature sensor library */

#ifndef butterfly_temp_h
#define butterfly_temp_h

#define CELSIUS     0
#define FAHRENHEIT  1

class TempSensor
{
  public:
    TempSensor(int units);
    int getTemp();
    int getTemp(int units);
    bool overSample;
    int units;
  private:
    int mapToF(int a2d);
    int mapToC(int a2d);    
};

extern TempSensor TempSense;

#endif
