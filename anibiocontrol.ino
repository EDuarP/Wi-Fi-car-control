#include <WiFi.h>
#include <ESP32Servo.h>


//------------------Servidor Web en puerto 80---------------------

WiFiServer server(80);
Servo servo1;
Servo servo2;

//---------------------Credenciales de WiFi-----------------------

const char* ssid     = "ssid";
const char* password = "password";

IPAddress local_IP(xxx, xxx, xxx, xxx);
IPAddress gateway(xxx, xxx, xxx, xxx);
IPAddress subnet(255, 255, 255, 0);

//---------------------VARIABLES GLOBALES-------------------------
int contconexion = 0;

String header; // Variable para guardar el HTTP request

//-----------------------------------------------------
static const int servoPin = 13;
static const int servoPin2 = 9;
int motor1Pin1 = 27;
int motor1Pin2 = 26;

int motor2Pin1= 4;
int motor2Pin2= 15;

int vel = 100;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

int cont1=0;
int cont2=0;

//------------------------CODIGO HTML------------------------------
String pagina = "<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='utf-8' />"

"<title>Servidor Web ESP32</title>"
"</head>"
"<body>"
"</body>"
"</html>";


//---------------------------SETUP--------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("");
  servo1.attach(servoPin);
  servo2.attach(servoPin2);

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);

  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  // Conexión WIFI
  WiFi.begin(ssid, password);
  //Cuenta hasta 50 si no se puede conectar lo cancela
  while (WiFi.status() != WL_CONNECTED and contconexion <50) {
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion <50) {
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
      server.begin(); // iniciamos el servidor
  }
  else {
      Serial.println("");
      Serial.println("Error de conexion");
  }
 
}

//----------------------------LOOP----------------------------------

void loop(){
  WiFiClient client = server.available();   // Escucha a los clientes entrantes
 

  if (client) {                             // Si se conecta un nuevo cliente
     currentTime=millis();
     previousTime=currentTime;
    Serial.println("New Client.");         //
    String currentLine = "";                //
    while (client.connected() && currentTime - previousTime <= timeoutTime) {            // loop mientras el cliente está conectado
      currentTime = millis();
      if (client.available()) {             // si hay bytes para leer desde el cliente
        char c = client.read();             // lee un byte
        Serial.write(c);                    // imprime ese byte en el monitor serial
        header += c;
        if (c == '\n') {                    // si el byte es un caracter de salto de linea
          // si la nueva linea está en blanco significa que es el fin del
          // HTTP request del cliente, entonces respondemos:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
           
            // configura pwm
            if (header.indexOf("GET /pwmmax") >= 0) {
              vel = 255;
            } else if (header.indexOf("GET /pwmmin") >= 0) {
              vel = 150;
            }
            else if(header.indexOf("GET /adelante") >= 0){
             
                analogWrite(motor1Pin1, 0);
                analogWrite(motor1Pin2, vel);
                analogWrite(motor2Pin1, 0);
                analogWrite(motor2Pin2, vel);  
            }
            else if(header.indexOf("GET /atras") >= 0){
                analogWrite(motor1Pin1, vel);
                analogWrite(motor1Pin2, 0);
                analogWrite(motor2Pin1, vel);
                analogWrite(motor2Pin2, 0);  
            }
            else if(header.indexOf("GET /parar") >= 0){
                analogWrite(motor1Pin1, 0);
                analogWrite(motor1Pin2, 0);
                analogWrite(motor2Pin1, 0);
                analogWrite(motor2Pin2, 0);  
            }
             else if(header.indexOf("GET /derecha") >= 0){
               vel = 150;
                analogWrite(motor1Pin1, vel);
                analogWrite(motor1Pin2, 0);
                analogWrite(motor2Pin1, 0);
                analogWrite(motor2Pin2, vel);
                vel = 255;
                 
            }  
            else if(header.indexOf("GET /izquierda") >= 0){
                vel = 150;
                analogWrite(motor1Pin1, 0);
                analogWrite(motor1Pin2, vel);
                analogWrite(motor2Pin1, vel);
                analogWrite(motor2Pin2, 0);
                vel = 255;
            }    
             else if(header.indexOf("GET /subir") >= 0){
                Serial.println("pala arriba");
                servo1.write(90);
            }
            else if(header.indexOf("GET /bajar") >= 0){
                Serial.println("pala abajo");
                servo1.write(0);
            }
           else if(header.indexOf("GET /dispara") >= 0){
                servo2.write(180);
            }
            else if(header.indexOf("GET /regresa") >= 0){
                servo2.write(0);
            }
            // Muestra la página web
           client.println(pagina);
           
            // la respuesta HTTP temina con una linea en blanco
            client.println();
            break;
          } else { // si tenemos una nueva linea limpiamos currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // si C es distinto al caracter de retorno de carro
          currentLine += c;      // lo agrega al final de currentLine
        }
      }
    }
    // Limpiamos la variable header
    header = "";
    // Cerramos la conexión
    client.stop();
    Serial.println("");
    delay(100);
  }
}
