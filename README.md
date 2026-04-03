Electrical Engineering University of Campinas - FEE - UNICAMP

https://www.wisstek-iot.cc/

Extension Course - FEE-247 FEE-247 (DEVELOPMENT OF IOT SOLUTIONS WITH LORA AND LORAWAN)

This Repository is an IoT Solution using Three-Phase Methodology for a LoRa Site Survey (LSS) with
Automatic ToA (LoRa Time on Air) calculation. Also is possible to runs the LSS with Manual ToA.
Composed in base of ESP32 core & LoRa radio RFM95 The End Device LoRa is composed in addtional of Sensor LDR

This repository is composed of 6 Levels of an IoT Solution developed through Three-Phase Methodology.

Running in Levels 3 to 6 in Python - Three-Phase Methodology

The IoT Operator of the LSS Tool, must enter the LoRa radio parameters, as well as the number of LSS Packets (Wisstek MoT based).
And the time between Downlinks [Edge/LoRa Gateway => LoRa End Device] and Uplinks MoT Packets [LoRa End Device => LoRa Gateway/Edge].

LoRa Radio Configuration:
  Spreading Factor  => from 7 to 12
  Bandwith          => 125 | 250 | 500 kHz
  CodingRate        => 5 to 8 <=> { 4/5 | 4/6 | 4/7 | 4/8 }
  TX Power          => from 2 to 20 dBm according to RFM95 radio module

Level3 and Level6 with Automatic ToA (LoRa Time on Air) calculation - IoT Operator inserts only the LoRa Radio Configuration.
The calculation of LoRa ToA is done in on Level3 (Nivel3-ToA-auto.py) according to parameters from LoRa Radio selected.


<img width="1366" height="728" alt="Captura de tela 2026-04-03 190112" src="https://github.com/user-attachments/assets/7159464b-7b3d-4294-852b-f453072c74b0" />



Level3 and Level6 with Manual ToA (LoRa Time on Air) - IoT Operator inserts also the LoRa Time Radio manually.


<img width="1366" height="728" alt="Captura de tela 2026-04-03 185406" src="https://github.com/user-attachments/assets/a41d54b6-2964-47b5-a067-0a01d448ca54" />
