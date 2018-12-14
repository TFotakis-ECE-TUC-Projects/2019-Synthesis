//-----------------------------------------------------------------------------
// AB_tx.c
//-----------------------------------------------------------------------------
//
// Program Description:
//
// This program tests CC2500 tx mode with C8051F320/1 target board,
// based on Chipcon/TI cc2500 example code.
// Aiming for a single-file code for teaching purposes.
//
// Author: aggelos@telecom -

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

/*#include "compiler_defs.h"
 #include "C8051F320_defs.h"            // SFR declarations
 #include "common.h"
 #include "cc2500.h"*/
#include "radio.h"

// #define SYSCLK       12000000          // SYSCLK frequency in Hz
#define BAUDRATE     115200            // Baud rate of UART in bps
#define ID 0x00					  // ID of slave
int counter=0;
UINT16 mV;
BYTE mVlow;
BYTE mVhigh;
//sfr16 TMR2RL   = 0xca;                 // Timer2 reload value
//sfr16 TMR2     = 0xcc;                 // Timer2 counter
//sfr16 ADC0     = 0xbd;                 // ADC0 result

//-----------------------------------------------------------------------------
// Function PROTOTYPES (Declarations)
//-----------------------------------------------------------------------------
void ADC0_Init(void);
void Timer2_Inititialize(void);

//-------------------------------------------------------------------------------------------------------
//  Global Variables

// BYTE  xdata txBuffer[] = {20, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
BYTE xdata txBuffer[] = { 3, 0, 1, 2 };


int flag_req;
//BYTE  xdata rxBuffer[61];    // Length byte  + 2 status bytes are not stored in this buffer
//UINT8 xdata mode = MODE_NOT_SET;
//BYTE  xdata asciiString[11];

//radio settings
// Chipcon
// Product = CC2500
// Chip version = E   (VERSION = 0x03)
// Crystal accuracy = 10 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 541.666667 kHz
// Phase = 1
// Datarate = 249.938965 kBaud
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 2432.999908 MHz
// Channel spacing = 199.951172 kHz
// Channel number = 0
// Optimization = Sensitivity
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 255
// Preamble count = (2)  4 bytes
// Append status = 1
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (41) CHIP_RDY
RF_SETTINGS code rfSettings = { 0x08, // FSCTRL1		Frequency synthesizer control.
		0x00,	// FSCTRL0		Frequency synthesizer control.
		0x5D,	// FREQ2		Frequency control word, high byte.
		0x93,	// FREQ1		Frequency control word, middle byte.
		0xB1,	// FREQ0		Frequency control word, low byte.
		0x87,	// MDMCFG4		Modem configuration.
		0x43,	// MDMCFG3		Modem configuration.
		0x03,	// MDMCFG2		Modem configuration.
		0x22,	// MDMCFG1		Modem configuration.
		0xF8,	// MDMCFG0		Modem configuration.
		0x06,	// CHANNR		Channel number.
		0x44,// DEVIATN		Modem deviation setting (when FSK modulation is enabled).
		0xB6,	// FREND1		Front end RX configuration.
		0x10,	// FREND0		Front end TX configuration.
		0x18,	// MCSM0		Main Radio Control State Machine configuration.
		0x16,	// FOCCFG		Frequency Offset Compensation Configuration.
		0x1C,	// BSCFG		Bit synchronization Configuration.
		0xC7,	// AGCCTRL2		AGC control.
		0x00,	// AGCCTRL1		AGC control.
		0xB0,	// AGCCTRL0		AGC control.
		0xEA,	// FSCAL3		Frequency synthesizer calibration.
		0x0A,	// FSCAL2		Frequency synthesizer calibration.
		0x00,	// FSCAL1		Frequency synthesizer calibration.
		0x11,	// FSCAL0		Frequency synthesizer calibration.
		0x59,	// FSTEST		Frequency synthesizer calibration.
		0x88,	// TEST2		Various test settings.
		0x31,	// TEST1		Various test settings.
		0x0B,	// TEST0		Various test settings.
		0x07,	// FIFOTHR		RXFIFO and TXFIFO thresholds.
		0x29,	// IOCFG2		GDO2 output pin configuration.
		0x06,	// IOCFG0D		GDO0 output pin configuration.
		0x04,	// PKTCTRL1		Packet automation control.
		0x05,	// PKTCTRL0		Packet automation control.
		0x00,	// ADDR			Device address.
		0xFF 	// PKTLEN		Packet length.
		};

// PATABLE (0 dBm output power)
BYTE code paTable = 0xFE;

//-------------------------------------------------------------------------------------------------------

void setup(void) {
	// PCA0MD &= ~0x40;         // Disable watchdog timer
	// SYSCLK_Init();          // Initialize system clock to 24.5MHz

	CLOCK_INIT()
	;               // Initialize clock
	PORT_Init();               // Initialize crossbar and GPIO
	SPI_INIT(SCLK_6_MHZ);       // Initialize SPI

	Timer2_Inititialize();
	ADC0_Init();
	//txBuffer[3]=ID;
	// mVlow= mV & 0x00ff;
	// mVhigh= (mV>>8) & 0x00ff;
	// txBuffer[1]=mVlow;
	// txBuffer[2]=mVhigh;
	//   txBuffer[1]=0x11;
	// txBuffer[2]=0x00;
	//txBuffer[3] = ID;

	waitRadioForResponce(); // you need to wait ~41 usecs, before CC2500 responds
	resetRadio();

	halRfWriteRfSettings(&rfSettings);
	halSpiWriteReg(CCxxx0_PATABLE, paTable);

	//EA=0;
	LED = 0;
	EA = 1;                     // Enable global interrupts

}

