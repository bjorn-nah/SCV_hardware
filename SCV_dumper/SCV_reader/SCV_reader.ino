// Super Cassette Vision Cart Reader
// 2020-04-14

const int serialSpeed = 9600;
const int clockPin = 12;
const int dataPin = 13;
unsigned int maxAddress = 0x2000;
const int gcDataBit[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };


// Shifts a 16-bit value out to a shift register.
// Parameters:
//   dataPin - Arduino Pin connected to the data pin of the shift register.
//   clockPin - Arduino Pin connected to the data clock pin of the shift register.
//----------------------------------------------------------------------------------
void shiftOut16(int bitOrder, int value)
{
  // Shift out highbyte for MSBFIRST
  shiftOut(dataPin, clockPin, bitOrder, (bitOrder == MSBFIRST ? (value >> 8) : value));  
  // shift out lowbyte for MSBFIRST
  shiftOut(dataPin, clockPin, bitOrder, (bitOrder == MSBFIRST ? value : (value >> 8)));
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

  for(int currentBit = 0; currentBit < 8; currentBit++)
  {
    dataBits[currentBit] = digitalRead(gcDataBit[currentBit]);
  }

  highNibble = (dataBits[7] << 3) + (dataBits[6] << 2) + (dataBits[5] << 1) + dataBits[4];
  lowNibble = (dataBits[3] << 3) + (dataBits[2] << 2) + (dataBits[1] << 1) + dataBits[0];

  Serial.write(cHexLookup[highNibble]);
  Serial.write(cHexLookup[lowNibble]);
  //Serial.println();
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
    ReadDataLines();  
  }
  
  Serial.println(":END");
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


void setup() {
  unsigned int initAddress = 0;

  // Setup Serial Monitor
  Serial.begin(serialSpeed);

  // Setup pins
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  for(int pinInt = 0; pinInt < 8; pinInt++){
    pinMode(gcDataBit[pinInt], INPUT_PULLUP);
  }

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