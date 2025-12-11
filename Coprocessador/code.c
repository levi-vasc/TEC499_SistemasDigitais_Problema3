#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

#define LW_BRIDGE_BASE  0xFF200000
#define LW_BRIDGE_SPAN  0x00005000

#define PIO_INSTRUCTION_BASE   0x00000000
#define PIO_ENABLE_BASE        0x00000010
#define PIO_DATAOUT_BASE       0x00000020
#define PIO_FLAGS_BASE         0x00000030

int main() {
    int fd = open("/dev/mem", (O_RDWR | O_SYNC));
    if (fd < 0) {
        perror("Erro ao abrir /dev/mem");
        return -1;
    }

    void *lw_bridge = mmap(NULL, LW_BRIDGE_SPAN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, LW_BRIDGE_BASE);
    if (lw_bridge == MAP_FAILED) {
        perror("Erro no mmap");
        close(fd);
        return -1;
    }

    volatile uint32_t *pio_instruction = (volatile uint32_t *)(lw_bridge + PIO_INSTRUCTION_BASE);
    volatile uint32_t *pio_enable      = (volatile uint32_t *)(lw_bridge + PIO_ENABLE_BASE);
    volatile uint32_t *pio_dataout     = (volatile uint32_t *)(lw_bridge + PIO_DATAOUT_BASE);
    volatile uint32_t *pio_flags       = (volatile uint32_t *)(lw_bridge + PIO_FLAGS_BASE);

    // Exemplo de escrita
    *pio_instruction = 0x000000FF;
    *pio_enable = 1;
    *pio_dataout = 0x00000003;

    printf("Valores enviados:\n");
    printf("instruction = 0x%08X\n", *pio_instruction);
    usleep(1000000);
    *pio_enable = 0;

    munmap(lw_bridge, LW_BRIDGE_SPAN);
    close(fd);
    return 0;
}

