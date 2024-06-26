/* ****************************************************************************
 * Created By: Tauseef Ahmad
 * Create On: 8th January, 2022
 * 
 * How To Interface ADXL335 Analog Accelerometer Sensor With Arduino
 * Tutorial: https://youtu.be/HrIBGJep-1o
 * 
 *****************************************************************************/

//--------------------------------------------------------------
#define xPin A1
#define yPin A2
#define zPin A3
//--------------------------------------------------------------
//calibrated minimum and maximum Raw Ranges for each axis
//use calibration.ino file to get these values
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int xMin = 272;
int xMax = 408;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int yMin = 269;
int yMax = 408;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int zMin = 287;
int zMax = 430;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//--------------------------------------------------------------
// Take multiple samples to reduce noise
const int samples = 10;
//--------------------------------------------------------------



void setup() 
{
  //analogReference(EXTERNAL);
  Serial.begin(9600);
}


void loop() 
{
  //--------------------------------------------------------------
  //Read raw values
  int xRaw=0,yRaw=0,zRaw=0;
  for(int i=0;i<samples;i++)
  {
    xRaw+=analogRead(xPin);
    yRaw+=analogRead(yPin);
    zRaw+=analogRead(zPin);
  }
  xRaw/=samples;
  yRaw/=samples;
  zRaw/=samples;
  //--------------------------------------------------------------
  //Convert raw values to 'milli-Gs"
  //Convert value of RawMin to -1000
  //Convert value of RawMax to 1000
  long xMilliG = map(xRaw, xMin, xMax, -1000, 1000);
  long yMilliG = map(yRaw, yMin, yMax, -1000, 1000);
  long zMilliG = map(zRaw, zMin, zMax, -1000, 1000);
  //--------------------------------------------------------------
  // re-scale to fractional Gs
  float x_g_value = xMilliG / 1000.0;
  float y_g_value = yMilliG / 1000.0;
  float z_g_value = zMilliG / 1000.0;
  //--------------------------------------------------------------
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.print(x_g_value,0);
  //Serial.print(x_g_value,2);
  Serial.print("G");
  Serial.print("\t");
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.print(y_g_value,0);
  //Serial.print(y_g_value,2);
  Serial.print("G");
  Serial.print("\t");
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.print(z_g_value,0);
  //Serial.print(z_g_value,2);
  Serial.print("G");
  Serial.println();
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //--------------------------------------------------------------
  delay(200);
}