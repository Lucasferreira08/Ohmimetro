#include "init_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

// Variáveis globais para controle de tempo e estado
static volatile uint32_t last_time = 0; // Armazena o último tempo de interrupção
static volatile bool botao = false; // Armazena o último tempo de interrupção

#define Botao_A 5
#define ADC_PIN 28 // GPIO para o voltímetro

// Função de interrupção com debouncing aprimorado
void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Obtém o tempo atual em microssegundos

    // Verifica se passaram pelo menos 250ms desde a última interrupção (debouncing)
    if (current_time - last_time > 250000) // 250 ms para evitar pulos
    {
        // Se o botão A foi pressionado e 'a' é menor que 9, incrementa 'a'
        if (gpio == Botao_A) {
            botao=!botao;
            printf("botao: %d\n", botao);
        }

        // Atualiza o último tempo de interrupção
        last_time = current_time;
    }
}

void buttons_config()
{
    stdio_init_all();
    
   gpio_init(Botao_A);
   gpio_set_dir(Botao_A, GPIO_IN);
   gpio_pull_up(Botao_A);

   // Habilita a interrupção para os botões A e B na borda de descida (quando o botão é pressionado)
   gpio_set_irq_enabled_with_callback(Botao_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

void adc_config()
{
    adc_init();
    adc_gpio_init(ADC_PIN); // GPIO 28 como entrada analógica
}

bool estado_botao()
{
    return botao; // botao
}