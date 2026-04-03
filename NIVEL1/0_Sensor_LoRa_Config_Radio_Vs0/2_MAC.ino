//================ RECEBE O PACOTE DA CAMADA FÍSICA ========
void Mac_radio_receive_DL() { 
  // Aqui pode ser adicionado o Sleep Mode

  //
  tempo_radio = PacoteDL[MAC3]; // Byte DL[6] Recebe tempo de radio tx rx
  recebe_comando_nova_radio = PacoteDL[MAC4]; // Byte DL[7] Recebe comando de reconfiguração de Rádio LoRa
  // Primeiro ciclo
    
    if (recebe_comando_nova_radio == 1){
      confirma_novo_radio_sensor = 1;
    }
    if ((recebe_comando_nova_radio == 2) & ((valor_novo_spreadingfactor != valor_atual_spreadingfactor) || (valor_novo_bandwidth != valor_atual_bandwidth) || (valor_novo_codingrate != valor_atual_codingrate) || (valor_novo_potencia_radio != valor_atual_potencia_radio))){
        confirma_novo_radio_sensor = 2;
        
        valor_anterior_spreadingfactor = valor_atual_spreadingfactor;
        valor_anterior_bandwidth = valor_atual_bandwidth;
        valor_anterior_codingrate = valor_atual_codingrate;
        valor_anterior_potencia_radio = valor_atual_potencia_radio;

        valor_atual_spreadingfactor = valor_novo_spreadingfactor;
        valor_atual_bandwidth = valor_novo_bandwidth;
        valor_atual_codingrate = valor_novo_codingrate;
        valor_atual_potencia_radio = valor_novo_potencia_radio;
    }
    if (recebe_comando_nova_radio == 3){
      confirma_novo_radio_sensor = 3;
    }

    if (recebe_comando_nova_radio > 0){
    // Para debug durante desenvolvimento do firmware
      Serial.print("Confirma Nova Configuração: ");
      Serial.println(confirma_novo_radio_sensor);
    }
    
    Net_radio_receive_DL();

}

//================ ENVIA O PACOTE À CAMADA FÍSICA ========
void Mac_radio_send_UL() {
  // Aqui pode ser adicionado o Sleep Mode


  // Caso Nó Sensor Devices recebeu comando de alteração de config. de rádio escreve no Byte[11] para a Base LoRa
  // confirmação do primeiro ciclo  
  if (confirma_novo_radio_sensor == 1){
    PacoteUL[MAC4] = 1;
    Serial.print("PacoteUL[7] =: ");
    Serial.println(confirma_novo_radio_sensor);
    confirma_novo_radio = 0;
  }
  else if (confirma_novo_radio_sensor == 2){
    // Confirmação do segundo ciclo para alteração das config. de rádio do Nó Sensor
    PacoteUL[MAC4] = 2;
    Serial.print("PacoteUL[7] =: ");
    Serial.println(confirma_novo_radio_sensor);
    confirma_novo_radio = 1; // Habilita Nó Sensor a alterar as configurações de Rádio
  }
  else if (confirma_novo_radio_sensor == 3){
    //  Confirmação do terceiro ciclo confirmando a alteração das config. de rádio do Nó Sensor
    PacoteUL[MAC4] = 3;
    Serial.print("PacoteUL[7] =: ");
    Serial.println(confirma_novo_radio_sensor);
    recebe_comando_nova_radio = 0;
    confirma_novo_radio = 0;
    confirma_novo_radio_sensor = 0;
  }
  else {
    // Sem necessidade de alteração, ou confirmação de alteração
    PacoteUL[MAC4] = 0;
    recebe_comando_nova_radio = 0;
    confirma_novo_radio = 0;
    confirma_novo_radio_sensor = 0;
    Serial.print("PacoteUL[7] =: ");
    Serial.println(confirma_novo_radio_sensor);
  }


  Phy_radio_send_UL();
}
