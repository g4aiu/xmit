/*  NTX2B-FA  -  70cm Transmitter-G4AIU   RTTY Radio Test Part 1
   
    Project:  1.  Build a transmitter using the NTX2B-FA 70cm module.
    	      2.  Use the Raspberry Pi computer to transmit RTTY data.
	      3.  Change the baud rate by altering "delay" - line 190
	      4.  Change the shift frequency by altering the parallel resistor across R3(47k)  
	      5.  eg. adding a 47k resistor in parallel with R3 changes shift to 600Hz.
	      6.  Removing the EN/Vcc strap and adding Vcc/GPIO19 strap switches OFF idle carrier.
	      
   RTTY Set-up: Breadboard / RasPi Break-out board: This set-up is for RTTY only!!
   
   	1.   Connect BLACK wire to GND.
	2.   Connect RED wire to 3v3.
	3.   Connect GREEN wire to pin #17. 
	      
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
#define TXENABLE 1
 
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include "xmit.h"
#include "crc.h"
#include "mode_rtty.h"

#define _GNU_SOURCE
#include <getopt.h>

 
char datastring[150];
 

int main(int argc, char *argv[] )

{

	int i1, i2;
	int sendRYs = 0;
	int stopbits = 2;
	enum digimode mode = rtty;
	enum asciibits ascii = bits8;
	
	
	int opt;
	struct option longopts[] = {
	{"mode", 	required_argument, 	NULL, 'm'},
	{"baud", 	required_argument, 	NULL, 'b'},
	{"text", 	required_argument, 	NULL, 't'},
	{"stopbits", 	required_argument, 	NULL, 's'},
	{"ascii", 	required_argument, 	NULL, 'a'},
	{"ry",		no_argument,		&sendRYs, 1},
	{"picture",	required_argument, 	NULL, 'p'},
	{"help", 	no_argument, 		NULL, 'h'},
	{0,0,0,0}};
	
	
	while((opt = getopt_long(argc, argv, "m:b:t:s:a:rp:h", longopts, NULL)) !=-1)
	{
		switch(opt)
		{
			case 'h':
				printf("xmit : a program to transmit data via a Raspberry Pi and NTX2B\n");
				printf("=============================================================\n\n");
				printf("Options :-\n\n");
				printf("\t--ascii <int Number of ASCII bits>\n");
				printf("\t\tSet the number of ASCII bits to transmit.\n");
				printf("\t\tValid options are 7 or 8\n\n");
				
				printf("\t--baud <int Baudrate>\n");
				printf("\t\tSet the baud rate of transmission.\n");
				printf("\t\tValid options are 50, 100, 150, 300, 600\n\n");
				
				printf("\t--mode <string ModeType>\n");
				printf("\t\tSet the mode of transmission.\n");
				printf("\t\tValid options are rtty\n\n");
				
				printf("\t--picture <string filename>\n");
				printf("\t\tSet the picture to be transmitted.\n\n");
				
				printf("\t--ry\n");
				printf("\t\tEnable the transmission of RYs before an after an RTTY transmission.\n\n");

				printf("\t--stopbits <int NumberOfStopBits>\n");
				printf("\t\tSets the number of stop bits to be used.\n");
				printf("\t\tValid options are 0, 1 or 2\n");
				printf("\t\tDefault Value = 2\n\n");
				
				printf("\t--text <string Text-to-send>\n");
				printf("\t\tSet the text message to be transmitted by the requested mode.\n\n");

				exit(0);
				break;
			case 'a':
				// identify the number of ASCII bits in the transmission
				i1 = strcmp("7", optarg);
				i2 = strcmp("8", optarg);
				
				if ((i1 == 0) || (i2 == 0))
				{
					//user has entered 7 or 8 bits
					if (i1==0)
						ascii = bits7;
					else
						ascii = bits8;
					 
				}
				else 
				{
					printf("UNIDENTIFIED ENCODING.  Exiting\n");
					exit(0);
				
				}

				break;
			case 'b':
				printf("Baud Rate Selected : %s\n", optarg);
				break;
			case 'm':
				// identify the mode that has ben specified remembering that the default is rtty
				i1 = strcmp("rtty", optarg);
				i2 = strcmp("dominoex16", optarg);
				
				if ((i1 == 0) || (i2 == 0))
				{
					//user has entered a valid mode
					if (i1==0)
						mode = rtty;
					else
						mode = dominoex16;
					 
				}
				else 
				{
					printf("UNIDENTIFIED ENCODING.  Exiting\n");
					exit(0);
				
				}				
				break;
			case 'p':
				printf("Picture Selected : %s\n", optarg);
				break;
			case 'r':
				printf("Transmit RYs before & after : : %s\n", optarg);
				break;
			case 's':
				printf("Number of Stop Bits Selected : %s\n", optarg);
				break;
			case 't':
				printf("Text to transmit : %s\n", optarg);
				break;
			case ':':
				printf("Option needs a value\n");
				break;
			case '?':
				printf("Unknown Option : %c\n", optopt);
				break;
		}
	}
	
	
	// Initialise the Raspberry Pi and set GPIO17 to be an output only
	wiringPiSetup () ;                
  	pinMode(GPIO17,OUTPUT);

  	// Enable the TXENABLE GPIO port to be the one that enables the transmitter
	pinMode(TXENABLE,OUTPUT);
	// Now enable the transmitter and wait for 1s
	digitalWrite(TXENABLE, HIGH);
	delayMicroseconds(100000);
 	

	printf("xmit : a program to transmit data via a Raspberry Pi and NTX2B\n");
	printf("=============================================================\n\n");
	printf("Transmitting using :-\n");
	if (mode == rtty) { printf("\tMODE : RTTY\n"); }
	if (mode == dominoex16) { printf("\tMODE : DOMINOEX16\n"); }
	if (ascii == bits7) { printf("\tENC  : 7Bit ASCII\n"); } else{printf("\tENC  : 8Bit ASCII\n"); } 	 
	printf("\n\n");
	


	                                                                 
//	//Lets print out the number of arguments that have been passed to the program.
//	printf("There are %d arguments in your command line\n", argc);
	int i;
	for (i=1; i<argc; i++)
	{
		// Now send some RYs, so create the RY string to send
		sprintf(datastring,"$$RYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYR");
	
		// And send it.
 	  	rtty_txstring (datastring);
		snprintf(datastring, sizeof(datastring), "$$%s",  argv[i]);
		rtty_txstring (datastring);
	
	}
	
//	// All finished sending so disable the transmitter after sending some more RYs
	sprintf(datastring,"$$RYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRYRY de G4AIU G4AIU K");

	// And send it.
  	rtty_txstring (datastring);

        digitalWrite(TXENABLE, LOW);
	
	

return (0) ;
	
}
