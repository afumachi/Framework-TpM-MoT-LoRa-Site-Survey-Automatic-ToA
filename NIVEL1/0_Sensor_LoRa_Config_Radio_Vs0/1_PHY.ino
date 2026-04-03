void Phy_radio_receive_DL() {

  
  // Parâmetros do LoRa caso primeira energização do módulo NodeMCU/ESP32
  if (primeiro_setup == 1){
    Serial.println("Primeiro SETUP");
    LoRa.sleep();
    LoRa.setTxPower(txPower);
    LoRa.setSpreadingFactor(spreadingFactor);
    LoRa.setSignalBandwidth(signalBandwidth);
    LoRa.setCodingRate4(codingRateDenominator);
    LoRa.idle(); // Retorna ao modo standby/recepção
    primeiro_setup = 0;
  }


  // Escuta o Rádio LoRa se identificou algum Pacote
  uint8_t packetSize = LoRa.parsePacket();

  // Caso positivo, identifica o tamanho do Payload do Pacote
  if (packetSize) {
    Serial.println("Pacote DL IDENTIFICADO");
    digitalWrite(PIN_LED_VERDE, HIGH); // Liga Led Verde Indicando Inicio da leitura do Pacote

    // Realiza a leitura caso Payload do Pacote seja compatível com o Pacote de 6 Bytes
    if (packetSize >= TAMANHO_PACOTE) {
      Serial.println("Pacote DL >= TAMANHO_PACOTE");

      for (int i = 0; i < TAMANHO_PACOTE; i++) {
        PacoteDL[i] = LoRa.read();  // Aloca no Pacote de DL os 6 bytes que vieram do RFM95
      }

      RSSI_dBm_DL = LoRa.packetRssi();
      SNR_DL = LoRa.packetSnr();
      digitalWrite(PIN_LED_VERDE, LOW); // Fim da leitura do Pacote

       
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
      //tempo_radio = PacoteDL[MAC3]; // Byte DL[6] Recebe tempo de radio tx rx
      //recebe_comando_nova_radio = PacoteDL[MAC4]; // Byte DL[7] Recebe comando de reconfiguração de Rádio LoRa



      // Caso Pacote direcionado a este sensor, chama função MAC
      Mac_radio_receive_DL();

    }
  }
}  

//================ ENVIA O PACOTE UL ========
void Phy_radio_send_UL() {

  //--- Bloco que faz adequação da leitura de RSSI para um byte ---

  if(RSSI_dBm_DL > -10.5)  // Caso a RSSI medida esteja acima do valor superior -10,5 dBm
  {
   RSSI_DL = 127; // equivalente a -10,5 dBm 
  }

  if(RSSI_dBm_DL <= -10.5 && RSSI_dBm_DL >= -74) // Caso a RSSI medida esteja no intervalo [-10,5 dBm e -74 dBm]
  {
   RSSI_DL = ((RSSI_dBm_DL +74)*2) ;
  }

  if(RSSI_dBm_DL < -74) // Caso a RSSI medida esteja no intervalo ]-74 dBm e -138 dBm]
  {
   RSSI_DL = (((RSSI_dBm_DL +74)*2)+256) ;
  }

  // =================Informações de gerência do pacote Início da montagem do pacote de UL
  PacoteUL[0] = RSSI_DL;
  PacoteUL[1] = (byte)SNR_DL%256;

  Serial.println("LoRa UPLINK enable.");

  // Pisca o LED de transmissão de pacote UL
  digitalWrite(PIN_LED_VERMELHO, HIGH); // Início da Transmissão

  LoRa.beginPacket();                 // Inicia o envio do pacote ao rádio
  for (int i = 0; i < TAMANHO_PACOTE; i++) {
    LoRa.write(PacoteUL[i]);          // Envia byte a byte as informações para o Rádio
  }
  LoRa.endPacket();                   // Finaliza o envio do pacote


  // 5. Coloca o rádio em modo de escuta novamente
  LoRa.receive();    
  LoRa.idle();


  digitalWrite(PIN_LED_VERMELHO, LOW); // Fim da Transmissão

  // Realiza a alteração das config. da Rádio LoRa apenas após o envio do segundo Pacote UL, 
  // e prepara o Nó Sensor para o terceiro ciclo já com as alterações realizadas
  if (confirma_novo_radio == 1){
    AplicarConfiguracoesRadio();
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

      // Imprime na Serial para DEBUG
      Serial.print("Valor SF: ");
      Serial.println(valor_novo_spreadingfactor);
      Serial.print("Valor BW: ");
      Serial.println(valor_novo_bandwidth);
      Serial.print("Valor CR: ");
      Serial.println(valor_novo_codingrate);
      Serial.print("Valor novo_potencia_radio TX: ");
      Serial.println(valor_novo_potencia_radio);
      Serial.print("Valor Tempo Potencia Radio: ");
      Serial.println(tempo_radio);
      Serial.print("recebe_comando_nova_radio: ");
      Serial.println(recebe_comando_nova_radio);

    confirma_novo_radio_sensor = 0;
    confirma_novo_radio_base = 0;
    confirma_novo_radio = 0;
    recebe_comando_nova_radio = 0;
    primeiro_setup = 0;

    Serial.println("Configurações de rádio atualizadas.");

  }  

}
