.arch armv7-a
    .syntax unified
    .cpu cortex-a9
    .text

    /* ----------- Constantes --------------- */
    .equ SYS_EXIT,    1
    .equ SYS_OPEN,    5
    .equ SYS_CLOSE,   6
    .equ SYS_MUNMAP,  91
    .equ SYS_MMAP2,   192
    .equ SYS_LSEEK, 19
    .equ SYS_READ,  3

    .equ O_RDWR,      2
    .equ PROT_READ,   1
    .equ PROT_WRITE,  2
    .equ MAP_SHARED,  1
    .equ PROT_RW, (PROT_READ | PROT_WRITE)

    .equ LW_BRIDGE_BASE,  0xFF200000
    .equ LW_BRIDGE_SPAN,  0x00005000

    .equ PIO_INSTRUCTION_BASE, 0x00000000
    .equ PIO_ENABLE_BASE,      0x00000010
    .equ PIO_DATAOUT,          0x00000020
    .equ PIO_FLAGS,            0x00000030

    .equ INSTRUCTION_REFRESH, 0x00000000
    .equ INSTRUCTION_LOAD,    0x00000001
    .equ INSTRUCTION_STORE,   0x00000002  
    .equ INSTRUCTION_VMPIN,   0x00000003
    .equ INSTRUCTION_REPIXEL, 0x00000004
    .equ INSTRUCTION_VMPOUT,  0x00000005
    .equ INSTRUCTION_MBLOCOS, 0x00000006
    .equ INSTRUCTION_RESET,   0x00000007

    .data
dev_mem:            .asciz "/dev/mem"
fd_word:            .word 0
lw_bridge_addr:     .word 0
pio_instruction_ptr:.word 0
pio_enable_ptr:     .word 0
pio_dataout_ptr:    .word 0
pio_flags_ptr:      .word 0

    .text
    .global api_init
    .type api_init, %function
api_init:
    /* preservar callee-saved */
    push {r4-r11, lr}

    /* open("/dev/mem", O_RDWR, 0) */
    ldr r0, =dev_mem
    mov r1, #O_RDWR
    mov r2, #0
    mov r7, #SYS_OPEN
    svc #0
    
    /* salvar fd em r8 IMEDIATAMENTE após syscall */
    mov r8, r0
    
    /* verificar se open falhou */
    cmp r0, #0
    blt .api_init_err
    
    /* salvar fd em fd_word */
    ldr r1, =fd_word
    str r8, [r1]

    /* mmap2(NULL, LW_BRIDGE_SPAN, PROT_RW, MAP_SHARED, fd, pgoff) */
    mov r0, #0                   /* addr = NULL */
    ldr r1, =LW_BRIDGE_SPAN      /* length */
    mov r2, #PROT_RW             /* prot = PROT_READ | PROT_WRITE */
    mov r3, #MAP_SHARED          /* flags */
    mov r4, r8                   /* fd */
    
    /* compute pgoff = LW_BRIDGE_BASE >> 12 */
    ldr r5, =LW_BRIDGE_BASE
    lsr r5, r5, #12              /* pgoff = base >> 12 */

    mov r7, #SYS_MMAP2
    svc #0
    
    /* salvar resultado do mmap ANTES de comparar */
    mov r9, r0
    
    /* verificar se mmap2 falhou (endereços negativos ou -1) */
    cmn r0, #4096
    bhi .api_init_mmap_err

    /* salvar lw_bridge_addr */
    ldr r1, =lw_bridge_addr
    str r9, [r1]

    /* Calcular e salvar ponteiros dos PIOs */
    add r1, r9, #PIO_INSTRUCTION_BASE
    add r2, r9, #PIO_ENABLE_BASE
    add r3, r9, #PIO_DATAOUT
    add r4, r9, #PIO_FLAGS

    ldr r10, =pio_instruction_ptr
    str r1, [r10]
    
    ldr r10, =pio_enable_ptr
    str r2, [r10]
    
    ldr r10, =pio_dataout_ptr
    str r3, [r10]
    
    ldr r10, =pio_flags_ptr
    str r4, [r10]

    mov r0, #0          /* success return 0 */
    b .api_init_done

.api_init_mmap_err:
    /* fechar fd antes de retornar erro */
    mov r0, r8
    mov r7, #SYS_CLOSE
    svc #0
    mov r0, #-1
    b .api_init_done

.api_init_err:
    mov r0, #-1

.api_init_done:
    pop {r4-r11, pc}

    .size api_init, .-api_init


    .global api_finalize
    .type api_finalize, %function
api_finalize:
    push {r4-r11, lr}

    /* munmap(lw_bridge, LW_BRIDGE_SPAN) */
    ldr r0, =lw_bridge_addr
    ldr r0, [r0]
    ldr r1, =LW_BRIDGE_SPAN
    mov r7, #SYS_MUNMAP
    svc #0

    /* close(fd) */
    ldr r0, =fd_word
    ldr r0, [r0]
    mov r7, #SYS_CLOSE
    svc #0

    mov r0, #0
    pop {r4-r11, pc}

    .size api_finalize, .-api_finalize

    /* ---- rotina de polling comum ----
       testa bit0 = done */
    .global wait_done