//-------------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------------
//BYTE  xdata txBuffer[] = {3, 0, 1, 2};
BYTE xdata rxBuffer[61]; // Length byte  + 2 status bytes are not stored in this buffer
UINT8 xdata mode = MODE_NOT_SET;
//int flag_req;
UINT32 packetsReceived = 0;
UINT32 packetsSent = 0;
UINT8 length;

void txMode(void) {
	//TX mode
	halRfSendPacket(txBuffer, sizeof(txBuffer));
	//intToAscii(++packetsSent);
	//endof TX mode

	halWait(30000);
	halWait(30000);
	LED = ~LED;
	halWait(30000);
	halWait(30000);
	// EA=1;
}

void rxMode(void) {
	//RX mode

	length = sizeof(rxBuffer);
	if (halRfReceivePacket(rxBuffer, &length)) {
		intToAscii(++packetsReceived);
		LED = ~LED;

	}
	//LED = ~LED;
	if (rxBuffer[0] == ID) {

		flag_req = 1;
	}
	else{
		//LED = ~LED;
		flag_req=0;
	}


			halWait(30000);
			halWait(30000);
			halWait(30000);
			halWait(30000);
	        halWait(30000);
			halWait(30000);
			halWait(30000);
			halWait(30000);
}

//-------------------------------------------------------------------------------------------------------
//  Loop
//-------------------------------------------------------------------------------------------------------
int conversion_ended = 0;
void loop(void) {
//  txMode();
	rxMode();
	if(flag_req==0) {
				TR2=0;
			}
			else {
				conversion_ended = 0;
				TR2=1;
				while (!conversion_ended){}
			}
}

//-------------------------------------------------------------------------------------------------------
// MAIN Routine
//-------------------------------------------------------------------------------------------------------
void main(void) {

	setup();
//EA = 1;                     // Enable global interrupts

	while (1) {
		txBuffer[3]=ID;
		loop();
	}
}

void Timer2_Inititialize(void) {

	TMR2CN = 0x00;                     // Stop Timer2; Clear TF2;
									   // use SYSCLK as timebase, 16-bit
									   // auto-reload
	CKCON |= 0x10;                     // select SYSCLK for timer 2 source
	TMR2RL = 65535 - (SYSCLK / 10000); // init reload value for 10uS
	TMR2 = 0xffff;                   // set to reload immediately
	//TR2 = 1;                        // start Timer2
}

void ADC0_ISR(void) interrupt 10 {

	static unsigned long accumulator = 0;     // accumulator for averaging
	static unsigned int measurements = 2048;// measurement counter
	unsigned long result=0;
//  unsigned long mV;                         // measured voltage in mV

// clear ADC0 conv. complete flag
	counter++;
	//if (counter==4)
	//{LED =1;
		//		}
	accumulator += ADC0;
	measurements--;

	if(measurements == 0)
	{
		TR2=0;
		measurements = 2048;
		result = accumulator / 2048;
		accumulator=0;

		// The 10-bit ADC value is averaged across 2048 measurements.
		// The measured voltage applied to P1.4 is then:
		//
		//                           Vref (mV)
		//   measurement (mV) =   --------------- * result (bits)
		//                       (2^10)-1 (bits)

		mV = result * 4000 / 1023;
		//mV = ADC0;
		//printf("P2.4 voltage: %d mV\n",mV);
		mVlow= mV & 0x00ff;
		mVhigh= (mV>>8) & 0x00ff;
		txBuffer[1]=mVlow;
		txBuffer[2]=mVhigh;

		txMode();
		//LED = 0;
		AD0INT = 0;
		conversion_ended = 1;

	} else {
		AD0INT = 0;
	}

// LED = ~LED;

//  mVlow= mV & 0x00ff;
//      mVhigh= (mV>>8) & 0x00ff;
//      txBuffer[1]=mVlow;
//      txBuffer[2]=mVhigh;

}

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configures ADC0 to make single-ended analog measurements on pin P2.4
//
//-----------------------------------------------------------------------------

void ADC0_Init(void) {
	ADC0CN = 0x02;                      // ADC0 disabled, normal tracking,
										// conversion triggered on TMR2 overflow

	REF0CN = 0x0A;                      // Enable on-chip VREF and buffer

//AMX0P = 0x0C;                       // ADC0 positive input = P2.4
//AMX0P = 0x00;
	AMX0P = 0x04;
	AMX0N = 0x1F;                       // ADC0 negative input = GND
										// i.e., single ended mode

	ADC0CF = ((SYSCLK / 3000000) - 1) << 3;   // set SAR clock to 3MHz

	ADC0CF |= 0x00;                     // right-justify results

	EIE1 |= 0x08;                       // enable ADC0 conversion complete int.

	AD0EN = 1;                          // enable ADC0
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
