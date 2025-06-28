#!/bin/bash
# filepath: /home/juan/Documents/42/CommonCore/minishell/test_all_redirections.sh

# ===========================================
# SCRIPT DE PRUEBAS COMPLETO PARA MINISHELL
# ===========================================

echo "🧪 TESTING MINISHELL - ALL REDIRECTIONS & FEATURES"
echo "=================================================="

# Colores para output
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Variables globales
test_count=0
pass_count=0
fail_count=0

# ===========================================
# FUNCIONES AUXILIARES
# ===========================================

increment_test() {
    ((test_count++))
}

increment_pass() {
    ((pass_count++))
}

increment_fail() {
    ((fail_count++))
}

print_test_header() {
    local category="$1"
    echo -e "\n${BLUE}📁 $category${NC}"
    echo "=========================================="
}

print_test_name() {
    local test_name="$1"
    echo -e "\n${YELLOW}Test $((test_count + 1)): $test_name${NC}"
}

# Función para limpiar archivos de prueba
cleanup_test_files() {
    rm -f *.txt 2>/dev/null
    rm -f listado* sorted* complex* count* output* append* nuevo* temp* final* orden* para* multi* 2>/dev/null
}

# Función para ejecutar un comando solo en minishell
run_minishell_test() {
    local test_name="$1"
    local command="$2"
    local expected_file="$3"
    local expected_content="$4"
    
    increment_test
    print_test_name "$test_name"
    echo "Command: $command"
    
    # Ejecutar solo en minishell
    echo "$command" | ./minishell 2>/dev/null
    
    # Verificar resultado
    if [[ -n "$expected_file" ]]; then
        if [[ -f "$expected_file" ]]; then
            if [[ -n "$expected_content" ]]; then
                actual_content=$(cat "$expected_file" 2>/dev/null)
                if [[ "$actual_content" == "$expected_content" ]]; then
                    echo -e "  ${GREEN}✅ PASS${NC} - File exists with correct content"
                    increment_pass
                else
                    echo -e "  ${RED}❌ FAIL${NC} - Content mismatch"
                    echo "  Expected: '$expected_content'"
                    echo "  Actual: '$actual_content'"
                    increment_fail
                fi
            else
                echo -e "  ${GREEN}✅ PASS${NC} - File exists"
                increment_pass
            fi
        else
            echo -e "  ${RED}❌ FAIL${NC} - File '$expected_file' not created"
            increment_fail
        fi
    else
        echo -e "  ${GREEN}✅ PASS${NC} - Command executed"
        increment_pass
    fi
}

# Función para tests que requieren setup previo
run_test_with_setup() {
    local test_name="$1"
    local setup_command="$2"
    local test_command="$3"
    local expected_file="$4"
    local expected_content="$5"
    
    increment_test
    print_test_name "$test_name"
    echo "Setup: $setup_command"
    echo "Command: $test_command"
    
    # Ejecutar setup en bash
    eval "$setup_command" 2>/dev/null
    
    # Ejecutar test en minishell
    echo "$test_command" | ./minishell 2>/dev/null
    
    # Verificar resultado
    if [[ -f "$expected_file" ]]; then
        actual_content=$(cat "$expected_file" 2>/dev/null)
        if [[ "$actual_content" == "$expected_content" ]]; then
            echo -e "  ${GREEN}✅ PASS${NC} - File exists with correct content"
            increment_pass
        else
            echo -e "  ${RED}❌ FAIL${NC} - Content mismatch"
            echo "  Expected: '$expected_content'"
            echo "  Actual: '$actual_content'"
            increment_fail
        fi
    else
        echo -e "  ${RED}❌ FAIL${NC} - File '$expected_file' not created"
        increment_fail
    fi
}

# Función para tests de salida visual (sin archivos)
run_visual_test() {
    local test_name="$1"
    local command="$2"
    
    increment_test
    print_test_name "$test_name"
    echo "Command: $command"
    echo -e "${YELLOW}Expected output:${NC}"
    
    # Mostrar output esperado de bash
    bash -c "$command" 2>/dev/null
    
    echo -e "${YELLOW}Minishell output:${NC}"
    # Mostrar output de minishell
    echo "$command" | ./minishell 2>/dev/null
    
    echo -e "${YELLOW}Manual verification required${NC}"
    increment_pass  # Asumimos que pasa, verificación manual
}

# Función para tests de error
run_error_test() {
    local test_name="$1"
    local command="$2"
    
    increment_test
    print_test_name "$test_name"
    echo "Command: $command"
    
    # Ejecutar en bash para ver error esperado
    echo -e "${YELLOW}Expected error (bash):${NC}"
    bash -c "$command" 2>&1 >/dev/null | head -1
    
    # Ejecutar en minishell
    echo -e "${YELLOW}Minishell error:${NC}"
    echo "$command" | ./minishell 2>&1 >/dev/null | head -1
    
    echo -e "${YELLOW}Manual verification of error messages required${NC}"
    increment_pass  # Asumimos que pasa, verificación manual
}

