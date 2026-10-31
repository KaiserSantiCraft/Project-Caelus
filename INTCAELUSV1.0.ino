
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <SD.h>
#include <SFE_BMP180.h>
#include <Wire.h>


File myFile;
MPU6050 sensor;
SFE_BMP180 bmp180;

double PresionNivelMar=1024; //presion sobre el nibel del mar en mbar
int ax, ay, az;
int gx, gy, gz;

void setup() {
  Serial.begin(9600);    //Iniciando puerto serial
  Wire.begin();           //Iniciando I2C  
  sensor.initialize();    //Iniciando el sensor
  Serial.println("Start");
  if (sensor.testConnection()) Serial.println("Sensor iniciado correctamente");
  else Serial.println("Error al iniciar el sensor");
    Serial.print("Iniciando SD ...");
  if (!SD.begin(4)) {
    Serial.println("No se pudo inicializar");
    return;
  }
  if (bmp180.begin())
    Serial.println("BMP180 iniciado correctamenten");
  else
  {
    Serial.println("Error al iniciar el BMP180");
    while(1); // bucle infinito
  }
  Serial.println("inicializacion exitosa");
}

void loop() {
  // Leer las aceleraciones y velocidades angulares
  sensor.getAcceleration(&ax, &ay, &az);
  sensor.getRotation(&gx, &gy, &gz);
  float ax_m_s2 = ax * (9.81/16384.0);
  float ay_m_s2 = ay * (9.81/16384.0);
  float az_m_s2 = az * (9.81/16384.0);
  float gx_deg_s = gx * (250.0/32768.0);
  float gy_deg_s = gy * (250.0/32768.0);
  float gz_deg_s = gz * (250.0/32768.0);
  //Mostrar las lecturas separadas por un [tab]
  char status;
  double T,P,A;
  
  status = bmp180.startTemperature();//Inicio de lectura de temperatura
  if (status != 0)
  {   
    delay(status); //Pausa para que finalice la lectura
    status = bmp180.getTemperature(T); //Obtener la temperatura
    if (status != 0)
    {
      status = bmp180.startPressure(3);//Inicio lectura de presión
      if (status != 0)
      {        
        delay(status);//Pausa para que finalice la lectura        
        status = bmp180.getPressure(P,T);//Obtenemos la presión
        if (status != 0)
        {                  
          Serial.print("Temperatura: ");
          Serial.print(T);
          Serial.print(" *C , ");
          Serial.print("Presion: ");
          Serial.print(P);
          Serial.print(" mb , ");     
          //-------Calculamos la altitud--------
          A= bmp180.altitude(P,PresionNivelMar);
          Serial.print("Altitud: ");
          Serial.print(A);
          Serial.println(" m s.n.m.");    
        }      
      }      
    }   
  }
  myFile = SD.open("datalog.csv", FILE_WRITE);//abrimos  el archivo
  
    if (myFile) { 
      myFile.print("a[x y z](m/s2) g[x y z](deg/s):\t");
      myFile.print(",");
      myFile.print(ax_m_s2);
      myFile.print(",");
      myFile.print(ay_m_s2); 
      myFile.print(",");
      myFile.print(az_m_s2);
      myFile.print(","); 
      myFile.print(gx_deg_s);
      myFile.print(",");
      myFile.print(gy_deg_s);
      myFile.print(","); 
      myFile.print(gz_deg_s);
      myFile.print(","); 
      myFile.print(T);
      myFile.print(","); 
      myFile.print(P);
      myFile.print(",");    
      myFile.print(A);
      myFile.print(","); 
      myFile.println(" m s.n.m.");  
      myFile.close(); //cerramos el archivo   
      Serial.print("a[x y z](m/s2) g[x y z](deg/s):\t");
      Serial.print(ax_m_s2); Serial.print("\t");
      Serial.print(ay_m_s2); Serial.print("\t");
      Serial.print(az_m_s2); Serial.print("\t");
      Serial.print(gx_deg_s); Serial.print("\t");
      Serial.print(gy_deg_s); Serial.print("\t");
      Serial.println(gz_deg_s);    
    } else {
      Serial.println("Error al abrir el archivo");
    }
    delay(1000);

}