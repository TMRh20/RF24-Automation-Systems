# RF24-Automation-Systems
A repository for the code used in RF24 Home Automation Systems
See https://tmrautomation.myshopify.com/

# Getting Started with the Home Automation Kit:
1. Connecting the radios:
  Scroll down at https://nrf24.github.io/RF24/ to see the pin connection diagram for connecting the radios
2. Connecting the devices:
  DHT22 Temp/Humidity sensor:
  RGB LED light:
  
# Connecting to the Raspberry Pi
1. The Raspberry Pi will require some configuration to connect to your wifi, or you can plug it in via a LAN cable.
2. For configuration, a USB keyboard is required. The default username is `pi` and the default password is `raspberry`
3. For Wifi, run `sudo raspi-config` from the command line, choose "System Options" and "Wireless LAN", enter your WiFi info
4. Once complete, you should be able to connect to your Raspberry Pi by directing your browser to http://RPi4:1880/ui
5. If that doesn't work, your router might not support hostnames. You can get the IP address of the RPi by running `ifconfig` from the command line: ![IpAddress](https://user-images.githubusercontent.com/2610460/186565512-7b64d693-6d2c-436c-b27a-18ea89091427.jpg)

6. Once the IP address has been identified, you can connect via http://your-IP-here:1880/ui
7. The RPi Node-Red interface will display the current temperature and humidity on a chart: ![Temp Humidity](https://user-images.githubusercontent.com/2610460/186566156-c216e617-96fa-4550-bf92-21a3446b6abe.jpg)

8. The RGB LED can be adjusted, both color and brightness by clicking on a color palette and moving a slider for brightness
![colorPallette](https://user-images.githubusercontent.com/2610460/186566487-856d1504-bb1c-401a-9613-204de499fc32.jpg)


