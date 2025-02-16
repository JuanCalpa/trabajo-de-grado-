// DHT sensor library for ESPx - Version: Latest 
#include <DHTesp.h>

//Bluetooth
#include <BluetoothSerial.h>

//Serial
#include <SoftwareSerial.h>

//check if Bluetooth is properly enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

// SPI for esp32 - Version: Latest 
#include <SPI.h>

// FS library para ficheros SD library
#include <Arduino.h>
#include <FS.h>
#include <SD.h>

// Magnetometro HMC5883L
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

// Objetos o variables de una clase específica
hw_timer_t *Timer1_Cfg = NULL;

BluetoothSerial SerialBT;
DHTesp dht;
SoftwareSerial mySerial(33,32); // RX , TX
SoftwareSerial mySerial1(26,27); // RX , TX suelo
SPISettings mySetting(1000000, MSBFIRST, SPI_MODE0); //SPI
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345); /* Magnetometer Assign a unique ID to this sensor at the same time */

// Variables+
float dHT22_Humedad;
float dHT22_Temperatura;
float CO;

const byte LED=13; // PIN salida para LED
const byte button=25; // PIN entrada interrupción externa ISR
const byte PIN_SOIL=14; // PIN de salida para DE y RE que controla la dirección de transmision del conversor TTL a RS485
volatile byte pos = LOW; // variable para estado de LED
volatile byte cont_interrupt = LOW; // bandera para interrpción de timer1
volatile byte BT_in; // variable que almacena dato que ingresa por bluetooth
volatile byte send = LOW; // bandera para leer y enviar datos
volatile byte send_BT = LOW; // bandera para enviar datos bluetooth

int adc_MQ; // almacena valor en ADC del sensor de CO
const int pin_MQ = A0; //PIN digital para sensor CO

int adc_Hall; // Almacena el valor en ADC del sensor de efecto hall
const int pin_Hall = A3; // PIN digital para sensor efecto Hall

const byte LP331AP_CS=5; // Pin para controlar sensor presion y temperatura
const byte SD_CS=15; // Pin para controlar sensor presion y temperatura
byte exist = LOW;
float tiempo = 0;
String dataMessage;
volatile byte SD_OK = LOW; // bandera para estado de la SD
volatile byte SD_send = LOW; // bandera enviar datos

int indice = 0;
char value;
char previousValue;
unsigned int pm1 = 0;
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;

long int LPS331AP_Presion;
float LP_Presion;
long int LPS331AP_Temperatura;
float LP_Temp;

const int chipSelect = SS;//10;

float heading;

//list of all registers binary addresses LPS331AP;
int BufferSPI;
//byte REF_P_XL           = 0B00001000;
//byte REF_P_L            = 0B00001001;
//byte REF_P_H            = 0B00001010;
byte WHO_AM_I           = 0B00001111;
//byte RES_CONF           = 0B00010000;
byte CTRL_REG1          = 0B00100000;
//byte CTRL_REG2          = 0B00100001;
//byte CTRL_REG3          = 0B00100010;
//byte INT_CFG_REG        = 0B00100011;
//byte INT_SOURCE_REG     = 0B00100100;
//byte THS_P_LOW_REG      = 0B00100101;
//byte THS_P_HIGH_REG     = 0B00100110;
byte STATUS_REG         = 0B00100111;
byte PRESS_POUT_XL_REH  = 0B00101000;
byte PRESS_OUT_L        = 0B00101001;
byte PRESS_OUT_H        = 0B00101010;
byte TEMP_OUT_L         = 0B00101011;
byte TEMP_OUT_H         = 0B00101100;
byte AMP_CTRL           = 0B00110000;

byte Read  = 0B10000000;
byte Write = 0B00000000;

