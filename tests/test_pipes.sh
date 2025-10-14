#!/bin/bash
# Script de pruebas para Pipes
# Sistema Operativos - UNJBG 2025-I

echo "========================================="
echo "  MINI-SHELL - PRUEBAS DE PIPES"
echo "========================================="
echo

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Verificar ejecutable
if [ ! -f "./mini-shell" ]; then
    echo -e "${RED}Error: mini-shell no encontrado${NC}"
    exit 1
fi

echo -e "${YELLOW}Preparando archivos de prueba...${NC}"
# Crear archivo de prueba
cat > /tmp/test_pipe.txt << EOF
apple
banana
cherry
apricot
blueberry
avocado
EOF

echo -e "${GREEN}✓ Archivo de prueba creado${NC}"
echo

# Test 1: Pipe simple
echo -e "${YELLOW}Test 1: Pipe Simple (ls | grep)${NC}"
echo "-----------------------------------"
echo "Comando: ls | grep .sh"
echo "ls | grep .sh
salir" | ./mini-shell
echo

# Test 2: Pipe con wc
echo -e "${YELLOW}Test 2: Contar líneas con pipe${NC}"
echo "-----------------------------------"
echo "Comando: cat /tmp/test_pipe.txt | wc -l"
echo "cat /tmp/test_pipe.txt | wc -l
salir" | ./mini-shell
echo

# Test 3: Pipe doble
echo -e "${YELLOW}Test 3: Pipe Doble${NC}"
echo "-----------------------------------"
echo "Comando: cat /tmp/test_pipe.txt | grep a | wc -l"
echo "cat /tmp/test_pipe.txt | grep a | wc -l
salir" | ./mini-shell
echo

# Test 4: Pipe con sort
echo -e "${YELLOW}Test 4: Pipe con sort${NC}"
echo "-----------------------------------"
echo "Comando: cat /tmp/test_pipe.txt | sort"
echo "cat /tmp/test_pipe.txt | sort
salir" | ./mini-shell
echo

# Test 5: Pipe con redirección
echo -e "${YELLOW}Test 5: Pipe + Redirección${NC}"
echo "-----------------------------------"
echo "Comando: ls | grep .cpp > /tmp/cpp_files.txt"
echo "ls | grep .cpp > /tmp/cpp_files.txt
salir" | ./mini-shell

if [ -f "/tmp/cpp_files.txt" ]; then
    echo -e "${GREEN}✓ Archivo de salida creado${NC}"
    echo "Contenido:"
    cat /tmp/cpp_files.txt
else
    echo -e "${RED}✗ Error: Archivo no creado${NC}"
fi
echo

# Test 6: Pipe con head y tail
echo -e "${YELLOW}Test 6: Pipe con head${NC}"
echo "-----------------------------------"
echo "Comando: cat /tmp/test_pipe.txt | head -3"
echo "cat /tmp/test_pipe.txt | head -3
salir" | ./mini-shell
echo

# Limpieza
rm -f /tmp/test_pipe.txt /tmp/cpp_files.txt

echo "========================================="
echo -e "${GREEN}Pruebas de pipes completadas${NC}"
echo "========================================="