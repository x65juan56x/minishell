# Documentación del Parser de MiniShell

## Tabla de Contenidos
- [¿Qué es el Parsing?](#qué-es-el-parsing)
- [Abstract Syntax Tree (AST)](#abstract-syntax-tree-ast)
- [Arquitectura del Parser: Descenso Recursivo](#arquitectura-del-parser-descenso-recursivo)
- [Estructura de Archivos](#estructura-de-archivos)
- [Flujo de Parsing (por Función)](#flujo-de-parsing-por-función)
  - [parse](#parse)
  - [parse_pipe_expression](#parse_pipe_expression)
  - [parse_redirect_expression](#parse_redirect_expression)
  - [parse_command](#parse_command)
- [Ejemplos de Árbol de Sintaxis](#ejemplos-de-árbol-de-sintaxis)
  - [Comando Simple](#comando-simple)
  - [Redirección](#redirección)
  - [Pipe](#pipe)
  - [Combinación Compleja](#combinación-compleja)
- [Manejo de Errores](#manejo-de-errores)

---

## ¿Qué es el Parsing?
El _parsing_ transforma una lista plana de tokens, generada por el tokenizador, en un **Árbol de Sintaxis Abstracta** (AST) que refleja la estructura jerárquica y la precedencia de operadores de un comando de shell.

Flujo de datos:
```
Input (string) → Tokenizer → Lista de Tokens → Parser → AST → Executor
```

---

## Abstract Syntax Tree (AST)
Un AST es un grafo donde cada nodo representa una operación (comando, redirección, pipe) y sus hijos sus operandos o argumentos. La forma del árbol respeta la precedencia:

- `NODE_COMMAND` (nivel más bajo)
- `NODE_REDIRECT_*` (mayor precedencia)
- `NODE_PIPE` (menor precedencia)

**Ejemplo**  
Comando: `cat file.txt | grep test > out.txt`

AST resultante:
```
          NODE_REDIRECT_OUT (file="out.txt")
                 /
           NODE_PIPE
          /         \
NODE_COMMAND     NODE_COMMAND
  args:["cat","file.txt"]  args:["grep","test"]
```

---

## Arquitectura del Parser: Descenso Recursivo
Se emplea **Descenso Recursivo** para respetar la precedencia:

```
parse()
 └─ parse_pipe_expression()        # 1. '|' (menor precedencia)
     └─ parse_redirect_expression()# 2. '<', '>', '>>', '<<'
         └─ parse_command()        # 3. comando y sus args
```

Cada función delega en la siguiente para construir progresivamente el AST.

---

## Estructura de Archivos
- **parser.c**  
  Punto de entrada `parse()` y limpieza `cleanup_ast()`.
- **parser_expressions.c**  
  `parse_pipe_expression()`, `parse_redirect_expression()` y helpers.
- **parser_commands.c**  
  `parse_command()`, `count_command_args()`, `collect_command_args()`.
- **parser_utils.c** y **parser_redirect_utils.c**  
  Funciones auxiliares (`extract_args`, `consume_token`, `is_redirect_token`, …).
- **ast_utils.c**  
  Fábricas de nodos (`create_ast_node`, `create_binary_node`, `create_redirect_node`).

---

## Flujo de Parsing (por Función)

### parse
```c
t_ast_node *parse(t_token *tokens);
```
- Propósito: Orquesta el análisis desde la lista de tokens hasta el AST.
- Mecanismo:
  1. Inicializa `t_parser` (lista, token actual, flag `error`).
  2. Llama a `parse_pipe_expression(&parser)`.
  3. Si `parser.error` es 1, libera el AST parcial y retorna NULL.
  4. Devuelve la raíz completa del AST.
- Llamado por: `main` después de tokenizar.

### parse_pipe_expression
```c
t_ast_node *parse_pipe_expression(t_parser *parser);
```
- Propósito: Parsear expresiones separadas por `|`.
- Mecanismo:
  1. Llama a `parse_redirect_expression()` → subárbol izquierdo.
  2. Mientras el token actual sea `TOKEN_PIPE`:
     - Consume `|`.
     - Llama a `parse_redirect_expression()` → subárbol derecho.
     - Crea un nodo `NODE_PIPE(left, right)`.
     - Actualiza `left = nuevo nodo`.
  3. Retorna `left` (AST de pipes con asociatividad izquierda).
- Llamado por: `parse`.

### parse_redirect_expression
```c
t_ast_node *parse_redirect_expression(t_parser *parser);
```
- Propósito: Parsear redirecciones antes o después de un comando.
- Mecanismo:
  1. **Leading redirects**:  
     Repite mientras el token sea de redirección:
     - Consume operador y nombre de archivo.
     - Crea nodo `NODE_REDIRECT_*` encadenado a izquierda.
  2. Llama a `parse_command()` → `command_node`.
  3. **Trailing redirects**:  
     Similar a leading, envuelve `command_node` con nuevos nodos.
  4. Conecta la lista de `leading` con el subárbol resultante.
  5. Retorna la cabeza de la cadena de redirecciones (o `command_node` si no hay).
- Llamado por: `parse_pipe_expression`.

### parse_command
```c
t_ast_node *parse_command(t_parser *parser);
```
- Propósito: Parsear un comando simple y sus argumentos.
- Mecanismo:
  1. Verifica `parser->current->type == TOKEN_WORD`.
  2. Crea nodo `NODE_COMMAND`.
  3. Recolecta argumentos con `collect_command_args()`.
  4. Si falla al alocar o no hay args, libera y retorna NULL.
  5. Retorna el nodo con `node->args`.
- Llamado por: `parse_redirect_expression`.

---

## Ejemplos de Árbol de Sintaxis

### Comando Simple
Entrada: `echo hello`

AST:
```
NODE_COMMAND
  args: ["echo","hello"]
```

### Redirección
Entrada: `echo hi > file.txt`

AST:
```
     NODE_REDIRECT_OUT (file="file.txt")
            /
      NODE_COMMAND
        args:["echo","hi"]
```

### Pipe
Entrada: `ls | grep .c`

AST:
```
      NODE_PIPE
      /      \
NODE_COMMAND NODE_COMMAND
 args:["ls"] args:["grep",".c"]
```

### Combinación Compleja
Entrada: `cat < in.txt | grep foo >> out.txt`

AST:
```
                 NODE_REDIRECT_APPEND (file="out.txt")
                          /
                    NODE_PIPE
                   /        \
 NODE_REDIRECT_IN         NODE_COMMAND
   (file="in.txt")          args:["grep","foo"]
       /
 NODE_COMMAND
   args:["cat"]
```

---

## Manejo de Errores
- El parser dispone de un flag `parser.error`. Si alguna función detecta sintaxis inválida (p. ej. ausencia de nombre de archivo tras `>`), lo ajusta a `1`.
- Las funciones retornan `NULL` al fallar.
- Al finalizar `parse()`, si `error` es `1`, se invoca `cleanup_ast()` y se retorna `NULL`. Esto garantiza que no haya fugas