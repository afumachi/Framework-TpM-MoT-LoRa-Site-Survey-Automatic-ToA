/*
  MoT LoRa Site Survey Versão Zero | WissTek IoT
  Última versão: Branquinho / Felipe / Anderson
  Hardware: PKLoRa ESP32
*/

//=======================================================================
//                     1 - Bibliotecas
//=======================================================================
#include "Bibliotecas.h"  // Arquivo contendo declaração de bibliotecas e variáveis

//=======================================================================
// ------- 3 - Setup de inicialização ---------
//=======================================================================
// Inicializa as camadas
void setup() {
  //================= INICIALIZA SERIAL E MÓDULO RF95

  Serial.begin(115200);
  // Aguarda para estabilização da Serial
  delay(200);

  // declara Leds como saídas digital do ESP32
  pinMode(PIN_LED_VERMELHO, OUTPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);

  // --- Inicialização da Comunicação SPI entre o ESP32 e o Módulo LoRa RFM95 ---
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setSPI(SPI);

  // --- Inicialização da Comunicação LoRa em 915Mhz---
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(FREQUENCY_IN_HZ)) {
    Serial.println("[Nó Sensor] Falha ao iniciar LoRa. Verifique conexões.");
    while (true); // Trava se o LoRa falhar
  }

  //  --- Atua Led vermelho  --- 
  digitalWrite(PIN_LED_VERMELHO, HIGH); // LIGA LED VERMELHO - INDIFERENTE PARA O BOOT

  //  --- Atua Led verde  --- 
  digitalWrite(PIN_LED_VERDE, LOW);  // DESLIGA O LED VERDE - DEVE SER LOW DURANTE BOOT

  // Aguarda 1 segundo para estabilização
  delay(1000);

  //  --- Atua Led vermelho  --- 
  digitalWrite(PIN_LED_VERMELHO, LOW); // DESLIGA LED VERMELHO

  //  --- Atua Led verde  --- 
  digitalWrite(PIN_LED_VERDE, LOW);  // DESLIGA O LED VERDE

  #ifdef loraCRC   // Habilitação do CRC do chip lora  (Configurado em bibliotecas.h)
    LoRa.enableCrc();
  #endif

} // FIM DO SETUP


//=======================================================================
//                     4 - Loop de repetição
//=======================================================================
// A função loop irá executar repetidamente
void loop() {
  Phy_serial_receive_DL();
  Phy_radio_receive_UL();
}
