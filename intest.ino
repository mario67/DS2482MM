// richiesta temperatura e address da tutti i canali di due istanze 

#include <DS2482MM.h>

#include <DallasTempI2C.h>
#include <Wire.h>

DS2482MM DS2482MMA(0);
DS2482MM DS2482MMB(1);

DallasTempI2C sensorsA(&DS2482MMA);
DallasTempI2C sensorsB(&DS2482MMB);

DeviceAddress AddressA;
DeviceAddress AddressB;


void setup(void)
{

  Serial.begin(9600);
  Serial.println("Prova doppia istanza I2C to DS2482MM");
  
}


void loop(void)
{
  
  for (int i=0; i<8; i++){
    DS2482MMA.channel_select(i);
    sensorsA.begin();
  sensorsA.getAddress(AddressA, 0);
  Serial.print(" Richiesta Temperature...");
  sensorsA.requestTemperatures(); 
  Serial.println("OK");

  Serial.print("Temperatura per l'istanza A: ");
  Serial.print(sensorsA.getTempCByIndex(0)); 
  Serial.print("  Adr: ");
  for (byte J = 0; J < 8; J ++) {
  Serial.print(AddressA[J], HEX);
  Serial.print(" "); }
  }
  for (int i=0; i<8; i++){
    DS2482MMB.channel_select(i);
  sensorsB.begin();
  sensorsB.getAddress(AddressB, 0);
  Serial.print(" Richiesta Temperature...");
  sensorsB.requestTemperatures(); 
  Serial.println("OK");

  Serial.print("Temperatura per l'istanza B: ");
  Serial.print(sensorsB.getTempCByIndex(0)); 
  Serial.print("  Adr: ");
  for (byte J = 0; J < 8; J ++) {
  Serial.print(AddressB[J], HEX);
  Serial.print(" "); }
  }

}

