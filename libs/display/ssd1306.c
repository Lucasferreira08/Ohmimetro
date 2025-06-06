#include "ssd1306.h"
#include "font.h"

void ssd1306_init(ssd1306_t *ssd, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c) {
  ssd->width = width;
  ssd->height = height;
  ssd->pages = height / 8U;
  ssd->address = address;
  ssd->i2c_port = i2c;
  ssd->bufsize = ssd->pages * ssd->width + 1;
  ssd->ram_buffer = calloc(ssd->bufsize, sizeof(uint8_t));
  ssd->ram_buffer[0] = 0x40;
  ssd->port_buffer[0] = 0x80;
}

void ssd1306_config(ssd1306_t *ssd) {
  ssd1306_command(ssd, SET_DISP | 0x00);
  ssd1306_command(ssd, SET_MEM_ADDR);
  ssd1306_command(ssd, 0x01);
  ssd1306_command(ssd, SET_DISP_START_LINE | 0x00);
  ssd1306_command(ssd, SET_SEG_REMAP | 0x01);
  ssd1306_command(ssd, SET_MUX_RATIO);
  ssd1306_command(ssd, HEIGHT - 1);
  ssd1306_command(ssd, SET_COM_OUT_DIR | 0x08);
  ssd1306_command(ssd, SET_DISP_OFFSET);
  ssd1306_command(ssd, 0x00);
  ssd1306_command(ssd, SET_COM_PIN_CFG);
  ssd1306_command(ssd, 0x12);
  ssd1306_command(ssd, SET_DISP_CLK_DIV);
  ssd1306_command(ssd, 0x80);
  ssd1306_command(ssd, SET_PRECHARGE);
  ssd1306_command(ssd, 0xF1);
  ssd1306_command(ssd, SET_VCOM_DESEL);
  ssd1306_command(ssd, 0x30);
  ssd1306_command(ssd, SET_CONTRAST);
  ssd1306_command(ssd, 0xFF);
  ssd1306_command(ssd, SET_ENTIRE_ON);
  ssd1306_command(ssd, SET_NORM_INV);
  ssd1306_command(ssd, SET_CHARGE_PUMP);
  ssd1306_command(ssd, 0x14);
  ssd1306_command(ssd, SET_DISP | 0x01);
}

void ssd1306_command(ssd1306_t *ssd, uint8_t command) {
  ssd->port_buffer[1] = command;
  i2c_write_blocking(
    ssd->i2c_port,
    ssd->address,
    ssd->port_buffer,
    2,
    false
  );
}

void ssd1306_send_data(ssd1306_t *ssd) {
  ssd1306_command(ssd, SET_COL_ADDR);
  ssd1306_command(ssd, 0);
  ssd1306_command(ssd, ssd->width - 1);
  ssd1306_command(ssd, SET_PAGE_ADDR);
  ssd1306_command(ssd, 0);
  ssd1306_command(ssd, ssd->pages - 1);
  i2c_write_blocking(
    ssd->i2c_port,
    ssd->address,
    ssd->ram_buffer,
    ssd->bufsize,
    false
  );
}

void ssd1306_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool value) {
  uint16_t index = (y >> 3) + (x << 3) + 1;
  uint8_t pixel = (y & 0b111);
  if (value)
    ssd->ram_buffer[index] |= (1 << pixel);
  else
    ssd->ram_buffer[index] &= ~(1 << pixel);
}

/*
void ssd1306_fill(ssd1306_t *ssd, bool value) {
  uint8_t byte = value ? 0xFF : 0x00;
  for (uint8_t i = 1; i < ssd->bufsize; ++i)
    ssd->ram_buffer[i] = byte;
}*/

void ssd1306_fill(ssd1306_t *ssd, bool value) {
    // Itera por todas as posições do display
    for (uint8_t y = 0; y < ssd->height; ++y) {
        for (uint8_t x = 0; x < ssd->width; ++x) {
            ssd1306_pixel(ssd, x, y, value);
        }
    }
}



