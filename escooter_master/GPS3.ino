 #include <SoftwareSerial.h>
 
 #define GPS_RX 13
 SoftwareSerial GpsSerial(GPS_RX,GPS_RX); // RX, TX

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
	mySerial.println (message);
}

String strUrlBase = "https://maps.google.com/maps?q=";

String getGPSMessage()
{
  String latitude = getDecimalDegrees(gps_gga_lat_s());
  String longitude = getDecimalDegrees(gps_gga_long_s());

  String result = strUrlBase + latitude + "," + longitude;
  return result;
}

void GPSSetup()  
{
  
  // set the data rate for the SoftwareSerial port
  pinMode(GPS_RX, INPUT);
//  pinMode(8, OUTPUT);
  // set the data rate for the SoftwareSerial port
  GpsSerial.begin(9600);

}

static int gps_read_test () {
	uint32_t start_time = millis ();
	if (!GpsSerial.available()) {
		//mySerial.println("No serial info from GPS");
		return 0;
	}
	gpsBufferSize = 0;	
//	for (int i=0; i<GPS_BUF_SIZE; i++) {
//		if (GpsSerial.available ()) {
			//gps_buf [i] = GpsSerial.read ();
//                        mySerial.print((char) GpsSerial.read());
//		} else {
//			gpsBufferSize = i;
//			return 1;
//		}
//	}
        while (GpsSerial.available()) {
          mySerial.print((char) GpsSerial.read());
        }
	return 0;
}


void GPSLoop() // run over and over
{
//  mySerial.println("in GPS loop");
//	int stat = gps_get_gga ();	// read data from GPS, return 0 is ok
      if (gps_read_test ()) {
//		int gpggaIndex = get_GPGGA();
//mySerial.println(gpsBufferSize);

//              int gpggaIndex = -1;
//	for (int i = 0; i < gpsBufferSize; i++) {
//		if (is_GPGGA(i)) {			
//			gpggaIndex = i;
//		}
//        	}
//	  gpggaIndex = -1;
        }

/*		
		if (gpggaIndex >= 0) {
			build_gga_p (gpggaIndex);	// build *gga_p[] by gps_buf
			gps_gga_set_str ();
			if (checksum_gga () == 0)
				stat = 0;
			else 
				stat = 1;
		} else 
			stat = 2;
	} else 
		stat = 3;
*/	
	int stat = 3;
if ((stat == 0 || stat == 1)) {
//          char* strPhoneNumber = "18601602912";
            if (gps_gga_is_fix ()) {	//true if fix

             String message = getGPSMessage();
              mySerial.println(message.c_str());
            } else {
              mySerial.println("No GPS fix");
            }
        } else {
  //        mySerial.print("Stat: ");
    //      mySerial.println(stat);

       }
   
}
