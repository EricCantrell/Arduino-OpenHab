/* Garage Module
 *
 * Pins:
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13
 *
 * Configurable:
 * CE -> 8
 * CSN -> 7
 *
 * Note: To see best case latency comment out all Serial.println
 * statements not displaying the result and load 
 * 'ping_server_interupt' on the server.
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

void setup(){
  Serial.begin(9600);

  //LED
  pinMode (10,OUTPUT);

  //Distance Sensor
  pinMode (2,OUTPUT);//attach pin 2 to vcc
  pinMode (3,OUTPUT);// attach pin 3 to Trig
  pinMode (4, INPUT);//attach pin 4 to Echo
  pinMode (5,OUTPUT);//attach pin 5 to GND
  digitalWrite(2, HIGH);

  //RF
  Mirf.spi = &MirfHardwareSpi; //Set the SPI Driver.
  Mirf.init(); //Setup pins / SPI.
  Mirf.setRADDR((byte *)"clie1"); //Configure reciving address.
  /* Set the payload length to sizeof(unsigned long) the
   * return type of millis().
   * NB: payload on client and server must be the same.
   */
  Mirf.payload = sizeof(unsigned long);
  /* Write channel and payload config then power up reciver. */
  /* To change channel:
   * Mirf.channel = 10;
   * NB: Make sure channel is legal in your area.
   */
  Mirf.config();  //Write channel and payload config then power up reciver.
  Serial.println("Firing up the Garage Module ... "); 
}

long lastState = 0;

void loop(){
  digitalWrite(10,LOW);

  //Distance Sensor
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, inches, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(3, LOW);
  delayMicroseconds(2);
  digitalWrite(3, HIGH);
  delayMicroseconds(5);
  digitalWrite(3, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration = pulseIn(4, HIGH);

  // convert the time into a distance
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
 
//  Serial.print("distance:");
//  Serial.print(inches);
//  Serial.print("in, ");
//  Serial.print(cm);
//  Serial.print("cm");
//  Serial.println();
  
  //RF out
  if (cm != lastState) {
    lastState = cm;
    Mirf.setTADDR((byte *)"serv1");
    Mirf.send((byte *)&cm);
    Serial.print("Sending: ");
    Serial.println(cm);
    while(Mirf.isSending()){
    }
    Serial.println("Finished sending");
    //RF in
    byte data[Mirf.payload]; //A buffer to store the data.
    /* If a packet has been recived.
     * isSending also restores listening mode when it 
     * transitions from true to false.
     */
    if(!Mirf.isSending() && Mirf.dataReady()){
      Mirf.getData(data); //Get load the packet into the buffer.
      Serial.print("Received: ");
      Serial.println(long (data[0]));
      if (long (data[0])==77) {
        digitalWrite(10,HIGH);
        delay(1000);  
      }
    }
  }
  delay(1000);
} 
  
long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
