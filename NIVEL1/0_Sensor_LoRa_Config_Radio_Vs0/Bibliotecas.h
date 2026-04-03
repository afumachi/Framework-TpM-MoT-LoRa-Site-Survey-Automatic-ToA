
//=======================================================================
//                     1 - Bibliotecas
//=======================================================================

#include <SPI.h>
#include <LoRa.h>

//=======================================================================
//                     2 - Variáveis
//=======================================================================
// Identificação do Nó Sensor e Tamanho de Pacote

#define MY_ID 1 
#define TAMANHO_PACOTE 52
#define NUM_LEITURA_LDR 8
byte PacoteDL[TAMANHO_PACOTE];
byte PacoteUL[TAMANHO_PACOTE];

// Taxa de comunicação Serial/USB para Debug
#define TAXA_SERIAL 115200

// Identificação de Leitura do Comando do LED AMARELO
#define CMD_LED_AMARELO 44 // BYTE de Controle de Comandar/Ligar CMD_LED_AMARELO

// --- 2. Definição de Pinos (Hardware) ---
#define PIN_LED_VERMELHO 15 // Status ENVIO por RF
#define PIN_LED_AMARELO 2   // ATUADOR (Controlado remotamente)
#define PIN_LED_VERDE 4     // Status de RECEBIMENTO por RF
#define PIN_LDR 36          // Sensor (APP)
#define PIN_BOTAO 39        // Botão do Nó Sensor

// ---- DECLARAÇÃO DIAGRAMA DE PINOS DO PROJETO ----
// Pinos utilizados para comunicação SPI entre ESP32 e RFM95 - Módulo LoRa
#define SCK 5
#define MISO 19			
#define MOSI 27		

// Pinos do RFM95 - Módulo LoRa
#define SS 18
#define RST 14			
#define DIO0 26

// --- Configuração Rádio LoRa ---
#define FREQUENCY_IN_HZ 915E6    // Frequência do Canal LoRa (ex: 915MHz)
#define txPower 20               // Potência de Transmissão (dBm) [2 a 20 - padrão 14]
#define spreadingFactor 12       // Fator de Espalhamento - range de [6-12, padrão 7]
#define signalBandwidth 125E3    // Banda do Sinal [125E3 | 250E3 | 500E3]
#define codingRateDenominator 8  // Coding Rate (4/5) [4/6 | 4/7 | 4/8 | 4/5 |]
//#define loraCRC                // Habilita ou disabilita o uso CRC, por padrão o CRC não é usado.

// Váriáveis utilizadas no código

int contadorUL;
int contadorDL;
int RSSI_dBm_DL, RSSI_DL, LQI_DL;
int tipo, saltos, saltosTotal, dataInitAddress; // Variáveis utilizadas para o roteamento
float SNR_DL;
int lum;

// # Configuração Atual Rádio LoRa
int valor_atual_spreadingfactor = 12; // # Spreading Factor inicial = Maior espalhamento possível 12 (de 7 a 12)
int valor_atual_bandwidth = 125E3; // # Bandwidth inicial = 125kHz (1 = 125kHz | 2 = 250kHz | 3 = 500kHz)
int valor_atual_codingrate = 8; // # CodingRate Denominator = 5/4 (5/4 | 6/4 | 7/4 | 8/4)
int valor_atual_potencia_radio = 20; // # TX Power = 1 a 17???

// # Configuração Nova Rádio LoRa
int valor_novo_spreadingfactor = 12; // # Spreading Factor inicial = Maior espalhamento possível 12 (de 7 a 12)
int valor_novo_bandwidth = 125E3; // # Bandwidth inicial = 125kHz (1 = 125kHz | 2 = 250kHz | 3 = 500kHz)
int valor_novo_codingrate = 8; // # CodingRate Denominator = 5/4 (5/4 | 6/4 | 7/4 | 8/4)
int valor_novo_potencia_radio = 20; // # TX Power = 1 a 17???

