#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h> // Biblioteca para entrada do mouse
#include <stdbool.h>
#include <fcntl.h> 
#include <termios.h> // Biblioteca para entradas com getch()

#include "api.h" // Biblioteca da API

// Função Módulo: retorna o valor absoluto do x
#define MOD(x) ((x < 0) ? (-x) : x)

// ------- Globais ----------------
int buffer_recorte [76800] = {0};
int copy_image[76800] = {0};
int buffer_recorte_initial[76800] = {0};

int count_zoom = 1;
int zoom_corre = 1;


// -------------------------------------------------------------
// Struct do Mouse
// -------------------------------------------------------------

typedef struct {
    int x_0;
    int y_0;
    int x_1;
    int y_1;
} mouse;


// -------------------------------------------------------------
// Função de controle do mouse — captura 2 cliques
// -------------------------------------------------------------

void control_mouse(mouse *m, int fd, struct input_event *ev) {

    int x = 0, y = 0;
    int count_clicks = 0;
    int is_area = 0;  

    printf("\nUse o mouse e clique duas vezes para selecionar a área...\n");

    while (count_clicks < 2 && (is_area) == 0){

        ssize_t n = read(fd, ev, sizeof(*ev));
        if (n != sizeof(*ev)) continue;

        if (ev->type == EV_REL) {

            // Atualiza posição
            if (ev->code == REL_X) x += ev->value;
            if (ev->code == REL_Y) y += ev->value;
            
            // Estabelece limites para as coordenadas
            if (x < 0) x = 0;
            if (x > 319) x = 319;
            if (y < 0) y = 0;
            if (y > 239) y = 239;

            api_update_cursor(x, y);
            printf("Mouse: x=%d y=%d\n", x, y);


        }

        // Clique ESQUERDO
        if (ev->type == EV_KEY && ev->code == BTN_LEFT && ev->value == 1) {

            if (count_clicks == 0) {
                m->x_0 = x;
                m->y_0 = y;
                printf("Primeiro clique: (%d, %d)\n", x, y);
                count_clicks++;
            }
            else {
                m->x_1 = x;
                m->y_1 = y;
                printf("Segundo clique: (%d, %d)\n", x, y);

                if ((MOD(m->x_1 - m->x_0) > 80) || (MOD(m->y_1 - m->y_0) > 60)){
                    is_area = 0;
                    count_clicks = 0;
                    printf("Area invalida. Selecione uma area com uma resolucao maxima de 80x60.\n");
                }
                else{
                    is_area = 1;
                    count_clicks++;
                }
            }

        }
    }
}

//----------------------------------------------
// Função para aplicar zoom em área selecionada
//----------------------------------------------
void apply_zoom_to_crop(int x_min, int x_max, int y_min, int y_max, void (*zoom_function)()) {

    int x, y, i;
    int w = x_max - x_min;   // largura do recorte
    int h = y_max - y_min;   // altura do recorte

    // Offsets para centralizar no buffer 320x240
    int ofx = (320 - w) / 2;
    int ofy = (240 - h) / 2;

    // =============================================================
    // 1) Envia buffer centralizado para execução do zoom
    // =============================================================
    for (i = 0; i < 76800; i++)
        api_store(buffer_recorte_initial[i], i);

    api_reset_cpa();
    zoom_function();

    for (i = 0; i < 76800; i++) {
        buffer_recorte_initial[i] = api_load(1, i);
    }

    for (i = 0; i < 76800; i++)
        buffer_recorte[i] = copy_image[i];

    // =============================================================
    // 2) Copia apenas o recorte ampliado para a posição original
    // =============================================================
    for (y = 0 ; y < h; y++) {
        for (x = 0 ; x < w; x++) {

            int addr_src = (ofy + y+(zoom_corre/2)) * 320 + (ofx + x+(zoom_corre/2));   // origem no buffer zoomado
            int addr_dst = (y_min + y ) * 320 + (x_min + x );   // ponto original da imagem

            buffer_recorte[addr_dst] = buffer_recorte_initial[addr_src];
        }
    }
    // =============================================================
    // 3) Envia imagem corrigida para a tela
    // =============================================================
    for (i = 0; i < 76800; i++)
        api_store(buffer_recorte[i], i);

    api_refresh_cpa();
}


// ----------------------
// Leitura de tecla sem Enter (getch)   [Código fornecido por IA]
// ----------------------
static struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void enable_raw_mode() {
    struct termios raw;

    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON); // desabilita eco e modo canônico (linha)
    raw.c_cc[VMIN] = 1;  // ler pelo menos 1 byte
    raw.c_cc[VTIME] = 0; // sem timeout

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

// Retorna o código do caractere lido (0-255). Se for ESC sequence, caller deve fazer as leituras seguintes.
int getch_raw() {
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) == 1) return c;
    return -1;
}


// -------------------------------------------------------------
// MAIN
// -------------------------------------------------------------

