#!/bin/bash
# Script de pruebas básicas para Mini-Shell
# Sistema Operativos - UNJBG 2025-I

echo "========================================="
echo "  MINI-SHELL - PRUEBAS BÁSICAS"
echo "========================================="
echo

# Colores
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Contador de pruebas
PASSED=0
FAILED=0

# Función para ejecutar prueba
test_command() {
    local test_name=$1
    local command=$2
    local expected=$3
    
    echo -n "Prueba: $test_name ... "
    
    # Ejecutar comando
    echo "$command" | ./mini-shell > /tmp/mini_shell_test.out 2>&1
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}✗ FAIL${NC}"
        ((FAILED++))
    fi
}

# Verificar que el ejecutable existe
if [ ! -f "./mini-shell" ]; then
    echo -e "${RED}Error: Ejecutable mini-shell no encontrado${NC}"
    echo "Por favor, ejecuta 'make' primero"
    exit 1
fi

echo -e "${YELLOW}1. Pruebas de Comandos Básicos${NC}"
echo "-----------------------------------"

# Test 1: pwd
test_command "Comando pwd" "pwd\nsalir"

# Test 2: echo
test_command "Comando echo" "echo Hola Mundo\nsalir"

# Test 3: ls
test_command "Comando ls" "ls\nsalir"

echo

echo -e "${YELLOW}2. Pruebas de Redirección${NC}"
echo "-----------------------------------"

# Test 4: Redirección de salida
echo "ls
salir" | ./mini-shell > /dev/null 2>&1
test_command "Redirección >" "ls > /tmp/test_redir.txt\nsalir"
if [ -f "/tmp/test_redir.txt" ]; then
    echo -e "${GREEN}  Archivo creado correctamente${NC}"
    rm -f /tmp/test_redir.txt
else
    echo -e "${RED}  Error: Archivo no creado${NC}"
fi

echo

echo -e "${YELLOW}3. Pruebas de Comandos Built-in${NC}"
echo "-----------------------------------"

# Test 5: help
test_command "Comando help" "help\nsalir"

# Test 6: cd
test_command "Comando cd" "cd /tmp\npwd\nsalir"

# Test 7: meminfo
test_command "Comando meminfo" "meminfo\nsalir"

echo

echo -e "${YELLOW}4. Pruebas de Rutas${NC}"
echo "-----------------------------------"

# Test 8: Ruta absoluta
test_command "Ruta absoluta /bin/ls" "/bin/ls\nsalir"

# Test 9: Ruta relativa
test_command "Ruta relativa date" "date\nsalir"

echo
echo "========================================="
echo "  RESUMEN DE PRUEBAS"
echo "========================================="
echo -e "${GREEN}Pruebas exitosas: $PASSED${NC}"
echo -e "${RED}Pruebas fallidas: $FAILED${NC}"
echo

# Limpieza
rm -f /tmp/mini_shell_test.out

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}¡Todas las pruebas pasaron!${NC}"
    exit 0
else
    echo -e "${RED}Algunas pruebas fallaron${NC}"
    exit 1
fi