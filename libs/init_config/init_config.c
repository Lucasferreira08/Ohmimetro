#include "init_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"


#define Botao_A 5
#define ADC_PIN 28 // GPIO para o voltímetro

void buttons_config()
{
    stdio_init_all();
    
   gpio_init(Botao_A);
   gpio_set_dir(Botao_A, GPIO_IN);
   gpio_pull_up(Botao_A);
}

void adc_config()
{
    adc_init();
    adc_gpio_init(ADC_PIN); // GPIO 28 como entrada analógica
}