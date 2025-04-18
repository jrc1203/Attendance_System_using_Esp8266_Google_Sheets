// Viral Science www.viralsciencecreativity.com www.youtube.com/c/viralscience
// Write name into RFID Tag using Serial Monitor input

#include <SPI.h>
#include <MFRC522.h>

// Define RFID pins
constexpr uint8_t RST_PIN = D3;
constexpr uint8_t SS_PIN = D4;

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

int blockNum = 2;  // Block number to write
byte bufferLen = 18;
byte readBlockData[18];

MFRC522::StatusCode status;

// Variable to store name from Serial Monitor
String inputName = "";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println("Enter the name you want to write to RFID Tag (max 16 characters):");

  // Wait for user to input name
  while (Serial.available() == 0) {
    // waiting for input
  }

  inputName = Serial.readStringUntil('\n');
  inputName.trim(); // Remove extra whitespace

  // Truncate if name exceeds 16 bytes
  if (inputName.length() > 16) {
    inputName = inputName.substring(0, 16);
    Serial.println("Name too long. Truncated to 16 characters.");
  }

  Serial.print("Name received: ");
  Serial.println(inputName);
  Serial.println("Now scan your RFID Tag to write the name...");
}

void loop() {
  // Set default key (factory setting: FF FF FF FF FF FF)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  // Wait for card
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.println("\n**Card Detected**");

  // Print UID
  Serial.print("Card UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Print card type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.print("PICC type: ");
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Convert inputName to byte array
  byte blockData[16] = {0};  // Initialize with zeros
  inputName.getBytes(blockData, 16);

  Serial.println("\nWriting to Data Block...");
  WriteDataToBlock(blockNum, blockData);

  Serial.println("\nReading from Data Block...");
  ReadDataFromBlock(blockNum, readBlockData);

  // Print data from block
  Serial.print("\nData in Block ");
  Serial.print(blockNum);
  Serial.print(" --> ");
  for (int j = 0; j < 16; j++) {
    Serial.write(readBlockData[j]);
  }
  Serial.println();
}

//------------------------------------------------------
void WriteDataToBlock(int blockNum, byte blockData[]) {
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Write: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Writing to Block failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("Data was written into Block successfully");
}

//------------------------------------------------------
void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Read: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("Block was read successfully");
}
