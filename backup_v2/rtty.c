/*  NTX2B  -  434.425MHz Transmitter-G4AIU   RTTY Radio Test Part 1
   
    Project:  1.  Build a transmitter using the NTX2B 70cm module.
    	      2.  Use the Raspberry Pi computer to transmit RTTY data.
	      3.  Change the baud rate by altering "delay" - line 136
	      4.  Change the shift frequency by altering the parallel resistor across R3(47k) 


   RTTY Set-up: Breadboard / RasPi Break-out board:
   
   	1.   Connect BLUE wire between Line 12e (TXD) and Line 20e (LH side of R3-47K ohms)
	2.   On BO board - connect GREEN wire between P17 and Line 28f (RH side of BB: R3-47K ohms) 
	      
    Created 2012 by M0UPU as part of a UKHAS Guide on linking
    NTX2B Modules to Raspberry Pi.
    RTTY code from Rob Harrison - Icarus Project.
    http://ukhas.org.uk
    Code amended by Ciaran Morgan / M0XTD - 04 Feb 14
    
    
    
    
*/ 
 
#define GPIO17 0
 
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include "rtty.h"

 
char datastring[100];
 
void setup()

 {
 
  wiringPiSetup () ;                
  pinMode(GPIO17,OUTPUT);
  
}
 
void loop()

{
	char checksum[10];
	
	snprintf(datastring, sizeof(datastring), "$$RTTY TEST BEACON de G4AIU 100 bauds - 600Hz shift");
	
	snprintf(checksum, sizeof(checksum), "*%04X\n", gps_CRC16_checksum(datastring));
	
	if (strlen(datastring) > sizeof(datastring) - 4 - 1)
	{
		// Don't overflow the buffer. You should have made it bigger.
		return;
	}
 
	// Also copy checksum's terminating \0 (hence the +1).
	memcpy(datastring + strlen(datastring), checksum, strlen(checksum) + 1);

/*	
   sprintf(datastring,"$$$$$RYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRY\n");
   // Puts the text in the datastring
   
  rtty_txstring (datastring);
  
  sprintf(datastring,"ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789 ! @ Â £ . : ; -\n\n");
  //Puts the text in the datastring
  
  rtty_txstring (datastring);
     
  sprintf(datastring,"$$RTTY TEST BEACON de G4AIU 100 bauds - 300Hz shift\n\n"); 
  //Puts the text in the datastring

*/

  rtty_txstring (datastring);


  delay (2000);
  
}
  
void rtty_txstring (char * string)

{
 
  /* Simple function to send one char at a time to 
   	** rtty_txbyte function. 
   	** NB Each char is one byte (8 Bits)
   	*/
 
  char c;
 
  c = *string++;
 
  while ( c != '\0')
  
  {
    rtty_txbyte (c);
    c = *string++;
  }
  
}
 
 void rtty_txbyte (char c)
 
{

  /* Simple function to sent each bit of a char to 
   	** rtty_txbit function. 
   	** NB The bits are sent Least Significant Bit (LSB) first
   	** All chars should be preceded with a 0 and 
   	** proceded with a 1. 0 = Start bit; 1 = Stop bit
   	**
   	*/
 
  int i;
 
  rtty_txbit (0); // Start bit
 
  // Send bits for char LSB first      	
 
  for (i=0;i<7;i++) // Change here for 7 or 8 bit  ASCII-7 / ASCII-8
  
  {
  
    if (c & 1) rtty_txbit(1); 
 
    else rtty_txbit(0);	
 
    c = c >> 1;
 
  }
 
  rtty_txbit (1); // Stop bit	     // Send 2 Stop Bits after each char has been sent                           
  rtty_txbit (1); // Stop bit	     // For 1 Stop Bit - delete either Line 109 or 110
  
}
 
void rtty_txbit (int bit)

{
  if (bit)
  
  {
  
    // high
    digitalWrite(GPIO17, HIGH);
    
  }
  else
  {
  
    // low
    digitalWrite(GPIO17, LOW);
 
  }
 
 
 delayMicroseconds(20000);

 
 
 				// Calculate baud speed as follows:
   				// delayMicroseconds(3380) = 1,000,000 / 300 = 3333
				// Now vary this figure for best copy.
				// eg. 50  bauds = 1,000,000/50   = 20000us  (correct)
 				// eg. 100 bauds = 1,000,000/100  = 10000us  (correct)
				// eg. 150 bauds = 1,000,000/150  = 6666us   (6500us looks good!)
				// eg. 300 bauds = 1,000,000/300  = 3333us   (needs adjusting)
				// eg. 600 bauds = 1,000,000/600  = 1666us   (not accurate! - see below) 
				                
				// See:  http://arduino.cc/en/Reference/DelayMicroseconds				 
 				// For Raspberry Pi:
				// 50  baud = 20000us
				// 100 baud = 10000us
				// 150 baud = 6500us
				// 300 baud = 3380us 
				// 600 baud = 1540us	Needs fine tuning -this one took some time to find!!
							
}
 

uint16_t gps_CRC16_checksum (char *string)
{
 size_t i;
 uint16_t crc;
 uint8_t c;
 
crc = 0xFFFF;
 
// Calculate checksum ignoring the first two $s
 for (i = 2; i < strlen(string); i++)
 {
 c = string[i];
 crc = _crc_xmodem_update (crc, c);
 }
  
return crc;
}


 uint16_t _crc_xmodem_update (uint16_t crc, uint8_t data)
    {
        int i;

        crc = crc ^ ((uint16_t)data << 8);
        for (i=0; i<8; i++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }

        return crc;
    }

 

 
int main(void)

{
	int i;
	
	setup();
	
	for(i=0; i<5; i++)	// Vary the middle figure (5) for short/long repeatition
	
	{
		loop();
	}

return 0;
	
}
