// libraries_____________________________________________________________________
#include <SPI.h>
#include <epd2in9.h> //epaper
#include <epdpaint.h> 
#define COLORED     0
#define UNCOLORED   1
#include <MS5611.h>  //barosensor
#define Addr 0x18 // MCP9808 I2C address is 0x18(24) tempsensor
#include <Wire.h> // serial monitor


//epaper module___________________________________________________________________
/**
  * Due to RAM not enough in Arduino UNO, a frame buffer is not allowed.
  * In this case, a smaller image buffer is allocated and you have to 
  * update a partial display several times.
  * 1 byte = 8 pixels, therefore you have to set 8*N pixels at a time.
  */
unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8 
Epd epd;
int counterDisp = 0;

//Barosensor module______________________________________________________________
MS5611 baro;
// create variable insta pressure
double pressure;
double pressureA;
double pressureB;
double pressureC;
double pressureD;
double pressureE;
// create variable recorded pressure
double pressureP;
// create variable pressure diff between recorded and insta
double diffPress;

//tempsensor module______________________________________________________________
// create variable recorded temp
float temperatureP = 0;
// create variable recorded temp when change above abs treshold
float treshTemp = 0;
// create variable temp diff between recorded and insta
float diffTemp;
// create variable for temp correction with recorded temp
float tempCorr;

//Buzzer module_______________________________________________________________________
// create la note for buzzer
float tonal = 440;
// create one note pitch for buzzer
float pitch = 1.059463119;
// strictly positive number for pitch modulo
int variaNote = 1;
// create counter for resetting tonal to 440 hz if no temp change for 5 seconds
int counter = 1;

//sensibility thermal sniffer potentiometer module____________________________________
int treshCount = 1; //
// select the analog input pin for the potentiometer
int potPin = 2;
// variable to store the value coming from the sensor
float pot = 0;
float val = 0;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(9600); // Start serial (UART) for monitor
  //Barosensor module______________________________________________________________
  // Start barometer
  baro = MS5611();
  baro.begin();
  // Serial.begin(9600);
  delay(2);
  
//epaper module___________________________________________________________________
 if (epd.Init(lut_full_update) != 0) {
      return;
  }
  /** 
   *  there are 2 memory areas embedded in the e-paper display
   *  and once the display is refreshed, the memory area will be auto-toggled,
   *  i.e. the next action of SetFrameMemory will set the other memory area
   *  therefore you have to clear the frame memory twice.
   */
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();

  paint.SetRotate(ROTATE_0);
  paint.SetWidth(128);
  paint.SetHeight(24);

  /* For simplicity, the arguments are explicit numerical coordinates */
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 4, "Captain Ovious", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());
  
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 4, "Thermal sniffer", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 30, paint.GetWidth(), paint.GetHeight());
  
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 4, "V 1.0", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 50, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();

 delay(2000);
 
  if (epd.Init(lut_partial_update) != 0) {
      return;
    }
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();
  
