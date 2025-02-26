# Documentação do Projeto: Controle de LEDs RGB e Display OLED com Joystick  

## Demonstração  
[Link para o vídeo de demonstração](https://youtu.be/2a1S9PvHx08)  

---

## Introdução  
Este projeto tem como objetivo consolidar os conceitos de uso de conversores analógico-digitais (ADC), modulação por largura de pulso (PWM) e comunicação I2C no microcontrolador RP2040, utilizando a placa de desenvolvimento BitDogLab. O projeto consiste em controlar a intensidade de LEDs RGB com base nos valores analógicos fornecidos por um joystick, além de exibir a posição do joystick em um display OLED SSD1306.  

---

## Componentes Utilizados  
- **Joystick**: Fornece valores analógicos para os eixos X e Y, que são convertidos em valores digitais pelo ADC.  
- **LED RGB**: Controlado via PWM para ajustar a intensidade das cores vermelha, verde e azul.  
- **Display OLED SSD1306**: Exibe um quadrado que se move de acordo com a posição do joystick.  
- **Botões**: O botão do joystick e o botão A são utilizados para alternar estados dos LEDs e da borda do display.  

---

## Funcionalidades do Projeto  

### Controle de LEDs RGB:  
- O LED azul ajusta seu brilho conforme o movimento do eixo Y do joystick.  
- O LED vermelho ajusta seu brilho conforme o movimento do eixo X do joystick.  
- O LED verde é alternado a cada pressionamento do botão do joystick.  

### Exibição no Display OLED:  
- Um quadrado de 8x8 pixels é exibido no display e se move proporcionalmente aos valores dos eixos X e Y do joystick.  
- A borda do display é alternada entre diferentes estilos a cada pressionamento do botão do joystick.  

### Controle via Botões:  
- **Botão A**: Alterna o estado dos LEDs vermelho e azul, habilitando ou desabilitando o controle via joystick.  
- **Botão do Joystick**: Alterna o estado do LED verde e modifica o estilo da borda do display.  

---

## Estrutura do Código  
O código está organizado em vários arquivos, cada um responsável por uma funcionalidade específica:  

- **adc_util.c**: Contém funções para configurar e ler valores do ADC.  
- **interrupt_util.c**: Implementa o tratamento de interrupções para os botões, incluindo debouncing.  
- **oled_util.c**: Gerencia a comunicação com o display OLED e desenha elementos na tela.  
- **pwm_util.c**: Configura e controla o PWM para os LEDs RGB.  
- **ssd1306.c**: Implementa funções de baixo nível para controle do display OLED.  
- **main.c**: Contém a lógica principal do projeto, integrando todas as funcionalidades.  

---

## Detalhamento das Funções Principais  

### `initialize_peripherals()`  
Esta função inicializa todos os periféricos utilizados no projeto:  
- **OLED**: Configura a comunicação I2C com o display OLED.  
- **ADC**: Configura os pinos do joystick (eixos X e Y) para leitura analógica.  
- **Botões**: Configura os pinos dos botões como entradas com pull-up e habilita interrupções.  
- **PWM**: Configura os pinos dos LEDs RGB para controle de intensidade via PWM.  

---

### `Callback_BTs()`  
Função de callback chamada quando um botão é pressionado. Ela alterna os estados dos LEDs e da borda do display:  
- **Botão A**: Alterna o estado dos LEDs vermelho e azul.  
- **Botão do Joystick**: Alterna o estado do LED verde e modifica o estilo da borda do display.  

---

### `manipulation_RGBled_pwm()`  
Controla a intensidade dos LEDs vermelho e azul com base nos valores do joystick:  
- Se o joystick estiver fora da posição central, o LED correspondente (vermelho ou azul) acende com intensidade proporcional ao deslocamento.  
- Se o joystick estiver na posição central, os LEDs permanecem apagados.  

---

### `manipulation_pixel_oled()`  
Atualiza a posição do quadrado no display OLED com base nos valores do joystick:  
- Converte os valores analógicos do joystick (0-4095) em coordenadas do display (0-127 para X e 0-63 para Y).  
- Limita o movimento do quadrado para que ele não saia da tela.  
- Desenha o quadrado na nova posição e atualiza o display.  

---

### `create_border_oled()`  
Desenha a borda do display OLED, alternando entre dois estilos a cada pressionamento do botão do joystick:  
- **Borda Simples**: Um retângulo com contorno fino.  
- **Borda em Negrito**: Um retângulo com contorno mais espesso.  

---

## Fluxo de Execução  
1. **Inicialização**: Todos os periféricos são configurados e o display OLED é limpo.  
2. **Leitura do Joystick**: Os valores dos eixos X e Y são lidos e convertidos em coordenadas para o display e intensidades para os LEDs.  
3. **Atualização dos LEDs**: A intensidade dos LEDs vermelho e azul é ajustada com base nos valores do joystick.  
4. **Atualização do Display**: O quadrado é movido para a nova posição e a borda é desenhada conforme o estado atual.  
5. **Tratamento de Interrupções**: Quando um botão é pressionado, a função de callback é chamada para alternar os estados dos LEDs e da borda.  

---

## Considerações Finais  
Este projeto demonstra a integração de vários periféricos comuns em sistemas embarcados, como ADCs, PWMs, displays e botões. A utilização de interrupções e debouncing garante um controle preciso e responsivo dos componentes. O código foi estruturado de forma modular, facilitando a compreensão e a manutenção.  

Para mais detalhes, consulte os arquivos fonte e o vídeo de demonstração do projeto.  
