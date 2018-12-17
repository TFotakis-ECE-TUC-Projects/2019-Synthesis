//-----------------------------------------------------------------------------
// Library for testing CC2500 tx mode with C8051F320/1 target board, 
// based on Chipcon/TI cc2500 example code.
//  
// Author: aggelos@telecom
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "compiler_defs.h"
#include "C8051F320_defs.h"            // SFR declarations
#include "common.h"
#include "cc2500.h"


//-----------------------------------------------------------------------------
// RF_SETTINGS is a data structure which contains all relevant CCxxx0 registers
//-----------------------------------------------------------------------------
typedef struct S_RF_SETTINGS{
    BYTE FSCTRL1;   // Frequency synthesizer control.
    BYTE FSCTRL0;   // Frequency synthesizer control.
    BYTE FREQ2;     // Frequency control word, high byte.
    BYTE FREQ1;     // Frequency control word, middle byte.
    BYTE FREQ0;     // Frequency control word, low byte.
    BYTE MDMCFG4;   // Modem configuration.
    BYTE MDMCFG3;   // Modem configuration.
    BYTE MDMCFG2;   // Modem configuration.
    BYTE MDMCFG1;   // Modem configuration.
    BYTE MDMCFG0;   // Modem configuration.
    BYTE CHANNR;    // Channel number.
    BYTE DEVIATN;   // Modem deviation setting (when FSK modulation is enabled).
    BYTE FREND1;    // Front end RX configuration.
    BYTE FREND0;    // Front end RX configuration.
    BYTE MCSM0;     // Main Radio Control State Machine configuration.
    BYTE FOCCFG;    // Frequency Offset Compensation Configuration.
    BYTE BSCFG;     // Bit synchronization Configuration.
    BYTE AGCCTRL2;  // AGC control.
	BYTE AGCCTRL1;  // AGC control.
    BYTE AGCCTRL0;  // AGC control.
    BYTE FSCAL3;    // Frequency synthesizer calibration.
    BYTE FSCAL2;    // Frequency synthesizer calibration.
	BYTE FSCAL1;    // Frequency synthesizer calibration.
    BYTE FSCAL0;    // Frequency synthesizer calibration.
    BYTE FSTEST;    // Frequency synthesizer calibration control
    BYTE TEST2;     // Various test settings.
    BYTE TEST1;     // Various test settings.
    BYTE TEST0;     // Various test settings.
    BYTE FIFOTHR;   // RXFIFO and TXFIFO thresholds.
    BYTE IOCFG2;    // GDO2 output pin configuration
    BYTE IOCFG0;    // GDO0 output pin configuration
    BYTE PKTCTRL1;  // Packet automation control.
    BYTE PKTCTRL0;  // Packet automation control.
    BYTE ADDR;      // Device address.
    BYTE PKTLEN;    // Packet length.
} RF_SETTINGS;


//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
#define CRC_OK              0x80  
#define RSSI                0
#define LQI                 1
#define BYTES_IN_RXFIFO     0x7F        


#define MODE_NOT_SET    0
#define TX              1
#define RX              2 


#define SYSCLK       24000000   // SYSCLK frequency in Hz


#define BM_SPI0E        0x02    // SPI I/O Enable
#define BM_XBAR         0x40    // Crossbar Enable
#define BM_PCA0ME_1     0x01    //      001: CEX0 routed to Port pin.
#define BM_WEAKPUD      0x80    // Port I/O Weak Pull-up Disable:
                                //      0: Weak Pull-ups enabled (except for Ports whose I/O are
                                //         configured as analog input or push-pull output).
                                //      1: Weak Pull-ups disabled.
#define BM_SPIEN        0x01    // SPI0 Enable: Active high
#define BM_MSTEN        0x40    // Master Mode Enable: Active High
#define BM_NSSMD1       0x08    // Slave Select Mode


// Port 0
#define SCLK_           0x01    // P0.0     SPI Serial clock                            	Output
#define SO_             0x02    // P0.1     SPI MISO signal, xx00 SO/GDO1               	Output
#define GDO1_           0x02    // P0.1
#define SI_             0x04    // P0.2     SPI MOSI signal, xx00 SI                    	Input
#define CSn_            0x08    // P0.3     SPI slave select signal                     	Output
#define GDO0_           0x40    // P0.6     xx00 GDO0                                   	Input
#define GDO2_           0x80    // P0.7     xx00 GDO2                                   	Input
SBIT(P0_1, SFR_P0, 1);
SBIT(GDO0_PIN, SFR_P0, 6);


