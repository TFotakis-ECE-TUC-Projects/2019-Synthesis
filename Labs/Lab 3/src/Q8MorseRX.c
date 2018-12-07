#include "radio.h"


//-------------------------------------------------------------------------------------------------------
//  Radio settings
//-------------------------------------------------------------------------------------------------------
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
// Channel number = 6
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

RF_SETTINGS code rfSettings = {
    0x0A,   // FSCTRL1      Frequency synthesizer control.
    0x00,   // FSCTRL0      Frequency synthesizer control.
    0x5D,   // FREQ2        Frequency control word, high byte.
    0x93,   // FREQ1        Frequency control word, middle byte.
    0xB1,   // FREQ0        Frequency control word, low byte.
    0x2D,   // MDMCFG4      Modem configuration.
    0x3B,   // MDMCFG3      Modem configuration.
    0x73,   // MDMCFG2      Modem configuration.
    0x22,   // MDMCFG1      Modem configuration.
    0xF8,   // MDMCFG0      Modem configuration.
    0x06,   // CHANNR       Channel number.
    0x01,   // DEVIATN      Modem deviation setting (when FSK modulation is enabled).
    0xB6,   // FREND1       Front end RX configuration.
    0x10,   // FREND0       Front end TX configuration.
    0x18,   // MCSM0        Main Radio Control State Machine configuration.
    0x1D,   // FOCCFG       Frequency Offset Compensation Configuration.
    0x1C,   // BSCFG        Bit synchronization Configuration.
    0xC7,   // AGCCTRL2     AGC control.
    0x00,   // AGCCTRL1     AGC control.
    0xB0,   // AGCCTRL0     AGC control.
    0xEA,   // FSCAL3       Frequency synthesizer calibration.
    0x0A,   // FSCAL2       Frequency synthesizer calibration.
    0x00,   // FSCAL1       Frequency synthesizer calibration.
    0x11,   // FSCAL0       Frequency synthesizer calibration.
    0x59,   // FSTEST       Frequency synthesizer calibration.
    0x88,   // TEST2        Various test settings.
    0x31,   // TEST1        Various test settings.
    0x0B,   // TEST0        Various test settings.
    0x07,   // FIFOTHR      RXFIFO and TXFIFO thresholds.
    0x29,   // IOCFG2       GDO2 output pin configuration.
    0x06,   // IOCFG0D      GDO0 output pin configuration.
    0x04,   // PKTCTRL1     Packet automation control.
    0x05,   // PKTCTRL0     Packet automation control.
    0x00,   // ADDR         Device address.
    0xFF    // PKTLEN       Packet length.
};

BYTE code paTable = 0xFE;   // PATABLE (0 dBm output power)

//-------------------------------------------------------------------------------------------------------
//  Setup
//-------------------------------------------------------------------------------------------------------
void setup(void) {
    // PCA0MD &= ~0x40;         // Disable watchdog timer
    // SYSCLK_Init ();          // Initialize system clock to 24.5MHz
    CLOCK_INIT();               // Initialize clock
    PORT_Init ();               // Initialize crossbar and GPIO
    SPI_INIT(SCLK_6_MHZ);       // Initialize SPI

    waitRadioForResponce();     // you need to wait ~41 usecs, before CC2500 responds
    resetRadio();

    halRfWriteRfSettings(&rfSettings);
    halSpiWriteReg(CCxxx0_PATABLE, paTable);

    EA = 1;                     // Enable global interrupts
}


//-------------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------------
//BYTE  xdata txBuffer[] = {20, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
BYTE  xdata onPacket[] = {20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
BYTE  xdata offPacket[] = {20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

BYTE  xdata rxBuffer[61];    // Length byte  + 2 status bytes are not stored in this buffer
UINT8 xdata mode = MODE_NOT_SET;

UINT32 packetsReceived = 0;
UINT32 packetsSent = 0;
UINT8 length;
BYTE mytestbyte;

uint8_t modeSelected = 1;
void txMode(void){
    if(modeSelected == 1){
        halRfSendPacket(onPacket, sizeof(onPacket));
    } else {
        halRfSendPacket(offPacket, sizeof(offPacket));
    }
}


void rxMode(void){
    length = sizeof(rxBuffer);
    if (halRfReceivePacket(rxBuffer, &length)) {
        LED=rxBuffer[0] == 1 ? 1 : 0;
    }
}


//-------------------------------------------------------------------------------------------------------
//  Loop
//-------------------------------------------------------------------------------------------------------
void loop(void) {
    // LED=~SW2;
    // modeSelected = ~SW2;
    // txMode();
   rxMode();
}


//-------------------------------------------------------------------------------------------------------
// MAIN Routine
//-------------------------------------------------------------------------------------------------------
void main (void) {
    setup();
    LED =0;
    while (1) loop();
}
