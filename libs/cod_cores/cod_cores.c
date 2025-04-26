#include "cod_cores.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include <math.h>

// Função para encontrar o valor E24 mais próximo da resistência medida
float encontrar_valor_e24(float resistencia) {
    // Determina a magnitude (potência de 10)
    float magnitude = 1.0;
    float valor_normalizado = resistencia;
    
    // Normaliza para um valor entre 1.0 e 10.0
    while (valor_normalizado >= 10.0) {
        valor_normalizado /= 10.0;
        magnitude *= 10.0;
    }
    while (valor_normalizado < 1.0) {
        valor_normalizado *= 10.0;
        magnitude /= 10.0;
    }
    
    // Procura o valor E24 mais próximo
    float menor_diferenca = 100.0;
    float valor_mais_proximo = E24[0];
    
    for (int i = 0; i < 25; i++) {      // 24
        float diferenca = fabs(valor_normalizado - E24[i]);
        if (diferenca < menor_diferenca) {
            menor_diferenca = diferenca;
            valor_mais_proximo = E24[i];
        }
    }
    
    // Retorna o valor E24 com a magnitude correta
    return valor_mais_proximo * magnitude;
  }
  
  // Função para calcular os códigos de cores para um resistor
  void calcular_codigo_cores(float resistencia, int *cor1, int *cor2, int *multiplicador) {
    // Normaliza para o padrão de cores (1-999 ohms)
    float valor = resistencia;
    *multiplicador = 0;
    
    // Ajusta o multiplicador (potência de 10)
    while (valor >= 100.0) {
        valor /= 10.0;
        (*multiplicador)++;
    }
    
    // Calcula as duas primeiras bandas de cores
    *cor1 = (int)(valor / 10.0);
    *cor2 = (int)fmod(valor, 10.0);
  }