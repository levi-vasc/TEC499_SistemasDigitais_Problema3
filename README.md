# TEC499_SistemasDigitais_Problema3
Terceira e última etapa do problema de MI de Sistemas Digitais, semestre 2025.2 da Universidade Estadual de Feira de Santana (UEFS). Este projeto consiste em um controlador gráfico que realiza operações de zoom-in e zoom-out em imagens de 320x240 pixels.

---
<details>
  <summary><h2>🧩 Descrição do Projeto</h2></summary>

Este projeto tem como objetivo geral desenvolver um sistema embarcado de processamento de imagens na plataforma **DE1-SoC** (FPGA + HPS ARM). Nesta etapa específica, o foco foi a construção de uma aplicação em **linguagem C** que atua como interface de alto nível para o usuário, integrando-se à biblioteca de drivers (API em Assembly) desenvolvida na etapa anterior. A aplicação permite carregar imagens bitmap, interagir via mouse e realizar operações de zoom em tempo real.

A aplicação em C comunica-se com o hardware dedicado (coprocessador gráfico na FPGA) através de chamadas ao driver desenvolvido na etapa 2. O fluxo de dados envolve a leitura do arquivo pelo HPS (processador ARM), transferência para o coprocessador e exibição do resultado processado na saída VGA.

Para mais informações sobre a integração entre o HPS e FPGA da placa, além do detalhamento sobre a API, confira o [repositório da etapa 2](https://github.com/levi-vasc/TEC499_SistemasDigitais_Problema2). O coprocessador utilizado foi desenvolvido por Maike de Oliveira Nascimento, e o [repositório](https://github.com/DestinyWolf/Problema-SD-2025-2) do seu trabalho pode ser consultado para uma descrição mais detalhada.

## Funcionalidades implementadas 
O sistema opera através de uma interface híbrida (texto e gráfica via VGA) com as seguintes capacidades:

* **Carregamento de Imagem:** Leitura de arquivos de imagem no formato **bitmap (.bmp) de 24 bits** para a memória do sistema. A API converte a imagem para um formato de **8 bits** em **escala de cinza** para ser exibida no VGA.

* **Seleção de Algoritmos:** Interface textual para escolha do método de zoom a ser aplicado.
  * Vizinho mais próximo para zoom in;
  * Vizinho mais próximo para zoom out;
  * Replicação de pixels (zoom in);
  * Média de blocos (zoom out).

* **Interação via Mouse:**
  * Uso do mouse para definir, através de dois cliques, uma região de interesse (janela) na tela;
  * Exibição das coordenadas (x, y) do mouse em tempo real na interface de texto;
  * Exibição do cursor no VGA.

* **Controle de Zoom:**
  * Zoom ajustado em passos discretos de **2x**;
  * Aplicação do zoom na imagem inteira, com limite máximo de **8x** e mínimo de **0,125x** (1/8 do tamanho original);
  * Aplicação do zoom na região selecionada desenhada sobre a imagem original, com limite máximo de **4x** e mínimo de **1x** (estado original da imagem);
  * Uso da tecla `+` para realizar zoom in;
  * Uso da tecla `-` para realizar zoom out.
</details>

---
<details>
  <summary><h2>🛠 Ferramentas Utilizadas</h2></summary>

## Hardware
### Kit de desenvolvimento DE1-SoC
O projeto foi desenvolvido no kit de desenvolvimento DE1-SoC, que integra em um único chip um processador ARM e uma FPGA Cyclone V. Para mais informações, é possível consultar o [manual](https://drive.google.com/file/d/1dBaSfXi4GcrSZ0JlzRh5iixaWmq0go2j/view).

#### FPGA (Field-Programmable Gate Array)
- Modelo **Altera Cyclone® V SE 5CSEMA5F31C6N**
- **110K Logic Elements (LEs)**
- **Blocos M10K** para armazenamento 
- **USB-Blaster II** onboard (para programação); Modo JTAG
- VGA DAC (DACs triplos de alta velocidade de 8 bits) com conector de saída VGA
- Capaz de implementar:
  - Coprocessadores;  
  - RAM interna;  
  - Controle VGA;  
  - Máquinas de estado e pipelines.
 
#### HPS (Hard Processor System)
- **ARM Cortex-A9 Dual-Core**
- 1 Gigabit Ethernet PHY com conector RJ45
- Host USB de 2 portas, conector USB tipo A normal  
- Baseado na **Arquitetura ARMv7-A**  
  - Suporte a NEON SIMD;  
  - Suporte a MMU (Memory Management Unit);  
  - Conjunto de instruções ARM e Thumb-2.  
- Subsystem incluído:
  - Controlador DDR3;  
  - UART, I²C, SPI;  
  - Timers, GIC (interrupt controller).
 
<img width="464" height="354" alt="placaSD" src="https://github.com/user-attachments/assets/8ba7c8be-ecc2-468f-8c1b-c44e2e5f05de" />

### Ambiente de teste e desenvolvimento
#### **Computador host**
  - Utilizado para desenvolvimento da solução de software e hardware
  - Conexão física com a placa de desenvolvimento através das interfaces **USB-Blaster II** (para programação e depuração do FPGA/HPS) e **Ethernet** (para comunicação de dados)
#### Periféricos
  - **Monitor VGA:** Um monitor capaz de exibir a resolução **640x480 a 60Hz** para conexão direta à saída VGA da placa
  - **Mouse:** Utilizado para interação com a interface gráfica/visualização, conectado à porta **USB Host** da placa
  - **Teclado:** Conectado ao computador host para interação via terminal    

## Software
Foram utilizadas as ferramentas **Quartus Prime** e **Visual Studio Code**, que em conjunto oferecem todo o suporte necessário tanto para o desenvolvimento em FPGA quanto para a implementação da API em Assembly ARMv7 e código C no ambiente Linux do HPS.  
O Quartus Prime possibilita configurar pinos, validar o hardware e gerar o projeto para a placa DE1-SoC, enquanto o Visual Studio Code fornece um ambiente leve e eficiente para edição, organização e compilação do código de software.

### Quartus Prime
- Versão utilizada: **23.1 Lite**
- Principais ferramentas:
  - **Editor de código**: permite escrever código em Verilog;
  - **Compilador/Síntese**: traduz o código HDL em uma representação lógica (netlist);
  - **Programador**: carrega o arquivo final (.sof) para o dispositivo FPGA real.
 
### Visual Studio Code
- Versão utilizada: **1.107.0**
- Principais ferramentas:
  - **Editor de código**: moderno e interativo, suporta C e Assembly;
  - **Extensões**: disponibiliza diversas extensões que estilizam o código e facilitam o desenvolvimento da programação. 
</details>

---
<details>
  <summary><h2>🗺Tutorial de Instalação e Execução</h2></summary>
</details>

---
<details>
  <summary><h2> 🔍Testes e Análise de Resultados</h2></summary>
</details>

---
