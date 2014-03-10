// Product name: GPS/GPRS/GSM Module V3.0
// # Product SKU : TEL0051

// # Description:
// # The sketch for controling the GSM/GPRS/GPS module via SMS.
// # Steps:
// #        1. Turn the S1 switch to the Prog(right side)
// #        2. Turn the S2 switch to the USB side(left side)
// #        3. Plug the GSM/GPS jumper caps to the GSM side
// #        4. Upload the sketch to the Arduino board(Make sure turn off other Serial monitor )
// #        5. Turn the S1 switch to the comm(left side)
// #        6. Turn the S2 switch to the Arduino(right side)
// #        7. RST the board until the START led is on


#include <RCSwitch.h> // from transmitter code
#include <SoftwareSerial.h>

RCSwitch mySwitch = RCSwitch();  // from transmitter code

byte gsmDriverPin[3] = {
  3,4,5};//The default digital driver pins for the GSM and GPS mode
//If you want to change the digital driver pins
//or you have a conflict with D3~D5 on Arduino board,
//you can remove the J10~J12 jumpers to reconnect other driver pins for the module!
const int IGNrelay = 8;
const int MagSwitch = 12;
char inchar;
const int smsLedPin = 7;
const int selfDestruct = 13;

int ButtonState = 0;
int lastButtonState = 0;

const int softSerialIn = 9;
const int softSerialOut = 11;

SoftwareSerial mySerial(softSerialIn, softSerialOut);

void setup()
{
  // set the data rate for the SoftwareSerial port
  //  pinMode(softSerialIn, INPUT);
  pinMode(softSerialOut, OUTPUT);
  mySerial.begin(4800);
  mySerial.println("\nSetup Beginning");
  GPSSetup();

  // Transmitter is connected to Arduino Pin #10
  mySwitch.enableTransmit(10);

  //Init the driver pins for GSM function
  for(int i = 0 ; i < 3; i++){
    pinMode(gsmDriverPin[i],OUTPUT);
  }
  pinMode(IGNrelay,OUTPUT);
  Serial.begin(9600);                                      //set the baud rate
  digitalWrite(5,HIGH);                                     //Output GSM Timing
  delay(1500);
  digitalWrite(5,LOW);
  digitalWrite(3,LOW);                                      //Enable the GSM mode
  digitalWrite(4,HIGH);                                     //Disable the GPS mode
  delay(2000);
  delay(5000);                                              //call ready
  delay(5000);
  Serial.println("AT+CMGD=1,4");                           //Delete all SMS in box
  pinMode(MagSwitch, INPUT);
  pinMode(2, OUTPUT);
  pinMode(smsLedPin, OUTPUT);

  // start off as locked;
  sendScooterLock(true);

  mySerial.println("Setup Complete");
}

void sendScooterLock(boolean bLock)
{
  if (bLock) {
    mySwitch.send(15141954, 24);                       //Lock signal from RF Transmitter, using decimal code */
    mySerial.println("Preparing to send Lock Signal");
  }
  else {
    mySwitch.send(15141953, 24);                       //Unlock signal from RF Transmitter, using decimal code */
    mySerial.println("Preparing to send Unlock Signal");
  }
}

void checkForSMS()
{
  if(Serial.available()>0)
  {

    inchar=Serial.read();
    if(inchar=='T')
    {
      //      mySerial.println("Received character 'T'");
      delay(10);
      inchar=Serial.read();
      if (inchar=='I')                                      //When the GSM module get the message, it will display the sign '+CMTI "SM", 1' in the serial port
      {
        mySerial.println("Received SMS");
        delay(10);
        Serial.println("AT+CMGR=1");                       //When Arduino read the sign, send the "read" AT command to the module
        delay(10);
      } 
      else {
        //        mySerial.println("Received unexpected character: " + inchar);
      }
    } 
    else if (inchar=='L')
    {
      delay(10);
      inchar = Serial.read();
      if (inchar=='L') {
        mySerial.println("Received lock SMS");

        mySerial.println("Turning off eScooter power");
        delay(10);
        digitalWrite(IGNrelay,LOW);                         //Turn off eScooter power

        delay(500);
        sendScooterLock(true);
      } 
      else if (inchar=='H') {
        mySerial.println("Received unlock SMS");
        delay(500);
        sendScooterLock(false);
        digitalWrite(IGNrelay,HIGH);                         //Turn on led
      }
      Serial.println("AT+CMGD=1,4");                   //Delete all message
      delay(500);
    }
    //    else if (inchar=='U')                                     //Thw SMS("U") was display in the Serial port, and Arduino has recognize it.
    //    {
    //       mySerial.println("Received unlock signal");
    //
    //        delay(500);
    //        sendScooterLock(false);
    //
    //        delay(10);
    //        digitalWrite(IGNrelay,HIGH);                         //Turn on led
    //        Serial.println("AT+CMGD=1,4");                    //Delete all message
    //        delay(500);
    //    }
    else if (inchar=='D')
    {
      // activate self destruct
//      digitalWrite(selfDestruct, HIGH);
    }
  }
}

void sendSMS(String strPhoneNumber, String strMessage)
{
  mySerial.println("Sending SMS");
  return;
  Serial.println("AT"); //Send AT command
  delay(2000);
  Serial.println("AT");
  delay(2000);
  //Send message
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"" + strPhoneNumber+"\"");//Change the receiver phone number
  delay(1000);
  Serial.print(strMessage);
  delay(1000);
  Serial.write(26);
}

boolean bSMSSent=false;
int ALARM_INPUT=6;

void enableSMSLed(boolean bOn) {
  digitalWrite(smsLedPin, bOn ? HIGH : LOW);
}

boolean confirmAlarmOn()
{
  for (int i = 1; i < 3; i++)
  {
    enableSMSLed(false);
    delay(1000);
    if (digitalRead(ALARM_INPUT) != HIGH) {
      enableSMSLed(true);
      return false;
    }
  }
  return true;
}

boolean confirmMagSwitchOn()
{
  for (int i = 1; i < 3; i++)
  {
    delay(1000);
    if (digitalRead(MagSwitch) != HIGH) {
      return false;
    }
  }
  return true;
}


String gStrPhoneNumber = "18601602912";
String gStrMessage = "Your bike is being stolen";

void loop()
{
  // call the GPS3 file's loop
  GPSLoop();

  // read the magnetic switch state:
  ButtonState = digitalRead(MagSwitch);

  // compare the switch state to its previous state
  if (ButtonState != lastButtonState) {
    // if the kickstand is down, MagSwitch will be HIGH, need to lock and power off eScooter
    // from off to on:
    if (ButtonState == HIGH && confirmMagSwitchOn()) {
      mySerial.println("Magnetic switch is being turned on");
      digitalWrite(IGNrelay, LOW);                            // turns off ignition
      sendScooterLock(true);                       //Lock signal from RF Transmitter, using decimal code
    }
  }
  // save the current state as the last state for next time through the loop
  lastButtonState = ButtonState;
  checkForSMS();
  boolean bAlarmOn = digitalRead(ALARM_INPUT) == HIGH;
  if (bAlarmOn) {
    mySerial.println("Alarm seens to be on, confirming");
    bAlarmOn = confirmAlarmOn();
  }
  if (bAlarmOn) {
    mySerial.println("Alarm is on");
  }
  if (!bSMSSent && bAlarmOn) {
    sendSMS(gStrPhoneNumber, gStrMessage);
    bSMSSent=true;
  }
  if (!bAlarmOn && bSMSSent) {
    bSMSSent = false;
    enableSMSLed(false);
  }
}

