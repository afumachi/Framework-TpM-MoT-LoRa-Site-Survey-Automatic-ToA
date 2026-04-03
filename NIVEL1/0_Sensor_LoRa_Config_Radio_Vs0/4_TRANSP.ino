//================ RECEBE O PACOTE DE DL DA CAMADA DE REDE ========
void Transp_radio_receive_DL() { 

  //neste ponto pode ser implementado um controle relacionado ao recebimento não sequencial de pacotes de DL
  
  App_radio_receive_DL();
}


//================ ENVIA O PACOTE DE UL À CAMADA DE REDE ========
void Transp_radio_send_UL() { 

  contadorUL = contadorUL + 1;  // Incrementa o contador de pacote de UL
  
  PacoteUL[UL_COUNTER_MSB] =(byte)contadorUL/256;
  PacoteUL[UL_COUNTER_LSB] = (byte)contadorUL%256;
  // neste ponto pode ser implementado um controle relacionado ao recebimento não sequencial de pacotes de DL
  
  Net_radio_send_UL();
}
