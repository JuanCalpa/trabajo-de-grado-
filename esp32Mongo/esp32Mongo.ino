// DHT sensor biblioteca
#include <DHTesp.h>
#include "time.h"


//Importar WiFi
#include <WiFi.h>
#include <HTTPClient.h>


//Serial
#include <SoftwareSerial.h>
// FS library para ficheros SD library
#include <Arduino.h>
#include <FS.h>
#include <SD.h>

// SPI for esp32
#include <SPI.h>



// Configuracion wifi
#define WIFI_SSID "Admin"
#define WIFI_PASSWORD "admin123"

// *** CONFIGURACIÓN DE LA BASE DE DATOS MONGO ***
const char* apiServerIP = "10.53.231.45"; // ¡LA IP DE TU PC DONDE CORRE EL SERVIDOR NODE.JS!
const int apiServerPort = 3000;             // El puerto de tu servidor Node.js
const char* apiEndpoint = "/api/datos";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -18000; // UTC-5
const int daylightOffset_sec = 0;


// Necesitas una instancia de WiFiClient para la conexión TCP
WiFiClient client; // 

String formattedTimeString; //Almacenar la hora

// Objetos o variables de una clase específica
hw_timer_t *Timer1_Cfg = NULL;

DHTesp dht;
SoftwareSerial mySerial(33, 32);// RX , TX
SPISettings mySetting(1000000, MSBFIRST, SPI_MODE0);//SPI

// Variables (mantenerlas fuera de loop si son globales)
float dHT22_Humedad;
float dHT22_Temperatura;
float CO;

const byte LED = 13; 
// PIN salida para LED
const byte button = 25; 
// PIN entrada interrupción externa ISR
volatile byte pos = LOW; 
// variable para estado de LED
volatile byte cont_interrupt = LOW; // bandera para interrpción de timer1

volatile byte sendFlag = LOW; // bandera para leer y enviar datos

int adc_MQ; // almacena valor en ADC del sensor de CO
const int pin_MQ = A0;  //PIN digital para sensor CO

int adc_Hall;// Almacena el valor en ADC del sensor de efecto hall
const int pin_Hall = A3; // PIN digital para sensor efecto Hall

const byte LP331AP_CS = 5; // Pin para controlar sensor presion y temperatura
const byte SD_CS = 15;// Pin para controlar sensor presion y temperatura
byte exist = LOW;
float tiempo = 0;
String dataMessage;

// variables para SD
volatile byte SD_OK = LOW; // bandera para estado de la SD
volatile byte SD_send = LOW; // bandera enviar datos
volatile byte SD_file = LOW; // bandera existencia file

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

const int chipSelect = SS; //10;

float heading;

//list of all registers binary addresses LPS331AP;
int BufferSPI;
byte WHO_AM_I = 0B00001111;
byte CTRL_REG1 = 0B00100000;
byte STATUS_REG = 0B00100111;
byte PRESS_POUT_XL_REH = 0B00101000;
byte PRESS_OUT_L = 0B00101001;
byte PRESS_OUT_H = 0B00101010;
byte TEMP_OUT_L = 0B00101011;
byte TEMP_OUT_H = 0B00101100;

byte Read = 0B10000000;
byte Write = 0B00000000;

// FUNCIÓN SETUP
void setup() {
  delay(500);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(LP331AP_CS, OUTPUT);
  digitalWrite(LP331AP_CS, HIGH);
  pinMode(SD_CS, OUTPUT);

  SPI.begin(18, 19, 23, 5);

  Serial.begin(9600);
  while (!Serial);
  mySerial.begin(9600);
  while (!mySerial);

  pos = !pos;
  digitalWrite(LED, pos);
  delay(2000);
  pos = !pos;
  digitalWrite(LED, pos);

  delay(500);

  //config sensor DHT22
  dht.setup(4, DHTesp::DHT22);

  pinMode(digitalPinToInterrupt(button), INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button), isr, FALLING);

  //Config sensor LPS441AP
  WriteRegister(CTRL_REG1, 0B11100000, LP331AP_CS);
  delay(50);

  //iniciar WiFi
  initWifi();

 

  // Configurar hora por NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error al obtener la hora");
  } else {
    char tempChar[20];
    Serial.println(&timeinfo, "Hora actual: %Y-%m-%d %H:%M:%S");
    strftime(tempChar, sizeof(tempChar), "%Y-%m-%d %H:%M:%S", &timeinfo);
    formattedTimeString = String(tempChar);
      //Serial.print("Hora formateada para DB: ");
      //Serial.println(currentTimeString);
  }
  set_Timer();
}

