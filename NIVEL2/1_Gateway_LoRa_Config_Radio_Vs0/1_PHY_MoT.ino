//==================================================================================================================
//======================= PACOTE DOWN LINK - PACOTE VINDO DO PYTHON PARA SER ENVIADO AO NÓ SENSOR ===========================
//==================================================================================================================
// Pacote proveniente do Python pela serial deve ser enviado para o nó sensor. Primeiro o pacote é recebido pela serial USB e depois é encaminhado para o RF95
// ----------------------LEITURA DO BUFFER RX DA SERIAL PACOTE VINDO DA MAC DO PYTHON PARA DL----------------------------------------------------------------------------------------------------//

void Phy_serial_receive_DL() {  // Funcao de recepcao de pacote da Camada Física


  // Parâmetros do LoRa caso primeira energização do módulo NodeMCU/ESP32
  if (primeiro_setup == true){
    LoRa.sleep();
    LoRa.setTxPower(txPower);
    LoRa.setSpreadingFactor(spreadingFactor);
    LoRa.setSignalBandwidth(signalBandwidth);
    LoRa.setCodingRate4(codingRateDenominator);
    LoRa.idle(); // Retorna ao modo standby/recepção
      // Coloca o rádio em modo de escuta
    primeiro_setup = false;
  }


  //===================== RECEPCAO DO PACOTE DL
  if (Serial.available() >= TAMANHO_PACOTE) {  // Testa se tem 52 bytes na serial

    for (byte i = 0; i < TAMANHO_PACOTE; i++)  // PacoteUL[#] é preenchido com zero e PacoteDL[#] recebe os bytes do buffer
    {
      PacoteDL[i] = Serial.read();  // Zera o pacote de transmissão
    }

    
    // ADICIONADO Variáveis de recebimento do valores de rádio LoRa
    valor_novo_spreadingfactor = PacoteDL[0]; // Byte DL[0] valor de rádio LoRa de Spreading Spectrum
    valor_novo_bandwidth = PacoteDL[1]; // Byte DL[1] valor de rádio LoRa de Bandwidth

    // Configura Valor de Bandwidth de acordo com o valor recebido no Byte[1]
    if (valor_novo_bandwidth == 3){
      valor_novo_bandwidth = 500E3;
    }
    else if (valor_novo_bandwidth == 2){
      valor_novo_bandwidth = 250E3;
    }
    else if (valor_novo_bandwidth == 1){
      valor_novo_bandwidth = 125E3;
    }

    valor_novo_codingrate = PacoteDL[2]; // Byte DL[2] valor de rádio LoRa de CodingRate
    valor_novo_potencia_radio = PacoteDL[3]; // Byte DL[3] valor de rádio LoRa de Potência de Rádio LoRa
    tempo_radio = PacoteDL[MAC3]; // Byte DL[6] Recebe tempo de radio tx rx
    recebe_comando_nova_radio = PacoteDL[MAC4]; // Byte DL[7] Recebe comando de reconfiguração de Rádio LoRa

    // ----------------------ENVIO DO PACOTE DE DOWN LINK ATRAVÉS DO RF95----------------------------------------------------------------------------------------------------//
    Phy_radio_send_DL();  // chama a funcao de recepcao da camada de controle de acesso ao meio
  }
}

//========================= ENVIA PACOTE DL PARA NÓ SENSOR ATRAVÉS DO RF95
//O pacote DL recebido pela serial proveniente do Nível 3 é enviado para o RF95
void Phy_radio_send_DL() {

  // Pisca o LED de transmissão de pacote DL
  digitalWrite(PIN_LED_VERMELHO, HIGH);// Início da Transmissão

  LoRa.beginPacket();                   // start packet
  for (int i = 0; i < TAMANHO_PACOTE; i++) {
    LoRa.write(PacoteDL[i]);              // add data to packet
  }
  LoRa.endPacket();                     // finish packet and send it

    // Pisca o LED de transmissão de pacote DL
  digitalWrite(PIN_LED_VERMELHO, LOW); // FIM da Transmissão

}

//==================================================================================================================
//======================= PACOTE UL LINK - PACODE VINDO NÓ SENSOR ENCAMINHADO PARA PYTHON===========================
//==================================================================================================================
// Pacote que chega no RF95 vindo do nó sensor e é passado para o buffer de TX da serial
//--------------------------- RECEBE PACOTE UL VINDO DO NÓ SENSOR ATRAVÉS DO MÓDULO RF95

