# Documentación del Parser de Minishell

## Tabla de Contenidos
- [¿Qué es el Parsing?](#qué-es-el-parsing)
- [El Abstract Syntax Tree (AST)](#el-abstract-syntax-tree-ast)
- [Arquitectura del Parser: Descenso Recursivo](#arquitectura-del-parser-descenso-recursivo)
- [De Tokens a AST: Ejemplos Detallados](#de-tokens-a-ast-ejemplos-detallados)
  - [Ejemplo 1: Comando con Redirección](#ejemplo-1-comando-con-redirección)
  - [Ejemplo 2: Pipe Múltiple](#ejemplo-2-pipe-múltiple)
  - [Ejemplo 3: Combinación Compleja](#ejemplo-3-combinación-compleja)
- [Análisis Detallado del Flujo (Por Archivo)](#análisis-detallado-del-flujo-por-archivo)
- [Manejo de Errores](#manejo-de-errores)

---

## ¿Qué es el Parsing?
El parsing (análisis sintáctico) es el segundo paso en el procesamiento de comandos de una shell. Toma la lista de tokens generada por el tokenizador y construye un **Abstract Syntax Tree (AST)** que representa la estructura jerárquica y la gramática del comando.

**Flujo de Datos:**
`Input String` → **Tokenizer** → `Lista de Tokens` → **Parser** → `AST` → **Executor**

---

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

---

## Arquitectura del Parser: Descenso Recursivo
Nuestro parser utiliza una estrategia de **Descenso Recursivo**, una técnica elegante y potente para analizar gramáticas jerárquicas.

- **Descenso (Descent):** El parser empieza analizando la regla gramatical más general (una línea de comando que puede tener pipes) y "desciende" a reglas más específicas (un comando con redirecciones, y finalmente, un comando simple con argumentos).

- **Recursivo (Recursive):** Las funciones que analizan estas reglas se llaman unas a otras. La función que busca pipes llamará a la función que busca redirecciones para encontrar sus operandos. Esta llamada entre funciones es la "recursión" que le da el nombre.

**La Clave: La Precedencia de Operadores**

El objetivo es respetar el orden de las operaciones. En `bash` (y en nuestra minishell), la precedencia es (de menor a mayor):
1.  `|` (Pipe)
2.  `<`, `>`, `>>`, `<<` (Redirecciones)
3.  `comando y argumentos` (Comando)

**Jerarquía de Llamadas en Nuestro Código:**

Para respetar esta precedencia, nuestras funciones de parsing se llaman en un orden específico que forma la columna vertebral del descenso recursivo:

```
parse()
  └── parse_pipe_expression()      // 1. Busca el operador de MENOR precedencia ('|')
      └── parse_redirect_expression()  // 2. Para encontrar sus operandos, busca operadores de MAYOR precedencia ('>', '<')
          └── parse_command()          // 3. Para encontrar sus operandos, busca la unidad fundamental (un comando)
```
Este diseño garantiza que `echo hello > file` se agrupe como una unidad *antes* de que el parser busque un `|`.

**Separación de Responsabilidades:**
- **`parser.c`**: Contiene el punto de entrada `parse()` y la limpieza del AST.
- **`parser_expressions.c`**: Maneja la lógica de operadores (pipes y redirects).
- **`parser_commands.c`**: Maneja el parsing de comandos simples y sus argumentos.
- **`ast_utils.c`**: Funciones de fábrica para crear y gestionar nodos del AST.

---

## De Tokens a AST: Ejemplos Detallados

### Ejemplo 1: Comando con Redirección
**Input del Usuario:** `echo hello > file.txt`

#### 1. Tokenización
El tokenizador produce la siguiente lista enlazada:
`[WORD:"echo"] -> [WORD:"hello"] -> [REDIRECT_OUT:">"] -> [WORD:"file.txt"] -> [EOF]`

#### 2. Proceso de Parsing (El Viaje)
1.  `parse()` llama a `parse_pipe_expression()`.
2.  `parse_pipe_expression()`:
    -   No ve un `|`, así que para obtener su operando `left`, llama a `parse_redirect_expression()`.
3.  `parse_redirect_expression()`:
    -   Para obtener su operando `left`, llama a `parse_command()`.
    -   **`parse_command()`**:
        -   Consume `TOKEN_WORD:"echo"` y `TOKEN_WORD:"hello"`.
        -   Crea y retorna un `NODE_COMMAND`.
        -   **Sub-árbol retornado:**
            ```
            NODE_COMMAND
                |
            args: ["echo", "hello"]
            ```
    -   `parse_redirect_expression()` recibe este nodo. Ahora mira el siguiente token: `TOKEN_REDIRECT_OUT:">"`.
    -   Entra en su bucle `while`. Consume el `>` y el `TOKEN_WORD:"file.txt"`.
    -   Llama a `create_redirect_node()`, que "envuelve" el nodo de comando en un nuevo nodo de redirección.
    -   **Sub-árbol retornado:**
        ```
            NODE_REDIRECT_OUT (file: "file.txt")
                   /
            NODE_COMMAND
                |
            args: ["echo", "hello"]
        ```
4.  `parse_pipe_expression()` recibe este árbol. El siguiente token es `EOF`, no hay más pipes. Retorna el árbol tal cual.
5.  `parse()` recibe el árbol final.

#### 3. Salida: El AST Final
```
    NODE_REDIRECT_OUT (file: "file.txt")
           /
    NODE_COMMAND
        |
    args: ["echo", "hello"]
```

### Ejemplo 2: Pipe Múltiple
**Input del Usuario:** `ls -l | grep .c | wc -l`

#### 1. Tokenización
`[WORD:"ls"] -> [WORD:"-l"] -> [PIPE:"|"] -> [WORD:"grep"] -> [WORD:".c"] -> [PIPE:"|"] -> [WORD:"wc"] -> [WORD:"-l"] -> [EOF]`

#### 2. Proceso de Parsing (El Viaje)
1.  `parse()` llama a `parse_pipe_expression()`.
2.  `parse_pipe_expression()`:
    -   Llama a `parse_redirect_expression()` -> `parse_command()` y obtiene el nodo para `ls -l`.
    -   **Estado de `left`:**
        ```
        NODE_COMMAND
            |
        args: ["ls", "-l"]
        ```
    -   El bucle `while` ve el primer `TOKEN_PIPE:"|"`.
    -   Consume el `|`. Llama de nuevo a `parse_redirect_expression()` para obtener el `right`.
    -   `parse_redirect_expression()` -> `parse_command()` obtiene el nodo para `grep .c`.
    -   **Estado de `right`:**
        ```
        NODE_COMMAND
            |
        args: ["grep", ".c"]
        ```
    -   Crea un `NODE_PIPE` con `create_binary_node()`. La variable `left` se actualiza para apuntar a este nuevo árbol.
    -   **Estado de `left` (después de la primera iteración):**
        ```
            NODE_PIPE
           /         \
    NODE_COMMAND   NODE_COMMAND
          |              |
    args: ["ls",   args: ["grep",
           "-l"]          ".c"]
        ```
    -   El bucle `while` continúa y ve el segundo `TOKEN_PIPE:"|"`.
    -   Consume el `|`. Llama a `parse_redirect_expression()` para obtener el nuevo `right`.
    -   `parse_redirect_expression()` -> `parse_command()` obtiene el nodo para `wc -l`.
    -   **Nuevo estado de `right`:**
        ```
        NODE_COMMAND
            |
        args: ["wc", "-l"]
        ```
    -   Crea un nuevo `NODE_PIPE`. El `left` es el árbol de pipe que ya teníamos, y el `right` es el nuevo comando `wc`.
    -   La variable `left` se actualiza por última vez.
3.  El bucle termina al ver `EOF`. Se retorna el `left` final.

#### 3. Salida: El AST Final
```
        NODE_PIPE
       /         \
      /           \
NODE_PIPE        NODE_COMMAND
   /     \             |
  /       \       args: ["wc", "-l"]
NODE_COMMAND NODE_COMMAND
      |            |
args: ["ls", args: ["grep",
       "-l"]       ".c"]
```
*Este anidamiento hacia la izquierda es crucial y se conoce como **asociatividad a la izquierda**, que es como `bash` procesa los pipes.*

### Ejemplo 3: Combinación Compleja
**Input del Usuario:** `cat < file.txt | grep test > output.txt`

#### 1. Tokenización
`[WORD:"cat"] -> [REDIRECT_IN:"<"] -> [WORD:"file.txt"] -> [PIPE:"|"] -> [WORD:"grep"] -> [WORD:"test"] -> [REDIRECT_OUT:">"] -> [WORD:"output.txt"] -> [EOF]`

#### 2. Proceso de Parsing (El Viaje)
1.  `parse()` llama a `parse_pipe_expression()`.
2.  `parse_pipe_expression()`:
    -   Llama a `parse_redirect_expression()` para obtener el `left`.
    -   **Primera llamada a `parse_redirect_expression()`**:
        -   Llama a `parse_command()` y obtiene el `NODE_COMMAND` para `cat`.
        -   Ve el `TOKEN_REDIRECT_IN:"<"`. Lo consume junto con `file.txt`.
        -   Crea un `NODE_REDIRECT_IN` que envuelve al comando `cat`.
        -   **Retorna este sub-árbol como `left`:**
            ```
            NODE_REDIRECT_IN (file: "file.txt")
                   /
            NODE_COMMAND
                |
            args: ["cat"]
            ```
    -   El bucle `while` en `parse_pipe_expression()` ve el `TOKEN_PIPE:"|"`.
    -   Consume el `|`. Llama de nuevo a `parse_redirect_expression()` para obtener el `right`.
    -   **Segunda llamada a `parse_redirect_expression()`**:
        -   Llama a `parse_command()` y obtiene el `NODE_COMMAND` para `grep test`.
        -   Ve el `TOKEN_REDIRECT_OUT:">"`. Lo consume junto con `output.txt`.
        -   Crea un `NODE_REDIRECT_OUT` que envuelve al comando `grep`.
        -   **Retorna este sub-árbol como `right`:**
            ```
            NODE_REDIRECT_OUT (file: "output.txt")
                   /
            NODE_COMMAND
                |
            args: ["grep", "test"]
            ```
    -   De vuelta en `parse_pipe_expression()`, ahora tiene sus dos operandos (`left` y `right`).
    -   Llama a `create_binary_node()` para unirlos bajo un `NODE_PIPE`.

#### 3. Salida: El AST Final
```
              NODE_PIPE
             /         \
            /           \
NODE_REDIRECT_IN     NODE_REDIRECT_OUT
(file: "file.txt")   (file: "output.txt")
       |                    |
       /                    /
NODE_COMMAND           NODE_COMMAND
      |                      |
args: ["cat"]          args: ["grep", "test"]
```

---

## Análisis Detallado del Flujo (Por Archivo)
*Esta sección puede permanecer como la tenías, ya que describe las responsabilidades de cada archivo, lo cual sigue siendo correcto y complementa los ejemplos anteriores.*

## Manejo de Errores
*Esta sección también puede permanecer como la tenías. Es una buena explicación de tu estrategia de manejo de errores.*
```<!-- filepath: /home/jmondon/Documents/Git Personal/minishell/src/parser/PARSER.md -->
# Documentación del Parser de Minishell

## Tabla de Contenidos
- [¿Qué es el Parsing?](#qué-es-el-parsing)
- [El Abstract Syntax Tree (AST)](#el-abstract-syntax-tree-ast)
- [Arquitectura del Parser: Descenso Recursivo](#arquitectura-del-parser-descenso-recursivo)
- [De Tokens a AST: Ejemplos Detallados](#de-tokens-a-ast-ejemplos-detallados)
  - [Ejemplo 1: Comando con Redirección](#ejemplo-1-comando-con-redirección)
  - [Ejemplo 2: Pipe Múltiple](#ejemplo-2-pipe-múltiple)
  - [Ejemplo 3: Combinación Compleja](#ejemplo-3-combinación-compleja)
- [Análisis Detallado del Flujo (Por Archivo)](#análisis-detallado-del-flujo-por-archivo)
- [Manejo de Errores](#manejo-de-errores)

---

## ¿Qué es el Parsing?
El parsing (análisis sintáctico) es el segundo paso en el procesamiento de comandos de una shell. Toma la lista de tokens generada por el tokenizador y construye un **Abstract Syntax Tree (AST)** que representa la estructura jerárquica y la gramática del comando.

**Flujo de Datos:**
`Input String` → **Tokenizer** → `Lista de Tokens` → **Parser** → `AST` → **Executor**

---

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

---

## Arquitectura del Parser: Descenso Recursivo
Nuestro parser utiliza una estrategia de **Descenso Recursivo**, una técnica elegante y potente para analizar gramáticas jerárquicas.

- **Descenso (Descent):** El parser empieza analizando la regla gramatical más general (una línea de comando que puede tener pipes) y "desciende" a reglas más específicas (un comando con redirecciones, y finalmente, un comando simple con argumentos).

- **Recursivo (Recursive):** Las funciones que analizan estas reglas se llaman unas a otras. La función que busca pipes llamará a la función que busca redirecciones para encontrar sus operandos. Esta llamada entre funciones es la "recursión" que le da el nombre.

**La Clave: La Precedencia de Operadores**

El objetivo es respetar el orden de las operaciones. En `bash` (y en nuestra minishell), la precedencia es (de menor a mayor):
1.  `|` (Pipe)
2.  `<`, `>`, `>>`, `<<` (Redirecciones)
3.  `comando y argumentos` (Comando)

**Jerarquía de Llamadas en Nuestro Código:**

Para respetar esta precedencia, nuestras funciones de parsing se llaman en un orden específico que forma la columna vertebral del descenso recursivo:

```
parse()
  └── parse_pipe_expression()      // 1. Busca el operador de MENOR precedencia ('|')
      └── parse_redirect_expression()  // 2. Para encontrar sus operandos, busca operadores de MAYOR precedencia ('>', '<')
          └── parse_command()          // 3. Para encontrar sus operandos, busca la unidad fundamental (un comando)
```
Este diseño garantiza que `echo hello > file` se agrupe como una unidad *antes* de que el parser busque un `|`.

**Separación de Responsabilidades:**
- **`parser.c`**: Contiene el punto de entrada `parse()` y la limpieza del AST.
- **`parser_expressions.c`**: Maneja la lógica de operadores (pipes y redirects).
- **`parser_commands.c`**: Maneja el parsing de comandos simples y sus argumentos.
- **`ast_utils.c`**: Funciones de fábrica para crear y gestionar nodos del AST.

---

## De Tokens a AST: Ejemplos Detallados

### Ejemplo 1: Comando con Redirección
**Input del Usuario:** `echo hello > file.txt`

#### 1. Tokenización
El tokenizador produce la siguiente lista enlazada:
`[WORD:"echo"] -> [WORD:"hello"] -> [REDIRECT_OUT:">"] -> [WORD:"file.txt"] -> [EOF]`

#### 2. Proceso de Parsing (El Viaje)
1.  `parse()` llama a `parse_pipe_expression()`.
2.  `parse_pipe_expression()`:
    -   No ve un `|`, así que para obtener su operando `left`, llama a `parse_redirect_expression()`.
3.  `parse_redirect_expression()`:
    -   Para obtener su operando `left`, llama a `parse_command()`.
    -   **`parse_command()`**:
        -   Consume `TOKEN_WORD:"echo"` y `TOKEN_WORD:"hello"`.
        -   Crea y retorna un `NODE_COMMAND`.
        -   **Sub-árbol retornado:**
            ```
            NODE_COMMAND
                |
            args: ["echo", "hello"]
            ```
    -   `parse_redirect_expression()` recibe este nodo. Ahora mira el siguiente token: `TOKEN_REDIRECT_OUT:">"`.
    -   Entra en su bucle `while`. Consume el `>` y el `TOKEN_WORD:"file.txt"`.
    -   Llama a `create_redirect_node()`, que "envuelve" el nodo de comando en un nuevo nodo de redirección.
    -   **Sub-árbol retornado:**
        ```
            NODE_REDIRECT_OUT (file: "file.txt")
                   /
            NODE_COMMAND
                |
            args: ["echo", "hello"]
        ```
4.  `parse_pipe_expression()` recibe este árbol. El siguiente token es `EOF`, no hay más pipes. Retorna el árbol tal cual.
5.  `parse()` recibe el árbol final.

#### 3. Salida: El AST Final
```
    NODE_REDIRECT_OUT (file: "file.txt")
           /
    NODE_COMMAND
        |
    args: ["echo", "hello"]
```

### Ejemplo 2: Pipe Múltiple
**Input del Usuario:** `ls -l | grep .c | wc -l`

#### 1. Tokenización
`[WORD:"ls"] -> [WORD:"-l"] -> [PIPE:"|"] -> [WORD:"grep"] -> [WORD:".c"] -> [PIPE:"|"] -> [WORD:"wc"] -> [WORD:"-l"] -> [EOF]`

#### 2. Proceso de Parsing (El Viaje)
1.  `parse()` llama a `parse_pipe_expression()`.
2.  `parse_pipe_expression()`:
    -   Llama a `parse_redirect_expression()` -> `parse_command()` y obtiene el nodo para `ls -l`.
    -   **Estado de `left`:**
        ```
        NODE_COMMAND
            |
        args: ["ls", "-l"]
        ```
    -   El bucle `while` ve el primer `TOKEN_PIPE:"|"`.
    -   Consume el `|`. Llama de nuevo a `parse_redirect_expression()` para obtener el `right`.
    -   `parse_redirect_expression()` -> `parse_command()` obtiene el nodo para `grep .c`.
    -   **Estado de `right`:**
        ```
        NODE_COMMAND
            |
        args: ["grep", ".c"]
        ```
    -   Crea un `NODE_PIPE` con `create_binary_node()`. La variable `left` se actualiza para apuntar a este nuevo árbol.
    -   **Estado de `left` (después de la primera iteración):**
        ```
            NODE_PIPE
           /         \
    NODE_COMMAND   NODE_COMMAND
          |              |
    args: ["ls",   args: ["grep",
           "-l"]          ".c"]
        ```
    -   El bucle `while` continúa y ve el segundo `TOKEN_PIPE:"|"`.
    -   Consume el `|`. Llama a `parse_redirect_expression()` para obtener el nuevo `right`.
    -   `parse_redirect_expression()` -> `parse_command()` obtiene el nodo para `wc -l`.
    -   **Nuevo estado de `right`:**
        ```
        NODE_COMMAND
            |
        args: ["wc", "-l"]
        ```
    -   Crea un nuevo `NODE_PIPE`. El `left` es el árbol de pipe que ya teníamos, y el `right` es el nuevo comando `wc`.
    -   La variable `left` se actualiza por última vez.
3.  El bucle termina al ver `EOF`. Se retorna el `left` final.

#### 3. Salida: El AST Final
```
        NODE_PIPE
       /         \
      /           \
NODE_PIPE        NODE_COMMAND
   /     \             |
  /       \       args: ["wc", "-l"]
NODE_COMMAND NODE_COMMAND
      |            |
args: ["ls", args: ["grep",
       "-l"]       ".c"]
```
*Este anidamiento hacia la izquierda es crucial y se conoce como **asociatividad a la izquierda**, que es como `bash` procesa los pipes.*

### Ejemplo 3: Combinación Compleja
**Input del Usuario:** `cat < file.txt | grep test > output.txt`

#### 1. Tokenización
`[WORD:"cat"] -> [REDIRECT_IN:"<"] -> [WORD:"file.txt"] -> [PIPE:"|"] -> [WORD:"grep"] -> [WORD:"test"] -> [REDIRECT_OUT:">"] -> [WORD:"output.txt"] -> [EOF]`

#### 2. Proceso de Parsing (El Viaje)
1.  `parse()` llama a `parse_pipe_expression()`.
2.  `parse_pipe_expression()`:
    -   Llama a `parse_redirect_expression()` para obtener el `left`.
    -   **Primera llamada a `parse_redirect_expression()`**:
        -   Llama a `parse_command()` y obtiene el `NODE_COMMAND` para `cat`.
        -   Ve el `TOKEN_REDIRECT_IN:"<"`. Lo consume junto con `file.txt`.
        -   Crea un `NODE_REDIRECT_IN` que envuelve al comando `cat`.
        -   **Retorna este sub-árbol como `left`:**
            ```
            NODE_REDIRECT_IN (file: "file.txt")
                   /
            NODE_COMMAND
                |
            args: ["cat"]
            ```
    -   El bucle `while` en `parse_pipe_expression()` ve el `TOKEN_PIPE:"|"`.
    -   Consume el `|`. Llama de nuevo a `parse_redirect_expression()` para obtener el `right`.
    -   **Segunda llamada a `parse_redirect_expression()`**:
        -   Llama a `parse_command()` y obtiene el `NODE_COMMAND` para `grep test`.
        -   Ve el `TOKEN_REDIRECT_OUT:">"`. Lo consume junto con `output.txt`.
        -   Crea un `NODE_REDIRECT_OUT` que envuelve al comando `grep`.
        -   **Retorna este sub-árbol como `right`:**
            ```
            NODE_REDIRECT_OUT (file: "output.txt")
                   /
            NODE_COMMAND
                |
            args: ["grep", "test"]
            ```
    -   De vuelta en `parse_pipe_expression()`, ahora tiene sus dos operandos (`left` y `right`).
    -   Llama a `create_binary_node()` para unirlos bajo un `NODE_PIPE`.

#### 3. Salida: El AST Final
```
              NODE_PIPE
             /         \
            /           \
NODE_REDIRECT_IN     NODE_REDIRECT_OUT
(file: "file.txt")   (file: "output.txt")
       |                    |
       /                    /
NODE_COMMAND           NODE_COMMAND
      |                      |
args: ["cat"]          args: ["grep", "test"]
```

---

## Análisis Detallado del Flujo (Por Archivo)
*Esta sección puede permanecer como la tenías, ya que describe las responsabilidades de cada archivo, lo cual sigue siendo correcto y complementa los ejemplos anteriores.*

## Manejo de Errores
*Esta sección también puede permanecer como la tenías. Es una buena explicación de tu estrategia de