void loop() {
  unsigned long tiempoPrevio = 0;
  unsigned long intervalo = 60000;
  if (SD_OK == HIGH && cont_interrupt == HIGH) {
    digitalWrite(LED, HIGH);

    File file = SD.open("/registro.txt");
    if (!file) {
      Serial.println("File doesn't exist");
      Serial.println("Creating file...");
      dataMessage = "tiempo(s), TEMP_DH(°C), HUM_DH(%), LP_Presion(mbar), LP_Temp(°C), CO(ppm), pm1(ug/m3), pm2_5(ug/m3), pm10(ug/m3) \r\n";
      writeFile(SD, "/registro.txt", dataMessage.c_str());
    } else {
      Serial.println("File already exists");
    }
    file.close();
  } else if (SD_OK == LOW && cont_interrupt == HIGH) {
    if (!SD.begin(SD_CS)) {
      Serial.println("Card Mount Failed");
      SD_OK = LOW;
    } else {
      Serial.println("Card Mount Successful");
      SD_OK = HIGH;
    }
  }

  if (cont_interrupt == HIGH) {
    cont_interrupt = !cont_interrupt;

    //obtiene datos del sensor dht22
    dHT22_Temperatura = dht.getTemperature();
    dHT22_Humedad = dht.getHumidity();

    //verifica si hay un error en la lectura
    if (isnan(dHT22_Temperatura) || isnan(dHT22_Humedad)) {
      Serial.println("Error al leer el sensor DHT!");
      dHT22_Temperatura = 0;
      dHT22_Humedad = 0;
    }

    //obtiene los valore del sensor LPS331AP
    LPS331AP_Presion = ReadPressure();
    LPS331AP_Temperatura = ReadTemperature();

    //conversion
    LP_Presion = LPS331AP_Presion / 4096;
    if (LPS331AP_Temperatura >= 32768) {
      LPS331AP_Temperatura = LPS331AP_Temperatura ^ 0B1111111111111111;
      LPS331AP_Temperatura = LPS331AP_Temperatura + 1;
      LP_Temp = 42.5 - float(LPS331AP_Temperatura / 480);
    } else {
      LP_Temp = 42.5 + float(LPS331AP_Temperatura / 480);
    }

    // obtiene la informacion del sensor de CO
    adc_MQ = analogRead(pin_MQ);

    //converite a ppm
    CO = (float)adc_MQ * 4.28 * 1.3601 / 4095;
    CO = 0.918 * (4.28 - CO) / (CO * (4.28 - 0.918));
    CO = 103.3478 * pow(CO, -1.4808);

    //obtiene los datos de material particulado
    while (mySerial.available()) {
      value = mySerial.read();
      if ((indice == 0 && value != 0x42) || (indice == 1 && value != 0x4d)) {
        Serial.println("Cannot find the data header.");
        pm1 = 0;
        pm2_5 = 0;
        pm10 = 0;
        break;
      }

      if (indice == 4 || indice == 6 || indice == 8 || indice == 10 || indice == 12 || indice == 14) {
        previousValue = value;
      } else if (indice == 5) {
        pm1 = 256 * previousValue + value;
      } else if (indice == 7) {
        pm2_5 = 256 * previousValue + value;
      } else if (indice == 9) {
        pm10 = 256 * previousValue + value;
      } else if (indice > 15) {
        break;
      }
      indice++;
    }
    indice = 0;
    while (mySerial.available()) mySerial.read();

    delay(50);

    //imprimir datos
    Serial.print("Tiempo: ");
    Serial.println(tiempo);
    Serial.print(F("Temp_DH: "));
    Serial.print(dHT22_Temperatura);
    Serial.print(F(" °C "));
    Serial.print(F("Hum_DH: "));
    Serial.print(dHT22_Humedad);
    Serial.print(F(" % "));
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

    tiempo = tiempo + 1;
    // Reemplazar la llamada a enviarFirebase() con la nueva función para MySQL
    enviarDatosPorHTTP();
  }
}

void isr() {
  sendFlag = !sendFlag;
  if (sendFlag == LOW) {
    tiempo = 0;
  }
}