//Port 2
#define LED1_           0x04    // P2.2     LED1, (Green)                               	Output
#define LED2_           0x08    // P2.3     LED2, (Green)                               	Output
SBIT(LED, SFR_P2, 2);           // LED='1' means ON
SBIT(LED2, SFR_P2, 3);          // LED='1' means ON
// SBIT(SW2, SFR_P2, 0);           // SW2='0' means switch pressed


//-----------------------------------------------------------------------------
// CLOCK
//-----------------------------------------------------------------------------
#define INT_OSC             0x00
#define FOUR_X_CLK_MULT     0x02
#define INT_OSC_DIV_1       0x83
#define BM_MULRDY           0x20    // Clock Multiplier Ready. This read-only bit indicates the status of the
                                    // Clock Multiplier.
                                    //      0: Clock Multiplier not ready.
                                    //      1: Clock Multiplier ready (locked).


// Select the Internal Oscillator as Multiplier input source and disable the watchdog timer
// SYSCLK = 4X Clock Multiplier / 2    


//#define CLOCK_INIT() 
//    do { 
//          UINT8 i; 
//          PCA0MD &= ~0x40;  				//bit6 set to 0 i.e. disable watchdog timer 
//          CLKMUL = INT_OSC; 				//reset clock multiplier and set clock multiplier source the internal clock
//          CLKMUL |= 0x80; 				//clock multiplier enabled - after that you need to wait for at least 5usecs
//		    for (i = 0; i < 20; i++); 		//this performs a crude 5usecs wait
//		    CLKMUL |= 0xC0; 				//bit7 (enable) and bit6 (initialize) enabled after initialiazation, bit5 (ready) must be checked
//		    while (!(CLKMUL & BM_MULRDY));  //bit5 (read-only) indicates whether clock source multiplier is ready wait until clock sourse multiplier is ready
//		    CLKSEL |= FOUR_X_CLK_MULT;      // clock source: 4 x clock_mult/2
//          OSCICN = INT_OSC_DIV_1;   		//system clock enabled and divided by 1.
//											//after all the above system clock should be 4 x 12MHz /2 = 24MHz.
//    } while (0)  
#define CLOCK_INIT() \
    do { \
        UINT8 i; \
        PCA0MD &= ~0x40; \
        CLKMUL = INT_OSC; \
        CLKMUL |= 0x80; \
        for (i = 0; i < 20; i++); \
        CLKMUL |= 0xC0; \
        while (!(CLKMUL & BM_MULRDY)); \
        CLKSEL |= FOUR_X_CLK_MULT; \
        OSCICN = INT_OSC_DIV_1; \
    } while (0) 


//-----------------------------------------------------------------------------
// SPI
//-----------------------------------------------------------------------------
// SPI enable/disable macros:
// Definitions to support burst/single access:
#define WRITE_BURST     0x40
#define READ_SINGLE     0x80
#define READ_BURST      0xC0
#define BM_SPIEN        0x01    // SPI0 Enable: Active high
#define BM_MSTEN        0x40    // Master Mode Enable: Active High
#define BM_NSSMD1       0x08    // Slave Select Mode
#define BM_SPI0E        0x02    // SPI I/O Enable
#define SPI_ENABLE()    (SPI0CN |= BM_SPIEN)
#define SPI_DISABLE()   (SPI0CN &= ~BM_SPIEN)   


// SPI Initialization
// Enable SPI (4-wire Single Master Mode, data centered on first edge of SCK period.
// NSS is an mcu-output with value from bit NSSMD0, data centered on first edge of SCK period,
// SCK low in Idle State
#define SPI_INIT(freq) \
    do { \
        SPI0CFG = BM_MSTEN; \
        SPI0CN = BM_NSSMD1; \
        SPI0CKR = freq; \
        SPI_ENABLE(); \
    } while (0)


// where freq is one of:
#define SCLK_6_MHZ      1
#define SCLK_4_MHZ      2
#define SCLK_3_MHZ      3
#define SCLK_2_4_MHZ    4
#define SCLK_2_MHZ      5
#define SCLK_1_5_MHZ    7
#define SCLK_1_2_MHZ    9
#define SCLK_1_MHZ      11


