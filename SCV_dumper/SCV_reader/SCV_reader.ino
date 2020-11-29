// Super Cassette Vision Cart Reader
// 2020-04-14

const int serialSpeed = 9600;
const int clockPin = 12;
const int adressPin = 13;
const int oePin = 11;
const int wePin = 10;
unsigned int maxAddress = 0x2000;
const int gcDataBit[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };


// Shifts a 16-bit value out to a shift register.
// Parameters:
//   adressPin - Arduino Pin connected to the data pin of the shift register.
//   clockPin - Arduino Pin connected to the data clock pin of the shift register.
//----------------------------------------------------------------------------------
void shiftOut16(int bitOrder, int value)
{
  // Shift out highbyte for MSBFIRST
  shiftOut(adressPin, clockPin, bitOrder, (bitOrder == MSBFIRST ? (value >> 8) : value));  
  // shift out lowbyte for MSBFIRST
  shiftOut(adressPin, clockPin, bitOrder, (bitOrder == MSBFIRST ? value : (value >> 8)));
}

// Read data lines
//----------------------------------------------------------------------------------
void ReadDataLines()
{
  const char cHexLookup[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', 
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    
  int highNibble = 0;
  int lowNibble = 0;
  boolean dataBits[8];
  char byteReadHex[4];

  digitalWrite(oePin,LOW);
  delayMicroseconds(1);
  for(int currentBit = 0; currentBit < 8; currentBit++)
  {
    dataBits[currentBit] = digitalRead(gcDataBit[currentBit]);
  }
  digitalWrite(oePin,HIGH);

  highNibble = (dataBits[7] << 3) + (dataBits[6] << 2) + (dataBits[5] << 1) + dataBits[4];
  lowNibble = (dataBits[3] << 3) + (dataBits[2] << 2) + (dataBits[1] << 1) + dataBits[0];

  Serial.write(cHexLookup[highNibble]);
  Serial.write(cHexLookup[lowNibble]);
  //Serial.println();
}

// Write data lines
//----------------------------------------------------------------------------------
void WriteDataLines(byte data)
{
  
}

void setByte(byte out)
{
  for(int currentBit = 0; currentBit<8;currentBit++)
  {
    digitalWrite(gcDataBit[currentBit], bitRead(out ,currentBit));
  }
}

void writeByte(byte data, unsigned long address)
{
  digitalWrite(oePin,HIGH);
  digitalWrite(wePin,HIGH);
  shiftOut16(MSBFIRST, address);
  setByte(data);
  digitalWrite(oePin,LOW);
  delayMicroseconds(1);
  digitalWrite(wePin,LOW);  
}

// Read all of the data from the cartridge.
//----------------------------------------------------------------------------------
void ReadCartridge()
{
  unsigned int baseAddress = 0x8000;
  Serial.println("START:");
  
  // Read Current Chip
  for (unsigned int currentAddress = 0; currentAddress < maxAddress; currentAddress++) 
  {
    shiftOut16(MSBFIRST, baseAddress+currentAddress);
    //delay(1);
    ReadDataLines();
  }
  
  Serial.println(":END");
}

// Errase data from cartrige.
//----------------------------------------------------------------------------------
void EraseCartridge()
{
  setDataPinOut();
  writeByte(0xAA, 0x5555);
  writeByte(0x55, 0x2AAA);
  writeByte(0x80, 0x5555);
  writeByte(0xAA, 0x5555);
  writeByte(0x55, 0x2AAA);
  writeByte(0x10, 0x5555);
  delay(100);
  setDataPinOut();
}

// Returns the next line from the serial port as a String.
//----------------------------------------------------------------------------------
String SerialReadLine()
{
  const int BUFFER_SIZE = 81;
  char lineBuffer[BUFFER_SIZE];
  int currentPosition = 0;
  int currentValue;
  
  do
  {
    // Read until we get the next character
    do
    {
      currentValue = Serial.read();
    } while (currentValue == -1);
    
    // ignore '\r' characters
    if (currentValue != '\r')
    {
      lineBuffer[currentPosition] = currentValue;
      currentPosition++;
    } 
  
  } while ((currentValue != '\n') && (currentPosition < BUFFER_SIZE));
  lineBuffer[currentPosition-1] = 0;
  
  return String(lineBuffer);
}

// Indicate to remote computer Arduino is ready for next command.
//----------------------------------------------------------------------------------
void ReadyForCommand()
{
  Serial.println("READY:");
}

// Setup zone
//----------------------------------------------------------------------------------
void setDataPinOut()
{
  for(int pinInt = 0; pinInt < 8; pinInt++){
    pinMode(gcDataBit[pinInt], OUTPUT);
  }
}

void setDataPinIn()
{
  for(int pinInt = 0; pinInt < 8; pinInt++){
    pinMode(gcDataBit[pinInt], INPUT_PULLUP);
  }
}
void setCtrlPins()
{
  pinMode(clockPin, OUTPUT);
  pinMode(adressPin, OUTPUT);
  pinMode(oePin, OUTPUT);
  pinMode(wePin, OUTPUT);
}

void setup() {
  unsigned int initAddress = 0;

  // Setup Serial Monitor
  Serial.begin(serialSpeed);

  // Setup pins
  setCtrlPins();
  setDataPinIn();

    while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only.
  }  
  
  // Reset Output Lines
  shiftOut16(MSBFIRST, initAddress);
  
  ReadyForCommand();

}

void loop() {
  if (Serial.available() > 0)
  {
    String lineRead = SerialReadLine();
    String subString;
    lineRead.toUpperCase();
    
    if (lineRead == "READ ALL")
    {
      ReadCartridge();
    } // lineRead = "Read All"

    subString = lineRead.substring(0, 7);
    if (subString == "SETSIZE")
    {
      subString = lineRead.substring(8, 13);
      maxAddress = subString.toInt();
    }
    if (subString == "GETSIZE")
    {
      Serial.println(maxAddress);
    }
    
    ReadyForCommand();
    
  }

}
