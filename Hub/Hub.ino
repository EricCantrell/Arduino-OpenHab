/* The Hub
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
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

void setup(){
  Serial.begin(9600);
  Mirf.spi = &MirfHardwareSpi; //Set the SPI Driver.
  Mirf.init(); //Setup pins / SPI.
  Mirf.setRADDR((byte *)"serv1"); //Configure reciving address.
  /* Set the payload length to sizeof(unsigned long) the
   * return type of millis().
   * NB: payload on client and server must be the same.
   */
  Mirf.payload = sizeof(unsigned long);
  Mirf.config();  //Write channel and payload config then power up reciver.
  Serial.println("Hub is up and listening...");
}

String inputString = "";
boolean stringComplete = false;  
void serialEvent() {  
  while (Serial.available()) {  
    char inChar = (char)Serial.read();  
    inputString += inChar;
  
    if (inChar == '\n') {  
      stringComplete = true;  
    }  
  }  

  Serial.println("Sending command 77");
  Mirf.setTADDR((byte *)"clie1"); //Set the send address.
  long data = 77;
  Mirf.send((byte *)&data); //Send the data back to the client.

  inputString = "";
  stringComplete = false;  
}

void loop(){
  byte data[Mirf.payload]; //A buffer to store the data.
  /* If a packet has been recived.
   * isSending also restores listening mode when it 
   * transitions from true to false.
   */
  if(!Mirf.isSending() && Mirf.dataReady()){
    Serial.println("Got packet");
    Mirf.getData(data); //Get load the packet into the buffer.
    Serial.println(long (data[0]));
//    Mirf.setTADDR((byte *)"clie1"); //Set the send address.
//    Mirf.send(data); //Send the data back to the client.
    /* Wait untill sending has finished
     * NB: isSending returns the chip to receving after returning true.
     */
//    Serial.println("Reply sent.");
  }
}