//-----------------------------------------------------------------------------
// Function PROTOTYPES (Declarations)
//-----------------------------------------------------------------------------
void spi_wait(void);
void halSpiWriteReg(BYTE addr, BYTE value);
void halRfWriteRfSettings(RF_SETTINGS *pRfSettings);
void halRfSendPacket(BYTE *txBuffer, UINT8 size);
void halSpiWriteBurstReg(BYTE addr, BYTE *buffer, BYTE count);
void halSpiStrobe(BYTE strobe);
BYTE halSpiReadStatus(BYTE addr);
BYTE halSpiReadReg(BYTE addr);
void halSpiReadBurstReg(BYTE addr, BYTE *buffer, BYTE count);
BOOL halRfReceivePacket(BYTE *rxBuffer, UINT8 *length);

void halWait(UINT16 timeout);
void intToAscii(UINT32 value);
void SYSCLK_Init (void);
void PORT_Init (void);
//void Timer2_Init (int counts);

//INTERRUPT_PROTO(Timer2_ISR, INTERRUPT_TIMER2);


//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value: none
// Parameters:   none
//
// This routine initializes the system clock to use the internal 24.5MHz / 8
// oscillator as its clock source.  Also enables missing clock detector reset.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void) {
   OSCICN = 0x80;   // Configure internal oscillator for its lowest frequency
   RSTSRC = 0x04;   // Enable missing clock detector
}


//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value: none
// Parameters:   none
//
// Configure the Crossbar and GPIO ports.
//
// P2.2 - LED (push-pull)
//
// All other port pins unused
//
//-----------------------------------------------------------------------------
void PORT_Init (void) {
    P2MDOUT = LED1_ | LED2_;                    // Enable two LEDs as a push-pull output at port 2
    P0MDOUT = SCLK_ | SO_ | SI_ | CSn_;         // Enable push-pull output at port 0
    //P2MDIN = (~0x01);                           // Set P2.0 as analog input
    P1MDIN &= ~0x10;
    P0SKIP = GDO0_ | GDO2_;                     // Crossbar skips the selected bits, used as analog input/output
    XBR0 = BM_SPI0E;                            // SPI peripheral selected
    XBR1 = BM_XBAR | BM_PCA0ME_1;               // Enable crossbar, CEX0 routed to port pin and enable weak pull-ups
//  XBR1 = BM_XBAR | BM_PCA0ME_1  | BM_WEAKPUD; // Enable crossbar, CEX0 routed to port pin and disable weak pull-ups
}


//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value: none
// Parameters:   none
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
//-----------------------------------------------------------------------------
//void Timer2_Init (int counts) {
//   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
//                                       // use SYSCLK/12 as timebase
//   CKCON &= ~0x30;                     // Timer2 clocked based on T2XCLK;
//   TMR2RL = -counts;                   // Init reload values
//   TMR2 = 0xffff;                      // Set to reload immediately
//   ET2 = 1;                            // Enable Timer2 interrupts
//   TR2 = 1;                            // Start Timer2
//}


//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// This routine changes the state of the LED whenever Timer2 overflows.
//
//-----------------------------------------------------------------------------
//INTERRUPT(Timer2_ISR, INTERRUPT_TIMER2) {
//   TF2H = 0;        // Clear Timer2 interrupt flag
//   LED = !LED;      // Change state of LED
//   LED2 = !LED2;    // Change state of LED
//}


//-----------------------------------------------------------------------------
//  void intToAscii(UINT32 value)
//
//  DESCRIPTION:
//		Takes a 32 bits interger as input and converts it to ascii. Puts the 
//      result in the global variable asciiString[]
//
//	ARGUMENTS:
//		UINT32 value
//			The value to be converted
//-----------------------------------------------------------------------------
BYTE xdata asciiString[11];


void intToAscii(UINT32 value) {
    UINT8 i;
    UINT8 j = 0;
    UINT8 digit_start = 0;
    UINT16 digit = 0;
    UINT32 denom = 1000000000;

    if (value == 0) {
        asciiString[0] = '0';
        asciiString[1] = NULL;
    } else {
        for(i = 10; i > 0; i--) {
            digit = value / denom;
            if((digit_start == 1) || (digit != 0)) {
                digit_start = 1;
                value %= denom;
                asciiString[j++] = (digit + '0');
            }
            denom /= 10;
        }
        asciiString[j++] = NULL;
    }
}


//stops for approximatelly timeout usecs
void halWait(UINT16 timeout) {
    do {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    } while (--timeout);
}


