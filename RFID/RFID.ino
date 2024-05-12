#include <SPI.h>
#include <MFRC522.h>

#define uint8_t RST_PIN = 9     // Configurable, see typical pin layout above
#define SS_PIN = 10     // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
char c;

void setup() {
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
}

void loop()
{
  Serial.println("1. Escribir");
  Serial.println("2. Leer");
  while(!Serial.available()){}
  c=Serial.read();
  switch(c)
  {
    case '1':
    Escribir();
    break;
    case '2':
    Leer();
    break;
  }
}
void Escribir()
{
  
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  byte buffer[34];
  byte block;
  MFRC522::StatusCode status;
  byte tamano_codigo;
  Serial.println("Escribe nuevo codigo y finaliza con #");
  Serial.setTimeout(20000L) ;
  tamano_codigo = Serial.readBytesUntil('#', (char *) buffer, 30);
  
  //llena con espacio vacio lo que no usemos de espacio para el codigo
  for (byte i = tamano_codigo; i < 10; i++) buffer[i] = ' ';
  
  //inicializamos la key que nos permite escribir en la tarjeta
  for(byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  Serial.println("Ingresa tu tarjeta");
  //Look for new cards
  while(! mfrc522.PICC_IsNewCardPresent()){
    //return;
    //ciclo infinito. Sale hasta que se ingresa la tarjeta  
  }

  //select one of the cards
  if( ! mfrc522.PICC_ReadCardSerial()){
    //return;
    Serial.println("Tarjeta no compatible");
  }

 
  for(byte i = 0; i < mfrc522.uid.size; i++){
  }

  //especificamos el tipo de tarjeta
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

  //asignamos un bloque de escritura. 7 bloques de 30 caracteres 
  block = 1;

  //autentificacion de la tarjeta
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  //En caso de haber algun error con la key o con el bloque, etc. arroja un error
  if(status != MFRC522::STATUS_OK){
    return;
  }

  //realizamos la escritura en el bloque 1 lo que hay en el buffer limitado a 16 bits
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  //si hay algun problema en la escritura lanza un error
  if(status != MFRC522::STATUS_OK){
    return;
  }

  Serial.println("Escrito correctamente");
  mfrc522.PICC_HaltA(); //Halt PICC
  mfrc522.PCD_StopCrypto1(); //stop encryption
 
}



void Leer()
{
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte tamano_codigo;
  MFRC522::StatusCode status;
  Serial.println("Pase su tarjeta");

  //-------------------------------------------

  // Look for new cards
  while ( ! mfrc522.PICC_IsNewCardPresent()) {
    //return;
    //se queda en el ciclo hasta que detecta una nueva tarjeta
  }

  // Select one of the cards
  if( ! mfrc522.PICC_ReadCardSerial()) {
    //return;
    Serial.println("Tarjeta No Compatible");
  }
  Serial.println("Tarjeta OK");
  //Serial.println(F("**Tarjeta OK:**"));
  
  tamano_codigo = 18;

  //-------------------------------------------

  //mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex
  //aumentamos el tamaño para que haya un rango mas amplio de lectura
  tamano_codigo = 18;
  //---------------------------------------- GET LAST NAME

  //declaramos un buffer de la misma longitud de tamano_codigo
  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //hace una lectura del bloque y del Buffer
  status = mfrc522.MIFARE_Read(block, buffer2, &tamano_codigo);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  String data=""; //variable para guardar el vector de caracteres en una cadena
  for (uint8_t i = 0; i < 16; i++) {
    data+=(char)buffer2[i];
  }
  Serial.println(data);
  Serial.println(F(" "));


  //----------------------------------------

  //Serial.println(F("\n**End Reading**\n"));

  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

