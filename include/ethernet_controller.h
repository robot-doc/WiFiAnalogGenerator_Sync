#ifndef ETHERNET_CONTROLLER_H
#define ETHERNET_CONTROLLER_H

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <FS.h>
#include <ArduinoJson.h>

class EthernetController
{
private:
    // Network configuration
    uint8_t mac[6];
    IPAddress ip;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns;

    // Hardware pins
    int cs_pin;
    int rst_pin;

    // State tracking
    bool initialized;

    // Configuration management
    bool loadConfig();
    bool saveConfig();

public:
    EthernetController();
    ~EthernetController();

    // Initialization and loop
    bool begin(int cs_pin, int rst_pin);
    void loop();

    // Configuration getters
    IPAddress getIP() const { return ip; }
    IPAddress getGateway() const { return gateway; }
    IPAddress getSubnet() const { return subnet; }
    IPAddress getDns() const { return dns; }

    // Status functions
    bool isConnected() const;
    String getStatusJSON();

    // Configuration methods
    bool updateConfig(IPAddress newIp, IPAddress newGateway, IPAddress newSubnet, IPAddress newDns);
};

extern EthernetController ethernetController;

#endif // ETHERNET_CONTROLLER_H