void spi_wait(void) {
	do { 
    	while (!SPIF);
     	SPIF=0;
   	} while (0);
}


void halSpiWriteReg(BYTE addr, BYTE value) {
    NSSMD0 = 0;
    while (P0_1);
    SPI0DAT = addr;
    spi_wait();
    SPI0DAT = value;
    spi_wait();
    NSSMD0 = 1;
}


// Write register settings
void halRfWriteRfSettings(RF_SETTINGS *pRfSettings) {
    halSpiWriteReg(CCxxx0_FSCTRL1,  pRfSettings->FSCTRL1);
    halSpiWriteReg(CCxxx0_FSCTRL0,  pRfSettings->FSCTRL0);
    halSpiWriteReg(CCxxx0_FREQ2,    pRfSettings->FREQ2);
    halSpiWriteReg(CCxxx0_FREQ1,    pRfSettings->FREQ1);
    halSpiWriteReg(CCxxx0_FREQ0,    pRfSettings->FREQ0);
    halSpiWriteReg(CCxxx0_MDMCFG4,  pRfSettings->MDMCFG4);
    halSpiWriteReg(CCxxx0_MDMCFG3,  pRfSettings->MDMCFG3);
    halSpiWriteReg(CCxxx0_MDMCFG2,  pRfSettings->MDMCFG2);
    halSpiWriteReg(CCxxx0_MDMCFG1,  pRfSettings->MDMCFG1);
    halSpiWriteReg(CCxxx0_MDMCFG0,  pRfSettings->MDMCFG0);
    halSpiWriteReg(CCxxx0_CHANNR,   pRfSettings->CHANNR);
    halSpiWriteReg(CCxxx0_DEVIATN,  pRfSettings->DEVIATN);
    halSpiWriteReg(CCxxx0_FREND1,   pRfSettings->FREND1);
    halSpiWriteReg(CCxxx0_FREND0,   pRfSettings->FREND0);
    halSpiWriteReg(CCxxx0_MCSM0 ,   pRfSettings->MCSM0 );
    halSpiWriteReg(CCxxx0_FOCCFG,   pRfSettings->FOCCFG);
    halSpiWriteReg(CCxxx0_BSCFG,    pRfSettings->BSCFG);
    halSpiWriteReg(CCxxx0_AGCCTRL2, pRfSettings->AGCCTRL2);
	halSpiWriteReg(CCxxx0_AGCCTRL1, pRfSettings->AGCCTRL1);
    halSpiWriteReg(CCxxx0_AGCCTRL0, pRfSettings->AGCCTRL0);
    halSpiWriteReg(CCxxx0_FSCAL3,   pRfSettings->FSCAL3);
    halSpiWriteReg(CCxxx0_FSCAL2,   pRfSettings->FSCAL2);
	halSpiWriteReg(CCxxx0_FSCAL1,   pRfSettings->FSCAL1);
    halSpiWriteReg(CCxxx0_FSCAL0,   pRfSettings->FSCAL0);
    halSpiWriteReg(CCxxx0_FSTEST,   pRfSettings->FSTEST);
    halSpiWriteReg(CCxxx0_TEST2,    pRfSettings->TEST2);
    halSpiWriteReg(CCxxx0_TEST1,    pRfSettings->TEST1);
    halSpiWriteReg(CCxxx0_TEST0,    pRfSettings->TEST0);
    halSpiWriteReg(CCxxx0_FIFOTHR,  pRfSettings->FIFOTHR);
    halSpiWriteReg(CCxxx0_IOCFG2,   pRfSettings->IOCFG2);
    halSpiWriteReg(CCxxx0_IOCFG0,   pRfSettings->IOCFG0);    
    halSpiWriteReg(CCxxx0_PKTCTRL1, pRfSettings->PKTCTRL1);
    halSpiWriteReg(CCxxx0_PKTCTRL0, pRfSettings->PKTCTRL0);
    halSpiWriteReg(CCxxx0_ADDR,     pRfSettings->ADDR);
    halSpiWriteReg(CCxxx0_PKTLEN,   pRfSettings->PKTLEN);
}


