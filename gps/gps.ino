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

//
void loop () {
	int stat = gps_get_gga ();	// read data from GPS, return 0 is ok
	#ifdef DEBUG
	MySerial.println ("gps_get_gga () return stat:");
	MySerial.println (stat);	//for test
	#endif

	if (stat == 0 || stat == 1) {
		if (gps_gga_is_fix ()) {	//true if fix
			//send_message ("18501683475", gps_gga_utc_s ());
			gsm_set_numble ("18601602912");	//
			gsm_send_message (gps_gga_utc_s ());
			gsm_send_message (gps_gga_EW ());
			gsm_send_message (gps_gga_NS ());
			gsm_send_message (gps_gga_lat_s ());
			gsm_send_message (gps_gga_long_s ());
			gsm_end_send ();
			delay(60000);
                        static int numOfSends = 0;
                        numOfSends++;
                        if (numOfSends > 5)
                          while (1);
		}
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
