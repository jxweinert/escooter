 #include <SoftwareSerial.h>
 
 SoftwareSerial GpsSerial(13,13); // RX, TX
 SoftwareSerial MySerial(11,10); // RX, TX

//#define DEBUG
 #include "gps_serial.h"


String getDecimalDegrees(const char* gpsInput)
{
  String inputString = gpsInput;
  int decimalPosition = -1;
  for (int i = 0; i < inputString.length(); i++)
  {
    if (inputString[i] == '.') 
      decimalPosition = i;
  }
  if (decimalPosition < 0) {
    return "";
  }
  int minutesPosition = decimalPosition - 2;
  String strDegrees = inputString.substring(0, minutesPosition);
  String strMinutes = inputString.substring(minutesPosition, inputString.length());

  double dDegrees = atof(strDegrees.c_str());
  double dMinutes = atof(strMinutes.c_str());
  double dCalculatedDegrees = dDegrees + dMinutes/60;
  
  char calculatedDegrees[30];
  dtostrf(dCalculatedDegrees,2,5,calculatedDegrees);
  return calculatedDegrees;  
}


// send message to mobile
void gsm_send_message (const char *message) {
	MySerial.println (message);
}

String strUrlBase = "https://maps.google.com/maps?q=";

String getGPSMessage()
{
  String latitude = getDecimalDegrees(gps_gga_lat_s());
  String longitude = getDecimalDegrees(gps_gga_long_s());
// MySerial.println("Longitude: " + longitude);
// MySerial.println("Latitude: " + latitude);

  String result = strUrlBase + latitude + "," + longitude;
  return result;
}

void setup()  
{
  // set the data rate for the SoftwareSerial port
//  pinMode(13, INPUT);
  pinMode(11, INPUT);
  pinMode(10, OUTPUT);

  // Open serial communications and wait for port to open:
  MySerial.begin(9600);
 
 
   
//  pinMode(8, OUTPUT);
  // set the data rate for the SoftwareSerial port
  //GpsSerial.begin(9600);

 MySerial.println("GPS Soft Serial setup complete");


}

  String message = "no location";
  
void loop() // run over and over
{
  MySerial.println("GPS LOOP");
        char GPSCall='A';
        /*
	int stat = gps_get_gga ();	// read data from GPS, return 0 is ok

        if ((stat == 0 || stat == 1)) {
            if (gps_gga_is_fix ()) {	//true if fix

             message = getGPSMessage();
            // MySerial.println(message.c_str());
           // } else {
           //  MySerial.println("No GPS fix");
            }
        } 
     */   

    if (MySerial.available() ) 
    GPSCall =MySerial.read();
    
     if(GPSCall=='C')
    MySerial.println(message);          
}
