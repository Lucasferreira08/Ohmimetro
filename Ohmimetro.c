 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "pico/stdlib.h"
 #include "hardware/adc.h"
 #include "hardware/i2c.h"
 #include "ssd1306.h"
  #include "font.h"
 #include "init_config.h"
#include "cod_cores.h"

 int R_conhecido = 10000;   // Resistor de 10k ohm
 float R_x = 0.0;           // Resistor desconhecido
 float ADC_VREF = 3.31;     // Tensão de referência do ADC
 int ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)
 
 int main()
 {
  
  ssd1306_t ssd;
  
  display_init(&ssd);
  buttons_config();
  adc_config();
 
   float tensao;
   char str_x[10]; // Buffer para armazenar a string
   char str_y[10]; // Buffer para armazenar a string

  char comercial_str[10]; // Buffer para o valor comercial E24
 
   while (true)
   {
     adc_select_input(2); // Seleciona o ADC para eixo X. O pino 28 como entrada analógica
 
     float soma = 0.0f;
     for (int i = 0; i < 500; i++)
     {
       soma += adc_read();
       sleep_ms(1);
     }
     float media = soma / 500.0f;

     if ((ADC_RESOLUTION - media)!=0) {
      R_x = (R_conhecido * media) / (ADC_RESOLUTION - media);
     } else {
      R_x=0.0;
     }

     float valor_comercial = encontrar_valor_e24(R_x);
        
     int cor1, cor2, multiplicador;
     calcular_codigo_cores(valor_comercial, &cor1, &cor2, &multiplicador);
 
     sprintf(str_x, "%.2f", media); // Converte em string
     sprintf(str_y, "%.2f", R_x);   // Converte float em string
     sprintf(comercial_str, "%.2f", valor_comercial);

    if (R_x < 510 || R_x > 100000) 
    {
      desenhar_erro(&ssd, str_y, str_x);
      
      sleep_ms(1000);
      continue; // Volta ao início do loop
    }

     if (estado_botao()==true)
     {
      desenhar_resistor(&ssd, cores_nomes[cor1], cores_nomes[cor2], cores_nomes[multiplicador]);
     }
     else 
     {
      desenhar_display(&ssd, str_y, comercial_str, str_x);
     }

     sleep_ms(700);
   }
 }