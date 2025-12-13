# ============================================
# Makefile para DE1-SoC (ARM Cortex-A9)
# Compila main.c e api.s
# ============================================

# Compilador cruzado (para compilar no PC x86 e rodar na DE1-SoC)
CC = arm-linux-gnueabihf-gcc

# Flags de compilação
CFLAGS = -Wall -O2

# Arquivos fonte
SRC_C = main.c
SRC_S = api.s

# Cabeçalhos
HDR = api.h

# Nome do executável final
TARGET = main

# --------------------------------------------
# Regras
# --------------------------------------------

all: $(TARGET)

$(TARGET): $(SRC_C) $(SRC_S) $(HDR)
	$(CC) $(CFLAGS) $(SRC_C) $(SRC_S) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)

# --------------------------------------------
# Notas:
# make       → compila o projeto
# make clean → remove binários
# make run   → executa localmente (útil se compilar na própria DE1-SoC)
# --------------------------------------------
