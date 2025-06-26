# Documentación del Tokenizador de Minishell

## ¿Qué es la Tokenización?
La tokenización es el primer paso en el procesamiento de comandos de una shell. Consiste en dividir una cadena de entrada del usuario en unidades más pequeñas llamadas **tokens**, cada una con un significado específico.

**Ejemplo Visual:**
Input: `echo "hello world" | grep test > output.txt`

Se convierte en una lista enlazada de tokens:
- `TOKEN_WORD`: "echo"
- `TOKEN_WORD`: "hello world"
- `TOKEN_PIPE`: "|"
- `TOKEN_WORD`: "grep" 
- `TOKEN_WORD`: "test"
- `TOKEN_REDIRECT_OUT`: ">"
- `TOKEN_WORD`: "output.txt"
- `TOKEN_EOF`: (marcador de fin)

## Arquitectura del Tokenizador
El tokenizador está diseñado con una clara separación de responsabilidades para facilitar su mantenimiento y extensión.

- **`tokenizer.c`**: Contiene el flujo principal y las funciones que orquestan el proceso.
- **`token_utils.c`**: Utilidades para crear, añadir y limpiar la lista de tokens.
- **`token_operators.c`**: Lógica para identificar operadores como `|`, `>`, `>>`, etc.
- **`token_words.c`**: Lógica compleja para extraer palabras y manejar strings entrecomillados.

---

## Análisis Detallado por Archivos

### 1. `tokenizer.c` - El Conductor Principal
Este archivo coordina todo el proceso, iterando sobre la entrada y delegando la creación de cada token.

#### `t_token *tokenize(const char *input)`
- **Responsabilidad**: Es la función pública y punto de entrada para la tokenización.
- **Mecanismo**:
  1. Inicializa los punteros `head` y `current` de la lista a `NULL`.
  2. Llama a `generate_token_list` para que construya la lista de tokens.
  3. Si `generate_token_list` falla, limpia la memoria con `cleanup_tokens` y retorna `NULL`.
  4. Si tiene éxito, crea y añade un token final `TOKEN_EOF`, que es crucial para el parser.
  5. Devuelve el puntero al inicio de la lista (`head`).
- **Llamado por**: `main()`

#### `static int generate_token_list(...)`
- **Responsabilidad**: Es el motor principal. Recorre la cadena de entrada y construye la lista de tokens.
- **Mecanismo**:
  1. Itera sobre la cadena de entrada `input`.
  2. En cada iteración, primero salta los espacios en blanco.
  3. Llama a `is_operator_char()` para decidir si el token es un operador o una palabra.
  4. Delega la creación del token a `process_operator()` o `process_word()`.
  5. Añade el nuevo token a la lista con `add_token()`.
  6. Retorna `1` si falla la creación de un token, `0` si tiene éxito.

#### `static t_token *process_operator(...)`
- **Responsabilidad**: Procesar un token de operador (`|`, `>`, `>>`, etc.).
- **Mecanismo**:
  1. Llama a `get_operator_type()` para determinar el tipo exacto de operador y su longitud.
  2. Extrae el valor del operador (ej: ">>") usando `ft_substr`.
  3. Llama a `create_token()` para ensamblar el token final.

#### `static t_token *process_word(...)`
- **Responsabilidad**: Procesar un token de palabra (un comando, un argumento, etc.).
- **Mecanismo**:
  1. Delega la extracción de la palabra y el manejo de comillas a `extract_word_token()`.
  2. Llama a `create_token()` para crear un token de tipo `TOKEN_WORD`.

### 2. `token_utils.c` - Gestión de Memoria y Tokens

#### `t_token *create_token(...)`
- **Responsabilidad**: Crear y alocar memoria para un único nodo de token.
- **Mecanismo**:
  1. Usa `malloc` para reservar espacio para la estructura `t_token`.
  2. Asigna el `type` y el `value` a la nueva estructura.
  3. Inicializa `next` a `NULL`.
  4. Si `malloc` falla, libera el `value` (si existe) para evitar fugas.

#### `void add_token(...)`
- **Responsabilidad**: Añadir un nuevo token al final de la lista enlazada.
- **Mecanismo**:
  - Usa punteros dobles (`**head`, `**current`) para poder modificar los punteros originales en la función que la llama.
  - Si la lista está vacía, el nuevo token es `head` y `current`.
  - Si no, enlaza `(*current)->next` al nuevo token y actualiza `current`.

#### `void cleanup_tokens(...)`
- **Responsabilidad**: Liberar toda la memoria utilizada por una lista enlazada de tokens.
- **Mecanismo**: Itera por la lista, liberando `current->value` y luego `current` en cada paso.

### 3. `token_operators.c` - Identificación de Operadores

#### `int is_operator_char(char c)`
- **Responsabilidad**: Comprobar rápidamente si un carácter puede ser el inicio de un operador.
- **Llamado por**: `generate_token_list()` y `find_word_end()`.

#### `t_token_type get_operator_type(...)`
- **Responsabilidad**: Es el dispatcher principal para identificar cualquier operador.
- **Mecanismo**:
  - Usa una serie de `if` para comprobar el primer carácter.
  - Delega a `get_pipe_operator()` y `get_redirect_operator()` para los casos que pueden tener 1 o 2 caracteres (`|` vs `||`, `>` vs `>>`).
  - Maneja directamente otros casos como `&&`, `(`, `)`.

### 4. `token_words.c` - Procesamiento de Palabras y Comillas
Esta es la parte más compleja del tokenizador.

#### `char *extract_word_token(const char *s, int *i)`
- **Responsabilidad**: Orquestar la extracción completa de un token de palabra.
- **Mecanismo**:
  1. Marca la posición de inicio.
  2. Llama a `find_word_end()` para determinar dónde termina la palabra.
  3. Actualiza el índice principal `i` para que el tokenizador continúe desde la posición correcta.
  4. Llama a `process_quoted_string()` para obtener el valor final procesado (sin comillas externas).

#### `static int find_word_end(...)`
- **Responsabilidad**: Encontrar el índice donde termina una "palabra" en la entrada.
- **Mecanismo**:
  1. Itera desde la posición `start`.
  2. El bucle se detiene si encuentra un espacio, un operador o el final de la cadena.
  3. Si encuentra una comilla (`'` o `"`), salta toda la sección entrecomillada de una vez, avanzando el índice hasta pasar la comilla de cierre. Esto asegura que los delimitadores dentro de las comillas no terminen la palabra.

#### `char *process_quoted_string(...)`
- **Responsabilidad**: Crear la cadena de valor final para un token de palabra, eliminando las comillas externas que actúan como delimitadores.
- **Mecanismo**:
  1. Reserva memoria para la nueva cadena.
  2. Itera a través de la subcadena original (de `start` a `end`).
  3. Si encuentra una comilla, copia el contenido interno de la sección entrecomillada, omitiendo las comillas delimitadoras.
  4. Si encuentra un carácter normal, lo copia directamente.
  5. El resultado es una cadena "limpia", lista para ser usada. (Ej: `"hello world"` se convierte en `hello world`).