//tempsensor module______________________________________________________________
// Initialise I2C communication as MASTER  
Wire.begin();  
// Initialise Serial Communication, set baud rate = 9600  
Serial.begin(9600);    
// Start I2C Transmission  
Wire.beginTransmission(Addr);  
// Select configuration register  
Wire.write(0x01);  
// Continuous conversion mode, Power-up default  
Wire.write(0x00);  
Wire.write(0x00);  
// Stop I2C Transmission  
Wire.endTransmission();    
// Start I2C Transmission  
Wire.beginTransmission(Addr);  
// Select resolution register  
Wire.write(0x08);  
// Resolution = +0.0625 / C  
Wire.write(0x03);  
// Stop I2C Transmission  
Wire.endTransmission();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop()
{  
  Serial.begin(9600);
  //Barosensor module______________________________________________________________
   // Read insta pressure in mbar and make average pressure baro can be offset
  pressureA = baro.getPressure();
  //tempsensor module______________________________________________________________
//tempsensor first read
unsigned int data[2];    
// Starts I2C communication  
Wire.beginTransmission(Addr);  
// Select data register  
Wire.write(0x05);  
// Stop I2C transmission  
Wire.endTransmission();    
// Request 2 bytes of data  
Wire.requestFrom(Addr, 2);    
// Read 2 bytes of data  
// temp MSB, temp LSB  
if(Wire.available() == 2)  
{    
data[0] = Wire.read();    
data[1] = Wire.read();  
}    
// Convert the data to 13-bits  
int temp = ((data[0] & 0x1F) * 256 + data[1]);  
if(temp > 4095)  
{    
temp -= 8192;  
}  
float cTemp1 = temp * 0.0625;  
  //Barosensor module______________________________________________________________
  pressureB = baro.getPressure();
  //tempsensor module______________________________________________________________
  //tempsensor second read
// Starts I2C communication  
Wire.beginTransmission(Addr);  
// Select data register  
Wire.write(0x05);  
// Stop I2C transmission  
Wire.endTransmission();    
// Request 2 bytes of data  
Wire.requestFrom(Addr, 2);    
// Read 2 bytes of data  
// temp MSB, temp LSB  
if(Wire.available() == 2)  
{    
data[0] = Wire.read();    
data[1] = Wire.read();  
}    
// Convert the data to 13-bits  
temp = ((data[0] & 0x1F) * 256 + data[1]);  
if(temp > 4095)  
{    
temp -= 8192;  
}  
float cTemp2 = temp * 0.0625;  
  //Barosensor module______________________________________________________________
  pressureC = baro.getPressure();
  //tempsensor module______________________________________________________________
  //tempsensor third read
// Starts I2C communication  
Wire.beginTransmission(Addr);  
// Select data register  
Wire.write(0x05);  
// Stop I2C transmission  
Wire.endTransmission();    
// Request 2 bytes of data  
Wire.requestFrom(Addr, 2);    
// Read 2 bytes of data  
// temp MSB, temp LSB  
if(Wire.available() == 2)  
{    
data[0] = Wire.read();    
data[1] = Wire.read();  
}    
// Convert the data to 13-bits  
temp = ((data[0] & 0x1F) * 256 + data[1]);  
if(temp > 4095)  
{    
temp -= 8192;  
}  
float cTemp3 = temp * 0.0625;  
  //Barosensor module______________________________________________________________
  pressureD = baro.getPressure();
 //tempsensor module______________________________________________________________
  //tempsensor fourth read
// Starts I2C communication  
Wire.beginTransmission(Addr);  
// Select data register  
Wire.write(0x05);  
// Stop I2C transmission  
Wire.endTransmission();    
// Request 2 bytes of data  
Wire.requestFrom(Addr, 2);    
// Read 2 bytes of data  
// temp MSB, temp LSB  
if(Wire.available() == 2)  
{    
data[0] = Wire.read();    
data[1] = Wire.read();  
}    
// Convert the data to 13-bits  
temp = ((data[0] & 0x1F) * 256 + data[1]);  
if(temp > 4095)  
{    
temp -= 8192;  
}  
float cTemp4 = temp * 0.0625;   
  //Barosensor module______________________________________________________________
  pressureE = baro.getPressure();
  //tempsensor module______________________________________________________________
  //tempsensor fifth read
// Starts I2C communication  
Wire.beginTransmission(Addr);  
// Select data register  
Wire.write(0x05);  
// Stop I2C transmission  
Wire.endTransmission();    
// Request 2 bytes of data  
Wire.requestFrom(Addr, 2);    
// Read 2 bytes of data  
// temp MSB, temp LSB  
if(Wire.available() == 2)  
{    
data[0] = Wire.read();    
data[1] = Wire.read();  
}    
// Convert the data to 13-bits  
temp = ((data[0] & 0x1F) * 256 + data[1]);  
if(temp > 4095)  
{    
temp -= 8192;  
}  
float cTemp5 = temp * 0.0625;  
  //Barosensor module______________________________________________________________
  pressure = (pressureA + pressureB + pressureC + pressureD + pressureE)/500;
  if (pressureP < 10 )
    {
      pressureP = pressure;
    }
  // get pressure diff between recorded press and insta press
  diffPress = (pressure - pressureP);
  // get temp correction with pressure diff
  tempCorr = diffPress*0.09411616;
//record pressure for next loop
 pressureP = pressure ;  

//epaper module___________________________________________________________________
counterDisp = counterDisp +1;
if (counterDisp > 40)
{char temp[8]; // Buffer big enough for 7-character float
  // display temp
  dtostrf(temperatureP, 6, 2, temp); // Leave room for too large numbers! dtostrf transform float to char
  paint.SetRotate(ROTATE_0);
  paint.SetWidth(128);
  paint.SetHeight(24);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 10, temp , &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());
  // display baro
  char pressu[8];
  dtostrf(pressureP, 6, 2, pressu); 
  paint.DrawStringAt(0, 30, pressu , &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 30, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
  counterDisp=0;
}
//tempsensor module______________________________________________________________
  // get temp diff between recorded temp and insta temp

  diffTemp = (cTemp1 + cTemp2 + cTemp3 + cTemp4 + cTemp5)/5 - temperatureP + tempCorr;
 //record temp for next loop
 temperatureP = (cTemp1 + cTemp2 + cTemp3 + cTemp4 + cTemp5)/5;
 
 //sensibility thermal sniffer potentiometer module____________________________________
 // read the value from the potentiometer as treshold for beep with temp change, analogRead() returns a number between 0 and 1023 that is proportional to the amount of voltage being applied to the pin. 
  pot = analogRead(potPin)/100;   
  val = pot/10 + 0.6; 
   
 //Buzzer module_______________________________________________________________________
 // speakertone code to be tweaked 
 // val = treshold breaker from potentiometer

    if (treshCount < 2) // set treshTemp as temperatureP at startup and if temp stagnation over 20 loops
          {
          treshTemp = temperatureP; 
          treshCount = 2;
          }
  if ( (abs(diffTemp) - (val/10)) > 0)
    {
      if (diffTemp > 0.00) // temp increase
        {
        counter = 2;
        // create absolute value of temp diff
        variaNote = abs(treshTemp - temperatureP)*5;
        //Serial.println(variaNote);
        tonal = 440 * pow(pitch,variaNote);
        tone(6,tonal,1000);
        }  
      if (-diffTemp > 0.00 ) // temp decrease
        {
        counter = 2;
        variaNote = (1 - abs(treshTemp - temperatureP))*5;
        tonal = 440 / pow(pitch,variaNote);
        tone(6,tonal,1000);
        } 
    }
 if ( (abs(diffTemp) - (val/10)) < 0) // temp stagnation
  {
  counter = counter +1;
  if (counter < 20)
    {
    tone(6,tonal,1000); //reset tonal if stagnation
    }
  //Serial.print(" counter = ");Serial.println(counter);
  if (counter > 20) //
    {
    treshCount = 1;
    counter = 22;
    
   }
  }
}
