/*  NTX2B  -  434.425MHz Transmitter-G4AIU   RTTY Radio Test Part 1
   
    Project:  1.  Build a transmitter using the NTX2B 70cm module.
    	      2.  Use the Raspberry Pi computer to transmit RTTY data.
	      3.  Change the baud rate by altering "delay" - line 190
	      4.  Change the shift frequency by altering the parallel resistor across R3(47k)  
	      5.  eg. adding a 47k resistor in parallel with R3 changes shift to 600Hz.
	      6.  Removing the EN/Vcc strap and adding Vcc/GPIO19 strap switches OFF idle carrier.
	      
   RTTY Set-up: Breadboard / RasPi Break-out board:
   
   	1.   Connect BLUE wire between Line 12e (TXD) and Line 20e (LH side of R3-47K ohms)
	2.   On BO board - connect GREEN wire between P17 and Line 28f (RH side of BB: R3-47K ohms) 
	      
    Created 2012 by M0UPU as part of a UKHAS Guide on linking
    NTX2B Modules to Raspberry Pi.
    RTTY code from Rob Harrison - Icarus Project.
    http://ukhas.org.uk
    Code amended by Ciaran Morgan / M0XTD - 04 Feb 14
    
+----------+-Rev2-+------+--------+------+-------+
| wiringPi | GPIO | Phys | Name   | Mode | Value |
+----------+------+------+--------+------+-------+
|      0   |  17  |  11  | GPIO 0 | OUT  | Low   |
|      1   |  18  |  12  | GPIO 1 | OUT  | Low   |
|      2   |  27  |  13  | GPIO 2 | IN   | Low   |
|      3   |  22  |  15  | GPIO 3 | IN   | Low   |
|      4   |  23  |  16  | GPIO 4 | IN   | Low   |
|      5   |  24  |  18  | GPIO 5 | IN   | Low   |
|      6   |  25  |  22  | GPIO 6 | IN   | Low   |
|      7   |   4  |   7  | GPIO 7 | IN   | Low   |
|      8   |   2  |   3  | SDA    | IN   | High  |
|      9   |   3  |   5  | SCL    | IN   | High  |
|     10   |   8  |  24  | CE0    | IN   | Low   |
|     11   |   7  |  26  | CE1    | IN   | Low   |
|     12   |  10  |  19  | MOSI   | IN   | Low   |
|     13   |   9  |  21  | MISO   | IN   | Low   |
|     14   |  11  |  23  | SCLK   | IN   | Low   |
|     15   |  14  |   8  | TxD    | IN   | Low   |
|     16   |  15  |  10  | RxD    | IN   | High  |
|     17   |  28  |   3  | GPIO 8 | IN   | Low   |
|     18   |  29  |   4  | GPIO 9 | IN   | Low   |
|     19   |  30  |   5  | GPIO10 | IN   | Low   |
|     20   |  31  |   6  | GPIO11 | IN   | Low   |
+----------+------+------+--------+------+-------+
    
    
    
*/ 
 
#define GPIO17 0
#define GPIO25 6
 
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include "xmit.h"
#include "crc.h"
#include "mode_rtty.h"



void rtty_txstring (char * string)

{
 
  	/* Simple function to send one char at a time to 
   	** rtty_txbyte function.
	** We will always calculate the CRC checksum before sending the string. 
   	** NB Each char is one byte (8 Bits)
   	*/
 
   
  char c;
  char checksum[10];
  snprintf(checksum, sizeof(checksum), "*%04X\n", gps_CRC16_checksum(string));
  // Also copy checksum's terminating \0 (hence the +1).
  memcpy(string + strlen(string), checksum, strlen(checksum) + 1);

  printf("\tSending : ");

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
 
  printf("%c", c);
  
  rtty_txbit (0); // Start bit
 
  // Send bits for char LSB first      	
 
  for (i=0;i<=7;i++) // Change here for 7 or 8 bit  ASCII-7 / ASCII-8
  
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
 