void Phy_radio_receive_UL() {

  // Escuta o Rádio LoRa se identificou algum Pacote
  uint8_t packetSize = LoRa.parsePacket();

  // Caso positivo, identifica o tamanho do Payload do Pacote  
  if (packetSize) {

    // Realiza a leitura caso Payload do Pacote seja compatível com o Pacote de 52 Bytes    
    if (packetSize >= TAMANHO_PACOTE) {

      digitalWrite(PIN_LED_VERDE, HIGH); // Liga Led Verde Indicando Inicio da leitura do Pacote
      for (int i = 0; i < TAMANHO_PACOTE; i++) {
        PacoteUL[i] = LoRa.read();
      }
      
      RSSI_dBm_UL = LoRa.packetRssi();
      SNR_UL = LoRa.packetSnr();

      digitalWrite(PIN_LED_VERDE, LOW); // Fim da leitura do Pacote
//===================================== IMPORTANTE - OPÇÃO VERIFICAÇÃO DE ENDEREÇO OU MODO PROMÍSCUO========================
// Quando recebe o pacote a base pode verificar o endereço de destino ou trabalhar em modo promíscuo.
//===================== QUANDO A BASE  VERIFICA O ENDENREÇO DE DESTINO O PACOTE SÓ É ENVIADO PARA A SERIAL CASO A BASE SEJA O DESTINATÁRIO - nesse caso descomentar o bloco abaixo
// Esta é uma função originalmente da camada de rede, mas existe um cross-layer para verificação do endereço de destino, recebendo somente os pacotes que são destinados para a base

      if (PacoteUL[RECEIVER_ID] == MY_ID) {
        // Garante que Nó Sensor também recebeu comando de alteração de rádio e confirmou
        confirma_novo_radio_sensor = PacoteUL[MAC4]; // PacoteUL[7] recebe confirmação do nó sensor do recebimento
        Mac_radio_receive_DL();
        //Phy_serial_send_UL();  //Chama a função de envio da Camada Física
      }


    }
  }
}

//===================== ESCREVE NA SERIAL PACOTE UL
void Phy_serial_send_UL() { // Funcao de envio de pacote de UL para o computador via buffer TX da serial do NodeMCU
//--- Bloco que faz adequação da leitura de RSSI para um byte ---
  if(RSSI_dBm_UL > -10.5)  // Caso a RSSI medida esteja acima do valor superior -10,5 dBm
  {
   RSSI_UL = 127; // equivalente a -10,5 dBm 
  }

  if(RSSI_dBm_UL <= -10.5 && RSSI_dBm_UL >= -74) // Caso a RSSI medida esteja no intervalo [-10,5 dBm e -74 dBm]
  {
   RSSI_UL = ((RSSI_dBm_UL +74)*2) ;
  }

  if(RSSI_dBm_UL < -74) // Caso a RSSI medida esteja no intervalo ]-74 dBm e -138 dBm]
  {
   RSSI_UL = (((RSSI_dBm_UL +74)*2)+256) ;
  }

  // =================Informações de gerência do pacote
  PacoteUL[2] = RSSI_UL;  // aloca RSSI_UL
  PacoteUL[3] = (byte)SNR_UL%256;


  if (confirma_novo_radio == 1){
    AplicarConfiguracoesRadio();
  }

  // Transmissão do pacote pela serial do Arduino
  for (int i = 0; i < TAMANHO_PACOTE; i++) {
    Serial.write(PacoteUL[i]);
  }
}


void AplicarConfiguracoesRadio() {

  if (confirma_novo_radio == 1){
    primeiro_setup = 0;
    LoRa.sleep(); // Coloca em sleep para garantir a mudança de parâmetros
    LoRa.setTxPower(valor_novo_potencia_radio);                       // Potência de Transmissão (Configurado em bibliotecas.h)
    LoRa.setSpreadingFactor(valor_novo_spreadingfactor);       // Fator de Espalhamento  (Configurado em bibliotecas.h)
    LoRa.setSignalBandwidth(valor_novo_bandwidth);       // Banda do Sinal (Configurado em bibliotecas.h)
    LoRa.setCodingRate4(valor_novo_codingrate);     // Coding Rate  (Configurado em bibliotecas.h)
    LoRa.idle(); // Retorna ao modo standby/recepção
    //LoRa.receive();

    confirma_novo_radio_sensor = 0;
    confirma_novo_radio_base = 0;
    confirma_novo_radio = 0;
    recebe_comando_nova_radio = 0;
    primeiro_setup = 0;
    
  }  

}


