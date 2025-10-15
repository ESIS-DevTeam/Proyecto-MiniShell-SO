#!/bin/bash
# Script de instalación para Mini-Shell
# Sistema Operativos - UNJBG 2025-I

set -e  # Salir si hay errores

# Colores
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}"
echo "╔════════════════════════════════════════╗"
echo "║   MINI-SHELL - INSTALACIÓN             ║"
echo "║   Sistema Operativos - UNJBG           ║"
echo "╚════════════════════════════════════════╝"
echo -e "${NC}"

# Verificar sistema operativo
echo -e "${YELLOW}[1/5] Verificando sistema operativo...${NC}"
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo -e "${RED}Error: Este proyecto solo funciona en Linux${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Sistema Linux detectado${NC}"
echo

# Verificar dependencias
echo -e "${YELLOW}[2/5] Verificando dependencias...${NC}"

# Verificar g++
if ! command -v g++ &> /dev/null; then
    echo -e "${RED}Error: g++ no está instalado${NC}"
    echo "Instalar con: sudo apt-get install build-essential"
    exit 1
fi
echo -e "${GREEN}✓ g++ encontrado: $(g++ --version | head -n1)${NC}"

# Verificar make
if ! command -v make &> /dev/null; then
    echo -e "${RED}Error: make no está instalado${NC}"
    echo "Instalar con: sudo apt-get install make"
    exit 1
fi
echo -e "${GREEN}✓ make encontrado: $(make --version | head -n1)${NC}"
echo

# Crear estructura de directorios
echo -e "${YELLOW}[3/5] Creando estructura de directorios...${NC}"
mkdir -p obj
mkdir -p docs
mkdir -p tests

# Hacer ejecutables los scripts de prueba
if [ -f "tests/test_basico.sh" ]; then
    chmod +x tests/test_basico.sh
    echo -e "${GREEN}✓ test_basico.sh ejecutable${NC}"
fi

if [ -f "tests/test_pipes.sh" ]; then
    chmod +x tests/test_pipes.sh
    echo -e "${GREEN}✓ test_pipes.sh ejecutable${NC}"
fi
echo

# Compilar proyecto
echo -e "${YELLOW}[4/5] Compilando proyecto...${NC}"
make clean > /dev/null 2>&1 || true
if make; then
    echo -e "${GREEN}✓ Compilación exitosa${NC}"
else
    echo -e "${RED}✗ Error en compilación${NC}"
    exit 1
fi
echo

# Verificar ejecutable
echo -e "${YELLOW}[5/5] Verificando instalación...${NC}"
if [ -f "./mini-shell" ]; then
    echo -e "${GREEN}✓ Ejecutable mini-shell creado${NC}"
    echo -e "${GREEN}✓ Tamaño: $(du -h mini-shell | cut -f1)${NC}"
else
    echo -e "${RED}✗ Error: Ejecutable no encontrado${NC}"
    exit 1
fi
echo

# Resumen
echo -e "${CYAN}"
echo "╔════════════════════════════════════════╗"
echo "║   INSTALACIÓN COMPLETADA               ║"
echo "╚════════════════════════════════════════╝"
echo -e "${NC}"

echo -e "${GREEN}Mini-Shell está listo para usar!${NC}"
echo
echo "Para ejecutar:"
echo -e "  ${BLUE}./mini-shell${NC}"
echo
echo "Para ejecutar pruebas:"
echo -e "  ${BLUE}./tests/test_basico.sh${NC}"
echo -e "  ${BLUE}./tests/test_pipes.sh${NC}"
echo
echo "Para limpiar:"
echo -e "  ${BLUE}make clean${NC}"
echo
echo "Para ver ayuda:"
echo -e "  ${BLUE}make help${NC}"
echo