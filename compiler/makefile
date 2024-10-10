# Variáveis
CC = g++
CFLAGS = -Wall

# Arquivos de saída
TARGET = exec

# Lista de arquivos fonte
SRCS = main.cpp tokens.cpp fsm.cpp symbols_table.cpp
OBJS = $(SRCS:.cpp=.o)

# Regra padrão para construir o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regra para compilar os arquivos .cpp em .o
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Limpa os arquivos gerados (objetos e binário)
clean:
	rm -f $(OBJS) $(TARGET)