//list of all registers hexadecimal addresses SOIL SENSOR;
const byte soil_init[] = {0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0A}; // CRC-16 / MODBUS
//const byte soil_mois[] = {0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0A}; // CRC-16 / MODBUS
const byte soil_mois[] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA}; // CRC-16 / MODBUS
const byte soil_temp[] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x01, 0x25, 0xCA}; // CRC-16 / MODBUS
//const byte soil_temp[] = {0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x75, 0xCF}; // CRC-16 / MODBUS
const byte soil_cond[] = {0x01, 0x03, 0x00, 0x03, 0x00, 0x01, 0x74, 0x0A}; // CRC-16 / MODBUS
const byte soil_ph[] = {0x01, 0x03, 0x00, 0x04, 0x00, 0x01, 0xC5, 0xCB}; // CRC-16 / MODBUS
const byte soil_nitro[] = {0x01, 0x03, 0x00, 0x05, 0x00, 0x01, 0x94, 0x0B}; // CRC-16 / MODBUS
const byte soil_fosf[] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0B}; // CRC-16 / MODBUS
const byte soil_pot[] = {0x01, 0x03, 0x00, 0x07, 0x00, 0x01, 0x35, 0xCB}; // CRC-16 / MODBUS
const byte soil_sal[] = {0x01, 0x03, 0x00, 0x08, 0x00, 0x01, 0x05, 0xC8}; // CRC-16 / MODBUS
const byte soil_TDS[] = {0x01, 0x03, 0x00, 0x09, 0x00, 0x01, 0x54, 0x08}; // CRC-16 / MODBUS

float soil_moisture;
float soil_temperature;
float soil_conductivity;
float soil_pH;
float soil_Nitrogeno;
float soil_Fosforo;
float soil_Potasio;
float soil_Salinidad;
float soil_SolidosDisueltos;


