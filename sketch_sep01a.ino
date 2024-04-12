#define SERIAL_IN 2
#define SERIAL_CLK 3
#define REGISTER_CLK 4
#define WRITE_EN 13
#define IO_1 5
#define IO_8 12

void setAdr(short adr, boolean outputEnable)
{
  PORTD &= 0xe7;
  
  if(!outputEnable)
    adr |= 0x8000;
  for(uint8_t i = 0; i<16 ; i++)
  {
    if(adr & 1)
      PORTD |= 0x04;
    else
      PORTD &= ~0x04;

    PORTD |= 0x08;
    PORTD &= ~0x08;
    adr >>= 1;   
  }
  
  PORTD |= 0x10;
  PORTD &= ~0x10;
  
}

//mode must be = "INPUT" or "OUTPUT" 
void dataBusMode(int mode)
{
    if(mode == INPUT)
      setAdr(0x00, true);
    else
      setAdr(0x00, false);
    for(uint8_t pin = IO_1; pin<=IO_8; pin++)
      pinMode(pin, mode);
}


//Requires the "Data Bus" to be in INPUT mode
byte readData(short adr)
{
  setAdr(adr, true);
  byte data = 0;
  for(uint8_t pin = IO_8; pin>=IO_1; pin--)
    data = (data<<1) | digitalRead(pin);      
  
  return data;
}

void setData(byte data)
{
  for(int pin = IO_1; pin<=IO_8 ; pin++)
  {
    if((data | 0xfe) == 0xff)
      digitalWrite(pin, HIGH);
    else
      digitalWrite(pin, LOW);

      data >>= 1;
  }
}


//Requires the "Data Bus" to be in OUTPUT mode
void writeData(byte data, short adr)
{
  setAdr(adr, false);
  setData(data);
  digitalWrite(WRITE_EN, LOW);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}


void softwareDataProtectionDisable()
{
  setAdr(0x5555,false);
  setData(0xaa);
  digitalWrite(WRITE_EN, LOW);
  digitalWrite(WRITE_EN, HIGH);

  setAdr(0x2aaa, false);
  setData(0x55);
  digitalWrite(WRITE_EN, LOW);
  digitalWrite(WRITE_EN, HIGH);

  setAdr(0x5555, false);
  setData(0x80);
  digitalWrite(WRITE_EN, LOW);
  digitalWrite(WRITE_EN, HIGH);

  setAdr(0x5555, false);
  setData(0xaa);
  digitalWrite(WRITE_EN, LOW);
  digitalWrite(WRITE_EN, HIGH);

  setAdr(0x2aaa, false);
  setData(0x55);
  digitalWrite(WRITE_EN, LOW);
  digitalWrite(WRITE_EN, HIGH);

  setAdr(0x5555, false);
  setData(0x20);
  digitalWrite(WRITE_EN, LOW);
  digitalWrite(WRITE_EN, HIGH);
}


void printContent()
{
  char buf[80];
  byte data[16];
  for(int base = 0 ; base<= 319/*2047*/ ; base+=16)
  {
    for(int offset = 0; offset <=15 ; offset++)
      data[offset] = readData(base + offset);
    sprintf(buf, "%04x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
    base, data[0], data[1],data[2], data[3],data[4], data[5],data[6], data[7],data[8], data[9],
    data[10], data[11],data[12], data[13],data[14], data[15]); 
    Serial.println(buf);
  }
}
void setup() {
  Serial.begin(57600);
  //maybe add some initialization here for the clocks (and then serial in ?? i doubt it)
  pinMode(SERIAL_IN, OUTPUT);
  digitalWrite(SERIAL_CLK, LOW);
  pinMode(SERIAL_CLK, OUTPUT);
  digitalWrite(REGISTER_CLK, LOW);
  pinMode(REGISTER_CLK, OUTPUT);
  dataBusMode(INPUT);
  digitalWrite(WRITE_EN,HIGH);
  pinMode(WRITE_EN, OUTPUT);
  
  /*
  dataBusMode(OUTPUT); 
  for(short i = 0; i<=159; i++)
    writeData(0xBB, i); 
    
  dataBusMode(INPUT);
  */
  printContent();

 

  /* -----------------7SEG coder----------------------------
  dataBusMode(OUTPUT);
  byte hex[] = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b};
  
  byte value = 0;//to use for the adress
  short adress = 0;
  short counter = 0;
  Serial.println("programming ...");



  for(counter = 0; counter <=255 ; counter ++)
  {
    //for the positive values
    adress = value;
    adress <<= 3;
    writeData(hex[value%10], adress);
    adress++; 
    writeData(hex[(value/10)%10], adress);
    adress++;
    writeData(hex[(value/100)%10], adress);
    adress++;
    writeData(0x00, adress);
    
    Serial.println(value);
    value++;
  }

  char svalue = -128;
  for(counter = 0; counter <=255; counter ++)
  {
    adress = (byte)svalue;
    adress <<= 3;
    adress += 4; //or maybe an or operation could do the trick 
    writeData(hex[abs(svalue)%10], adress);
    adress++; 
    writeData(hex[(abs(svalue)/10)%10], adress);
    adress++;
    writeData(hex[(abs(svalue)/100)%10], adress);
    adress++;
    if(svalue<0)
      writeData(0x01, adress);
    else
      writeData(0x00, adress);
    Serial.println(svalue, DEC);
    svalue++;
  }

  Serial.println("done");
  //Printing Results
  dataBusMode(INPUT);
  printContent();
  
  */
 
 }

void loop() {
  
}