void ssd1306_rect(ssd1306_t *ssd, uint8_t top, uint8_t left, uint8_t width, uint8_t height, bool value, bool fill) {
  for (uint8_t x = left; x < left + width; ++x) {
    ssd1306_pixel(ssd, x, top, value);
    ssd1306_pixel(ssd, x, top + height - 1, value);
  }
  for (uint8_t y = top; y < top + height; ++y) {
    ssd1306_pixel(ssd, left, y, value);
    ssd1306_pixel(ssd, left + width - 1, y, value);
  }

  if (fill) {
    for (uint8_t x = left + 1; x < left + width - 1; ++x) {
      for (uint8_t y = top + 1; y < top + height - 1; ++y) {
        ssd1306_pixel(ssd, x, y, value);
      }
    }
  }
}

void ssd1306_line(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool value) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        ssd1306_pixel(ssd, x0, y0, value); // Desenha o pixel atual

        if (x0 == x1 && y0 == y1) break; // Termina quando alcança o ponto final

        int e2 = err * 2;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}


void ssd1306_hline(ssd1306_t *ssd, uint8_t x0, uint8_t x1, uint8_t y, bool value) {
  for (uint8_t x = x0; x <= x1; ++x)
    ssd1306_pixel(ssd, x, y, value);
}

void ssd1306_vline(ssd1306_t *ssd, uint8_t x, uint8_t y0, uint8_t y1, bool value) {
  for (uint8_t y = y0; y <= y1; ++y)
    ssd1306_pixel(ssd, x, y, value);
}

// Função para desenhar um caractere
void ssd1306_draw_char(ssd1306_t *ssd, char c, uint8_t x, uint8_t y)
{
  uint16_t index = 0;

  // Verifica o caractere e calcula o índice correspondente na fonte
  if (c >= ' ' && c <= '~') // Verifica se o caractere está na faixa ASCII válida
  {
    index = (c - ' ') * 8; // Calcula o índice baseado na posição do caractere na tabela ASCII
  }
  else
  {
    // Caractere inválido, desenha um espaço (ou pode ser tratado de outra forma)
    index = 0; // Índice 0 corresponde ao caractere "nada" (espaço)
  }

  // Desenha o caractere na tela
  for (uint8_t i = 0; i < 8; ++i)
  {
    uint8_t line = font[index + i]; // Acessa a linha correspondente do caractere na fonte
    for (uint8_t j = 0; j < 8; ++j)
    {
      ssd1306_pixel(ssd, x + i, y + j, line & (1 << j)); // Desenha cada pixel do caractere
    }
  }
}

// Função para desenhar uma string
void ssd1306_draw_string(ssd1306_t *ssd, const char *str, uint8_t x, uint8_t y)
{
  while (*str)
  {
    ssd1306_draw_char(ssd, *str++, x, y);
    x += 8;
    if (x + 8 >= ssd->width)
    {
      x = 0;
      y += 8;
    }
    if (y + 8 >= ssd->height)
    {
      break;
    }
  }
}

