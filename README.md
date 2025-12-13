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
  <summary><h2>🗺Tutorial de Instalação e Configuração</h2></summary>
  
## 1. Pré-requisitos

Antes de iniciar o processo de instalação, compilação e execução do projeto, certifique-se de que o ambiente de desenvolvimento atende aos seguintes requisitos.

### 1.1 Sistema Operacional
- **Linux**

### 1.2 Ferramentas de Hardware e HDL
- **Quartus Prime Lite** (versão recomendada: 23.1)  
- **USB-Blaster** devidamente configurado para programação da FPGA  
- Placa de Desenvolvimento **De1-SoC**.
- **Mouse com conexão USB** conectado à De1-SoC (utilizado na etapa de Execução)

### 1.3 Ferramentas de Software
- **GCC** (compilador C)
- **GNU Assembler (as)** para compilação dos arquivos Assembly
- **Make** para automação do processo de build
- **Terminal Linux** com permissões para execução de binários (`sudo`, quando necessário)
  
> **💡Observação**  
>
> Recomenda-se verificar se todas as ferramentas estão corretamente instaladas e acessíveis pelo `PATH` do sistema antes de prosseguir para as próximas etapas.

## 2. Preparação do Ambiente e dos Arquivos

Antes da compilação e execução do projeto, é necessário preparar o ambiente de desenvolvimento e garantir que todos os arquivos estejam corretamente posicionados.

### 2.1 Obtenção do Projeto

Clone o repositório para a máquina local:

```bash
git clone https://github.com/levi-vasc/TEC499_SistemasDigitais_Problema3.git
```

Acesse o diretório do projeto:

```bash
cd TEC499_SistemasDigitais_Problema3
```

### 2.2 Organização dos Arquivos

Certifique-se de que os arquivos do projeto estejam distribuídos conforme o repositório original, sem alterações na hierarquia de pastas.
A hierarquia segue dividida em:
* ```Coprocessador``` - Pasta com os arquivos HDL que descrevem o Co-Processador Aritmético
* ```zoom_digital```  - Pasta com todos os arquivos de Software encontrados no HPS. Inclui-se:
  * api.s - Implementação das funções em assembly para comunicação HPS-FPGA
  * api.h - Arquivo cabeçalho com documentação e protótipos das funções em api.s
  * main.c - Arquivo C que contém a lógica do projeto, como interfaces e chamada de funções da API.
  * Makefile - Arquivo de Compilação e Linkagem do projeto    

Nenhuma modificação estrutural é necessária para a execução padrão do projeto.

### 2.3 Verificação do Ambiente

Antes de prosseguir para as etapas de compilação, recomenda-se verificar se as principais ferramentas estão corretamente instaladas e acessíveis pelo sistema:

```bash
gcc --version
make --version
quartus --version
```

Caso algum dos comandos não seja reconhecido, verifique a instalação da ferramenta correspondente ou a configuração do `PATH` do sistema.

## 3. Compilação e Execução do Projeto no Quartus

> **💡 Importante**
> 
> Para compilar e executar o projeto, é necessário ter o Quartus instalado. Certifique-se de que a placa esteja ligada e conectada ao computador pela porta **USB Blaster-II** e ao display de destino pela saída **VGA**.

1. Faça o download da pasta `Coprocessador`, presente neste repositório;
2. Abra o Quartus;
3. Clique em `Open Project`;
4. No explorador de arquivos, abra `soc_system.qpf`, que está dentro da pasta do projeto;
5. Clique em `Start Compilation`, representado por uma seta azul na barra de ferramentas;
6. Aguarde a barra de `Compile Design` da aba Task chegar a 100% (indica que a compilação foi concluída).

