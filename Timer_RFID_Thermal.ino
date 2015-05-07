#include <Time.h>

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN		9		// 
#define SS_PIN		10		//

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance

unsigned char readCard[4];

// TAGS
unsigned char TAG_MAKERBOT[4] =  {0x16,0x54,0x75,0x65};
unsigned char TAG_ULTIMAKER[4] = {0x93,0xC2,0xDE,0xC7};

//Last times

unsigned int Last_Makerbot = 0;
unsigned int Last_Ultimaker = 0;

//ON-OFF

unsigned char On_Makerbot = 0;
unsigned char On_Ultimaker = 0;


//AUX

unsigned int delta;

void setup() {
  Serial.begin(9600);		// Initialize serial communications with the PC
  while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();		// Init MFRC522
  Serial.println(F("Sistema Inicializado"));
}

int getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  for (int i = 0; i < 4; i++) {  // 
    readCard[i] = mfrc522.uid.uidByte[i];
  }
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}


unsigned int compareUUID(unsigned char* readTag,unsigned char* tagToCompare)
{
  unsigned int sumUUID = 0;
  
  for (int i = 0; i < 4; i++)
  {
   sumUUID+= readTag[i] - tagToCompare[i]; 
  }  
  return sumUUID;
}


void loop() {
  while (!getID()); //the program will not go further while you not get a successful read
  
  if (compareUUID(readCard,TAG_MAKERBOT) == 0)
  {
    Serial.println(F("MAKERBOT"));
    
    if (!On_Makerbot)
    {
      On_Makerbot = 1;
      Serial.println(F("LIGANDO"));
      Last_Makerbot = now();
    }
    else
    {
      On_Makerbot = 0;
      delta = now() - Last_Makerbot;
      Serial.println(F("DESLIGANDO"));
      Serial.print(F("TEMPO LIGADO: "));
      Serial.print((day(delta)-1)*24 + hour(delta));
      Serial.print("h");
      Serial.print(minute(delta));
      Serial.print("m");
      Serial.print(second(delta));
      Serial.print("s");
    }
    
  }
  if (compareUUID(readCard,TAG_ULTIMAKER) == 0)
  {
    Serial.println(F("ULTIMAKER"));
  }
}
