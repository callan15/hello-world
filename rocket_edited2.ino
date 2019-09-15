#include "Wire.h"       // allows communication to i2c devices connected to arduino
#include "I2Cdev.h"     // I2Connection library (communication to serial port)
#include "MPU6050.h"    // IMU library
#include "Servo.h"      // servo control library
#include "Adafruit_BMP085.h" //BMP180 Alt,Pressure,Temp sensor
#include "SD.h"   //SD card reader
#include "SPI.h"  //SD card

File myFile;
int pinCS = 10;
MPU6050 mpu; //defines the chip as a MPU so it can be called in the future
 
int16_t ax, ay, az;  // x y z orientation values from accelerometer
int16_t gx, gy, gz;  // x y z orientation values from gyrscope
/////////////////////////////////////////////////////////////////////////////////////// 
Servo outer; 
Servo inner;
///////////////////////////////////////////////////////////////////////////////////////
int valo;     // outer val
int prevValo; // outer prev val
///////////////////////////////////////////////////////////////////////////////////////
int vali;  //inner val
int prevVali; //outer prev val
/////////////////////////////////////////////////////////////////////////////////////// 
//initializes the IMU

Adafruit_BMP085 bmp; 
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000;

void setup() 
{
    Wire.begin(); 
    Serial.begin(38400); 
    Serial.println("Initialize MPU");
    mpu.initialize();
    Serial.println(mpu.testConnection() ? "Connected" : "Connection failed"); 
    outer.attach(9); //servo on pin 9 for large ring y
    inner.attach(10);//servo on pin 8 for small ring x
    startMillis = millis();
    pinMode(pinCS,OUTPUT);
    if (SD.begin())  
    {
      Serial.println("SD card is ready to use.");
    } else
    {
      Serial.println("SD card initialization failed");
      return;
    }
    myFile = SD.open("test.txt",FILE_WRITE);
    if (myFile) {
      myFile.print("Time(ms), Temperature(C), Pressure(Pa), Altitude(m), ServoInner(degrees), ServoOuter(degrees)");
      myFile.print("\n");
    }

    bmp.begin();     
}
/////////////////////////////////////////////////////////////////////////////////////// 
void loop() 
{
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); 
 
    valo = map(ax/2, -17000, 17000, 180, 0);
    if (valo != prevValo)
    {
        outer.write(valo); 
        prevValo = valo; 
    }
    
    vali = map(ay/2, -17000, 17000, 180, 0);
    if (vali != prevVali)
    {
        inner.write(vali); 
        prevVali = vali; 
    }
    
  currentMillis = millis();
  
  if (currentMillis - startMillis >= period)
  {
    Serial.print(currentMillis);
    Serial.print(",");
    Serial.print(int(bmp.readTemperature()));
    Serial.print(",");
    Serial.print(int(bmp.readPressure()));
    Serial.print(",");
    Serial.print(int(bmp.readAltitude()));
    myFile = SD.open("test.txt",FILE_WRITE);
    
    if (currentMillis<1500) {
      myFile = SD.open("test.txt",FILE_WRITE);
      if (myFile) {
      myFile.print("Time(ms), Temperature(C), Pressure(Pa), Altitude(m), ServoInner(degrees), ServoOuter(degrees)");
      myFile.print("\n");
      myFile.print("\n");
      }
    }
    
    if (myFile) {
      myFile.print(currentMillis);
      myFile.print(",");
      myFile.print(bmp.readTemperature());
      myFile.print(",");
      myFile.print(bmp.readPressure());
      myFile.print(",");
      myFile.print(bmp.readAltitude());
      myFile.print(",");
      myFile.print(vali);
      myFile.print(",");
      myFile.print(valo);
      myFile.print("\n");
      myFile.close();
    } else {
      Serial.println("Error in opening file");
    }
    
    Serial.println();
    startMillis = currentMillis;
  }
}