//FUNCIÓN PARA LEER REGISTRO POR SPI
byte ReadRegister(byte Address, byte CS) {
  byte result = 0;
  digitalWrite(CS, LOW);
  SPI.transfer(Read | Address);
  result = SPI.transfer(0x00);
  digitalWrite(CS, HIGH);
  return (result);
}

//FUNCIÓN PARA ESCRIBIR REGISTRO POR SPI
void WriteRegister(byte Address, byte Value, byte CS) {
  digitalWrite(CS, LOW);
  SPI.transfer(Write | Address);
  SPI.transfer(Value);
  digitalWrite(CS, HIGH);
}

//FUNCIÓN PARA LEER PRESIÓN DE LPS331AP
long int ReadPressure() {
  byte Pressure_H = ReadRegister(PRESS_OUT_H, LP331AP_CS);
  byte Pressure_L = ReadRegister(PRESS_OUT_L, LP331AP_CS);
  byte Pressure_XL = ReadRegister(PRESS_POUT_XL_REH, LP331AP_CS);
  long int Pressurei = Pressure_H << 8 | Pressure_L;
  long int Pressure = Pressurei << 8 | Pressure_XL;
  return (Pressure);
}

//FUNCIÓN PARA LEER TEMPERATURA DE LPS331AP
long int ReadTemperature() {
  byte Temp_H = ReadRegister(TEMP_OUT_H, LP331AP_CS);
  byte Temp_L = ReadRegister(TEMP_OUT_L, LP331AP_CS);
  long int Temperature = Temp_H << 8 | Temp_L;
  return (Temperature);
}

// INTERRUPCIÓN E INICIALIZACIÓN DE TIMER 1
void Timer1_ISR() {
  cont_interrupt = !cont_interrupt;
}

void set_Timer() {
  Timer1_Cfg = timerBegin(1, 80, true); // para 5 segundos: divisor en 16000
  timerAttachInterrupt(Timer1_Cfg, &Timer1_ISR, true);
  timerAlarmWrite(Timer1_Cfg, 5000000, true); // para 5 segundos: 25000
  timerAlarmEnable(Timer1_Cfg);
}

//Funcion SD
void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.println(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

//FUNCIONES DE WIFI
// hacer conexion de WiFi
void initWifi() {
  Serial.print("Conectando a: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA); // Configura en modo cliente
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int retries = 10; // Número de intentos de conexión

  while (WiFi.status() != WL_CONNECTED && retries-- > 0) {
    delay(1000); // Espera 1 segundo entre intentos
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nError al conectar a WiFi");
  }
}

 String urlEncodeString(String str) {
  str.replace(" ", "%20"); // Reemplaza espacios por %20
  str.replace(":", "%3A"); // Reemplaza dos puntos por %3A
  // Puedes añadir más reemplazos si necesitas codificar otros caracteres en el futuro
  return str;
}
 void enviarDatosPorHTTP() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    CO=0;
    // Construye la URL completa del endpoint
    String url = String("http://") + apiServerIP + ":" + apiServerPort + apiEndpoint;
    
    http.begin(url);
    // Especifica que el contenido que enviarás es JSON
    http.addHeader("Content-Type", "application/json");

    // Crea el cuerpo de la solicitud en formato JSON
    // Nota: Los nombres de las claves ("temperatura", "humedad", etc.) deben coincidir
    // con lo que esperas recibir en tu backend.
    String jsonPayload = "{";
    jsonPayload += "\"timestamp\":\"" + formattedTimeString + "\",";
    jsonPayload += "\"temperatura\":" + String(dHT22_Temperatura) + ",";
    jsonPayload += "\"humedad\":" + String(dHT22_Humedad) + ",";
    jsonPayload += "\"presion\":" + String(LP_Presion) + ",";
    jsonPayload += "\"temp_lp\":" + String(LP_Temp) + ",";
    jsonPayload += "\"co\":" + String(CO) + ",";
    jsonPayload += "\"pm1\":" + String(pm1) + ",";
    jsonPayload += "\"pm2_5\":" + String(pm2_5) + ",";
    jsonPayload += "\"pm10\":" + String(pm10);
    jsonPayload += "}";

    Serial.println("Enviando JSON: " + jsonPayload);
    
    // Envía la solicitud POST con el payload JSON
    int httpCode = http.POST(jsonPayload);

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.print("Código HTTP: ");
      Serial.println(httpCode);
      Serial.print("Respuesta del servidor: ");
      Serial.println(payload);
    } else {
      Serial.printf("Error en la solicitud POST: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi no conectado para enviar datos.");
  }
}