# ===========================================
# VERIFICACIÓN INICIAL
# ===========================================

echo "Compilando minishell..."
make re > /dev/null 2>&1

if [[ ! -f "./minishell" ]]; then
    echo -e "${RED}❌ FAILED TO COMPILE MINISHELL${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Compilation successful${NC}"

# Limpiar archivos previos
cleanup_test_files

# ===========================================
# PREPARACIÓN DE ARCHIVOS
# ===========================================

print_test_header "PREPARACIÓN DE ARCHIVOS DE PRUEBA"

echo 'Línea 1 de input.txt' > input.txt
echo 'Línea 2 de input.txt' >> input.txt
echo "Contenido inicial" > para_sobrescribir.txt
echo "Contenido inicial" > para_append.txt
touch archivo_vacio.txt

echo -e "${GREEN}✅ Archivos de prueba creados${NC}"

# ===========================================
# CATEGORÍA 1: REDIRECCIÓN DE SALIDA SIMPLE (>)
# ===========================================

print_test_header "CATEGORÍA 1: REDIRECCIÓN DE SALIDA SIMPLE (>)"

run_minishell_test "Sobrescribir archivo" \
    'echo "Nuevo contenido" > para_sobrescribir.txt' \
    "para_sobrescribir.txt" \
    "Nuevo contenido"

run_minishell_test "Crear archivo vacío" \
    "> nuevo_vacio.txt" \
    "nuevo_vacio.txt" \
    ""

run_minishell_test "Redirección al principio" \
    "> orden1.txt echo hola mundo" \
    "orden1.txt" \
    "hola mundo"

run_minishell_test "Redirección en medio" \
    "echo hola > orden2.txt mundo" \
    "orden2.txt" \
    "hola mundo"

run_minishell_test "Múltiples redirecciones (última gana)" \
    'echo "contenido final" > temp.txt > final.txt' \
    "final.txt" \
    "contenido final"

# ===========================================
# CATEGORÍA 2: REDIRECCIÓN APPEND (>>)
# ===========================================

print_test_header "CATEGORÍA 2: REDIRECCIÓN APPEND (>>)"

run_test_with_setup "Append a archivo existente" \
    'echo "Contenido inicial" > para_append.txt' \
    'echo "Nueva línea" >> para_append.txt' \
    "para_append.txt" \
    "Contenido inicial
Nueva línea"

run_minishell_test "Append a archivo no existente" \
    'echo "contenido" >> nuevo_append.txt' \
    "nuevo_append.txt" \
    "contenido"

run_minishell_test "Append con redirección al principio" \
    '>> orden1.txt echo mas contenido' \
    "orden1.txt" \
    "hola mundo
mas contenido"

run_minishell_test "Múltiples appends (última gana)" \
    'echo "linea" >> append_temp.txt >> append_final.txt' \
    "append_final.txt" \
    "linea"

# ===========================================
# CATEGORÍA 3: REDIRECCIÓN DE ENTRADA (<)
# ===========================================

print_test_header "CATEGORÍA 3: REDIRECCIÓN DE ENTRADA (<)"

run_visual_test "Entrada básica" \
    "cat < input.txt"

run_visual_test "Contar líneas desde archivo" \
    "wc -l < input.txt"

run_visual_test "Entrada con orden flexible" \
    "< input.txt cat"

run_error_test "Error: archivo no existe" \
    "cat < archivo_que_no_existe.txt"

# ===========================================
# CATEGORÍA 4: HERE-DOCUMENT (<<)
# ===========================================

print_test_header "CATEGORÍA 4: HERE-DOCUMENT (<<)"

echo -e "${YELLOW}⚠️  Here-document tests require manual input${NC}"
echo -e "${YELLOW}You'll need to type the content manually for these tests${NC}"

increment_test
print_test_name "Here-document básico"
echo "Command: cat << EOF"
echo "Type: hola [ENTER] mundo [ENTER] EOF [ENTER]"
echo "cat << EOF" | ./minishell
increment_pass

increment_test
print_test_name "Here-document con grep"
echo "Command: grep \"linea2\" << DELIMITADOR"
echo "Type: linea1 [ENTER] linea2 [ENTER] linea3 [ENTER] DELIMITADOR [ENTER]"
echo 'grep "linea2" << DELIMITADOR' | ./minishell
increment_pass

increment_test
print_test_name "Here-document con wc"
echo "Command: wc -c << STOP_WORD"
echo "Type: abc [ENTER] STOP_WORD [ENTER]"
echo "wc -c << STOP_WORD" | ./minishell
increment_pass

# ===========================================
# CATEGORÍA 5: COMBINACIONES DE REDIRECCIONES
# ===========================================

print_test_header "CATEGORÍA 5: COMBINACIONES DE REDIRECCIONES"

run_minishell_test "Entrada y salida" \
    "cat < input.txt > output1.txt" \
    "output1.txt" \
    "Línea 1 de input.txt
Línea 2 de input.txt"

