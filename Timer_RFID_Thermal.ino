
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Time.h>
#include <SPI.h>
#include <MFRC522.h>

#include "impressora.h"

#define RST_PIN		9		// 
#define SS_PIN		10		//

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance

// pin 3 - Serial clock out (SCLK)
// pin 4 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 6 - LCD chip select (CS/CE)
// pin 7 - LCD reset (RST)

Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 6, 7);

unsigned char readCard[4];

// TAGS
const char TAG_MAKERBOT[4] =  {0x16,0x54,0x75,0x65};
const char TAG_ULTIMAKER[4] = {0x93,0xC2,0xDE,0xC7};

Impressora Makerbot = Impressora(&display, TAG_MAKERBOT, "MAKERBOT        ");
Impressora Ultimaker = Impressora(&display, TAG_ULTIMAKER, "ULTIMAKER ");

void setup() {
  Serial.begin(9600);		// Initialize serial communications with the PC
  while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();		// Init MFRC522
  
  display.begin();
  display.setContrast(35); //Ajusta o contraste do display
  display.clearDisplay();   //Apaga o buffer e o display
  display.setTextSize(1);  //Seta o tamanho do texto
  display.setTextColor(BLACK); //Seta a cor do texto
  display.setCursor(0,0);  //Seta a posição do cursor
  display.println(F("   Sistema"));  
  display.println(F(" Inicializado"));
  display.println(F("     BSB"));
  display.println(F("     FAB"));
  display.println(F("     LAB"));
  display.display();
  delay(500);
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



void loop() {
  while (!getID()); //the program will not go further while you not get a successful read
  Serial.println(F("Tag Lida"));
  
  Makerbot.Check_tag(readCard);
  Ultimaker.Check_tag(readCard);
  
}
