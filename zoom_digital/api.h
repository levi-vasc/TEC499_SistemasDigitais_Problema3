#ifndef API_H
#define API_H

/// @brief Inicializa a API de acesso ao bridge de hardware.
/// @details Abre "/dev/mem", faz mmap da região LW_BRIDGE_BASE e calcula/salva os ponteiros para os registradores PIO (instruction, enable, dataout, flags).
/// @return 0 em caso de sucesso; -1 em caso de erro (falha em open/mmap).
int api_init(void);

/// @brief Finaliza a API e libera recursos alocados na inicialização.
/// @details Executa munmap da região mapeada e fecha o descritor de arquivo.
/// @return 0 em caso de sucesso; valor negativo em caso de erro.
int api_finalize(void);

/// @brief Reset do CPA.
/// @details Escreve a instrução de reset no registrador de instruções e aciona o registrador de enable; aguarda o bit "done" nas flags.
/// @return 0 em caso de sucesso; valor negativo em caso de erro.
int api_reset_cpa(void);

/// @brief Atualiza/refresh do CPA.
/// @details Envia a instrução de refresh para o PIO, aciona enable e espera o bit "done".
/// @return 0 em caso de sucesso; valor negativo em caso de erro.
int api_refresh_cpa(void);

/// @brief Atualiza a posição do cursor na tela
/// @details Envia a instrução de refresh com campos dos eixos X,Y e sinal SEL_MEM
/// @param x Eixo X da coordenada
/// @param y Eixo Y da coordenada
/// @return 0 em caso de sucesso; valor negativo em caso de erro
int api_update_cursor(int x, int y);

/// @brief Executa a operação vizinho_mais_proximo_in no hardware.
/// @details Envia a instrução vizinho_mais_proximo_in para o registrador de instruções, aciona enable e aguarda conclusão (bit done).
/// @return 0 em caso de sucesso; valor negativo em caso de erro.
int api_vmp_in(void);

/// @brief Executa a operação vizinho_mais_proximo_out no hardware.
/// @details Envia a instrução vizinho_mais_proximo_out ao PIO, aciona enable e aguarda conclusão.
/// @return 0 em caso de sucesso; valor negativo em caso de erro.
int api_vmp_out(void);

/// @brief Executa a operação replicação de pixel no hardware.
/// @details Envia a instrução de replicação ao PIO, aciona enable e espera o bit "done".
/// @return 0 em caso de sucesso; valor negativo em caso de erro.
int api_repixel(void);

/// @brief Executa a operação de média de blocos no hardware.
/// @details Envia a instrução de média ao PIO, aciona enable e aguarda conclusão.
/// @return 0 em caso de sucesso; valor negativo em caso de erro.
int api_mblocos(void);

/// @brief Executa a operação de load que retorna o valor do pixel de determinado endereço.
/// @details Envia a instrução LOAD ao PIO, aciona enable, aguarda o bit "done" e retorna o valor presente em DATAOUT (registrador de saída).
/// @param mem_sel Memória a ser lida, 0 para A, 1 para C
/// @param address Endereço do pixel a ser lido na memória
/// @return Valor lido não negativo em caso de sucesso; valor negativo em caso de erro.
int api_load(int mem_sel, int address);

/// @brief Armazena um pixel em um endereço da memória A
/// @param pixel Valor do pixel em escala de cinza
/// @param address Endereço a ser inserido o novo pixel
/// @return 0 em caso de sucesso; valor negativo em caso de erro
int api_store(int pixel, int address);

/// @brief Armazena dados a partir de um arquivo BMP no hardware.
/// @details Abre o arquivo indicado por file_path, lê o cabeçalho BMP e os pixels, converte RGB para tons de cinza e envia instruções STORE ao PIO para cada pixel.
/// @param file_path Caminho para o arquivo BMP a ser lido (ex: "/caminho/imagem.bmp").
/// @return 0 em caso de sucesso; -1 em caso de erro (abertura/leitura/formatos não suportados).
int api_store_img(const char *file_path);


#endif /* API_H */