wait_done:
    push {r4, lr}

    ldr r4, =pio_flags_ptr
    ldr r4, [r4]        /* carregar o endereço do registro de flags */
    
.wait_loop:
    ldr r2, [r4]        /* ler flags */
    tst r2, #1          /* bit0 = done? */
    beq .wait_loop
    
    pop {r4, pc}

    .global api_reset_cpa
    .type api_reset_cpa, %function
api_reset_cpa:
    push {r4-r7, lr}

    /* write instruction reset */
    ldr r3, =pio_instruction_ptr
    ldr r3, [r3]
    mov r4, #INSTRUCTION_RESET
    str r4, [r3]

    /* enable = 1 */
    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #1
    str r4, [r3]

    bl wait_done

    /* disable enable */
    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #0
    str r4, [r3]

    mov r0, #0
    pop {r4-r7, pc}

    .size api_reset_cpa, .-api_reset_cpa

    .global api_refresh_cpa
    .type api_refresh_cpa, %function
api_refresh_cpa:
    push {r4-r7, lr}
    
    ldr r3, =pio_instruction_ptr
    ldr r3, [r3]
    mov r4, #INSTRUCTION_REFRESH
    str r4, [r3]
    
    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #1
    str r4, [r3]

    bl wait_done

    /* disable enable */
    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #0
    str r4, [r3]

    mov r0, #0
    pop {r4-r7, pc}

    .size api_refresh_cpa, .-api_refresh_cpa

    .global api_load
    .type api_load, %function
api_load:
    push {r4-r11, lr}
    
    /* Deslocamento e junção dos parâmetros de endereço e seleção de memória em um registrador */
    lsl r8, r0, #20
    lsl r9, r1, #3
    orr r10, r8, r9 

    /* escreve INSTRUCTION_LOAD com mem_sel e endereços*/
    ldr r3, =pio_instruction_ptr
    ldr r3, [r3]
    orr r4, r10, #INSTRUCTION_LOAD 
    str r4, [r3]

    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #1
    str r4, [r3]

    bl wait_done

    /* disable enable */
    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #0
    str r4, [r3]

    /* read dataout e retorna em r0 */
    ldr r3, =pio_dataout_ptr
    ldr r3, [r3]
    ldr r0, [r3]
    
    pop {r4-r11, pc}

    .size api_load, .-api_load

    .global api_store_img
    .type api_store_img, %function