void display_init(ssd1306_t *ssd) 
{
    i2c_init(I2C_PORT, 400 * 1000);
 
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line

    ssd1306_init(ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(ssd);                                         // Configura o display
    ssd1306_send_data(ssd);                                      // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}


// Função para desenhar um resistor estilizado no display monocromático
void desenhar_resistor(ssd1306_t *ssd, char *cor1, char *cor2, char *multiplicador) {
  // // Desenha o corpo do resistor (retângulo horizontal)
  // ssd1306_rect(ssd, 10, 20, 108, 15, true, false);
  
  // // Desenha os terminais (linhas)
  // ssd1306_line(ssd, 0, 27, 10, 27, true);
  // ssd1306_line(ssd, 118, 27, 128, 27, true);
  
  // // Posições para as faixas (sem preenchimento, apenas localizações marcadas)
  // ssd1306_rect(ssd, 8, 25, 12, 20, true, false);  // Primeira faixa
  // ssd1306_rect(ssd, 8, 45, 12, 20, true, false);  // Segunda faixa
  // ssd1306_rect(ssd, 8, 65, 12, 20, true, false);  // Faixa multiplicador

  printf("Desenho resistor.\n");

  // Atualiza o display
ssd1306_fill(ssd, false); // Limpa o display

ssd1306_line(ssd, 65, 10, 65, 0, true); // perna do resistor superior

ssd1306_line(ssd, 20, 10, 20, 55, true); // 1 linha vertical

ssd1306_line(ssd, 20, 10, 110, 10, true); // 1 linha horizontal

ssd1306_draw_string(ssd, "1", 24, 15);
ssd1306_draw_string(ssd, cor1, 35, 15);

ssd1306_line(ssd, 20, 25, 110, 25, true);

ssd1306_draw_string(ssd, "2", 24, 30);
ssd1306_draw_string(ssd, cor2, 35, 30);

ssd1306_line(ssd, 20, 40, 110, 40, true);

ssd1306_draw_string(ssd, "3", 24, 45);
ssd1306_draw_string(ssd, multiplicador, 35, 45);

ssd1306_line(ssd, 20, 55, 110, 55, true); // 2 linha horizontal

ssd1306_line(ssd, 110, 10, 110, 55, true); // 2 linha vertical

ssd1306_line(ssd, 65, 55, 65, 63, true); // perna do resistor inferior

// Atualiza o display
ssd1306_send_data(ssd);
}

void desenhar_erro(ssd1306_t *ssd, char *str_y, char *str_x)
{
  printf("Desenho erro.\n");
// Caso esteja fora da faixa, exibimos uma mensagem de aviso
ssd1306_fill(ssd, false); // Limpa o display
ssd1306_draw_string(ssd, "OHMIMETRO", 5, 2);
ssd1306_line(ssd, 0, 10, 128, 10, true); // Linha divisória

// Formatar o valor medido
ssd1306_draw_string(ssd, "Valor (Ohms):", 5, 13);
ssd1306_draw_string(ssd, str_y, 5, 23);

ssd1306_line(ssd, 0, 33, 128, 33, true); // Linha divisória

// Formatar o valor medido
ssd1306_draw_string(ssd, "ADC:", 5, 36);
ssd1306_draw_string(ssd, str_x, 50, 36);

ssd1306_line(ssd, 0, 45, 128, 45, true); // Linha divisória

// Aviso de fora da faixa
ssd1306_draw_string(ssd, "Fora da faixa!", 5, 47);
ssd1306_draw_string(ssd, "Faixa: 510-100k", 5, 55);

// Atualiza o display
ssd1306_send_data(ssd);
}

void desenhar_display(ssd1306_t *ssd, char *str_y, char *comercial_str, char *str_x)
{
  printf("Desenho display.\n");
// Atualiza o display
ssd1306_fill(ssd, false); // Limpa o display
      
// Desenha o título
ssd1306_draw_string(ssd, "OHMIMETRO", 5, 2);
ssd1306_line(ssd, 0, 10, 128, 10, true); // Linha divisória

// Exibe as informações textuais dos valores
ssd1306_draw_string(ssd, "Medido (Ohms):", 5, 15);
ssd1306_draw_string(ssd, str_y, 5, 25);

ssd1306_line(ssd, 0, 34, 128, 34, true); // Linha divisória

ssd1306_draw_string(ssd, "ADC:", 5, 36);
ssd1306_draw_string(ssd, str_x, 50, 36);

ssd1306_line(ssd, 0, 44, 128, 44, true); // Linha divisória

ssd1306_draw_string(ssd, "Valor E24(Ohms)", 5, 46);
ssd1306_draw_string(ssd, comercial_str, 5, 55);

// // Desenha uma linha divisória antes das informações de cores
// ssd1306_line(ssd, 0, 35, 128, 35, true);

// Atualiza o display
ssd1306_send_data(ssd);
}