// # Configuração Anterior Rádio LoRa
int valor_anterior_spreadingfactor = 12; // # Spreading Factor inicial = Maior espalhamento possível 12 (de 7 a 12)
int valor_anterior_bandwidth = 125E3; // # Bandwidth inicial = 125kHz (1 = 125kHz | 2 = 250kHz | 3 = 500kHz)
int valor_anterior_codingrate = 8; // # CodingRate Denominator = 5/4 (5/4 | 6/4 | 7/4 | 8/4)
int valor_anterior_potencia_radio = 20; // # TX Power = 1 a 17???
int recebe_comando_anterior_radio = 0; // # Comando de Downlink de mudança de configuração de rádio LoRa

int inicia_lora_site_survey = 0;
int confirma_novo_radio = 0;
int confirma_novo_radio_base = 0;
int confirma_novo_radio_sensor = 0;
int recebe_comando_nova_radio = 0; // # Comando de Downlink de mudança de configuração de rádio LoRa

uint primeiro_setup = 1; // Indica o Startup do Módulo pela primeira vez

int tempo_radio = 8;

unsigned long lastPacketMillis = 0; 
unsigned long lastPacketTime = 0; // Timestamp local do último pacote recebido
int lostPacketCounter = 0;        // Contador de falhas
bool communicationLost = false;


  // adicionar um conjunto de variáveis PKT_UL e PKT_DL para deixar os pacotes independentes

  // --- Physical Layer ---
#define RSSI_DOWNLINK 0
#define LQI_DOWNLINK  1
#define RSSI_UPLINK   2
#define LQI_UPLINK    3

  // --- MAC Layer ---
#define MAC_COUNTER_MSB 4 
#define MAC_COUNTER_LSB 5
#define MAC3 6
#define MAC4 7

  // --- Network Layer ---
#define  RECEIVER_ID     8
#define  NET2            9
#define  TRANSMITTER_ID  10
#define  NET4            11

  // --- Transport Layer ---
#define DL_COUNTER_MSB 12
#define DL_COUNTER_LSB 13
#define UL_COUNTER_MSB 14
#define UL_COUNTER_LSB 15


/*

enum bytes_do_pacote{

  // adicionar um conjunto de variáveis PKT_UL e PKT_DL para deixar os pacotes independentes

  // --- Physical Layer ---
  RSSI_DOWNLINK   = 0,
  LQI_DOWNLINK    = 1,
  RSSI_UPLINK     = 2,
  LQI_UPLINK      = 3,

  // --- MAC Layer ---
  MAC_COUNTER_MSB = 4, 
  MAC_COUNTER_LSB = 5,
  MAC3 = 6,
  MAC4 = 7,

  // --- Network Layer ---
  RECEIVER_ID     = 8,
  NET2            = 9,
  TRANSMITTER_ID  = 10,
  NET4            = 11,

  // --- Transport Layer ---
  DL_COUNTER_MSB = 12,
  DL_COUNTER_LSB = 13,
  UL_COUNTER_MSB = 14,
  UL_COUNTER_LSB = 15,

  // --- Application Layer ---
  APP1 = 16,  // Tipo de sensor - no caso da PK-LoRa é um LDR
  APP2 = 17,  // Valor inteiro da luminosidade da conta de divisão por 256
  APP3 = 18,  // Valor de resto da conta de divisão por 256
  APP4 = 19,
  APP5 = 20,
  APP6 = 21,
  APP7 = 22,
  APP8 = 23,
  APP9 = 24,
  APP10 = 25,
  APP11 = 26,
  APP12 = 27,
  APP13 = 28, 
  APP14 = 29,
  APP15 = 30,
  APP16 = 31,
  APP17 = 32,
  APP18 = 33,
  APP19 = 34,
  APP20 = 35,
  APP21 = 36,
  APP22 = 37,
  APP23 = 38,
  APP24 = 39,
  APP25 = 40,
  APP26 = 41,
  APP27 = 42,
  APP28 = 43,
  APP29 = 44,
  APP30 = 45,
  APP31 = 46,
  APP32 = 47,
  APP33 = 48,
  APP34 = 49,
  APP35 = 50,
  APP36 = 51,
};

*/