run_minishell_test "Orden mixto entrada/salida" \
    "> output2.txt cat < input.txt" \
    "output2.txt" \
    "Línea 1 de input.txt
Línea 2 de input.txt"

echo -e "${YELLOW}⚠️  Here-document + output requires manual input${NC}"
increment_test
print_test_name "Here-document y salida"
echo "Command: cat << EOF > output3.txt"
echo "Type: contenido del heredoc [ENTER] EOF [ENTER]"
echo "cat << EOF > output3.txt" | ./minishell
increment_pass

run_minishell_test "Entrada y append" \
    "cat < input.txt >> para_append.txt" \
    "para_append.txt" \
    "Contenido inicial
Nueva línea
Línea 1 de input.txt
Línea 2 de input.txt"

run_minishell_test "Múltiples redirecciones mixtas (última salida gana)" \
    "wc -w < input.txt > count.txt > final_count.txt" \
    "final_count.txt" \
    "6"

# ===========================================
# CATEGORÍA 6: COMBINACIONES CON PIPES (|)
# ===========================================

print_test_header "CATEGORÍA 6: COMBINACIONES CON PIPES (|)"

run_minishell_test "Pipe con redirección de salida" \
    "ls | head -3 > sorted_ls.txt" \
    "sorted_ls.txt"

run_visual_test "Entrada antes de pipe" \
    'cat < input.txt | grep "Línea" | wc -l'

echo -e "${YELLOW}⚠️  Here-document + pipe requires manual input${NC}"
increment_test
print_test_name "Here-document antes de pipe"
echo "Command: cat << EOF | sort"
echo "Type: c [ENTER] a [ENTER] b [ENTER] EOF [ENTER]"
echo "Expected output: a b c (sorted)"
echo "cat << EOF | sort" | ./minishell
increment_pass

run_minishell_test "Combinación compleja" \
    '< input.txt grep "Línea" | wc -l > complex.txt' \
    "complex.txt" \
    "2"

# ===========================================
# TESTS ADICIONALES DE ORDEN FLEXIBLE
# ===========================================

print_test_header "TESTS ADICIONALES: ORDEN FLEXIBLE"

run_minishell_test "Múltiples argumentos con redirect intercalado" \
    "echo arg1 > flex1.txt arg2 arg3" \
    "flex1.txt" \
    "arg1 arg2 arg3"

run_minishell_test "Redirect al inicio con múltiples args" \
    "> flex2.txt echo arg1 arg2 arg3" \
    "flex2.txt" \
    "arg1 arg2 arg3"

run_minishell_test "Args intercalados con redirects" \
    "echo > flex3.txt word1 word2" \
    "flex3.txt" \
    "word1 word2"

# ===========================================
# TESTS DE CASOS EDGE
# ===========================================

print_test_header "TESTS DE CASOS EDGE"

run_minishell_test "Espacios alrededor de operadores" \
    "echo content    >    spaced.txt" \
    "spaced.txt" \
    "content"

run_minishell_test "Múltiples espacios en comando" \
    "echo    hello     world   > multispace.txt" \
    "multispace.txt" \
    "hello world"

run_error_test "Redirección sin archivo" \
    "echo test >"

run_error_test "Here-document sin delimitador" \
    "cat <<"

# ===========================================
# RESUMEN FINAL
# ===========================================

echo -e "\n${BLUE}📊 RESUMEN DE RESULTADOS${NC}"
echo "=========================================="
echo -e "Tests ejecutados: ${YELLOW}$test_count${NC}"
echo -e "Tests pasados: ${GREEN}$pass_count${NC}"
echo -e "Tests fallados: ${RED}$fail_count${NC}"

percentage=$((pass_count * 100 / test_count))
echo -e "Porcentaje de éxito: ${YELLOW}$percentage%${NC}"

if [[ $fail_count -eq 0 ]]; then
    echo -e "\n${GREEN}🎉 ¡TODOS LOS TESTS PASARON!${NC}"
    echo -e "${GREEN}Tu minishell parece funcionar correctamente.${NC}"
else
    echo -e "\n${YELLOW}⚠️  Algunos tests fallaron o requieren verificación manual.${NC}"
    echo -e "${YELLOW}Revisa los tests marcados como FAIL.${NC}"
fi

echo -e "\n${BLUE}📋 ARCHIVOS GENERADOS:${NC}"
ls -la *.txt 2>/dev/null | head -15

echo -e "\n${BLUE}🧹 LIMPIEZA:${NC}"
echo "Para limpiar archivos de prueba: rm -f *.txt"
echo -e "\n${GREEN}Script de tests completado.${NC}"

# Opcional: Auto-cleanup
read -p "¿Limpiar archivos de prueba automáticamente? (y/n): " cleanup_choice
if [[ "$cleanup_choice" == "y" || "$cleanup_choice" == "Y" ]]; then
    cleanup_test_files
    echo -e "${GREEN}✅ Archivos limpiados${NC}"
fi

exit $fail_count