![tutorial1](https://github.com/user-attachments/assets/a9974525-1d1c-44ca-a538-37d585551a9e)

7. Clique em `Programmer`, representado por um losango, na barra de ferramentas;
8. Na nova janela, clique em `Hardware Setup`;
9. Dê dois cliques em `DE-SoC` e feche a janela;
10. Clique em `Auto Detect` e selecione a segunda opção (`5CSEMA5`) na nova janela;
11. Clique em "Yes" na nova janela;
12. Exclua e segundo arquivo da lista, e posteriormente clique em `Add File`;
13. No explorador de arquivos, acesse a pasta `output_files` do projeto e selecione `soc_system.sof`;
14. Clique em `Start` e aguarde a barra progress chegar a 100%.

![tutorial2](https://github.com/user-attachments/assets/1d68b321-1214-4f51-963a-b5d2e787911b)

O monitor deve exibir uma imagem pré-definida após execução.

## 4. Compilação e Linkagem do Código no HPS

> **💡Importante**
>
> Certifique-se de que a placa está ligada e conectada a mesma rede do computador host pela interface **Ethernet**
>
> Este tutorial assume que a placa utilizada é uma das disponibilizadas pela UEFS. Caso utilize outra placa, verifique o **nome de usuário**, o **endereço IP** e a **organização dos diretórios**. Substitua aluno e o IP usados nos exemplos pelas informações específicas da sua placa.

Com o hardware pronto, deve-se transferir a aplicação e a API para o ambiente Linux da DE1-SoC. Utilizaremos o protocolo `ssh` para estabelecer a conexão:

```
ssh aluno@172.65.213.120
```

> **💡Lembrete**  
>
> Ao configurar o acesso SSH ou a comunicação com o HPS, **substitua sempre os últimos 3 números do endereço IP** pelo IP correspondente à sua placa DE1-SoC. Cada placa utiliza um IP diferente na rede local, portanto ajuste antes de executar qualquer comando de conexão.

Em seguida, transfira os arquivos da pasta zoom_digital do computador host para a placa. No host, navegue até o diretório da pasta e execute:

```
scp * aluno@172.65.213.122:/home/aluno/zoom_digital
```

Isso enviará todos os arquivos para a pasta `zoom_digital` na placa. Depois, já no terminal da DE1-SoC, acesse essa pasta e execute:

```
make clean
```

```
make
```

Esses comandos realizam a limpeza, compilação e linkagem da aplicação. Por fim, inicie o executável:

```
sudo ./main
```

Os próximos passos serão detalhados na próxima seção, **Testes e Análise de Resultados**.

</details>

---
<details>
  <summary><h2> 🔍Testes e Análise de Resultados</h2></summary>
  
## 1. Inicialização do Sistema

Após a programação da FPGA e a execução do binário no HPS por meio do comando:
  
```bash
sudo ./main
```

O sistema inicializa a API de comunicação com o coprocessador gráfico, realizando o mapeamento da memória e dos registradores do hardware. Em caso de sucesso, a mensagem de confirmação é exibida no terminal, indicando que o sistema está pronto para operação. O menu inicial é exibido com as opções:

```bash
Escolha uma opção:

1. Vizinho mais próximo (Zoom In)
2. Replicação de pixel
3. Vizinho mais próximo (Zoom Out)
4. Média de blocos
5. Zoom em Janela
6. Carregar imagem
7. Reset
8. Carregar pixel
0. Sair

Opção:
```

Neste estado inicial, a imagem previamente carregada na memória do coprocessador é exibida no monitor VGA em resolução 320x240 pixels, convertida para tons de cinza, como a imagem da Figura 1.
<p align="center">Figura 1. Imagem de Resolução 320x240 em escala de cinza.</p>
<p align="center">
<img width="400" height="230" alt="image" src="https://github.com/user-attachments/assets/b79c5456-327e-4f36-9c88-26324d39ac58" />
</p>

## 2. Testes de Zoom em Imagem Completa
Os primeiros testes consistem na aplicação de zoom sobre a imagem inteira, utilizando os algoritmos disponíveis no menu principal da aplicação.

### 2.1 Zoom In — Vizinho Mais Próximo
Seleciona-se a opção 1 no menu.
O usuário pressiona repetidamente a tecla `+` para aplicar zoom-in sucessivos.
A cada acionamento, a imagem exibida no monitor VGA é ampliada por um fator de 2x (**com limite de dois passos de 2x, ou zoom-in/out total de 4x**), mantendo o comportamento esperado do algoritmo de vizinho mais próximo.

**Resultado esperado:**
A imagem torna-se progressivamente ampliada, com preservação de bordas e possível pixelização, característica do método.

### 2.2. Zoom Out — Vizinho Mais Próximo e Média de Blocos
Seleciona-se a opção 3 ou 4. O usuário utiliza a tecla - para reduzir a imagem.

**Comportamento**: A imagem é reduzida e centralizada ou alinhada conforme a lógica de endereçamento do hardware.

**Resultado Esperado**: O "Vizinho Mais Próximo (Out)" descarta pixels sistematicamente (aliasing potencial), enquanto a "Média de Blocos" realiza uma suavização (binning), resultando em uma imagem reduzida com menos ruído visual.

## 3. Teste de Zoom em Janela (Seleção por Mouse)
A funcionalidade implementada nesta etapa é a capacidade de selecionar uma área da imagem utilizando um mouse USB conectado ao kit de desenvolvimento.

### 3.1. Interação e Seleção de Área
Ao selecionar a opção 5 (Zoom em Janela), o software inicia a captura de eventos do dispositivo /dev/input/event0.
  1. Cursor Visual: O sistema desenha e atualiza um cursor sobre a imagem no monitor VGA em tempo real (api_update_cursor), permitindo ao usuário visualizar onde está clicando.
  2. Definição da Janela: O usuário deve clicar duas vezes para definir os cantos opostos da janela (retângulo).
     * Primeiro Clique: Define o ponto inicial $(x_0, y_0)$.
     * Segundo Clique: Define o ponto final $(x_1, y_1)$.
  3. Validação: O software verifica se a área selecionada respeita a resolução máxima de 80x60 pixels. Caso a área seja maior, uma mensagem de erro é exibida e o usuário deve repetir a seleção.

```bash
Use o mouse e clique duas vezes para selecionar a área...
Primeiro clique: (100, 80)
Segundo clique: (150, 120)
```

### 3.2. Configuração de Algoritmos e Modo Interativo
Após a seleção válida da área, o sistema separa a área selecionada para zoom. O usuário é então solicitado a escolher o par de algoritmos para o zoom interativo:
```bash
Selecione os algoritmos desejados:

1. Vizinho mais próximo In/ Vizinho mais próximo Out
2. Replicação de pixels/ Média de blocos
3. Vizinho mais próximo In/ Média de blocos
4. Replicação de pixel/ Vizinho mais próximo Out
```

Entra-se então no Modo Interativo, onde não é necessário pressionar Enter após cada comando:
* Tecla `+`: A área selecionada é ampliada utilizando o algoritmo de Zoom In escolhido.
* Tecla `-`: Reverte o zoom ou aplica o algoritmo de redução sobre a janela.
* Tecla `Esc` ou outras: Encerra o modo janela e retorna ao menu principal.

> **💡 Observação**
>
> Toda a sequência acima é detalhada na subseção {...} que descreve o algoritmo implementado em C para recorte e zoom em área.

**Resultado Esperado:** A funcionalidade permite isolar detalhes específicos da imagem original. O recorte é expandido na área selecionada, facilitando a inspeção visual de áreas pequenas. Com isso, a combinação de "Vizinho Mais Próximo In" para ampliação com "Média de Blocos" para redução deve mostrar-se eficiente para navegar entre os níveis de detalhe, sendo que a aplicação de um Zoom-Out com "Média de Blocos" retorna à imagem original, enquanto o "Vizinho Mais Próximo-Out" acarreta em ruídos (perda de informação) na imagem, em razão de sua implementação.

<p align="center">Figura 2. Sequência de operação: (a) Seleção da área (pontos ilustrativos destacados em vermelho); (b) Área ampliada.</p>
(a)
<p align="center">
<img width="400" height="230" alt="image" src="https://github.com/user-attachments/assets/1d8ef966-4a0b-4271-976b-eb3fbc37eb72" />
</p>
(b)
<p align="center">
<img width="400" height="230" alt="image" src="https://github.com/user-attachments/assets/7ac4be9a-14e1-4539-85fb-2faf92531b46" />
</p>
</details>

---