api_store_img:
    push {r4-r11, lr}

    /* r0 = path_file */
    mov r1, #O_RDWR
    mov r2, #0
    mov r7, #SYS_OPEN
    svc #0
    cmp r0, #0
    blt .store_err
    mov r8, r0              @ fd BMP

    /* ponteiros de PIO */
    ldr r9, =pio_instruction_ptr
    ldr r9, [r9]
    ldr r10, =pio_enable_ptr
    ldr r10, [r10]

    /* reservar buffers */
    sub sp, sp, #72         @ espaço para header (54B) + buffer
    mov r11, sp             @ r11 = buffer base

    /* ler cabeçalho BMP */
    mov r0, r8
    mov r1, r11
    mov r2, #54
    mov r7, #SYS_READ
    svc #0
    cmp r0, #54
    blt .header_err

    /* ler bfOffBits (bytes 10-13) */
    add r4, r11, #10
    ldrb r5, [r4]
    ldrb r6, [r4, #1]
    ldrb r7, [r4, #2]
    ldrb r12, [r4, #3]
    mov r0, r12
    lsl r0, r0, #24
    mov r1, r7
    lsl r1, r1, #16
    orr r0, r0, r1
    mov r1, r6
    lsl r1, r1, #8
    orr r0, r0, r1
    orr r0, r0, r5          @ r0 = bfOffBits

    /* verificar bitsPerPixel (byte 28) */
    ldrb r1, [r11, #28]
    cmp r1, #24
    bne .unsupported_bpp

    /* mover cursor p/ bfOffBits */
    mov r7, #SYS_LSEEK
    mov r1, r0
    mov r0, r8
    mov r2, #0
    svc #0

    mov r5, #0              @ endereço pixel
    mov r6, #76800          @ total pixels (320*240)

.loop_pixels:
    cmp r5, r6
    bge .done_pixels

    /* ler 3 bytes: B, G, R */
    mov r0, r8
    mov r1, r11
    mov r2, #3
    mov r7, #SYS_READ
    svc #0
    cmp r0, #3
    bne .read_error

    /* carregar B, G, R */
    ldrb r0, [r11]          @ B
    ldrb r1, [r11, #1]      @ G
    ldrb r2, [r11, #2]      @ R

    mov r3, #30
    mul r12, r2, r3         @ R*30

    mov r3, #59
    mla r12, r1, r3, r12    @ +G*59

    mov r3, #11
    mla r12, r0, r3, r12    @ +B*11

    mov r4, r12, lsr #7     @ dividir por ~128 (≈100)


    /* montar instrução completa */
    lsl r1, r4, #21         @ pixel << 21
    lsl r2, r5, #3          @ endereço << 3
    orr r1, r1, r2
    orr r1, r1, #2          @ opcode STORE (0b010)

    /* enviar para o pio_instruction */
    str r1, [r9]

    /* enable=1 -> start */
    mov r2, #1
    str r2, [r10]
    bl wait_done
    mov r2, #0
    str r2, [r10]

    add r5, r5, #1
    b .loop_pixels

.done_pixels:
    /* fechar arquivo */
    mov r0, r8
    mov r7, #SYS_CLOSE
    svc #0

    add sp, sp, #72
    mov r0, #0
    pop {r4-r11, pc}

.header_err:
.unsupported_bpp:
.read_error:
    mov r0, r8
    mov r7, #SYS_CLOSE
    svc #0
    add sp, sp, #72
    mov r0, #-1
    pop {r4-r11, pc}

.store_err:
    mov r0, #-1
    pop {r4-r11, pc}
.size api_store_img, .-api_store_img

    .global api_vmp_in
    .type api_vmp_in, %function
api_vmp_in:
    push {r4-r7, lr}
    
    ldr r3, =pio_instruction_ptr
    ldr r3, [r3]
    mov r4, #INSTRUCTION_VMPIN
    str r4, [r3]

    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #1
    str r4, [r3]

    bl wait_done

    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #0
    str r4, [r3]

    mov r0, #0
    pop {r4-r7, pc}
    .size api_vmp_in, .-api_vmp_in

    .global api_vmp_out
    .type api_vmp_out, %function
api_vmp_out:
    push {r4-r7, lr}
    
    ldr r3, =pio_instruction_ptr
    ldr r3, [r3]
    mov r4, #INSTRUCTION_VMPOUT
    str r4, [r3]

    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #1
    str r4, [r3]

    bl wait_done

    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #0
    str r4, [r3]

    mov r0, #0
    pop {r4-r7, pc}
    .size api_vmp_out, .-api_vmp_out

    .global api_repixel
    .type api_repixel, %function
api_repixel:
    push {r4-r7, lr}
    
    ldr r3, =pio_instruction_ptr
    ldr r3, [r3]
    mov r4, #INSTRUCTION_REPIXEL
    str r4, [r3]

    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #1
    str r4, [r3]

    bl wait_done

    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #0
    str r4, [r3]

    mov r0, #0
    pop {r4-r7, pc}
    .size api_repixel, .-api_repixel

    .global api_mblocos
    .type api_mblocos, %function
api_mblocos:
    push {r4-r7, lr}
    
    ldr r3, =pio_instruction_ptr
    ldr r3, [r3]
    mov r4, #INSTRUCTION_MBLOCOS
    str r4, [r3]
    
    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #1
    str r4, [r3]

    bl wait_done

    ldr r3, =pio_enable_ptr
    ldr r3, [r3]
    mov r4, #0
    str r4, [r3]

    mov r0, #0
    pop {r4-r7, pc}
    .size api_mblocos, .-api_mblocos


    .global api_update_cursor
    .type api_update_cursor, %function
api_update_cursor:
    push {r4-r11, lr}

    /* ponteiros de PIO */
    ldr r4, =pio_instruction_ptr
    ldr r4, [r4]
    ldr r5, =pio_enable_ptr
    ldr r5, [r5]

    lsl r8, r0, #3
    lsl r9, r1, #21
    orr r10, r8, r9

    mov r8, #1
    lsl r8, r8, #20
    orr r10, r10, r8 

    orr r10, r10, #INSTRUCTION_REFRESH  @ adiciona opcode REFRESH (0)
    
    /* Escrever instrução */
    str r10, [r4]

    /* Enable = 1 (ativa a instrução) */
    mov r3, #1
    str r3, [r5]

    /* Aguardar conclusão */
    bl wait_done

    /* Disable enable */
    mov r3, #0
    str r3, [r5]

    mov r0, #0              @ retorna sucesso
    pop {r4-r11, pc}

    .size api_update_cursor, .-api_update_cursor

    .global api_store
    .type api_store, %function


api_store:
    push {r4-r11, lr}

    /* ponteiros de PIO */
    ldr r9, =pio_instruction_ptr
    ldr r9, [r9]
    ldr r10, =pio_enable_ptr
    ldr r10, [r10]

    lsl r0, r0, #21 
    lsl r1, r1, #3
    orr r8, r0, #INSTRUCTION_STORE
    orr r8, r8, r1
    str r8, [r9]

    mov r8, #1
    str r8, [r10]
    
    bl wait_done

    mov r8, #0
    str r8, [r10]

    mov r0, #0
    pop {r4-r11, pc}
    .size api_store, .-api_store