// FUNCIÓN SETUP
void setup() {
  // put your setup code here, to run once:
  delay(500);
  pinMode(LED, OUTPUT);
  digitalWrite(LED,LOW);

  pinMode(PIN_SOIL, OUTPUT);
  digitalWrite(PIN_SOIL,LOW);

  pinMode(LP331AP_CS,OUTPUT);
  digitalWrite(LP331AP_CS,HIGH);
  pinMode(SD_CS,OUTPUT);
  //digitalWrite(SD_CS,HIGH);
  //SPI.beginTransaction(mySetting);
  SPI.begin(18,19,23,5);

  Serial.begin(9600);
  while (!Serial);
  mySerial.begin(9600);
  while (!mySerial);
  mySerial1.begin(4800);
  while (!mySerial1);

  // Serial.println("HMC5883 Magnetometer Test"); Serial.println("");
  
  // /* Initialise the sensor */
  // if(!mag.begin())
  // {
  //   /* There was a problem detecting the HMC5883 ... check your connections */
  //   Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
  //   while(1);
  // }

  /* Display some basic information on this sensor */
  // displaySensorDetails();

  if(!SD.begin(SD_CS)){
    Serial.println("Card Mount Failed");
    SD_OK = LOW;
    SerialBT.println("X");
  }else{
    Serial.println("Card Mount Succesfull");
    SD_OK = HIGH;
    SerialBT.println("Y");
  }

  pos =! pos;
  digitalWrite(LED, pos);
  delay(2000);
  pos =! pos;
  digitalWrite(LED, pos);

  // uint8_t cardType = SD.cardType();

  // if(cardType == CARD_NONE){
  //    Serial.println("No SD card attached");
  //    return;
  // }

  // delay(500);
  // Serial.println("TIPO DE TARJETA...");
  // Serial.print("SD Card Type: ");
  // if(cardType == CARD_MMC){
  //   Serial.println("MMC");
  // } else if(cardType == CARD_SD){
  //   Serial.println("SDSC");  //ESTA
  // } else if(cardType == CARD_SDHC){
  //   Serial.println("SDHC");
  // } else {
  //   Serial.println("UNKNOWN");
  // }

  // delay(500);
  // Serial.println("TAMAÑO TARJETA...");
  // uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  // Serial.printf("SD Card Size: %lluMB\n", cardSize);  //1910MB

  delay(500);
  // //deleteFile(SD, "/registro.txt");
  // //deleteFile(SD, "/data.txt");
  // listDir(SD, "/", 0);

  //  File file = SD.open("/registro.txt");
  //  if(!file) {
  //    Serial.println("File doens't exist");
  //    Serial.println("Creating file...");
  //    dataMessage = "tiempo(s), TEMP_DH(°C), HUM_DH(%), LP_Presion(mbar), LP_Temp(°C), MQ(ADC), pm1(ug/m3), pm2_5(ug/m3), pm10(ug/m3) \r\n";
  //    writeFile(SD, "/registro.txt", dataMessage.c_str());
  //  }
  //  else {
  //    Serial.println("File already exists");  
  //  }
  //  file.close();


  // Bluetooth Conection
  SerialBT.begin("ESMCod0001"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  dht.setup(4,DHTesp::DHT22); 

  pinMode(digitalPinToInterrupt(button), INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button), isr, FALLING);

  WriteRegister(CTRL_REG1,0B11100000,LP331AP_CS);
  soil_InitMeasure();
  delay(50);

  set_Timer();

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
    
  if (SerialBT.available()) {
    BT_in = SerialBT.read();
    Serial.println(BT_in);

    if (BT_in == 'A'){
      send = HIGH;
      // send_BT = HIGH;
      // digitalWrite(LED, HIGH);
      Serial.println("Hola A.");
    }else if (BT_in == 'a'){
      send = LOW;
      tiempo = 0;
      // digitalWrite(LED, LOW);
      // send_BT = LOW;
      Serial.println("Hola a.");
    }else if (BT_in == 'B'){
      Serial.println("Hola B.");
      SD_send = HIGH;
    }else if (BT_in == 'b'){
      Serial.println("Hola b.");
      SD_send = LOW;
    }
  }

  if (SerialBT.connected()){
    digitalWrite(LED,HIGH);
    if ((int)tiempo % 20 == 0 && (tiempo > 0 || tiempo ==1)){

      if (SD_OK = HIGH){
        SerialBT.println("Y");
        File file = SD.open("/registro.txt");
        if(!file) {
          Serial.println("File doens't exist");
          Serial.println("Creating file...");
          dataMessage = "tiempo(s), TEMP_DH(°C), HUM_DH(%), LP_Presion(mbar), LP_Temp(°C), MQ(ADC), pm1(ug/m3), pm2_5(ug/m3), pm10(ug/m3) \r\n";
          writeFile(SD, "/registro.txt", dataMessage.c_str());
        }else {
          Serial.println("File already exists");  
        }
        file.close();
      }else{
        if(!SD.begin(SD_CS)){
        Serial.println("Card Mount Failed");
        SD_OK = LOW;
        SerialBT.println("X");
        }else{
          Serial.println("Card Mount Succesfull");
          SD_OK = HIGH;
          SerialBT.println("Y");
        }
      }
    }

  }else{
    digitalWrite(LED,LOW);
  }

  // adc_Hall = analogRead(pin_Hall);
  // Serial.println((float) adc_Hall*4.41/4095);
  // delay(100);

  if (cont_interrupt == HIGH && send == HIGH){

    cont_interrupt =! cont_interrupt;

    dHT22_Temperatura = dht.getTemperature();
    dHT22_Humedad = dht.getHumidity();

     if (isnan(dHT22_Temperatura) || isnan(dHT22_Humedad)) {
       Serial.println("Error al leer el sensor DHT!");
       dHT22_Temperatura = 0;
       dHT22_Humedad = 0;
     }

    LPS331AP_Presion = ReadPressure();
    LPS331AP_Temperatura = ReadTemperature();

      LP_Presion = LPS331AP_Presion/4096;
      if (LPS331AP_Temperatura>=32768) {
        LPS331AP_Temperatura = LPS331AP_Temperatura^0B1111111111111111;
        LPS331AP_Temperatura = LPS331AP_Temperatura+1;
        LP_Temp = 42.5-float(LPS331AP_Temperatura/480);
      }else{
        //Serial.println(LPS331AP_Temperatura,BIN);
        LP_Temp = 42.5+float(LPS331AP_Temperatura/480);
      }

    adc_MQ = analogRead(pin_MQ);
    CO = (float) adc_MQ*4.28*1.3601/4095;
    CO= 0.918*(4.28-CO)/(CO*(4.28-0.918));
    CO = 103.3478*pow(CO,-1.4808);  

     while (mySerial.available()) {
       value = mySerial.read();
       if ((indice == 0 && value != 0x42) || (indice == 1 && value != 0x4d)){
         Serial.println("Cannot find the data header.");
         pm1 = 0;
         pm2_5 = 0;
         pm10 = 0;
         break;
       }

       if (indice == 4 || indice == 6 || indice == 8 || indice == 10 || indice == 12 || indice == 14) {
         previousValue = value;
       }
       else if (indice == 5) {
         pm1 = 256 * previousValue + value;
       }
       else if (indice == 7) {
         pm2_5 = 256 * previousValue + value;
       }
       else if (indice == 9) {
         pm10 = 256 * previousValue + value;
       } else if (indice > 15) {
         break;
       }
       indice++;
     }
     indice = 0;
     while(mySerial.available()) mySerial.read();

    // sensors_event_t event; 
    // mag.getEvent(&event);
 
    // /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
    // Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
    // Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
    // Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");

    // heading = atan2(event.magnetic.y, event.magnetic.x);

    // // Correct for when signs are reversed.
    // if(heading < 0){
    //   heading += 2*PI;
    // } 
    // // Check for wrap due to addition of declination.
    // if(heading > 2*PI){
    //   heading -= 2*PI;
    // }
    // // Convert radians to degrees for readability.
    // float headingDegrees = heading * 180/M_PI; 
    
    // Serial.print("Heading (degrees): "); Serial.println(headingDegrees);

    soil_InitMeasure();
    delay(50);
    soil_moisture = get_soilmoisture();
    if (soil_moisture > 100){
      soil_moisture = 0;
    }
    delay(50);
    soil_temperature = get_soiltemperature();
    delay(50);
    soil_conductivity = get_soilconductivity();
    if (soil_conductivity > 60000){
      soil_conductivity = 0;
    }
    delay(50);
    soil_pH = get_soilpH();
    if (soil_pH > 6000){
      soil_pH = 0;
    }
    delay(50);
    soil_Nitrogeno = get_soilNitrogeno();
    if (soil_Nitrogeno > 60000){
      soil_Nitrogeno = 0;
    }
    delay(50);
    soil_Fosforo = get_soilFosforo();
    if (soil_Fosforo > 60000){
      soil_Fosforo = 0;
    }
    delay(50);
    soil_Potasio = get_soilPotasio();
    if (soil_Potasio > 60000){
      soil_Potasio = 0;
    }
    delay(50);
    soil_Salinidad = get_soilSalinidad();
    if (soil_Salinidad > 60000){
      soil_Salinidad = 0;
    }
    delay(50);
    // soil_SolidosDisueltos = get_soilTDS();
    // delay(50);

    Serial.print(F("Temp_DH: "));
    Serial.print(dHT22_Temperatura);
    Serial.print(F(" °C   "));
    Serial.print(F("Hum_DH: "));
    Serial.print(dHT22_Humedad);
    Serial.print(F(" %   "));
    Serial.print(F("CO: "));
    Serial.print(CO);
    Serial.println(F("ppm "));

    Serial.print("Presión: ");
    Serial.print(LP_Presion);
    Serial.print(" mbar ");
    Serial.print("Temp_LP: ");
    Serial.print(LP_Temp);
    Serial.println(" °C ");

    Serial.print("{ ");
    Serial.print("\"pm1\": ");
    Serial.print(pm1);
    Serial.print(" ug/m3");
    Serial.print(", ");
    Serial.print("\"pm2_5\": ");
    Serial.print(pm2_5);
    Serial.print(" ug/m3");
    Serial.print(", ");
    Serial.print("\"pm10\": ");
    Serial.print(pm10);
    Serial.print(" ug/m3");
    Serial.println(" }");

    Serial.print("Temp_soil: ");
    Serial.print(soil_temperature);
    Serial.print(F(" °C, "));

    Serial.print("Humedad: ");
    Serial.print(soil_moisture);
    Serial.print("%,  ");
    Serial.print(F("Sigma: "));
    Serial.print(soil_conductivity);
    Serial.print(F("uS/cm  "));
    Serial.print(F("pH: "));
    Serial.print(soil_pH);
    Serial.println(F("  "));
    Serial.print(F("Nitrogeno: "));
    Serial.print(soil_Nitrogeno);
    Serial.print(F("mg/Kg, "));
    Serial.print(F("Fosforo: "));
    Serial.print(soil_Fosforo);
    Serial.print(F("mg/Kg, "));
    Serial.print(F("Potasio: "));
    Serial.print(soil_Potasio);
    Serial.println(F("mg/Kg, "));


    SerialBT.print('A');
    SerialBT.println((int)(dHT22_Temperatura*10));
    SerialBT.print('B');
    SerialBT.println((int)(dHT22_Humedad*10));
    SerialBT.print('C');
    SerialBT.println(LP_Presion);
    SerialBT.print('D');
    SerialBT.println((int)(CO*10));
    SerialBT.print('E');
    SerialBT.println(pm1);
    SerialBT.print('F');
    SerialBT.println(pm2_5);
    SerialBT.print('G');
    SerialBT.println(pm10);
    SerialBT.print('H');
    SerialBT.println(random(600,620));
    SerialBT.print('I');
    SerialBT.println(random(30,40));
    SerialBT.print('J');
    SerialBT.println(random(120,150));
    SerialBT.print('L');
    SerialBT.println((int)(soil_temperature*10));
    SerialBT.print('M');
    SerialBT.println((int)soil_moisture);
    SerialBT.print('O');
    SerialBT.println((int)(soil_pH*100));
    SerialBT.print('Q');
    SerialBT.println((int)soil_conductivity);
    SerialBT.print('N');
    SerialBT.println((int)soil_Nitrogeno);
    SerialBT.print('P');
    SerialBT.println((int)soil_Fosforo);
    SerialBT.print('K');
    SerialBT.println((int)soil_Potasio);
    //sendBTnum(random(-100,100));
    //sendBTstring(".",1);
    //Serial.println("send num");

    tiempo = tiempo + 1;

    if (SD_send = HIGH){
      Envio_SD();
    }
    
  } 

  // dataMessage = 
  // String(tiempo) + "," + String(96.2) + "," + 
  // String(20.8) + "," + String(86.2)+ "\r\n";
  // appendFile(SD, "/registro.txt", dataMessage.c_str());

  // tiempo = tiempo + 5;
  // delay(1000);
  
}

