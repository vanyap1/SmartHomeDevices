
#ifndef RFM69_h
#define RFM69_h




#define RF69_MAX_DATA_LEN       61  // to take advantage of the built in AES/CRC we want to limit the frame size to the internal FIFO size (66 bytes - 3 bytes overhead - 2 bytes crc)
#define CSMA_LIMIT             -90 // upper RX signal sensitivity threshold in dBm for carrier sense access
#define RF69_MODE_SLEEP         0   // XTAL OFF
#define RF69_MODE_STANDBY       1   // XTAL ON
#define RF69_MODE_SYNTH         2   // PLL ON
#define RF69_MODE_RX            3   // RX MODE
#define RF69_MODE_TX            4   // TX MODE
#define RF69_ListenAbort		0X10
#define null                  0
#define COURSE_TEMP_COEF    -90     // puts the temperature reading in the ballpark, user can fine tune the returned value
#define RF69_BROADCAST_ADDR   132
#define RF69_CSMA_LIMIT_MS 1000
#define RF69_TX_LIMIT_MS   100
#define RF69_FSTEP    61.03515625   // == FXOSC / 2^19 = 32MHz / 2^19 (p13 in datasheet) / FXOSC = module crystal oscillator frequency
// TWS: define CTLbyte bits
#define RFM69_CTL_SENDACK   0x80
#define RFM69_CTL_REQACK    0x40

// Global Variables
volatile uint8_t DATA[RF69_MAX_DATA_LEN+1];  // RX/TX payload buffer, including end of string NULL char
//volatile uint8_t rx_header[5];
volatile uint8_t rx_ready;

typedef struct {
	uint8_t rxtxBuffLenght;
	uint8_t destinationAddr;
	uint8_t senderAddr;
	uint8_t opcode;
	int16_t rssi;
	uint8_t dataValid;
	uint8_t dataCRC;
}rfHeader;



// Function Declerations
uint8_t rfm69_init(uint16_t freqBand, uint8_t nodeID, uint8_t networkID);
void calculateRFRegisters(uint32_t frequencyMHz, uint8_t *msb, uint8_t *mid, uint8_t *lsb);
void setAddress(uint8_t addr);
void setNetwork(uint8_t networkID);

void receiveBegin();
uint32_t getFrequency();
void setFrequency(uint32_t freqHz);
void encrypt(const char* key);
int16_t readRSSI(uint8_t forceTrigger);
void setHighPower(uint8_t onOFF);           // has to be called after initialize() for RFM69HW
void setPowerLevel(uint8_t level);            // reduce/increase transmit power level
void RF_sleep();
uint8_t readTemperature(uint8_t calFactor); // get CMOS temperature (8bit)
void rcCalibration();                         // calibrate the internal RC oscillator for use in wide temperature variations - see datasheet section [4.3.5. RC Timer Accuracy]
uint8_t readReg(uint8_t addr);
void writeReg(uint8_t addr, uint8_t val);
//void sendFrame(uint8_t address, uint8_t toAddress, uint8_t opcode, uint8_t extraArg, const void* buffer, uint8_t bufferSize);
void sendFrame(rfHeader * txHeader, const void* buffer);
void setMode(uint8_t mode);
void setHighPowerRegs(uint8_t onOff);
void promiscuous(uint8_t onOff);
void RF_select();
void RF_unselect();
rfHeader* data_ready();
uint8_t simpleCRC(uint8_t *array, uint8_t length);

#endif

