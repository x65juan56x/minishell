Documentación del Tokenizador de Minishell
¿Qué es la Tokenización?
La tokenización es el primer paso en el procesamiento de comandos de una shell. Consiste en dividir una cadena de entrada del usuario en unidades más pequeñas llamadas tokens, cada una con un significado específico.

Ejemplo Visual:
Input:  echo "hello world" | grep test > output.txt

Se convierte en:
TOKEN_WORD:        "echo"
TOKEN_WORD:        "hello world"    # Sin comillas, contenido procesado
TOKEN_PIPE:        "|"
TOKEN_WORD:        "grep" 
TOKEN_WORD:        "test"
TOKEN_REDIRECT:    ">"
TOKEN_WORD:        "output.txt"
TOKEN_EOF:         (marcador de fin)

Arquitectura del Tokenizador
Flujo Principal:
Entrada: Cadena de texto del usuario
Procesamiento: Análisis carácter por carácter
Clasificación: Identificar tipo de cada token
Extracción: Obtener el valor correcto del token
Salida: Lista enlazada de tokens

Separación de Responsabilidades:
tokenizer.c: Función principal y flujo de control
token_utils.c: Creación y gestión de memoria de tokens
token_operators.c: Identificación de operadores (|, >, <, etc.)
token_words.c: Extracción de palabras y manejo de comillas


Análisis Detallado por Archivos

1. tokenizer.c - El Conductor Principal

¿Qué hace?
Coordina todo el proceso de tokenización
Itera sobre cada carácter de la entrada
Delega el trabajo específico a funciones especializadas

Flujo paso a paso:
Validación: Verifica que input no sea NULL
Inicialización: Prepara variables de control (head, current, i)
Bucle principal: Mientras hay caracteres por procesar:
Salta espacios en blanco
Identifica si es operador o palabra
Delega a la función apropiada
Añade el token a la lista
Finalización: Añade un token EOF para marcar el final

¿Por qué este diseño?
Modularidad: Cada tipo de token tiene su lógica separada
Mantenibilidad: Fácil añadir nuevos tipos de tokens
Robustez: Manejo centralizado de errores

Funciones Auxiliares:
skip_spaces(const char *input, int *i)

Propósito: Avanza el índice saltando espacios en blanco
¿Por qué separada? Los espacios son delimitadores, no tokens
create_eof_token(void)

Propósito: Crea el token especial que marca el final
¿Por qué EOF? El parser necesita saber cuándo terminar
process_operator(const char *input, int *i)

Propósito: Maneja operadores como |, >, <<, &&
Delega a: get_operator_type() para identificación específica
process_word(const char *input, int *i)

Propósito: Maneja palabras, argumentos y strings con comillas
Delega a: extract_word_token() para procesamiento complejo


2. token_utils.c - Gestión de Memoria y Tokens

create_token(t_token_type type, char *value)
Responsabilidad: Crear un token individual en memoria
Gestión de errores: Si falla malloc, libera el value para evitar leaks
¿Por qué separar? Centraliza la creación y hace el código más limpio

add_token(t_token **head, t_token **current, t_token *new_token)
Responsabilidad: Añadir token a la lista enlazada
Casos especiales:
Si es el primer token → establece head
Si no → enlaza con el anterior
¿Por qué punteros dobles? Para modificar las variables del caller

cleanup_tokens(t_token *tokens)
Responsabilidad: Liberar toda la memoria de la lista
Prevención de leaks: Libera tanto el contenido como la estructura
Uso: Se llama cuando hay errores o al final del procesamiento


3. token_operators.c - Identificación de Operadores

is_operator_char(char c)
Propósito: Identificación rápida de caracteres especiales
Lista actual: |, <, >, &, (, )
¿Por qué separar? Se usa en múltiples lugares del código

get_operator_type(const char *s, int *advance)
Responsabilidad: Determinar el tipo exacto de operador
Casos complejos:
| vs || (pipe vs OR lógico)
< vs << (redirect vs heredoc)
> vs >> (redirect vs append)
Parámetro advance: Indica cuántos caracteres consumir (1 o 2)

Funciones auxiliares:

get_pipe_operator(): Distingue | de ||
get_redirect_operator(): Distingue redirects simples de dobles

¿Por qué esta modularidad?
Escalabilidad: Fácil añadir nuevos operadores (bonus)
Claridad: Cada función tiene una responsabilidad específica
Debugging: Fácil localizar problemas con operadores específicos


4. token_words.c - Procesamiento de Palabras y Comillas

Esta es la parte más compleja del tokenizador.

extract_word_token(const char *s, int *i)
Responsabilidad: Extraer una palabra completa del input
Proceso:
Encuentra el inicio de la palabra
Encuentra el final (respetando comillas)
Procesa el contenido (quita comillas)

find_word_end(const char *s, int start)
Responsabilidad: Determinar dónde termina una palabra
Casos especiales:
Respeta comillas (no para en espacios dentro de comillas)
Para en operadores
Para en espacios fuera de comillas

process_quoted_string(const char *s, int start, int end)
Responsabilidad: Procesar el contenido quitando comillas
Algoritmo:
Calcula espacio necesario
Copia contenido sin las comillas delimitadoras
Mantiene comillas internas

skip_quoted_section(const char *s, int *i)
Responsabilidad: Saltar una sección entrecomillada completa
Uso: Durante la búsqueda del final de palabra

Funciones de copia:

copy_quoted_section(): Copia contenido sin las comillas delimitadoras
copy_unquoted_char(): Copia caracteres normales

Ejemplos de Procesamiento:

Input: echo 'hello "world"' | grep

1. echo → TOKEN_WORD, valor: echo
2. 'hello "world"' → TOKEN_WORD, valor: hello "world" (sin comillas externas)
3. | → TOKEN_PIPE, valor: |
4. grep → TOKEN_WORD, valor: grep