void isr(){
  //pos =! pos;
  //digitalWrite(LED, pos);
  send =! send;
  if (send == LOW){
    tiempo = 0;
  }
}

//FUNCIÓN PARA LEER REGISTRO POR SPI
byte ReadRegister(byte Address, byte CS){
  byte result = 0;
  digitalWrite(CS, LOW); 
  SPI.transfer(Read | Address);
  result = SPI.transfer(0x00);
/*  Serial.print(Address, BIN);
  Serial.print(" : ");
  Serial.println(result, BIN); */
  digitalWrite(CS, HIGH);
  return(result);  
}

//FUNCIÓN PARA ESCRIBIR REGISTRO POR SPI
void WriteRegister(byte Address, byte Value,byte CS){
  digitalWrite(CS, LOW);
  SPI.transfer(Write | Address);
  SPI.transfer(Value);
  digitalWrite(CS, HIGH);
}

//FUNCIÓN PARA LEER PRESIÓN DE LPS331AP
long int ReadPressure(){
  byte Pressure_H = ReadRegister(PRESS_OUT_H,LP331AP_CS);  
  byte Pressure_L = ReadRegister(PRESS_OUT_L,LP331AP_CS);
  byte Pressure_XL = ReadRegister(PRESS_POUT_XL_REH,LP331AP_CS);
  
  long int Pressurei = Pressure_H <<8 | Pressure_L;
  long int Pressure = Pressurei <<8 | Pressure_XL;
  return(Pressure);
}

