 #include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
 
  // set the data rate for the SoftwareSerial port
  pinMode(10, INPUT);
  pinMode(11, OUTPUT);
  mySerial.begin(9600);
  Serial.println("Soft Serial setup complete");
}

void loop() // run over and over
{
  while (mySerial.available())
    Serial.write(mySerial.read());
//  if (Serial.available())
  //  mySerial.write(Serial.read());
}
