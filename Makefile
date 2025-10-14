# Makefile para Mini-Shell
# Sistema Operativos - UNJBG

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -I./include -pthread
LDFLAGS = -pthread

# Directorios
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
BIN_DIR = .

# Archivos fuente
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Ejecutable
TARGET = $(BIN_DIR)/mini-shell

# Colores para output
COLOR_GREEN = \033[0;32m
COLOR_BLUE = \033[0;34m
COLOR_RESET = \033[0m

# Regla por defecto
all: $(TARGET)

# Crear directorio de objetos si no existe
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Compilar objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "$(COLOR_BLUE)Compilando $<...$(COLOR_RESET)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Enlazar ejecutable
$(TARGET): $(OBJS)
	@echo "$(COLOR_GREEN)Enlazando $(TARGET)...$(COLOR_RESET)"
	@$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "$(COLOR_GREEN)¡Compilación exitosa!$(COLOR_RESET)"

# Limpiar archivos generados
clean:
	@echo "Limpiando archivos..."
	@rm -rf $(OBJ_DIR) $(TARGET)
	@echo "$(COLOR_GREEN)Limpieza completa$(COLOR_RESET)"

# Ejecutar el programa
run: $(TARGET)
	@./$(TARGET)

# Recompilar todo
rebuild: clean all

# Información de compilación
info:
	@echo "==================================="
	@echo "  Mini-Shell - Build Information"
	@echo "==================================="
	@echo "Compilador: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Fuentes: $(SRCS)"
	@echo "Objetos: $(OBJS)"
	@echo "Target: $(TARGET)"
	@echo "==================================="

# Ayuda
help:
	@echo "Opciones disponibles:"
	@echo "  make          - Compilar el proyecto"
	@echo "  make run      - Compilar y ejecutar"
	@echo "  make clean    - Limpiar archivos compilados"
	@echo "  make rebuild  - Recompilar todo"
	@echo "  make info     - Mostrar información del build"
	@echo "  make help     - Mostrar esta ayuda"

# Declarar targets que no son archivos
.PHONY: all clean run rebuild info help