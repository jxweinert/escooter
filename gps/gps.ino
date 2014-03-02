/********************* start of sample code *************************/

/*
 *	created:	2013-08-02
 *	by:		lisper (leyapin@gmail.com)
 *	Version:	0.1
 *	test gps gsm on Leonardo &XBEE R3
 *
 */

  #ifndef DEBUG
  #define DEBUG
  #endif

#include <SoftwareSerial.h>

//

const int softSerialIn = 9;
const int softSerialOut = 11;

SoftwareSerial MySerial(softSerialIn, softSerialOut);

#include "gps_gsm_sim908.h"

void setup () {
#ifdef DEBUG
  pinMode(softSerialOut, OUTPUT);
  MySerial.begin(4800);
  MySerial.println("Setup Beginning");
#endif
  
	gps_init ();	//init GPS pin

	Serial.begin (9600);	//serial1 connect GPS
	while (!Serial);

	#ifdef DEBUG
	MySerial.println ("start GPS! ");
	#endif

	start_gps ();	//open GPS
}

void clearGPSData()
{
  for (int i = 0; i < GPS_BUF_SIZE; i++)
    gps_buf[i]=0;
}

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
	Serial.println (message);
}

String strUrlBase = "https://maps.google.com/maps?q=";

String getGPSMessage()
{
  String latitude = getDecimalDegrees(gps_gga_lat_s());
  String longitude = getDecimalDegrees(gps_gga_long_s());
  MySerial.println("Longitude: " + longitude);
  MySerial.println("Latitude: " + latitude);

  String result = strUrlBase + latitude + "," + longitude;
  return result;
}

//

long lastMessageSent = 0;
boolean bCannotGetFixMessageSent = false;
void loop () {
	int stat = gps_get_gga ();	// read data from GPS, return 0 is ok
	#ifdef DEBUG
        if (stat != 0) {
    	  MySerial.print ("gps_get_gga () return stat: ");
	  MySerial.println (stat);	//for test
        }
	#endif

//       long currentTime = millis();
       boolean bSendMessage = true;
     //  if (currentTime - lastMessageSent > 60000) // once a minute
       //{
         //bSendMessage = true;
         //lastMessageSent = currentTime;
        //MySerial.println("More than a minute since the last message.");
       //} else {
         //char strTime[20];
         //ltoa(currentTime, strTime, 10);
         //MySerial.print("Time at: ");
         //MySerial.println(strTime);
       //}
       
        if ((stat == 0 || stat == 1) && bSendMessage) {
          char* strPhoneNumber = "18601602912";
            if (gps_gga_is_fix ()) {	//true if fix
              gsm_set_numble (strPhoneNumber);	//
              String message = getGPSMessage();
              gsm_send_message(message.c_str());
              clearGPSData();
              gsm_end_send ();
              bCannotGetFixMessageSent = false;
              while(1)
                ;
            }// else if (bCannotGetFixMessageSent == false) {
              //gsm_set_numble (strPhoneNumber);	//
              //gsm_send_message("Cannot get GPS fix");
              //gsm_end_send ();
              //bCannotGetFixMessageSent = true;
            //}
	}

	//
	switch (stat) {
	case 0:
		#ifdef DEBUG
		MySerial.println ("data checksum is ok");
		#endif
		break;
	case 1:
		#ifdef DEBUG
		MySerial.println ("GPGGA ID is error!");
		#endif
		break;
	case 2:
		#ifdef DEBUG
		MySerial.println ("data is error!");
		#endif
		break;
	}

	#ifdef DEBUG
	MySerial.println ("$GPGGA data:");
	gps_gga_print ();	//for test
	#endif

	

	/*
	if (gps_gga_is_fix () == 0)	//check if is fix
		MySerial.println ("can't fix! please go outside!");
	else {
		MySerial.println ("ok! is fix!");

		MySerial.println ("gps_gga_utc_hh ()");
		MySerial.println (gps_gga_utc_hh ());
		MySerial.println ("gps_gga_utc_mm ()");
		MySerial.println (gps_gga_utc_mm ());
		MySerial.println ("gps_gga_utc_ss ()");
		MySerial.println (gps_gga_utc_ss ());

		MySerial.println ("gps_gga_NS ()");
		MySerial.println (gps_gga_NS (), 6);
		MySerial.println ("gps_gga_EW ()");
		MySerial.println (gps_gga_EW (), 6);

		MySerial.println ("gps_gga_lat ()");
		MySerial.println (gps_gga_lat (), 6);
		MySerial.println ("gps_gga_long ()");
		MySerial.println (gps_gga_long (), 6);
		MySerial.println ("gps_gga_HDOP ()");
		MySerial.println (gps_gga_HDOP (), 6);
		MySerial.println ("gps_gga_MSL ()");
		MySerial.println (gps_gga_MSL (), 6);
		MySerial.println ();
	}
	*/
}
