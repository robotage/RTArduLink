#RTArduLink - a simple way to connect one or more Arduinos to Raspberry Pis, PCs and Linux-based embedded systems

RTArduLink is a system for interfacing multiple subsystem processors (such as Arduino) with Raspberry Pis, PCs or 
embedded controllers via serial links (over USB or Zigbee for example).

The software has been tested using the Arduino 1.0.6 IDE.

See www.richards-tech.com for updates and news.

Check RTArduLink.pdf for details on how to use RTArduLink and the demo programs.

## Release history

### October 8 2014 - V1.1.1
Changed signature of host RTArduLinkStatus signal in RTArduLinkHost.h to use QString instead of char *. Modified demo apps to use new signature.

### October 6 2014 - V1.1.0
Changed message address field to be 16 bits. Changed processCustomMessage callback signature to be consistent on host and subsystem sides. Incorporated the echo function into the subsystem standard library. Combined the Arduino examples into one sketch.

### October 6 2014 - V1.0.1
Changed license to MIT. Fixed issues with tabs.

### October 5 2014 - V1.0.0
Initial release.