//-----------------------------------------------------------------------------
//  void halRfSendPacket(BYTE *txBuffer, UINT8 size)
//
//  DESCRIPTION:
//      This function can be used to transmit a packet with packet length up to 63 bytes.
//      To use this function, GD00 must be configured to be asserted when sync word is sent and 
//      de-asserted at the end of the packet => halSpiWriteReg(CCxxx0_IOCFG0, 0x06);
//      The function implements polling of GDO0. First it waits for GD00 to be set and then it waits
//      for it to be cleared.  
//      
//  ARGUMENTS:
//      BYTE *txBuffer
//          Pointer to a buffer containing the data that are going to be transmitted
//
//      UINT8 size
//          The size of the txBuffer
//-----------------------------------------------------------------------------

void halRfSendPacket(BYTE *txBuffer, UINT8 size) {
    halSpiWriteBurstReg(CCxxx0_TXFIFO, txBuffer, size);
    halSpiStrobe(CCxxx0_STX);
	//LED = !LED; 
    // Wait for GDO0 to be set -> sync transmitted
    while (!GDO0_PIN);
    // Wait for GDO0 to be cleared -> end of packet
    while (GDO0_PIN);
}


//-----------------------------------------------------------------------------
//  void halSpiWriteBurstReg(BYTE addr, BYTE *buffer, BYTE count)
//
//  DESCRIPTION:
//      This function writes to multiple CCxxx0 register, using SPI burst access.
//
//  ARGUMENTS:
//      BYTE addr
//          Address of the first CCxxx0 register to be accessed.
//      BYTE *buffer
//          Array of bytes to be written into a corresponding range of
//          CCxx00 registers, starting by the address specified in _addr_.
//      BYTE count
//          Number of bytes to be written to the subsequent CCxxx0 registers.   
//-----------------------------------------------------------------------------
void halSpiWriteBurstReg(BYTE addr, BYTE *buffer, BYTE count) {
    UINT8 i;
    NSSMD0 = 0;
    while (P0_1);
    SPI0DAT = addr | WRITE_BURST;
    spi_wait();
    for (i = 0; i < count; i++) {
        SPI0DAT = buffer[i];
        spi_wait();
    }
    NSSMD0 = 1;
}


//-----------------------------------------------------------------------------

//  void halSpiStrobe(BYTE strobe)
//
//  DESCRIPTION:
//      Function for writing a strobe command to the CCxxx0
//
//  ARGUMENTS:
//      BYTE strobe
//          Strobe command
//-----------------------------------------------------------------------------
void halSpiStrobe(BYTE strobe) {
    NSSMD0 = 0;
    while (P0_1);
    SPI0DAT = strobe;
    spi_wait();
    NSSMD0 = 1;
}


//-----------------------------------------------------------------------------
//  BYTE halSpiReadStatus(BYTE addr)
//
//  DESCRIPTION:
//      This function reads a CCxxx0 status register.
//
//  ARGUMENTS:
//      BYTE addr
//          Address of the CCxxx0 status register to be accessed.
//
//  RETURN VALUE:
//      BYTE
//          Value of the accessed CCxxx0 status register.
//-----------------------------------------------------------------------------
BYTE halSpiReadStatus(BYTE addr) {
    UINT8 x;
    NSSMD0 = 0;
    while (P0_1);
    SPI0DAT = (addr | READ_BURST);
    spi_wait();
    SPI0DAT = 0;
    spi_wait();
    x = SPI0DAT;
    NSSMD0 = 1;
    return x;
}


//-----------------------------------------------------------------------------
//  BYTE halSpiReadReg(BYTE addr)
//
//  DESCRIPTION:
//      This function gets the value of a single specified CCxxx0 register.
//
//  ARGUMENTS:
//      BYTE addr
//          Address of the CCxxx0 register to be accessed.
//
//  RETURN VALUE:
//      BYTE
//          Value of the accessed CCxxx0 register.
//-----------------------------------------------------------------------------
BYTE halSpiReadReg(BYTE addr) {
    UINT8 x;
    NSSMD0 = 0;
    while (P0_1);
    SPI0DAT = (addr | READ_SINGLE);
    spi_wait();
    SPI0DAT = 0;
    spi_wait();
    x = SPI0DAT;
    NSSMD0 = 1;
    return x;
}


