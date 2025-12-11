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

* **Carregamento de Imagem:** Leitura de arquivos de imagem no formato bitmap para a memória do sistema.

* **Seleção de Algoritmos:** Interface textual para escolha do método de zoom a ser aplicado.
  * Vizinho mais próximo para zoom in;
  * Vizinho mais próximo para zoom out;
  * Replicação de pixels (zoom in);
  * Média de blocos (zoom out).

* **Interação via Mouse:**
  * Uso do mouse para definir, através de dois cliques, uma região de interesse (janela) na tela;
  * Exibição das coordenadas (x, y) do mouse em tempo real na interface de texto.

* **Controle de Zoom:**
  * Aplicação do zoom na região selecionada desenhada sobre a imagem original;
  * Uso da tecla `+` para realizar zoom in na janela;
  * Uso da tecla `-` para realizar zoom out até retornar a imagem ao estado original.
</details>

---
