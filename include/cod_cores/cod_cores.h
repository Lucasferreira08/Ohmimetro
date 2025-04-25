static float E24[] = {1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 2.7, 3.0, 
  3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1};

static char *cores_nomes[] = {"Preto", "Marrom", "Vermelho", "Laranja", "Amarelo", 
  "Verde", "Azul", "Violeta", "Cinza", "Branco"};

// Função para encontrar o valor E24 mais próximo da resistência medida
float encontrar_valor_e24(float resistencia);
  
  // Função para calcular os códigos de cores para um resistor
void calcular_codigo_cores(float resistencia, int *cor1, int *cor2, int *multiplicador);