//-----------------------------------------------------------------------------
//  void halSpiReadBurstReg(BYTE addr, BYTE *buffer, BYTE count)
//
//  DESCRIPTION:
//      This function reads multiple CCxxx0 register, using SPI burst access.
//
//  ARGUMENTS:
//      BYTE addr
//          Address of the first CCxxx0 register to be accessed.
//      BYTE *buffer
//          Pointer to a byte array which stores the values read from a
//          corresponding range of CCxxx0 registers.
//      BYTE count
//          Number of bytes to be written to the subsequent CCxxx0 registers.
//-----------------------------------------------------------------------------
void halSpiReadBurstReg(BYTE addr, BYTE *buffer, BYTE count) {
    UINT8 i;
    NSSMD0 = 0;
    while (P0_1);
    SPI0DAT = (addr | READ_BURST);
    spi_wait();  
    for (i = 0; i < count; i++) {
        SPI0DAT = 0;
        spi_wait();
        buffer[i] = SPI0DAT;
    }
    NSSMD0 = 1;
}


//-----------------------------------------------------------------------------
//  BOOL halRfReceivePacket(BYTE *rxBuffer, UINT8 *length)
//
//  DESCRIPTION: 
//      This function can be used to receive a packet of variable packet length (first byte in the packet
//      must be the length byte). The packet length should not exceed the RX FIFO size.
//      To use this function, GD00 must be configured to be asserted when sync word is sent and 
//      de-asserted at the end of the packet => halSpiWriteReg(CCxxx0_IOCFG0, 0x06);
//      Also, APPEND_STATUS in the PKTCTRL1 register must be enabled.
//      The function implements polling of GDO0. First it waits for GD00 to be set and then it waits
//      for it to be cleared.
//      After the GDO0 pin has been de-asserted, the RXBYTES register is read to make sure that there
//      are bytes in the FIFO. This is because the GDO signal will indicate sync received even if the
//      FIFO is flushed due to address filtering, CRC filtering, or packet length filtering. 
//  
//  ARGUMENTS:
//      BYTE *rxBuffer
//          Pointer to the buffer where the incoming data should be stored
//      UINT8 *length
//          Pointer to a variable containing the size of the buffer where the incoming data should be
//          stored. After this function returns, that variable holds the packet length.
//          
//  RETURN VALUE:
//      BOOL
//          TRUE:   CRC OK
//          FALSE:  CRC NOT OK (or no packet was put in the RX FIFO due to filtering)
//-----------------------------------------------------------------------------
BYTE RSSI_Measurement;

BOOL halRfReceivePacket(BYTE *rxBuffer, UINT8 *length) {
    BYTE status[2];
    UINT8 packetLength;

    halSpiStrobe(CCxxx0_SRX);

    // Wait for GDO0 to be set -> sync received
    while (!GDO0_PIN);

    // Wait for GDO0 to be cleared -> end of packet
    while (GDO0_PIN);

    // This status register is safe to read since it will not be updated after
    // the packet has been received (See the CC1100 and 2500 Errata Note)
    if ((halSpiReadStatus(CCxxx0_RXBYTES) & BYTES_IN_RXFIFO)) {

        // Read length byte
        packetLength = halSpiReadReg(CCxxx0_RXFIFO);
    
        // Read data from RX FIFO and store in rxBuffer
        if (packetLength <= *length) {
            halSpiReadBurstReg(CCxxx0_RXFIFO, rxBuffer, packetLength); 
            *length = packetLength;
        
            // Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI)
            halSpiReadBurstReg(CCxxx0_RXFIFO, status, 2); 
            RSSI_Measurement=status[0];
            
            // MSB of LQI is the CRC_OK bit
            return (status[LQI] & CRC_OK);
        } else {
            *length = packetLength;

            // Make sure that the radio is in IDLE state before flushing the FIFO
            // (Unless RXOFF_MODE has been changed, the radio should be in IDLE state at this point) 
            halSpiStrobe(CCxxx0_SIDLE);

            // Flush RX FIFO
            halSpiStrobe(CCxxx0_SFRX);
            return FALSE;
        }
    } else
        return FALSE;
}


void resetRadio(void){
    do { 
        NSSMD0 = 0; 
        while (P0_1);           //this is necessary for the specific cc2500 radio - see cc2500 manual page 21
        SPI0DAT = CCxxx0_SRES; 	//reset cc2500 chip 
        spi_wait();				//wait until data have been tranmitted through the spi interface
        NSSMD0 = 1; 			//tranmission complete - inform slave node (i.e. the cc2500)
    } while (0);
}


void waitRadioForResponce(void) {
    NSSMD0 = 1;
    halWait(1); 
    NSSMD0 = 0;
    halWait(1); 
    NSSMD0 = 1; 
    halWait(41); 
}