int main() {
   
    //-----------------------------------------------------------
    // Abrindo dispositivo do mouse
    //-----------------------------------------------------------

    const char *dev = "/dev/input/event0";
    struct input_event ev;
    int fd_mouse = open(dev, O_RDONLY);

    if (fd_mouse < 0) {
        perror("Erro abrindo dispositivo");
        return 1;
    }

    mouse cursor = {0,0,0,0};

    //-----------------------------------------------------------
    // Variáveis gerais
    //-----------------------------------------------------------
    int x, y, i; // Para loops em coordenadas selecionadas
    int x_min, y_min, x_max, y_max; // Coordenadas selecionadas

    bool flag_stay = true; // Flag do Loop principal
    int opcao; // Opção do menu
    
    char path_file[256]; // Caminho de arquivo para carregar imagem
    char zoom; // Zoom por teclas
    int mem_sel; // Memória selecionada
    int addr; // Endereços em funções load / store

    //-----------------------------------------------------------
    // Inicializando API
    //-----------------------------------------------------------

    if (api_init() == -1) {
        printf("Erro ao inicializar a API.\n");
        return 0;
    }

    printf("API Inicializada!\n\n");

    // Salva estado original da imagem para reset
    for (i = 0; i < 76800; i++)
        copy_image[i] = api_load(0, i);


    //-----------------------------------------------------------
    // Loop principal
    //-----------------------------------------------------------

    while (flag_stay) {

        printf("Escolha uma opção:\n\n");
        printf("1. Vizinho mais próximo (Zoom In)\n");
        printf("2. Replicação de pixel\n");
        printf("3. Vizinho mais próximo (Zoom Out)\n");
        printf("4. Média de blocos\n");
        printf("5. Zoom em Janela\n");
        printf("6. Carregar imagem\n");
        printf("7. Reset\n");
        printf("8. Carregar pixel\n");
        printf("0. Sair\n\n");
        printf("Opção: ");
        scanf(" %d", &opcao);
        printf("\n");

        switch (opcao) {
            
            // Executa zooms consecutivos pela tecla + usando Vizinho Mais Próximo (Zoom In).
            case 1:

                do {
                    printf("Aplicar zoom (+) ou retornar para menu (M):");
                    scanf(" %c", &zoom);

                    if (zoom == '+')
                        api_vmp_in();
                } while (zoom == '+');
            
                system("clear");
                break;
            
            // Executa zooms consecutivos pela tecla + usando Replicação de Pixel.
            case 2:

                do {
                    printf("Aplicar zoom (+) ou retornar para menu (M):");
                    scanf(" %c", &zoom);

                    if (zoom == '+')
                        api_repixel();
                } while (zoom == '+');
            
                system("clear");
                break;
            
            // Executa zooms consecutivos pela tecla - usando Vizinho Mais Próximo (Zoom Out).
            case 3:  
                
                do {
                    printf("Aplicar zoom (-) ou retornar para menu (M):");
                    scanf(" %c", &zoom);

                    if (zoom == '-')
                        api_vmp_out();
                } while (zoom == '-');
            
                system("clear");    

                break;
            
            // Executa zooms consecutivos pela tecla - usando Média de Blocos.        
            case 4:
            
                do {
                    printf("Aplicar zoom (-) ou retornar para menu (M):");
                    scanf(" %c", &zoom);
                    if (zoom == '-')
                        api_mblocos();
                } while (zoom == '-');
            
                system("clear");
                break;

            // Seleção da Janela    
            case 5:

                control_mouse(&cursor, fd_mouse, &ev);
                
                /* Lógica para determinar o ponto inicial e final da janela.
                Principalmente útil quando o segundo clique do usuário ocorre no canto superior esquerdo
                e o primeiro, no canto inferior direito.*/  
                x_min = cursor.x_0 < cursor.x_1 ? cursor.x_0 : cursor.x_1;
                x_max = cursor.x_0 > cursor.x_1 ? cursor.x_0 : cursor.x_1;
            
                y_min = cursor.y_0 < cursor.y_1 ? cursor.y_0 : cursor.y_1;
                y_max = cursor.y_0 > cursor.y_1 ? cursor.y_0 : cursor.y_1;                

                {
                    int x, y, i;
                    int x_mintemp, x_maxtemp;
                    int y_mintemp, y_maxtemp;
                    

                    int w = x_max - x_min;   // largura do recorte
                    int h = y_max - y_min;   // altura do recorte

                    // Offsets para centralizar no buffer 320x240
                    int ofx = (320 - w) / 2;
                    int ofy = (240 - h) / 2;

                    // Copia recorte para o buffer centralizado (buffer_recorte)
                    for (i = 0; i < 76800; i++)
                        buffer_recorte_initial[i] = 0;

                    for (y = 0; y < h; y++) {
                        for (x = 0; x < w; x++) {

                            int addr_dst = (ofy + y) * 320 + (ofx + x);     // destino centralizado
                            int addr_src = (y_min + y) * 320 + (x_min + x); // origem na imagem

                            buffer_recorte_initial[addr_dst] = api_load(0, addr_src);
                        }
                    }
                    
                    // Submenu para selecionar combinação desejada de algoritmos
                    printf("\nSelecione os algoritmos desejados:\n\n");
                    printf("1. Vizinho mais próximo In/ Vizinho mais próximo Out\n");
                    printf("2. Replicação de pixels/ Média de blocos\n");
                    printf("3. Vizinho mais próximo In/ Média de blocos\n");
                    printf("4. Replicação de pixel/ Vizinho mais próximo Out\n\n");
                    printf("Opção: ");
                    scanf(" %d", &opcao);

                    
                    printf("\nModo interativo: use '+' para zoom-in; '-' para zoom-out; outra tecla retorna ao menu.\n");
                    enable_raw_mode(); // ativa modo raw para stdin

                    while (1) {
            
                        int c = getch_raw();
                        
                        // Este trecho foi uma tentativa de implementar o movimento da janela de zoom através das teclas.
                        // Não foi finalizado
                        if (c == 27) { 
                            // tenta ler sequência: '[' + código
                            int c1 = getch_raw();
                            if (c1 == '[') {
                                int c2 = getch_raw();
                                // Tratar setas
                                if (c2 == 'A') { // up
                                    // mover a janela para cima (verifica limites)
                                    if (y_min > 0) {
                                        y_min--;
                                        y_max--;
                                    }
                                } else if (c2 == 'B') { // down
                                    if (y_max < 239) {
                                        y_min++;
                                        y_max++;
                                    }
                                } else if (c2 == 'C') { // right
                                    if (x_max < 319) {
                                        x_min++;
                                        x_max++;
                                    }
                                } else if (c2 == 'D') { // left
                                    if (x_min > 0) {
                                        x_min--;
                                        x_max--;
                                    }
                                }
                                // Após mover, atualize o buffer centralizado novamente (mesma operação acima)
                                // Recalcula w,h,ofx,ofy caso necessário
                                w = x_max - x_min;
                                h = y_max - y_min;
                                ofx = (320 - w) / 2;
                                ofy = (240 - h) / 2;

                                for (y = 0; y < h; y++) {
                                    for (x = 0; x < w; x++) {
                                        int addr_dst = (ofy + y) * 320 + (ofx + x);
                                        int addr_src = (y_min + y) * 320 + (x_min + x);
                                        buffer_recorte_initial[addr_dst] = api_load(0, addr_src);
                                    }
                                }

                                apply_zoom_to_crop(x_min, x_max, y_min, y_max, api_refresh_cpa);
                            }
                        }
                        else if (c == '+') {
                            // zoom in
                            if (count_zoom >= 3){
                                printf("Zoom maximo atingido! \n");
                                
                            }else {
                                
                                count_zoom++;
    
                                zoom_corre++;
                                
                                if(opcao == 1 || opcao == 3) // analisa a opção escolhida anteriormente
                                apply_zoom_to_crop(x_min, x_max, y_min, y_max, api_vmp_in);
                                else
                                apply_zoom_to_crop(x_min, x_max, y_min, y_max, api_repixel);
                                
                                printf("Aplicado zoom-in\n");
                            
                            }
                        }
                        else if (c == '-') {
                            // zoom out
                            if (count_zoom > 1) {
                                count_zoom--;

                                zoom_corre++;

                                if(opcao == 1 || opcao == 4) // analisa a opção escolhida anteriormente
                                    apply_zoom_to_crop(x_min, x_max, y_min, y_max, api_vmp_out);
                                else
                                    apply_zoom_to_crop(x_min, x_max, y_min, y_max, api_mblocos);

                                printf("Aplicado zoom-out\n");
                            } else {
                                printf("Já no nível mínimo de zoom \n");
                            }
                        }
                        else {
                            // qualquer outra tecla encerra o modo interativo e retorna ao menu
                            break;
                        }
                    } // fim while interativo
                    disable_raw_mode(); // restaura terminal
                    count_zoom = 1;
                    zoom_corre = 1;
                    system("clear");
                }
                break;
            
            // Realizar mudança da imagem exibida no VGA    
            case 6:
                printf("Diretório da imagem: ");
                scanf("%s", path_file);

                if (api_store_img(path_file) < 0)
                    printf("Erro ao acessar o arquivo: %s.\n", path_file);

                else {
                    for (i = 0; i < 76800; i++)
                        copy_image[i] = api_load(0,i);

                }
               
                api_refresh_cpa();
                printf("\nImagem atualizada!\n\n");
                system("clear");
                break;

            // Reset
            case 7:
                for (i = 0; i < 76800; i++)
                    api_store(copy_image[i], i);
                api_reset_cpa();
                break;

            // Leitura de um dado da memória
            case 8:
                printf("Selecione a memória (0: A; 1: C): ");
                scanf("%d", &mem_sel);

                printf("Selecione o endereço: ");
                scanf("%d", &addr);

                printf("\nPixel do Endereço %d na Memória %d: %d\n",
                    addr, mem_sel, api_load(mem_sel, addr));       
               
                break;

            // Finaliza programa
            case 0:
                api_finalize();
                printf("Finalizando a API\n");
                flag_stay = false; // Flag atribuída como falsa para sair do loop
                system("clear");
                break;

            default:
                printf("\nOpção inválida!\n");
                break;
        }
    }


    printf("API Finalizada!\n");

    close(fd_mouse);
    return 0;
}
