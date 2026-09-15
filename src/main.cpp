#include <Arduino.h>
#include "State.h"
#include "config.h"
#include "Peripherals.h"
#include <WiFi.h>
#include "Network.h"
#include "Telemetry.h"
#include "model.h"
#include "edge_ai.h"
#include "optimization.h"
#include "rpc.h"
#include "attributes.h"





void setup()
{
    //initialize serial monitor
    Serial.begin(115200);
    dht.begin();  
    // config esp32 with real time
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    //config periperal pins
    pinMode(BTN_PLUGIN, INPUT_PULLUP);
    pinMode(BTN_PLUGOUT, INPUT_PULLUP);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    //connect board to WiFi 
    connectWiFi();

    
    // Configure MQTT server
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);//mqqt server addr of things and port number
    //set call back function upon reciving data from the cloud
    mqtt.setCallback(mqttCallback);
    mqtt.setBufferSize(512); 

    //connect board to cloud
    connectMQTT(); //TOKEN, device id 
}

unsigned long now;
unsigned long last_print;

void loop()
{
    //print data every 5 sec
    now = millis();
    if((now - last_print) > 5000)
    {
        last_print = now;
        // read data from sensor // voltage, current, temperature, power, bay status
        sample_sensor();
        //run AI to get prediction
        runEdgeAIInference();
        //decide load based on the prediction
        if(manualOverrideActive == 0){
            runOptimization();
        }
        
        //publish the data
        publishTelemetry();
        
    }
    plug_status();
    updateLeds();
    
}

