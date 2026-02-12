# Test module for RS-232.  
## Made it for Testing RS-232 ports on a Cellular data modem.  

Simple unit that shows temperature and a 0-5 volt controled by pot. The system can be polled by a modbus master or 'a' sent from a terminal program will give a response.  
It works on either without a reboot.  

![20260212_115151](https://github.com/user-attachments/assets/313af8c7-87f3-4519-999d-bae7d3c4647c)  

![20260212_133540](https://github.com/user-attachments/assets/c322b7e8-51cf-44be-868d-3278a505dfe6)

structure:
- project
- Arduino uno/Nano
- RS-232 module
- DS18B20
- 10k Pot
- schematics
-See sketch
- docs
 	See Sketch

#### What?

MSA 100  
Baud	9600  
Format	8N1  
There are a few static registers to best find the proper format for the modbus RTU floats.  

<img width="202" height="199" alt="Terminal Screenshot" src="https://github.com/user-attachments/assets/63485d59-342f-428e-b9fb-1e6e13a01f50" />

#### Why?
Needed something easy to test cellular data modems.

#### [How?](docs/HOW.md)
Wrote stuff

--