//FUNCIÓN PARA LEER TEMPERATURA DE LPS331AP
long int ReadTemperature(){
  byte Temp_H = ReadRegister(TEMP_OUT_H,LP331AP_CS);
  byte Temp_L = ReadRegister(TEMP_OUT_L,LP331AP_CS); 
  long int Temperature = Temp_H <<8 | Temp_L;
  return(Temperature);
}

// INTERRUPCIÓN E INICIALIZACIÓN DE TIMER 1
void Timer1_ISR(){
  cont_interrupt =! cont_interrupt;
}

void set_Timer(){
  Timer1_Cfg = timerBegin(1, 80, true); // para 5 segundos: divisor en 16000
  timerAttachInterrupt(Timer1_Cfg, &Timer1_ISR, true);
  timerAlarmWrite(Timer1_Cfg, 1000000, true); // para 5 segundos: 25000
  timerAlarmEnable(Timer1_Cfg);
}

// FUNCIÓN PARA ENVIAR STRING POR BLUETOOTH
void sendBTstring(char *mensaje, byte dim){
  for(byte i=0; i<dim; i++){
    SerialBT.write(mensaje[i]);
  }
  delay(100);
}

void sendBTnum(long int num){
  long int ced = num;
  char buffer;
  byte cifras = 1;
  while(ced>9){
    ced = ced/10;
    cifras += 1;
  }

  for(byte i=cifras;i>0;i--){
    buffer = floor(num/pow(10,(i-1)));
    SerialBT.write(buffer+48);
    num = num - floor(num/pow(10,(i-1)))*pow(10,(i-1));
  }
  delay(100);
}

