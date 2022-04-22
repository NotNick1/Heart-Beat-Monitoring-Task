#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

//HRM stands for Heart Rate Monitor
uint8_t heart_beat[2];
uint8_t hrm_flags = 0b00000000;
uint8_t heart_rate = 0;
int check_status = 0; // variable used for checking the connection status

BLECharacteristic HRMCharacteristic (BLEUUID((uint16_t)0x2A37), BLECharacteristic::PROPERTY_READ); // defining characteristic by using Heart Rate Measurement UUID
                                                                                                   // setting the property to allow the client to read


class CheckConnection: public BLEServerCallbacks  // check if any devices are connected
{
  void onConnect(BLEServer *HRMServer)
  {
    check_status = 1;
  }

  void onDisconnect(BLEServer *HRMServer)
  {
    check_status = 0;
  }
};



void setup() 
{
  Serial.begin(9600);

  BLEDevice::init("Heart rate monitor"); // set device name

  BLEServer *HRMServer = BLEDevice::createServer();
  HRMServer->setCallbacks(new CheckConnection()); // set the function that handles callbacks
  
  BLEService *HRMService = HRMServer->createService(BLEUUID((uint16_t)0x180D)); // creating service by using Heart Rate Service UUID
  HRMService->addCharacteristic(&HRMCharacteristic); // adding the already defined characteristic
  HRMService->start();
  
  BLEAdvertising *HRMAdvertise = BLEDevice::getAdvertising();
  HRMAdvertise->addServiceUUID(BLEUUID((uint16_t)0x180D));
  HRMAdvertise->setScanResponse(true);
  BLEDevice::startAdvertising(); //start advertising
  Serial.println("Started Advertising...");
}

void loop() 
{
  if(check_status == 1)
  {
    heart_rate = random(60, 100); // choose a random number between the normal bpm limits of a resting person
  
    Serial.println(heart_rate); // print BPM value in serial monitor to check data accuracy
    
    heart_beat[0] = hrm_flags;
    heart_beat[1] = heart_rate;
  
    HRMCharacteristic.setValue((uint8_t*)&heart_beat, 2); // modify characteristic with the values allocated above
  
    delay(10000);
  }

  else if(check_status == 0)
  {
    Serial.println("Client not connected"); // if no device is connected print this
    delay(2000);
  }
}
