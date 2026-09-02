#include <Arduino.h>
#include "State.h"
#include "config.h"
#include "Peripherals.h"
#include <WiFi.h>
#include "Network.h"
#include "Telemetry.h"
#include "model.h"





void setup()
{
    //initialize serial monitor
    Serial.begin(115200);
    dht.begin();  
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
        //publish the data
        publishTelemetry();
    }
    plug_status();
    update_led_status();
    
}