///////// FUNCIONES PARA SD

// Función listar un directorio

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if(levels){
        listDir(fs, file.name(), levels -1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    if(file.name()== "/registro"){
      exist = HIGH;
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char * path){
  Serial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char * path){
  Serial.printf("Removing Dir: %s\n", path);
  if(fs.rmdir(path)){
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.println(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}


void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char * path){
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if(file){
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len){
      size_t toRead = len;
      if(toRead > 512){
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for(i=0; i<2048; i++){
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

void Envio_SD(){
  
dataMessage = 
String(tiempo) + "," + String(dHT22_Temperatura) + "," + String(dHT22_Humedad) + "," + String(LP_Presion) + "," + 
String(LP_Temp) + "," + String(adc_MQ)+ "," + String(pm1) + "," + String(pm2_5) + "," + String(pm10) + "\r\n";
appendFile(SD, "/registro.txt", dataMessage.c_str());
//   appendFileString(SD, "/registro.txt", "tiempo (s), ");
//   appendFileNum(SD, "/registro.txt", tiempo);
//   appendFileString(SD, "/registro.txt", ", Temp_DH (°C), ");
//   appendFileNum(SD, "/registro.txt", dHT22_Temperatura);
//   appendFileString(SD, "/registro.txt", ", Hum_DH (%), ");
//   appendFileNum(SD, "/registro.txt", dHT22_Humedad);
//   appendFileString(SD, "/registro.txt", ", LP_Presion (mbar), ");
//   appendFileNum(SD, "/registro.txt", LP_Presion);
//   appendFileString(SD, "/registro.txt", ", LP_Temp (°C), ");
//   appendFileNum(SD, "/registro.txt", LP_Temp);
//   appendFileString(SD, "/registro.txt", ", MQ (ADC), ");
//   appendFileNum(SD, "/registro.txt", adc_MQ);
//   appendFileString(SD, "/registro.txt", ", pm1 (ug/m3), ");
//   appendFileNum(SD, "/registro.txt", pm1);
//   appendFileString(SD, "/registro.txt", ", pm2_5 (ug/m3), ");
//   appendFileNum(SD, "/registro.txt", pm2_5);
//   appendFileString(SD, "/registro.txt", ", pm10 (ug/m3), ");
//   appendFileNum(SD, "/registro.txt", pm10);
//   appendFileString(SD, "/registro.txt", "\n");
}

//Funciones para el sensor magnetómetro HMC5883L
void displaySensorDetails(void)
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");  
  Serial.println("------------------------------------");
  Serial.println("");
}

//FUNCIONES PARA EL SENSOR DE SUELO FIVE IN ONE RENKER

void soil_InitMeasure(){
  byte values[7];
  digitalWrite(PIN_SOIL,HIGH);
  if(mySerial1.write(soil_init,sizeof(soil_init))==8){
    digitalWrite(PIN_SOIL,LOW);
    for (byte i=0;i<7; i++){
      values[i] = mySerial1.read();
    }
  }
  Serial.print(values[0],HEX); Serial.print(values[1],HEX); Serial.print(values[2],HEX); Serial.print(" ");
  Serial.print(values[3]<<8|values[4]); Serial.print(" "); Serial.print(values[5],HEX);Serial.println(values[6],HEX);
}

float get_soilmoisture(){
  byte values[7];
  digitalWrite(PIN_SOIL,HIGH);
  if(mySerial1.write(soil_mois,sizeof(soil_mois))==8){
    digitalWrite(PIN_SOIL,LOW);
    for (byte i=0;i<7; i++){
      values[i] = mySerial1.read();
    }
  }
  Serial.print(values[0],HEX); Serial.print(values[1],HEX); Serial.print(values[2],HEX); Serial.print(" ");
  Serial.print(values[3]<<8|values[4]); Serial.print(" "); Serial.print(values[5],HEX);Serial.println(values[6],HEX);
  return (float) (values[3]<<8|values[4])/10;
}

float get_soiltemperature(){
  byte values[7];
  int byte_temp;
  digitalWrite(PIN_SOIL,HIGH);
  if(mySerial1.write(soil_temp,sizeof(soil_temp))==8){
    digitalWrite(PIN_SOIL,LOW);
    for (byte i=0;i<7; i++){
      values[i] = mySerial1.read();
    }
  }
  Serial.print(values[0],HEX); Serial.print(values[1],HEX); Serial.print(values[2],HEX); Serial.print(" ");
  Serial.print(values[3]<<8|values[4]); Serial.print(" "); Serial.print(values[5],HEX);Serial.println(values[6],HEX);
  byte_temp = (values[3]<<8|values[4]);

  if(byte_temp>0x7FFF){
    byte_temp = (byte_temp ^ 0xFFFF)+1;
    return (float) byte_temp*(-1)/10;
  }else{
    return (float) byte_temp/10;
  }
}

float get_soilconductivity(){
  byte values[7];
  digitalWrite(PIN_SOIL,HIGH);
  if(mySerial1.write(soil_cond,sizeof(soil_cond))==8){
    digitalWrite(PIN_SOIL,LOW);
    for (byte i=0;i<7; i++){
      values[i] = mySerial1.read();
    }
  }
  Serial.print(values[0],HEX); Serial.print(values[1],HEX); Serial.print(values[2],HEX); Serial.print(" ");
  Serial.print(values[3]<<8|values[4]); Serial.print(" "); Serial.print(values[5],HEX);Serial.println(values[6],HEX);
  return (float) (values[3]<<8|values[4]);
}

float get_soilpH(){
  byte values[7];
  digitalWrite(PIN_SOIL,HIGH);
  if(mySerial1.write(soil_ph,sizeof(soil_ph))==8){
    digitalWrite(PIN_SOIL,LOW);
    for (byte i=0;i<7; i++){
      values[i] = mySerial1.read();
    }
  }
  Serial.print(values[0],HEX); Serial.print(values[1],HEX); Serial.print(values[2],HEX); Serial.print(" ");
  Serial.print(values[3]<<8|values[4]); Serial.print(" "); Serial.print(values[5],HEX);Serial.println(values[6],HEX);
  return (float) (values[3]<<8|values[4])/10;
}

float get_soilNitrogeno(){
  byte values[7];
  digitalWrite(PIN_SOIL,HIGH);
  if(mySerial1.write(soil_nitro,sizeof(soil_nitro))==8){
    digitalWrite(PIN_SOIL,LOW);
    for (byte i=0;i<7; i++){
      values[i] = mySerial1.read();
    }
  }
  Serial.print(values[0],HEX); Serial.print(values[1],HEX); Serial.print(values[2],HEX); Serial.print(" ");
  Serial.print(values[3]<<8|values[4]); Serial.print(" "); Serial.print(values[5],HEX);Serial.println(values[6],HEX);
  return (float) (values[3]<<8|values[4]);
}

float get_soilFosforo(){
  byte values[7];
  digitalWrite(PIN_SOIL,HIGH);
  if(mySerial1.write(soil_fosf,sizeof(soil_fosf))==8){
    digitalWrite(PIN_SOIL,LOW);
    for (byte i=0;i<7; i++){
      values[i] = mySerial1.read();
    }
  }
  Serial.print(values[0],HEX); Serial.print(values[1],HEX); Serial.print(values[2],HEX); Serial.print(" ");
  Serial.print(values[3]<<8|values[4]); Serial.print(" "); Serial.print(values[5],HEX);Serial.println(values[6],HEX);
  return (float) (values[3]<<8|values[4]);
}

float get_soilPotasio(){
  byte values[7];
  digitalWrite(PIN_SOIL,HIGH);
  if(mySerial1.write(soil_pot,sizeof(soil_pot))==8){
    digitalWrite(PIN_SOIL,LOW);
    for (byte i=0;i<7; i++){
      values[i] = mySerial1.read();
    }
  }
  Serial.print(values[0],HEX); Serial.print(values[1],HEX); Serial.print(values[2],HEX); Serial.print(" ");
  Serial.print(values[3]<<8|values[4]); Serial.print(" "); Serial.print(values[5],HEX);Serial.println(values[6],HEX);
  return (float) (values[3]<<8|values[4]);
}

float get_soilSalinidad(){
  byte values[7];
  digitalWrite(PIN_SOIL,HIGH);
  if(mySerial1.write(soil_sal,sizeof(soil_sal))==8){
    digitalWrite(PIN_SOIL,LOW);
    for (byte i=0;i<7; i++){
      values[i] = mySerial1.read();
    }
  }
  Serial.print(values[0],HEX); Serial.print(values[1],HEX); Serial.print(values[2],HEX); Serial.print(" ");
  Serial.print(values[3]<<8|values[4]); Serial.print(" "); Serial.print(values[5],HEX);Serial.println(values[6],HEX);
  return (float) (values[3]<<8|values[4]);
}

float get_soilTDS(){
  byte values[7];
  digitalWrite(PIN_SOIL,HIGH);
  if(mySerial1.write(soil_TDS,sizeof(soil_TDS))==8){
    digitalWrite(PIN_SOIL,LOW);
    for (byte i=0;i<7; i++){
      values[i] = mySerial1.read();
    }
  }
  Serial.print(values[0],HEX); Serial.print(values[1],HEX); Serial.print(values[2],HEX); Serial.print(" ");
  Serial.print(values[3]<<8|values[4]); Serial.print(" "); Serial.print(values[5],HEX);Serial.println(values[6],HEX);
  return (float) (values[3]<<8|values[4]);
}
