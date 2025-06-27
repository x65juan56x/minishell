# Documentación del Parser de Minishell

## ¿Qué es el Parsing?
El parsing (análisis sintáctico) es el segundo paso en el procesamiento de comandos de una shell. Toma la lista de tokens generada por el tokenizador y construye un **Abstract Syntax Tree (AST)** que representa la estructura jerárquica y la gramática del comando.

**Flujo de Datos:**
`Input String` → **Tokenizer** → `Lista de Tokens` → **Parser** → `AST` → **Executor**

## El Abstract Syntax Tree (AST)
Un AST es una representación en árbol de la estructura del comando. Cada nodo representa una operación (como un pipe o una redirección), y sus hijos representan los operandos. Este árbol respeta la precedencia de los operadores.

**Ejemplo Visual:**

**Input:** `cat file.txt | grep test > result.txt`

**AST Resultante:**
```
        NODE_REDIRECT_OUT (file: "result.txt")
               /
         NODE_PIPE
        /         \
NODE_COMMAND     NODE_COMMAND
      |                |
args: ["cat",     args: ["grep",
       "file.txt"]      "test"]
```
Este árbol muestra que el `pipe` se resuelve primero, y la salida de todo ese `pipe` se redirige a `result.txt`.

## Arquitectura del Parser
Nuestro parser utiliza una estrategia de **Descenso Recursivo**. Cada nivel de la gramática es manejado por una función específica. Las funciones se llaman entre sí siguiendo el orden de precedencia de los operadores.

**Jerarquía de Precedencia (de menor a mayor):**
1.  `|` (Pipe) - `parse_pipe_expression()`
2.  `<`, `>`, `>>`, `<<` (Redirects) - `parse_redirect_expression()`
3.  `comando y argumentos` (Command) - `parse_command()`

**Separación de Responsabilidades:**
- **`parser.c`**: Contiene el punto de entrada `parse()` y la limpieza del AST.
- **`parser_expressions.c`**: Maneja la lógica de operadores (pipes y redirects).
- **`parser_commands.c`**: Maneja el parsing de comandos simples y sus argumentos.
- **`ast_utils.c`**: Funciones de fábrica para crear y gestionar nodos del AST.

---

## Análisis Detallado del Flujo

### 1. `parser.c` - El Coordinador

#### `t_ast_node *parse(t_token *tokens)`
- **Responsabilidad**: Es el punto de entrada principal. Orquesta todo el proceso de parsing.
- **Mecanismo**:
  1.  Inicializa una estructura `t_parser` para mantener el estado del análisis (token actual, flag de error).
  2.  Llama a `parse_pipe_expression()`, la función que maneja el operador de menor precedencia, iniciando así el descenso recursivo.
  3.  Comprueba si el flag `parser.error` se activó. Si es así, limpia el AST parcialmente construido con `cleanup_ast()` y retorna `NULL`.
  4.  Si todo fue exitoso, retorna la raíz del AST completo.

### 2. `parser_expressions.c` - Manejo de Operadores

#### `t_ast_node *parse_pipe_expression(t_parser *parser)`
- **Responsabilidad**: Parsear expresiones de pipe (`|`), que tienen la menor precedencia.
- **Mecanismo**:
  1.  Llama a `parse_redirect_expression()` para obtener el subárbol del lado izquierdo.
  2.  Entra en un bucle `while` que se ejecuta mientras el token actual sea `TOKEN_PIPE`.
  3.  Dentro del bucle, consume el `|`, llama de nuevo a `parse_redirect_expression()` para obtener el lado derecho, y crea un `NODE_PIPE` que une ambos lados.
  4.  Esta estructura de bucle maneja correctamente la asociatividad a la izquierda (ej: `a | b | c` se agrupa como `(a | b) | c`).

#### `t_ast_node *parse_redirect_expression(t_parser *parser)`
- **Responsabilidad**: Parsear expresiones de redirección (`>`, `<`, `>>`, `<<`), que tienen mayor precedencia que los pipes.
- **Mecanismo**:
  1.  Primero, llama a `parse_command()` para obtener el nodo de comando base.
  2.  Entra en un bucle `while` que se ejecuta mientras el token actual sea de redirección.
  3.  Dentro del bucle, consume el token de redirect (ej: `>`) y el nombre del archivo (`TOKEN_WORD`).
  4.  Crea un nuevo nodo de redirección que "envuelve" al nodo actual. Esto permite anidar múltiples redirecciones (`cmd > a < b`).

### 3. `parser_commands.c` - La Unidad Fundamental

#### `t_ast_node *parse_command(t_parser *parser)`
- **Responsabilidad**: Parsear la unidad más simple: un comando y sus argumentos.
- **Mecanismo**:
  1.  Verifica que el token actual sea `TOKEN_WORD`. Si no, no hay comando que parsear.
  2.  Crea un nodo base de tipo `NODE_COMMAND`.
  3.  Llama a `collect_command_args()` para que recolecte todos los `TOKEN_WORD` consecutivos y los agrupe en un array `char **`.

#### `char **collect_command_args(t_parser *parser)`
- **Responsabilidad**: Construir un array de strings (terminado en `NULL`) apto para `execve`.
- **Mecanismo**:
  1.  Llama a `count_command_args()` para saber cuánta memoria alocar de antemano.
  2.  Aloca el array de punteros.
  3.  Itera, consumiendo cada `TOKEN_WORD` y duplicando su valor en el array.
  4.  Añade el `NULL` terminador.

### 4. `ast_utils.c` - La Fábrica de Nodos

Estas funciones ayudan a crear nodos de forma limpia y segura.
- **`create_ast_node()`**: Crea un nodo genérico e inicializa todos sus punteros a `NULL`.
- **`create_binary_node()`**: Crea un nodo para operadores binarios (como `|`), asignando los hijos `left` y `right`.
- **`create_redirect_node()`**: Crea un nodo para redirecciones. Asigna el comando al hijo `left` y el nombre del archivo al campo `file`. El hijo `right` no se usa.

## Manejo de Errores
- **Flag de Error**: La estructura `t_parser` contiene un flag `int error`. Cuando una función de parsing encuentra un error de sintaxis (ej: `> ` sin archivo), establece `parser->error = 1`.
- **Propagación**: Las funciones retornan `NULL` cuando detectan un error o cuando una función a la que llaman retorna `NULL`.
- **Limpieza Centralizada**: La función `parse()` es la única que revisa el flag `error` al final. Si está activado, llama a `cleanup_ast()` para liberar toda la memoria del árbol parcialmente construido, evitando fugas.
