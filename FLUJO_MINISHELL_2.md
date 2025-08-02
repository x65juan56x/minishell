🔧 Estructuras Principales Definidas en minishell.h
Antes de analizar las funciones, es fundamental entender las estructuras clave:

📊 t_shell_context
```c
typedef struct s_shell_context
{
    int         exit_status;    // Código de salida del último comando
    t_list      *heredoc_files; // Lista de archivos temporales de heredoc
    char        **envp_cpy;     // Copia del entorno del shell
    t_list      *local_vars;    // Variables locales del shell
    int         error_flag;     // Flag de error general
} t_shell_context;
```
Propósito: Centraliza todo el estado global del shell en una sola estructura, facilitando el paso de información entre funciones sin variables globales adicionales.

---
## ⭐ Función: `cleanup_shell_context`
```c
void cleanup_shell_context(t_shell_context *shell_context)
{
    if (!shell_context)
        return ;
    if (shell_context->envp_cpy)
        ft_freearr(shell_context->envp_cpy);
    if (shell_context->local_vars)
        ft_lstclear(&shell_context->local_vars, free);
    cleanup_heredoc_files(shell_context);
    free(shell_context);
}
```

### 🧠 **Análisis línea por línea**

1. **`if (!shell_context) return;`** - Protección contra punteros nulos. Evita segfaults si se llama con NULL.
2. **`if (shell_context->envp_cpy) ft_freearr(shell_context->envp_cpy);`** - Libera el array de variables de entorno copiado.
3. **``if (shell_context->local_vars) ft_lstclear(&shell_context->local_vars, free);``** - Libera la lista enlazada de variables locales.
4. **`cleanup_heredoc_files(shell_context);`** - Elimina archivos temporales de heredoc del sistema de archivos.
5. **`free(shell_context);`** - Libera la estructura principal.

### 🎯 **Propósito de la función**

Función de limpieza centralizada que garantiza la liberación completa de toda la memoria y recursos asociados al contexto del shell. Es crucial para evitar memory leaks al finalizar el programa.

### 🔄 **Flujo de liberación**

```
shell_context
    ├── envp_cpy[] → ft_freearr()
    ├── local_vars → ft_lstclear()
    ├── heredoc_files → cleanup_heredoc_files()
    └── shell_context → free()
```
🛡️ Cobertura de errores
✅ **Protección contra NULL:** Verifica que shell_context no sea `NULL`  
✅ **Verificación de campos:** Comprueba cada campo antes de liberarlo  
✅ **Orden de liberación:** Libera primero el contenido, luego el contenedor  
❌ **No verifica errores de unlink()** en cleanup_heredoc_files()  

---
## ⭐ **Función: main**
```c
int main(int ac, char **av, char **envp)
{
    int             exit_status;
    t_shell_context *shell_context;

    (void)ac;
    (void)av;
    shell_context = malloc(sizeof(t_shell_context));
    if (!shell_context)
        return (1);
    shell_context->exit_status = 0;
    shell_context->heredoc_files = NULL;
    shell_context->local_vars = NULL;
    shell_context->envp_cpy = init_shell_environment(envp, shell_context);
    update_shell_level(shell_context);
    shell_context->error_flag = 0;
    exit_status = run_shell_loop(shell_context);
    rl_clear_history();
    cleanup_shell_context(shell_context);
    return (exit_status);
}
```
### 🧠 **Análisis línea por línea**
1. **`(void)ac; (void)av;`** - Suprime warnings del compilador por parámetros no utilizados (norma de 42).
2. **`shell_context = malloc(sizeof(t_shell_context));`** - Aloca memoria para el contexto principal del shell.
3. **`if (!shell_context) return (1);`** - Verifica que malloc no falló. Retorna código 1 (error) si no hay memoria.
4. **`shell_context->exit_status = 0;`** - Inicializa el código de salida en 0 (éxito).
5. **`shell_context->heredoc_files = NULL;`** - Inicializa la lista de archivos heredoc como vacía.
6. **`shell_context->local_vars = NULL;`** - Inicializa la lista de variables locales como vacía.
7. **`shell_context->envp_cpy = init_shell_environment(envp, shell_context);`** - **CRÍTICO:** Copia y prepara las variables de entorno.
8. **`update_shell_level(shell_context);`** - Incrementa la variable SHLVL para indicar anidamiento de shells.
9. **`shell_context->error_flag = 0;`** - Inicializa el flag de error.
10. **`exit_status = run_shell_loop(shell_context);`** - **PUNTO CENTRAL:** Ejecuta el bucle principal del shell.
11. **`rl_clear_history();`** - Limpia el historial de readline al salir.
12. **`cleanup_shell_context(shell_context);`** - Libera todos los recursos.
13. **`return (exit_status);`** - Retorna el código de salida al sistema operativo.

### 🎯 **Propósito de la función**

**Función de entrada principal que:**

* Inicializa el contexto completo del shell
* Configura el entorno de ejecución
* Ejecuta el bucle interactivo principal
* Limpia recursos al finalizar

### 🔄 **Flujo de ejecución completo**

```
main()
  ↓
malloc(shell_context) ✓
  ↓
init_shell_environment() → Copia envp, valida PATH/HOME
  ↓
update_shell_level() → SHLVL++
  ↓
run_shell_loop() → 🔄 BUCLE PRINCIPAL
  ↓                   ├── readline()
  ↓                   ├── tokenize()
  ↓                   ├── parse()
  ↓                   ├── execute_ast()
  ↓                   └── cleanup()
  ↓
rl_clear_history()
  ↓
cleanup_shell_context()
  ↓
return exit_status → SO
```

### 🛡️ **Cobertura de errores y validaciones**

✅ **Verificación de malloc:** Comprueba que shell_context se allocó correctamente  
✅ **Inicialización completa:** Todos los campos se inicializan explícitamente  
✅ **Limpieza garantizada:** cleanup_shell_context() siempre se ejecuta  
✅ **Gestión de readline:** Limpia el historial antes de salir  
❌ **No verifica errores de init_shell_environment()** - Si falla, el shell continúa con un entorno inválido  
❌ **No maneja señales en main()** - Las señales se configuran dentro del bucle

### 🔒 **Consideraciones de seguridad y memoria**

- **Inicialización defensiva:** Todos los punteros se inicializan a NULL explícitamente
- **Orden de operaciones:** La inicialización sigue un orden lógico (contexto → entorno → bucle → limpieza)
- **Gestión de recursos:** Garantiza que todos los recursos se liberen, incluso si el shell termina abruptamente
- **Códigos de salida estándar:** Sigue convenciones UNIX (0=éxito, 1=error de sistema)

### 📊 **Representación del estado inicial**

```
shell_context
├── exit_status: 0
├── heredoc_files: NULL
├── local_vars: NULL
├── envp_cpy: ["PATH=/bin:/usr/bin", "HOME=/home/user", ...]
└── error_flag: 0
```

## 📝 **Notas clave para evaluación oral**

**¿Por qué usar una estructura en lugar de variables globales?**

- Encapsulación y claridad del código
- Facilita el paso de estado entre funciones
- Cumple con la restricción de 42 de una sola variable global (`g_signal_status`)

**¿Por qué copiar envp en lugar de usar el original?**

- El shell necesita poder modificar variables de entorno (`export`, `unset`)
- El `envp` original es read-only en algunos sistemas
- Permite reset completo del entorno si es necesario

**¿Qué pasa si malloc falla en la línea 10?**

- El programa retorna inmediatamente con código 1
- No hay memory leaks porque no se allocó nada más
- Es la única función que puede fallar "silenciosamente" sin mensaje de error

**¿Por qué se llama update_shell_level() antes del bucle?**

- `SHLVL` debe incrementarse cuando el shell inicia, no en cada comando
- Permite que los procesos hijos hereden el nivel correcto
- Es parte de la compatibilidad con bash estándar

---

## ⭐ Función: `init_shell_environment()`

```c
char **init_shell_environment(char **envp, t_shell_context *shell_context)
{
    char    **envp_copy;
    int     env_count;

    env_count = count_env_vars(envp);
    if (env_count == 0 || !has_essential_vars(envp))
        print_empty_env_warning();
    envp_copy = copy_envp(envp);
    if (!envp_copy)
    {
        ft_putstr_fd("minishell: failed to initialize environment\n",
            STDERR_FILENO);
        cleanup_shell_context(shell_context);
        exit(1);
    }
    return (envp_copy);
}
```

### 🧠 **Análisis línea por línea:**

1. **`env_count = count_env_vars(envp);`** - Cuenta las variables de entorno disponibles
2. **`if (env_count == 0 || !has_essential_vars(envp))`** - Verifica si el entorno está vacío o le faltan variables críticas
3. **`print_empty_env_warning();`** - Muestra advertencias si el entorno es problemático
4. **`envp_copy = copy_envp(envp);`** - **CRÍTICO:** Crea una copia completa del entorno
5. **`if (!envp_copy)`** - Verifica que la copia fue exitosa
6. **`cleanup_shell_context(shell_context); exit(1);`** - Si falla, limpia y termina el programa

### 🎯 **Propósito de la función:**

- Crear una copia independiente del entorno que el shell puede modificar
- Validar que el entorno contiene variables esenciales (`PATH`, `HOME`)
- Proporcionar advertencias tempranas sobre problemas de configuración

### 🔍 **Funcion auxiliar crítica: `copy_envp()`**

```c
static char **copy_envp(char **envp)
{
    char    **new_envp;
    int     i, count = 0;
    
    while (envp[count]) count++;  // Cuenta variables
    new_envp = malloc(sizeof(char *) * (count + 1));
    
    i = -1;
    while (++i < count)
    {
        new_envp[i] = ft_strdup(envp[i]);  // Duplica cada string
        if (!new_envp[i])
            return (ft_freearr(new_envp), NULL);
    }
    new_envp[i] = NULL;
    return (new_envp);
}
```

---

## ⭐ Función: `run_shell_loop()`

```c
int run_shell_loop(t_shell_context *shell_context)
{
    char    *input;
    int     multiline_status;

    while (1)
    {
        setup_interactive_signals();
        g_signal_status = 0;
        input = get_user_input();
        if (!input)
            break;
        multiline_status = handle_multiline_input(&input);
        if (multiline_status == -1)
        {
            free(input);
            input = ft_strdup("");
        }
        else if (multiline_status == 1)
            return (1);
        if (g_signal_status == SIGINT)
            shell_context->exit_status = 130;
        process_input(input, shell_context);
        free(input);
    }
    return (shell_context->exit_status);
}
```

### 🧠 **Análisis línea por línea:**

1. **`setup_interactive_signals();`** - Configura manejo de señales (Ctrl+C, Ctrl+, Ctrl+D)
2. **`g_signal_status = 0;`** - Resetea el estado de señales global
3. **`input = get_user_input();`** - PUNTO CENTRAL: Obtiene entrada del usuario
4. **`if (!input) break;`** - Si readline devuelve NULL (Ctrl+D), sale del bucle
5. **`multiline_status = handle_multiline_input(&input);`** - Maneja comillas/paréntesis sin cerrar
6. **`if (g_signal_status == SIGINT)`** - Si hubo Ctrl+C, establece código de salida 130
7. **`process_input(input, shell_context);`** - **CRÍTICO:** Procesa el comando
8. **`free(input);`** - Libera memoria de la entrada

### 🎯 **Propósito de la función:**

- Bucle infinito que mantiene el shell activo
- Gestión completa del ciclo de vida de cada comando
- Manejo robusto de señales y entrada multilinea

---

## ⭐ Función: `get_user_input()`

```c
char *get_user_input(void)
{
    char    *input;

    if (isatty(STDIN_FILENO))
    {
        input = readline(PROMPT);
        if (!input)
            return (ft_putstr_fd("exit\n", STDOUT_FILENO), NULL);
    }
    else
        input = get_next_line(STDIN_FILENO);
    return (input);
}
```

### 🧠 **Análisis línea por línea:**

1. **`if (isatty(STDIN_FILENO))`** - **IMPORTANTE:** Detecta si es terminal interactiva
2. **`input = readline(PROMPT);`** - Usa readline para entrada interactiva (con historial)
3. **`if (!input) return (ft_putstr_fd("exit\n", STDOUT_FILENO), NULL);`** - Maneja Ctrl+D
5. **`input = get_next_line(STDIN_FILENO);`** - Para entrada no-interactiva (pipes, archivos)

### 🎯 **Propósito de la función:**

- Adaptarse automáticamente al contexto (interactivo vs no-interactivo)
- Proporcionar funcionalidad de historial solo cuando es apropiado
- Manejar EOF (Ctrl+D) de forma elegante

### 💡 **Detalles técnicos importantes:**

- **`isatty()`** distingue entre `minishell` y `echo "ls" | ./minishell`
- **`readline()`** proporciona historial, autocompletado y edición de línea
- **`get_next_line()`** es más eficiente para entrada programática

---

## ⭐ Función: `process_command_line()`

```c
int process_command_line(t_token *tokens, t_shell_context *shell_context)
{
    t_ast_node  *ast;
    int         exit_status;
    int         heredoc_id;

    heredoc_id = 0;
    shell_context->heredoc_files = NULL;
    expander_var(tokens, shell_context);
    tokens = expand_wildcards(tokens);
    ast = parse(tokens, shell_context);
    if (!ast)
        return (handle_parsing_error(tokens, ast));
    ignore_signals();
    exit_status = execute_ast(ast, &heredoc_id, shell_context);
    setup_interactive_signals();
    cleanup_heredoc_files(shell_context);
    cleanup_ast(ast);
    cleanup_tokens(tokens);
    return (exit_status);
}
```

### 🧠 **Análisis línea por línea:**

1. **`heredoc_id = 0; shell_context->heredoc_files = NULL;`** - Inicializa estado para heredocs
2. **`expander_var(tokens, shell_context);`** - **CRÍTICO:** Expande variables (`$VAR`, `$?`, `$$`)
3. **`tokens = expand_wildcards(tokens);`** - Expande wildcards (`.txt`, `src/`)
4. **`ast = parse(tokens, shell_context);`** - **PUNTO CENTRAL:** Convierte tokens en `AST`
5. **`if (!ast) return (handle_parsing_error(tokens, ast));`** - Maneja errores de sintaxis
6. **`ignore_signals();`** - Durante ejecución, ignora señales en el padre
7. **`exit_status = execute_ast(ast, &heredoc_id, shell_context);`** - **EJECUTA** el comando
8. **`setup_interactive_signals();`** - Restaura manejo interactivo de señales
9. Limpieza completa de todos los recursos

### 🎯 **Propósito de la función:**

- Coordinador central del procesamiento de comandos
- Gestión del pipeline completo: expansión → parsing → ejecución → limpieza
- Manejo robusto de errores y recursos

---

## ⭐ Función: tokenize()

```c
t_token *tokenize(const char *input)
{
    t_token *head;
    t_token *current;
    t_token *new_token;

    if (!input)
        return (NULL);
    head = NULL;
    current = NULL;
    if (generate_token_list(input, &head, &current) != 0)
        return (cleanup_tokens(head), NULL);
    new_token = create_token(TOKEN_EOF, NULL);
    if (!new_token)
        return (cleanup_tokens(head), NULL);
    add_token(&head, &current, new_token);
    return (head);
}
```

### 🧠 **Análisis línea por línea**

1. **`if (!input) return (NULL);`** - Protección básica contra entrada nula
2. **`head = NULL; current = NULL;`** - Inicializa los punteros de la lista enlazada
3. **`if (generate_token_list(input, &head, &current) != 0)`** - **CRÍTICO:** Función que hace el trabajo real de tokenización
4. **`return (cleanup_tokens(head), NULL);`** - Si falla, limpia memoria parcial y retorna NULL
5. **`new_token = create_token(TOKEN_EOF, NULL);`** - Añade token de fin obligatorio
6. **`add_token(&head, &current, new_token);`** - Añade `EOF` al final de la lista

### 🎯 **Propósito de la función**

**Punto de entrada principal del sistema de tokenización.** Convierte una cadena de texto cruda del usuario en una lista enlazada estructurada de tokens que el parser puede procesar.

### 🔄 **Flujo de tokenización**

```
"echo hello > file.txt"
         ↓
generate_token_list()
         ↓
[TOKEN_WORD:"echo"] → [TOKEN_WORD:"hello"] → [TOKEN_REDIRECT_OUT:">"] → [TOKEN_WORD:"file.txt"] → [TOKEN_EOF:NULL]
```

### 🛡️ **Cobertura de errores**

✅ **Protección contra NULL:** Verifica entrada válida  
✅ **Limpieza en fallos:** Si generate_token_list falla, limpia tokens parciales  
✅ **EOF obligatorio:** Garantiza que siempre hay un token de terminación  
❌ **No valida sintaxis:** Solo tokeniza, no verifica si los tokens forman comandos válidos  

---

## ⭐ Función: `generate_token_list()` (auxiliar crítica)

```c
static int generate_token_list(const char *input, t_token **head, t_token **current)
{
    int     i;
    t_token *new_token;

    i = 0;
    while (input[i])
    {
        while (input[i] && (input[i] == ' ' || input[i] == '\t'))
            i++;
        if (!input[i])
            break;
        if (is_operator_char(input[i]))
            new_token = process_operator(input, &i);
        else
            new_token = process_word(input, &i);
        if (!new_token)
            return (1);
        add_token(head, current, new_token);
    }
    return (0);
}
```

### 🧠 **Análisis línea por línea**
1. **`while (input[i] && (input[i] == ' ' || input[i] == '\t'))`** - Salta espacios en blanco
2. **`if (!input[i]) break;`** - Si llegamos al final, termina el bucle
3. **`if (is_operator_char(input[i]))`** - **DECISIÓN CRÍTICA:** ¿Es operador o palabra?
4. **`new_token = process_operator(input, &i);`** - Procesa operadores (`|`, `>`, `<`, `&&`, etc.)
5. **`new_token = process_word(input, &i);`** - Procesa palabras (comandos, argumentos, archivos)
6. **`if (!new_token) return (1);`** - Si falla la creación del token, aborta
7. **`add_token(head, current, new_token);`** - Añade el token a la lista

### 🎯 **Propósito de la función**

**Corazón del tokenizador.** Implementa la lógica de clasificación que decide si cada carácter es parte de un operador o una palabra, manejando espacios y construyendo la lista de tokens.

---

## ⭐ Función: `expander_var()`

```c
void expander_var(t_token *token_list, t_shell_context *shell_context)
{
    t_token *tmp;

    tmp = token_list;
    while (tmp != NULL)
    {
        if (tmp->expand != 1)
        {
            tmp = tmp->next;
            continue;
        }
        update_tokens_values(shell_context, tmp);
        tmp = tmp->next;
    }
    return;
}
```

### 🧠 **Análisis línea por línea**
1. **`tmp = token_list;`** - Comienza desde el primer token
2. **`while (tmp != NULL)`** - Itera por toda la lista de tokens
3. **`if (tmp->expand != 1)`** - FILTRO INTELIGENTE: Solo procesa tokens marcados para expansión
5. **`tmp = tmp->next; continue;`** - Salta tokens que no necesitan expansión
6. **`update_tokens_values(shell_context, tmp);`** - **CRÍTICO:** Hace la expansión real de variables
7. **`tmp = tmp->next;`** - Avanza al siguiente token

### 🎯 **Propósito de la función**

**Coordinador de expansión de variables.** Filtra eficientemente qué tokens necesitan expansión de variables (`$VAR`, `$?`, `$$`) y delega el trabajo real a `update_tokens_values()`.

### 💡 **Detalle técnico importante**

El campo `expand` en cada token se establece durante la tokenización con `is_expand_needed()`. Esto evita procesar innecesariamente tokens que no contienen variables.

---

## ⭐ Función: `parse()`

```c
t_ast_node *parse(t_token *tokens, t_shell_context *shell_context)
{
    t_parser    parser;
    t_ast_node  *ast;

    if (!tokens)
        return (NULL);
    parser.tokens = tokens;
    parser.current = tokens;
    parser.error = 0;
    ast = parse_logical_expression(&parser);
    if (parser.error)
    {
        if (tokens == NULL)
        {
            shell_context->error_flag = 1;
            cleanup_ast(ast);
            return (NULL);
        }
    }
    if (parser.current->type != TOKEN_EOF)
    {
        consume_token_type(&parser, TOKEN_EOF);
        cleanup_ast(ast);
        return (NULL);
    }
    return (ast);
}
```

### 🧠 **Análisis línea por línea**

**`if (!tokens) return (NULL);`** - Protección contra lista de tokens vacía
**`parser.tokens = tokens; parser.current = tokens; parser.error = 0;`** - Inicializa estructura del parser
**`ast = parse_logical_expression(&parser);`** - **PUNTO CENTRAL:** Inicia parsing desde el nivel más alto
**`if (parser.error)`** - Verifica si hubo errores de sintaxis durante el parsing
**`shell_context->error_flag = 1;`** - Marca error en el contexto global
**`if (parser.current->type != TOKEN_EOF)`** - **VALIDACIÓN CRÍTICA:** Debe consumir todos los tokens
**`consume_token_type(&parser, TOKEN_EOF);`** - Muestra error si quedan tokens sin procesar
**`cleanup_ast(ast); return (NULL);`** - Limpia AST parcial en caso de error

🎯 **Propósito de la función**

**Punto de entrada del sistema de parsing.** Convierte la lista lineal de tokens en un árbol de sintaxis abstracta (AST) que representa la estructura jerárquica del comando.

### 📊 **Estructura `t_parser` revelada**

```c
typedef struct s_parser
{
    t_token     *tokens;    // Lista completa de tokens (para debugging)
    t_token     *current;   // Puntero al token actual siendo procesado
    int         error;      // Flag de error (0 = sin errores, 1 = error detectado)
} t_parser;
```

**Propósito:** Mantiene el estado del parsing, permitiendo que las funciones de parsing sepan dónde están y si encontraron errores.

### 🔄 **Flujo de parsing jerárquico**

```
parse()
  ↓
parse_logical_expression() → Maneja && y ||
  ↓
parse_pipe_expression() → Maneja |
  ↓
parse_primary_expression() → Decide entre () y comandos
  ↓
parse_redirect_expression() → Maneja redirecciones y argumentos
```

### 🛡️ **Cobertura de errores**

✅ **Validación de tokens:** Verifica que la lista no sea NULL  
✅ **Estado de error persistente:** `parser.error` se mantiene entre llamadas  
✅ **Consumo completo:** Verifica que todos los tokens se procesaron  
✅ **Limpieza en fallos:** Siempre limpia AST parcial si hay error  
❌ **Mensajes específicos:** Los errores son genéricos, no indican ubicación exacta

---

## 📝 **Notas clave para evaluación oral**

**¿Por qué usar una lista enlazada para tokens en lugar de un array?**

- Flexibilidad: No sabemos cuántos tokens habrá
- Expansión de wildcards puede generar tokens adicionales dinámicamente
- Memoria eficiente: Solo aloca lo necesario

**¿Qué hace que el tokenizer sea "inteligente"?**

- Reconoce operadores de múltiples caracteres (`>>`, `&&`, `||`)
- Maneja comillas correctamente durante la tokenización
- Marca automáticamente qué tokens necesitan expansión de variables

**¿Por qué el parser usa "recursive descent"?**

- Refleja la precedencia natural de operadores (logical > pipe > redirect)
- Cada función maneja un nivel de precedencia específico
- Fácil de entender y debuggear

**¿Qué pasa si hay un error de sintaxis?**

- El parser se detiene inmediatamente
- El AST parcial se limpia para evitar memory leaks
- El contexto del shell se marca con error para que el bucle principal lo maneje

---

## ⭐ Función: `parse_logical_expression()`

```c
t_ast_node	*parse_logical_expression(t_parser *parser)
{
    t_ast_node		*left;
    t_ast_node		*right;
    t_ast_node		*new_node;
    t_token_type	op_type;

    left = parse_pipe_expression(parser);
    if (!left)
        return (NULL);
    while (parser->current && (parser->current->type == TOKEN_AND
            || parser->current->type == TOKEN_OR))
    {
        op_type = parser->current->type;
        consume_token_type(parser, op_type);
        if (parser->error)
            return (cleanup_ast(left), NULL);
        right = parse_pipe_expression(parser);
        if (!right)
            return (cleanup_ast(left), NULL);
        new_node = create_binary_node(op_type, left, right);
        if (!new_node)
            return (cleanup_ast(left), cleanup_ast(right), NULL);
        left = new_node;
    }
    return (left);
}
```

### 🧠 **Análisis línea por línea**

1. **`left = parse_pipe_expression(parser);`** - **CRÍTICO:** Delega al siguiente nivel de precedencia (pipes)
2. **`if (!left) return (NULL);`** - Si no hay expresión válida, termina inmediatamente
3. **`while (parser->current && (parser->current->type == TOKEN_AND || parser->current->type == TOKEN_OR))`** - **BUCLE PRINCIPAL:** Procesa operadores lógicos de izquierda a derecha
4. **`op_type = parser->current->type;`** - Guarda el tipo de operador (`&&`, `||`)
5. **`consume_token_type(parser, op_type);`** - Consume el operador y avanza el parser
6. **`if (parser->error) return (cleanup_ast(left), NULL);`** - Protección ante errores de sintaxis
7. **`right = parse_pipe_expression(parser);`** - Parsea la expresión del lado derecho
8. **`new_node = create_binary_node(op_type, left, right);`** - **CONSTRUCCIÓN DEL AST:** Crea nodo binario
9. **`left = new_node;`** - **TÉCNICA DE ASOCIATIVIDAD IZQUIERDA:** El nuevo nodo se convierte en el izquierdo para la siguiente iteración

### 🎯 **Propósito de la función**

**Nivel más alto del parsing jerárquico.** Maneja operadores lógicos (&&, ||) con asociatividad izquierda y evaluación de cortocircuito. Es el punto de entrada del sistema de precedencia de operadores.

### 🔄 **Flujo de precedencia jerárquica**

```
parse_logical_expression() → && y ||  (precedencia más baja)
         ↓
parse_pipe_expression() → |           (precedencia media)
         ↓
parse_primary_expression() → comandos (precedencia más alta)
```
📊 Representación del AST generado
Para el comando: cmd1 | cmd2 && cmd3 || cmd4
```
        [OR (||)]
        /        \
   [AND (&&)]    [cmd4]
   /        \
[PIPE (|)]  [cmd3]
/        \
[cmd1]   [cmd2]
```

### 🛡️ **Cobertura de errores**

✅ **Limpieza en fallos:** Si parse_pipe_expression falla, limpia el AST izquierdo 
✅ **Verificación de memoria:** Si create_binary_node falla, limpia ambos lados 
✅ **Estado persistente:** Respeta `parser->error` de niveles inferiores 
❌ **No valida sintaxis lógica:** Acepta `&&` `||` sin comando intermedio

---

## ⭐ Función: `execute_ast()`

```c
int	execute_ast(t_ast_node *ast, int *heredoc_id_ptr,
    t_shell_context *shell_context)
{
    if (!ast)
        return (0);
    if (preprocess_heredocs(&ast, heredoc_id_ptr, shell_context) != 0)
        return (130);
    if (ast->type == NODE_OR || ast->type == NODE_AND)
        return (handle_logical_op(ast, heredoc_id_ptr, shell_context));
    if (ast->type == NODE_PIPE)
        return (execute_pipe_line(ast, heredoc_id_ptr, shell_context));
    return (handle_simple_command_fork(ast, shell_context));
}
```

### 🧠 **Análisis línea por línea**

1. **`if (!ast) return (0);`** - Protección básica contra AST nulo
2. **`if (preprocess_heredocs(&ast, heredoc_id_ptr, shell_context) != 0)`** - **CRÍTICO:** Procesa heredocs ANTES de ejecutar
3. **`return (130);`** - Si heredoc falla (Ctrl+C), retorna código de señal `SIGINT`
4. **`if (ast->type == NODE_OR || ast->type == NODE_AND)`** - **DESPACHO POR TIPO:** Operadores lógicos
5. **`if (ast->type == NODE_PIPE)`** - **DESPACHO POR TIPO:** Pipelines
6. **`return (handle_simple_command_fork(ast, shell_context));`** - **CASO BASE:** Comandos simples

### 🎯 **Propósito de la función**

**Ejecutor principal del AST.** Implementa el **patrón Strategy** para despachar diferentes tipos de nodos a sus ejecutores específicos. Coordina el procesamiento de heredocs y la gestión de códigos de salida.

### 📊 **Función auxiliar crítica: handle_logical_op()**

```c
static int	handle_logical_op(t_ast_node *ast, int *heredoc_id_ptr,
            t_shell_context *shell_context)
{
    int	status;

    status = execute_ast(ast->left, heredoc_id_ptr, shell_context);
    if (ast->type == NODE_OR && status == 0)
        return (0);
    if (ast->type == NODE_AND && status != 0)
        return (status);
    return (execute_ast(ast->right, heredoc_id_ptr, shell_context));
}
```

**Implementa evaluación de cortocircuito:**

- **OR (||):** Si el izquierdo tiene éxito (0), no ejecuta el derecho
- **AND (&&):** Si el izquierdo falla (≠0), no ejecuta el derecho

### 🔄 **Flujo de ejecución recursiva**

```
execute_ast(root)
    ↓
preprocess_heredocs() → Crea archivos temporales
    ↓
Despacho por tipo:
├── NODE_AND/OR → handle_logical_op() (recursivo)
├── NODE_PIPE → execute_pipe_line() (fork múltiple)
└── NODE_COMMAND → handle_simple_command_fork() (fork simple)
```

### 🛡️ **Cobertura de errores**

✅ **Heredocs interrumpidos:** Retorna 130 si Ctrl+C durante heredoc  
✅ **AST nulo:** Maneja graciosamente árboles vacíos  
✅ **Propagación de errores:** Mantiene códigos de salida a través de la recursión  
❌ **No valida estructura del AST:** Asume que el parser generó un árbol válido

---

## ⭐ Función: `apply_redirections()`

```c
int	apply_redirections(t_ast_node *node)
{
    int	fd;
    int	target_fd;

    if (!node || !is_redirect_node(node->type))
        return (0);
    if (node->type == NODE_HEREDOC)
        fd = open(node->file, O_RDONLY);
    else
        fd = open_redirect_file(node->file, node->type);
    if (fd < 0)
        return (1);
    if (node->type == NODE_REDIRECT_IN || node->type == NODE_HEREDOC)
        target_fd = STDIN_FILENO;
    else
        target_fd = STDOUT_FILENO;
    if (dup2(fd, target_fd) == -1)
        return (perror("minishell: dup2"), close(fd), 1);
    close(fd);
    return (0);
}
```

### 🧠 **Análisis línea por línea**

1. **`if (!node || !is_redirect_node(node->type)) return (0);`** - **FILTRO:** Solo procesa nodos de redirección
2. **`if (node->type == NODE_HEREDOC) - CASO ESPECIAL:`** Heredocs ya están procesados como archivos temporales
3. **`fd = open_redirect_file(node->file, node->type);`** - **DELEGACIÓN:** Abre archivo según tipo de redirección
4. **`if (fd < 0) return (1);`** - **MANEJO DE ERRORES:** Archivo no existe o sin permisos
5. **if (node->type == NODE_REDIRECT_IN || node->type == NODE_HEREDOC)** - **DECISIÓN CRÍTICA:** ¿Entrada o salida?
6. **`target_fd = STDIN_FILENO; else target_fd = STDOUT_FILENO;`** - Establece el descriptor objetivo
7. **`if (dup2(fd, target_fd) == -1)`** - **OPERACIÓN ATÓMICA:** Redirige el flujo
8. **`close(fd);`** - **LIMPIEZA:** Cierra el descriptor original

### 🎯 **Propósito de la función**

**Núcleo del sistema de redirecciones.** Convierte nodos del AST en operaciones de sistema (`open`, `dup2`) para redirigir flujos de entrada/salida estándar.

### 📊 **Función auxiliar: `open_redirect_file()`**

```c
static int	open_redirect_file(char *file, t_node_type type)
{
    int	fd;

    if (type == NODE_REDIRECT_IN)
        fd = open(file, O_RDONLY);
    else if (type == NODE_REDIRECT_OUT)
        fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else if (type == NODE_REDIRECT_APPEND)
        fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    else
        return (-1);
    if (fd < 0)
        perror(file);
    return (fd);
}
```

**Mapeo de tipos a flags de open():**

- **`<`: `O_RDONLY`** (solo lectura)
- **`>`: `O_WRONLY | O_CREAT | O_TRUNC`** (sobrescribe)
- **`>>`: `O_WRONLY | O_CREAT | O_APPEND`** (añade al final)

### 🔄 **Flujo de redirección**

```
node->file: "output.txt"
node->type: NODE_REDIRECT_OUT
         ↓
open("output.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644) → fd=3
         ↓
dup2(3, STDOUT_FILENO) → STDOUT ahora apunta a output.txt
         ↓
close(3) → Libera descriptor original
```

### 🛡️ **Cobertura de errores**

✅ **Archivos inexistentes:** `perror()` muestra mensaje descriptivo  
✅ **Permisos insuficientes:** Detecta y reporta errores de acceso  
✅ **Fallos de `dup2()`:** Limpia descriptor en caso de error  
✅ **Tipos inválidos:** Retorna -1 para tipos no reconocidos  
❌ **No restaura descriptores:** Una vez redirigido, no hay vuelta atrás

---

## ⭐ **Función: `execute_simple_command()`**

```c
int	execute_simple_command(t_ast_node *node, t_shell_context *shell_context)
{
    t_ast_node	*cmd_node;
    t_ast_node	*redirects[1024];
    int			i;
    int			j;

    i = 0;
    cmd_node = node;
    while (cmd_node && is_redirect_node(cmd_node->type))
    {
        redirects[i++] = cmd_node;
        cmd_node = cmd_node->left;
    }
    j = i - 1;
    while (j >= 0)
        if (apply_redirections(redirects[j--]) != 0)
            return (1);
    if (cmd_node && cmd_node->args && cmd_node->args[0]
        && ft_strchr(cmd_node->args[0], '=') && !cmd_node->args[1])
        return (handle_variable_assignment(cmd_node->args, shell_context), 0);
    if (!cmd_node || !cmd_node->args || !cmd_node->args[0]
        || cmd_node->args[0][0] == '\0')
        return (0);
    if (is_builtin(cmd_node->args[0]))
        return (execute_builtin(cmd_node->args, shell_context));
    return (launch_command(cmd_node->args, shell_context->envp_cpy), 127);
}
```

### 🧠 **Análisis línea por línea**

1. **`i = 0; cmd_node = node;`** - Inicializa recorrido del árbol de redirecciones
2. **`while (cmd_node && is_redirect_node(cmd_node->type))`** - **RECOLECCIÓN:** Encuentra todas las redirecciones
3. **`redirects[i++] = cmd_node; cmd_node = cmd_node->left;`** - **ARRAY FIJO:** Almacena redirecciones en orden reverso
4. **`j = i - 1; while (j >= 0)`** - **APLICACIÓN REVERSA:** Aplica redirecciones en orden correcto
5. **`if (apply_redirections(redirects[j--]) != 0) return (1);`** - Si una redirección falla, aborta
6. **`if (cmd_node && cmd_node->args && cmd_node->args[0] && ft_strchr(cmd_node->args[0], '='))`** - **DETECCIÓN:** ¿Es asignación de variable?
7. **`if (!cmd_node || !cmd_node->args || !cmd_node->args[0])`** - **VALIDACIÓN:** ¿Hay comando válido?
8. **`if (is_builtin(cmd_node->args[0]))`** - **DESPACHO:** ¿Builtin o comando externo?
9. **`return (launch_command(cmd_node->args, shell_context->envp_cpy), 127);`** - **EJECUCIÓN EXTERNA:** Fork + execve

### 🎯 **Propósito de la función**

**Ejecutor de comandos simples.** Coordina la aplicación de redirecciones, detección de builtins, asignaciones de variables y lanzamiento de comandos externos. Es el caso base del sistema de ejecución.

⚠️ **Problema de diseño identificado**
**t_ast_node *redirects[1024];** - **RIESGO DE SEGURIDAD:** Array de tamaño fijo puede causar buffer overflow con muchas redirecciones.

### 🔄 **Flujo de ejecución de comando simple**

```
execute_simple_command()
        ↓
Recolectar redirecciones: [>, <, >>] → redirects[]
        ↓
Aplicar redirecciones en orden reverso
        ↓
¿Es asignación de variable? → VAR=value
        ↓
¿Es comando vacío? → return 0
        ↓
¿Es builtin? → execute_builtin()
        ↓
Comando externo → launch_command() → fork + execve
```

### 📊 **Ejemplo de estructura de redirecciones**

Para: `cmd < in.txt > out.txt`

```AST estructura:
    [REDIRECT_OUT (>)]
           |
    [REDIRECT_IN (<)]
           |
      [COMMAND: cmd]

Array redirects[]:
redirects[0] = REDIRECT_OUT_NODE
redirects[1] = REDIRECT_IN_NODE

Aplicación (orden reverso):
1. Apply REDIRECT_IN  (< in.txt)
2. Apply REDIRECT_OUT (> out.txt)
```

### 🛡️ **Cobertura de errores**

✅ **Redirecciones fallidas:** Retorna 1 si alguna redirección falla  
✅ **Comandos vacíos:** Maneja comandos sin argumentos  
✅ **Validación de argumentos:** Verifica estructura de `cmd_node->args`  
⚠️ **Buffer overflow potencial:** Array fijo de 1024 redirecciones  
❌ **No limpia redirecciones aplicadas:** Si falla a mitad, quedan descriptores abiertos

---

## 📝 **Notas clave para evaluación oral**

**¿Por qué parse_logical_expression() es el nivel más alto?**

- Los operadores lógicos (`&&`, `||`) tienen la **precedencia más baja**
- Evalúan `después` de pipes y redirecciones
- Implementan `cortocircuito` como en bash

**¿Cómo funciona la evaluación de cortocircuito?**

- **`cmd1 && cmd2`:** Si `cmd1` falla, `cmd2` no se ejecuta
- **`cmd1 || cmd2`:** Si `cmd1` tiene éxito, `cmd2` no se ejecuta
- Optimización y compatibilidad con bash

**¿Por qué aplicar redirecciones en orden reverso?**

- El AST se construye de **derecha a izquierda**
- **La última redirección debe "ganar"**: `cmd > file1 > file2` → escribe en `file2`
- El orden reverso simula la **precedencia de bash**

**¿Cuál es la diferencia entre builtin y comando externo?**

- **Builtins:** Ejecutan en el **proceso del shell** (no hay fork)
- **Comandos externos:** Requieren **fork + execve** (proceso separado)
- **Algunos builtins especiales** (`cd`, `export`) deben ejecutarse en el padre

**¿Qué pasa si una redirección falla?**

- El comando **no se ejecuta**
- Se retorna **código 1** (error estándar)
- En bash: `cmd < noexiste` → error, pero shell continúa

---

## ⭐ Función: `handle_simple_command_fork()`

```c
int	handle_simple_command_fork(t_ast_node *ast, t_shell_context *shell_context)
{
	pid_t	pid;
	int		status;
	int		exit_code;

	if (should_execute_in_parent(ast))
		return (execute_simple_command(ast, shell_context));
	pid = fork();
	if (pid == -1)
		return (perror("minishell: fork"), 1);
	if (pid == 0)
	{
		exit_code = execute_simple_command(ast, shell_context);
		exit(exit_code);
	}
	ignore_signals();
	waitpid(pid, &status, 0);
	setup_interactive_signals();
	return (analyze_child_status(status));
}
```

### 🧠 **Análisis línea por línea**

1. **`if (should_execute_in_parent(ast))`** - **DECISIÓN CRÍTICA:** ¿Ejecutar en proceso padre o hijo?
2. **`return (execute_simple_command(ast, shell_context));`** - Builtins especiales (`cd`, `export`, `unset`) se ejecutan sin fork
3. **`pid = fork();`** - **PUNTO DE BIFURCACIÓN:** Crea proceso hijo para comandos externos
4. **`if (pid == -1) return (perror("minishell: fork"), 1);`** - Manejo de error de fork
5. **`if (pid == 0)`** - **CÓDIGO DEL PROCESO HIJO**
6. **`exit_code = execute_simple_command(ast, shell_context);`** - Ejecuta comando en el hijo
7. **`exit(exit_code);`** - **CRÍTICO:** El hijo debe terminar con exit(), nunca return
8. **`ignore_signals();`** - **PROCESO PADRE:** Ignora señales mientras espera al hijo
9. **`waitpid(pid, &status, 0);`** - Espera que el hijo termine
10. **`setup_interactive_signals();`** - Restaura manejo interactivo de señales
11. **`return (analyze_child_status(status));`** - Convierte status del hijo en código de salida

### 🎯 **Propósito de la función**

**Coordinador de ejecución de comandos simples.** Decide si ejecutar en proceso padre (builtins) o hijo (comandos externos), gestiona el ciclo de vida del proceso hijo y maneja las señales apropiadamente.

### 📊 **Función auxiliar crítica: `should_execute_in_parent()`**

```c
static int	should_execute_in_parent(t_ast_node *ast)
{
	if (!ast || !ast->args || !ast->args[0])
		return (0);
	if (ft_strcmp(ast->args[0], "cd") == 0)
		return (1);
	if (ft_strcmp(ast->args[0], "export") == 0)
		return (1);
	if (ft_strcmp(ast->args[0], "unset") == 0)
		return (1);
	if (ft_strcmp(ast->args[0], "exit") == 0)
		return (1);
	return (0);
}
```

**Builtins que DEBEN ejecutarse en el padre:**
- **`cd`:** Debe cambiar el directorio del shell, no de un proceso hijo
- **`export`/`unset`:** Deben modificar las variables del shell padre
- **`exit`:** Debe terminar el shell principal, no un proceso hijo

### 🔄 **Flujo de decisión padre/hijo**

```
handle_simple_command_fork()
        ↓
should_execute_in_parent()?
    ├── SÍ → execute_simple_command() (sin fork)
    └── NO → fork()
            ├── HIJO → execute_simple_command() + exit()
            └── PADRE → ignore_signals() + waitpid() + setup_interactive_signals()
```

### 📊 **Función auxiliar: `analyze_child_status()`**

```c
static int	analyze_child_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		print_signal_message(WTERMSIG(status));
		return (128 + WTERMSIG(status));
	}
	return (status);
}
```

**Conversión de status a código de salida:**
- **Exit normal:** Usa `WEXITSTATUS()` directamente
- **Señal:** `128 + número_de_señal` (convención UNIX)
- **Ctrl+C:** `128 + 2 = 130`

### 🛡️ **Cobertura de errores**

✅ **Error de fork:** Detecta y reporta fallos de `fork()`  
✅ **Gestión de señales:** Ignora señales en padre durante ejecución  
✅ **Status del hijo:** Convierte correctamente códigos de señales  
✅ **Restauración de señales:** Siempre restaura manejo interactivo  
❌ **No maneja SIGKILL:** Algunas señales no se pueden capturar  

---

## ⭐ Función: `launch_command()`

```c
void	launch_command(char **args, char **envp)
{
	char	*path;

	if (!args || !args[0] || args[0][0] == '\0')
		exit(127);
	path = find_command_path(args[0], envp);
	if (!path)
		exit(printnget_command_error(args[0]));
	execve(path, args, envp);
	perror("minishell: execve");
	free(path);
	exit(126);
}
```

### 🧠 **Análisis línea por línea**

1. **`if (!args || !args[0] || args[0][0] == '\0') exit(127);`** - **VALIDACIÓN:** Argumentos válidos
2. **`path = find_command_path(args[0], envp);`** - **BÚSQUEDA CRÍTICA:** Resuelve comando en PATH
3. **`if (!path) exit(printnget_command_error(args[0]));`** - Comando no encontrado → exit 127
4. **`execve(path, args, envp);`** - **REEMPLAZO DEL PROCESO:** Ejecuta comando externo
5. **`perror("minishell: execve");`** - **LÍNEA DE EMERGENCIA:** Solo se ejecuta si execve falla
6. **`free(path);`** - Libera memoria (aunque nunca se alcance en ejecución normal)
7. **`exit(126);`** - Código 126: comando encontrado pero no ejecutable

### 🎯 **Propósito de la función**

**Ejecutor final de comandos externos.** Resuelve la ruta del comando, valida permisos y reemplaza el proceso actual con el comando solicitado usando `execve()`. **Esta función nunca retorna** en ejecución exitosa.

### 📊 **Función auxiliar crítica: `find_command_path()`**

```c
char	*find_command_path(char *cmd, char **envp)
{
	char	*path_env;
	char	**dirs;
	char	*full_path;

	if (check_direct_path(cmd))
		return (ft_strdup(cmd));
	path_env = get_path_from_env(envp);
	if (!path_env)
		return (NULL);
	dirs = ft_split(path_env, ':');
	if (!dirs)
		return (NULL);
	full_path = search_cmd_in_dirs(dirs, cmd);
	ft_freearr(dirs);
	return (full_path);
}
```

**Algoritmo de resolución:**
1. **¿Es ruta absoluta/relativa?** (`/bin/ls`, `./script`) → usar directamente
2. **Buscar en PATH:** Divide `PATH` por `:` y busca en cada directorio
3. **Verificar permisos:** `access(path, X_OK)` para validar ejecución

### 📊 **Función auxiliar: `printnget_command_error()`**

```c
static int	printnget_command_error(char *cmd)
{
	if (ft_strchr(cmd, '/') != NULL)
	{
		if (access(cmd, F_OK) != 0)
			ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		else if (access(cmd, X_OK) != 0)
			ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
	}
	else
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
	}
	return (127);
}
```

**Tipos de error según bash:**
- **Ruta explícita sin archivo:** `./script: No such file or directory`
- **Ruta explícita sin permisos:** `./script: Permission denied`
- **Comando en PATH:** `asdf: command not found`

### 🔄 **Flujo de ejecución de comando externo**

```
launch_command(["ls", "-la", NULL], envp)
        ↓
find_command_path("ls", envp)
        ↓
search_cmd_in_dirs(["/bin", "/usr/bin", ...], "ls")
        ↓
access("/bin/ls", X_OK) == 0 ✓
        ↓
execve("/bin/ls", ["ls", "-la", NULL], envp)
        ↓
🔄 PROCESO REEMPLAZADO - nunca retorna
```

### 💡 **Códigos de salida estándar**

- **126:** Comando encontrado pero no ejecutable (sin permisos)
- **127:** Comando no encontrado
- **128+N:** Comando terminado por señal N

### 🛡️ **Cobertura de errores**

✅ **Argumentos nulos:** Valida estructura de argumentos  
✅ **Comando no encontrado:** Mensaje descriptivo según tipo de error  
✅ **Permisos:** Distingue entre "no existe" y "sin permisos"  
✅ **PATH inexistente:** Maneja entornos sin variable PATH  
❌ **Memory leak:** Si execve falla, `path` no se libera (pero proceso termina)  

---

## ⭐ Función: `execute_builtin()`

```c
int	execute_builtin(char **args, t_shell_context *shell_context)
{
	if (ft_strcmp(args[0], "echo") == 0)
		return (builtin_echo(args));
	if (ft_strcmp(args[0], "cd") == 0)
		return (builtin_cd(args, shell_context));
	if (ft_strcmp(args[0], "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(args[0], "export") == 0)
		return (builtin_export(args, shell_context));
	if (ft_strcmp(args[0], "unset") == 0)
		return (builtin_unset(args, shell_context));
	if (ft_strcmp(args[0], "env") == 0)
		return (builtin_env(shell_context->envp_cpy));
	if (ft_strcmp(args[0], "exit") == 0)
		return (builtin_exit(args, shell_context));
	return (127);
}
```

### 🧠 **Análisis línea por línea**

1-7. **Cadena de `if-else`:** **DESPACHADOR SIMPLE** - Cada builtin tiene su función específica
8. **`return (127);`** - **CASO IMPOSIBLE:** Si llega aquí, `is_builtin()` falló en la detección

### 🎯 **Propósito de la función**

**Despachador central de comandos internos.** Mapea nombres de comandos a sus implementaciones específicas y delega la ejecución. Mantiene el estado del shell al ejecutar en el mismo proceso.

### 📊 **Función auxiliar: `is_builtin()`**

```c
int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}
```

### 🔄 **Clasificación de builtins por comportamiento**

```
BUILTINS SIN EFECTOS DE ESTADO:
├── echo → Imprime argumentos
└── pwd → Muestra directorio actual

BUILTINS QUE MODIFICAN ESTADO DEL SHELL:
├── cd → Cambia directorio de trabajo
├── export → Modifica variables de entorno
├── unset → Elimina variables de entorno
└── exit → Termina el shell

BUILTINS DE CONSULTA:
└── env → Muestra variables de entorno (sin modificar)
```

### 📊 **Ejemplo de implementación: `builtin_echo()`**

```c
int	builtin_echo(char **args)
{
	int	i;
	int	newline;

	newline = 1;
	i = 1;
	if (args[1] && ft_strcmp(args[1], "-n") == 0)
	{
		newline = 0;
		i = 2;
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], STDOUT_FILENO);
		if (args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
	if (newline)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
```

**Características del `echo` de minishell:**
- **Opción `-n`:** Suprime nueva línea final
- **Espacios entre argumentos:** Separados por un espacio
- **Siempre retorna 0:** `echo` nunca falla en bash

### 🛡️ **Cobertura de errores**

✅ **Comando reconocido:** Todas las funciones builtin manejan sus propios errores  
✅ **Argumentos nulos:** Cada builtin valida sus argumentos  
✅ **Estado consistente:** Los builtins mantienen el estado del shell  
❌ **Comando inexistente:** El retorno 127 nunca debería alcanzarse  

---

## ⭐ Función: `preprocess_heredocs()`

```c
int	preprocess_heredocs(t_ast_node **node_ptr, int *hd_id_ptr,
		t_shell_context *shell_context)
{
	t_ast_node	*node;

	node = *node_ptr;
	if (!node)
		return (0);
	if (node->type == NODE_HEREDOC)
		return (process_heredoc_node(node, hd_id_ptr, shell_context));
	if (preprocess_heredocs(&node->left, hd_id_ptr, shell_context) != 0)
		return (1);
	if (preprocess_heredocs(&node->right, hd_id_ptr, shell_context) != 0)
		return (1);
	return (0);
}
```

### 🧠 **Análisis línea por línea**

1. **`node = *node_ptr;`** - Obtiene el nodo actual para procesamiento
2. **`if (!node) return (0);`** - **CASO BASE:** Nodo nulo, no hay nada que procesar
3. **`if (node->type == NODE_HEREDOC)`** - **CASO OBJETIVO:** Encontró un heredoc
4. **`return (process_heredoc_node(node, hd_id_ptr, shell_context));`** - **PROCESAMIENTO:** Crea archivo temporal
5. **`if (preprocess_heredocs(&node->left, hd_id_ptr, shell_context) != 0)`** - **RECURSIÓN IZQUIERDA**
6. **`if (preprocess_heredocs(&node->right, hd_id_ptr, shell_context) != 0)`** - **RECURSIÓN DERECHA**
7. **`return (0);`** - Éxito: todos los heredocs procesados

### 🎯 **Propósito de la función**

**Preprocesador recursivo de heredocs.** Recorre todo el AST antes de la ejecución, encuentra nodos `<<` y los convierte en archivos temporales. **Crítico:** Debe ejecutarse antes que cualquier comando para que los heredocs estén listos.

### 📊 **Función auxiliar crítica: `process_heredoc_node()`**

```c
static int	process_heredoc_node(t_ast_node *node, int *heredoc_id_ptr,
		t_shell_context *shell_context)
{
	char	*filename;
	int		status;

	filename = build_heredoc_filename(*heredoc_id_ptr);
	if (!filename)
		return (1);
	(*heredoc_id_ptr)++;
	status = execute_heredoc(filename, node->file, shell_context);
	if (status != 0)
		return (free(filename), 1);
	add_heredoc_file(shell_context, filename);
	free(node->file);
	node->file = filename;
	return (0);
}
```

**Proceso de conversión de heredoc:**
1. **Genera filename único:** `/tmp/minishell-PID-ID`
2. **Ejecuta heredoc:** Crea proceso hijo para leer entrada
3. **Actualiza AST:** Reemplaza delimitador con filename del archivo temporal
4. **Rastrea archivos:** Añade filename para limpieza posterior

### 📊 **Función auxiliar: `build_heredoc_filename()`**

```c
static char	*build_heredoc_filename(int heredoc_id)
{
	char	*filename;
	char	*pid_str;
	char	*id_str;
	char	*temp;

	pid_str = ft_itoa(getpid());
	id_str = ft_itoa(heredoc_id);
	temp = ft_strjoin("/tmp/minishell-", pid_str);
	free(pid_str);
	if (!temp)
		return (free(id_str), NULL);
	filename = ft_strjoin(temp, id_str);
	free(temp);
	free(id_str);
	return (filename);
}
```

**Formato del filename:** `/tmp/minishell-[PID][ID]`
- **PID:** Evita colisiones entre shells simultáneos
- **ID:** Evita colisiones entre heredocs del mismo shell
- **Directorio `/tmp/`:** Ubicación estándar para archivos temporales

### 🔄 **Flujo de preprocesamiento**

```
preprocess_heredocs(AST_root)
        ↓
Recorrido recursivo (DFS)
        ↓
¿NODE_HEREDOC encontrado?
├── SÍ → process_heredoc_node()
│        ├── build_heredoc_filename()
│        ├── execute_heredoc() → fork + lee stdin
│        ├── add_heredoc_file()
│        └── reemplaza node->file
└── NO → continúa recursión
```

### 📊 **Transformación del AST**

**ANTES del preprocesamiento:**
```
[HEREDOC]
   file: "EOF"  ← delimitador
```

**DESPUÉS del preprocesamiento:**
```
[HEREDOC]
   file: "/tmp/minishell-1234-0"  ← archivo temporal
```

### 🛡️ **Cobertura de errores**

✅ **Heredoc interrumpido:** Si usuario presiona Ctrl+C, retorna código de error  
✅ **Fallo de archivo temporal:** Detecta errores de creación de archivos  
✅ **Memoria insuficiente:** Maneja fallos de `malloc` en generación de filenames  
✅ **Limpieza posterior:** Los archivos se rastrean para eliminación  
❌ **Directorio `/tmp/` no existe:** No verifica existencia del directorio temporal  

### 🔄 **Ejemplo completo de heredoc**

```bash
cat << EOF
line 1
line 2
EOF
```

**Procesamiento:**
1. **Parser crea:** `NODE_HEREDOC` con `file = "EOF"`
2. **Preprocess:** Convierte a `file = "/tmp/minishell-1234-0"`
3. **Execute:** `cat < /tmp/minishell-1234-0`
4. **Cleanup:** `unlink("/tmp/minishell-1234-0")`

---

## 📝 **Notas clave para evaluación oral**

**¿Por qué algunos builtins se ejecutan en el padre y otros podrían ejecutarse en hijo?**
- **Modifican estado del shell:** `cd`, `export`, `unset`, `exit` DEBEN ejecutarse en el padre
- **Solo consultan/imprimen:** `echo`, `pwd`, `env` podrían ejecutarse en hijo, pero por simplicidad se ejecutan en padre

**¿Qué pasa si `execve()` falla?**
- **Líneas después de execve:** Solo se ejecutan si `execve()` falla (archivo corrupto, sin memoria)
- **Código 126:** Indica que el archivo existe pero no es ejecutable
- **El proceso hijo termina:** Nunca retorna al shell principal

**¿Por qué preprocesan los heredocs antes de ejecutar?**
- **Interactividad:** El usuario debe introducir el contenido antes de que el comando necesite leerlo
- **Pipes complejos:** En `cat << EOF | grep pattern`, el heredoc debe estar listo antes del pipe
- **Errores tempranos:** Si el heredoc falla (Ctrl+C), el comando principal no se ejecuta

**¿Cómo se evitan conflictos entre heredocs simultáneos?**
- **PID único:** Cada shell tiene un PID diferente
- **ID incremental:** Cada heredoc en el mismo shell tiene ID único
- **Directorio temporal:** `/tmp/` es el estándar UNIX para archivos temporales

**¿Qué sucede con las señales durante `waitpid()`?**
- **Padre ignora SIGINT:** Para que Ctrl+C llegue al hijo, no al padre
- **Hijo recibe señal:** Termina apropiadamente con su propio handler
- **Padre analiza status:** Convierte señales en códigos de salida (128 + señal)

---

## ⭐ Función: `execute_pipe_line()`

```c
int	execute_pipe_line(t_ast_node *ast, int *heredoc_id_ptr,
	t_shell_context *shell_context)
{
	t_pipe_state	st;
	t_child_context	ctx;

	ctx.hd_id_ptr = heredoc_id_ptr;
	ctx.shell_context = shell_context;
	if (init_pipe_state(&st, ast) != 0)
		return (1);
	while (st.curr->type == NODE_PIPE)
		if (execute_pipe_segment(&st, &ctx) != 0)
			return (free(st.pids), 1);
	if (execute_last_command(&st, &ctx) != 0)
		return (free(st.pids), 1);
	return (wait_for_all_children(st.pids, st.num_cmds));
}
```

### 🧠 **Análisis línea por línea**

1. **`ctx.hd_id_ptr = heredoc_id_ptr;`** - **CONTEXTO HIJO:** Pasa heredoc ID a procesos hijo
2. **`ctx.shell_context = shell_context;`** - Pasa contexto del shell completo
3. **`if (init_pipe_state(&st, ast) != 0)`** - **INICIALIZACIÓN:** Cuenta comandos y reserva memoria para PIDs
4. **`while (st.curr->type == NODE_PIPE)`** - **BUCLE PRINCIPAL:** Procesa cada segmento de pipe
5. **`if (execute_pipe_segment(&st, &ctx) != 0)`** - Crea proceso hijo para comando intermedio
6. **`return (free(st.pids), 1);`** - **GESTIÓN DE ERROR:** Libera memoria antes de salir
7. **`if (execute_last_command(&st, &ctx) != 0)`** - **COMANDO FINAL:** Sin pipe de salida
8. **`return (wait_for_all_children(st.pids, st.num_cmds));`** - **SINCRONIZACIÓN:** Espera a todos los procesos

### 🎯 **Propósito de la función**

**Coordinador maestro de pipelines.** Gestiona la creación secuencial de procesos conectados por pipes, manteniendo el estado de cada segmento y sincronizando la finalización de todos los procesos hijo.

### 📊 **Función auxiliar crítica: `init_pipe_state()`**

```c
static int	init_pipe_state(t_pipe_state *st, t_ast_node *ast)
{
	st->num_cmds = count_pipe_commands(ast);
	st->pids = malloc(sizeof(pid_t) * st->num_cmds);
	if (!st->pids)
		return (perror("minishell: malloc"), 1);
	st->i = 0;
	st->curr = ast;
	st->prev_pipe_fd = -1;
	return (0);
}
```

**Inicialización del estado del pipeline:**
- **`count_pipe_commands()`:** Cuenta comandos en el AST (`cmd1 | cmd2 | cmd3` = 3 comandos)
- **`malloc()`:** Reserva array para PIDs de todos los procesos hijo
- **`prev_pipe_fd = -1`:** Indica que no hay pipe anterior en el primer comando

### 📊 **Función auxiliar: `execute_pipe_segment()`**

```c
static int	execute_pipe_segment(t_pipe_state *st, t_child_context *ctx)
{
	int	pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		return (perror("minishell: pipe"), 1);
	st->pids[st->i] = fork();
	if (st->pids[st->i] == -1)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (perror("minishell: fork"), 1);
	}
	if (st->pids[st->i] == 0)
		child_process_logic(st, pipe_fd, 0, ctx);
	parent_process_logic(st, pipe_fd);
	return (0);
}
```

**Patrón de segmento intermedio:**
1. **`pipe(pipe_fd)`:** Crea nuevo pipe para comunicación
2. **`fork()`:** Crea proceso hijo para este comando
3. **`child_process_logic()`:** Configura redirecciones y ejecuta comando
4. **`parent_process_logic()`:** Cierra descriptores innecesarios y avanza estado

### 🔄 **Flujo de pipeline `ls | grep txt | wc -l`**

```
execute_pipe_line(ast_pipeline)
        ↓
init_pipe_state() → num_cmds=3, pids[3]
        ↓
execute_pipe_segment(ls):
├── pipe() → [r0, w0]
├── fork() → pid[0]
├── HIJO: dup2(w0, STDOUT) + exec(ls)
└── PADRE: close(w0), prev_pipe_fd=r0
        ↓
execute_pipe_segment(grep):
├── pipe() → [r1, w1]
├── fork() → pid[1]  
├── HIJO: dup2(r0, STDIN) + dup2(w1, STDOUT) + exec(grep)
└── PADRE: close(r0, w1), prev_pipe_fd=r1
        ↓
execute_last_command(wc):
├── fork() → pid[2]
├── HIJO: dup2(r1, STDIN) + exec(wc)
└── PADRE: close(r1)
        ↓
wait_for_all_children(pids, 3)
```

### 🛡️ **Cobertura de errores**

✅ **Fallo de pipe:** Detecta errores en `pipe()` y limpia descriptores  
✅ **Fallo de fork:** Cierra pipes antes de retornar error  
✅ **Memoria insuficiente:** Valida `malloc()` en `init_pipe_state()`  
✅ **Limpieza garantizada:** Siempre libera `st.pids` en casos de error  
❌ **Señales durante pipeline:** No maneja SIGPIPE explícitamente  

---

## ⭐ Función: `builtin_cd()`

```c
int	builtin_cd(char **args, t_shell_context *shell_context)
{
	char	*path;

	if (!args[1])
	{
		path = get_env_value("HOME", shell_context);
		if (!path)
			return (ft_putendl_fd("minishell: cd: HOME not set", 2), 1);
	}
	else if (args[2])
		return (ft_putendl_fd("minishell: cd: too many arguments", 2), 1);
	else if (ft_strcmp(args[1], "") == 0)
		return (0);
	else if (ft_strcmp(args[1], "-") == 0)
	{
		path = get_env_value("OLDPWD", shell_context);
		if (!path)
			return (ft_putendl_fd("minishell: cd: OLDPWD not set", 2), 1);
		ft_putendl_fd(path, STDOUT_FILENO);
	}
	else
		path = args[1];
	return (go_to_path(path, shell_context));
}
```

### 🧠 **Análisis línea por línea**

1. **`if (!args[1])`** - **CASO: `cd` sin argumentos** → ir a HOME
2. **`path = get_env_value("HOME", shell_context);`** - Busca directorio HOME
3. **`if (!path) return (..., 1);`** - Error si HOME no está definido
4. **`else if (args[2])`** - **VALIDACIÓN:** `cd` acepta máximo 1 argumento
5. **`return (..., 1);`** - Error: demasiados argumentos
6. **`else if (ft_strcmp(args[1], "") == 0)`** - **CASO ESPECIAL:** `cd ""` → no hace nada
7. **`return (0);`** - Éxito sin cambio de directorio
8. **`else if (ft_strcmp(args[1], "-") == 0)`** - **CASO: `cd -`** → ir a OLDPWD
9. **`path = get_env_value("OLDPWD", shell_context);`** - Busca directorio anterior
10. **`ft_putendl_fd(path, STDOUT_FILENO);`** - **BASH COMPATIBLE:** Imprime directorio de destino
11. **`else path = args[1];`** - **CASO NORMAL:** Usar argumento como path
12. **`return (go_to_path(path, shell_context));`** - **DELEGACIÓN:** Ejecutar cambio real

### 🎯 **Propósito de la función**

**Implementación completa del builtin `cd`.** Maneja todos los casos especiales de bash (sin argumentos, `cd -`, argumentos vacíos) y delega el cambio real de directorio a una función especializada.

### 📊 **Función auxiliar crítica: `go_to_path()`**

```c
static int	go_to_path(const char *path, t_shell_context *shell_context)
{
	char	*old_pwd_val;
	char	*old_pwd_copy;
	char	*new_pwd;

	old_pwd_val = get_env_value("PWD", shell_context);
	old_pwd_copy = NULL;
	if (old_pwd_val)
	{
		old_pwd_copy = ft_strdup(old_pwd_val);
		if (!old_pwd_copy)
			return (1);
	}
	if (chdir(path) != 0)
		return (perror("minishell: cd"), free(old_pwd_copy), 1);
	if (old_pwd_copy)
		update_env_var("OLDPWD", old_pwd_copy, shell_context);
	free(old_pwd_copy);
	new_pwd = getcwd(NULL, 0);
	if (new_pwd)
		update_env_var("PWD", new_pwd, shell_context);
	if (new_pwd)
		free(new_pwd);
	else
		return (perror("minishell: cd: getcwd"), 1);
	return (0);
}
```

**Secuencia de cambio de directorio:**
1. **Guardar PWD actual:** Para establecer OLDPWD después
2. **`chdir(path)`:** Cambio real de directorio del proceso
3. **Actualizar OLDPWD:** Con el directorio anterior
4. **`getcwd(NULL, 0)`:** Obtener nuevo directorio absoluto
5. **Actualizar PWD:** Con el nuevo directorio

### 📊 **Función auxiliar: `update_env_var()`**

```c
static int	update_env_var(const char *var_name, const char *value,
							t_shell_context *shell_context)
{
	char	*new_var;
	char	*temp;
	char	*export_args[3];

	temp = ft_strjoin(var_name, "=");
	if (!temp)
		return (1);
	new_var = ft_strjoin(temp, value);
	free(temp);
	if (!new_var)
		return (1);
	export_args[0] = "export";
	export_args[1] = new_var;
	export_args[2] = NULL;
	builtin_export(export_args, shell_context);
	free(new_var);
	return (0);
}
```

**Patrón de actualización de variables:**
- **Construye formato `VAR=valor`:** Usando `ft_strjoin()`
- **Llama a `builtin_export()`:** Reutiliza lógica existente
- **Gestión de memoria:** Libera todas las cadenas temporales

### 🔄 **Casos de uso del `cd`**

| Comando | Comportamiento | Variables actualizadas |
|---------|---------------|----------------------|
| `cd` | → HOME | PWD, OLDPWD |
| `cd /tmp` | → /tmp | PWD, OLDPWD |
| `cd -` | → OLDPWD (imprime destino) | PWD, OLDPWD |
| `cd ""` | No cambio | Ninguna |
| `cd dir1 dir2` | Error: demasiados argumentos | Ninguna |

### 🛡️ **Cobertura de errores**

✅ **HOME no definido:** Mensaje de error específico  
✅ **OLDPWD no definido:** Error al usar `cd -`  
✅ **Directorio inexistente:** `chdir()` falla con `perror()`  
✅ **Demasiados argumentos:** Validación de argumentos  
✅ **getcwd() falla:** Detecta errores de lectura del directorio  
❌ **Permisos insuficientes:** Manejado por `chdir()` pero no específico  

---

## ⭐ Función: `expand_wildcards()`

```c
t_token	*expand_wildcards(t_token *tokens)
{
	t_token	*current;
	t_token	*prev;
	t_token	*head;

	head = tokens;
	prev = NULL;
	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_WORD && ft_strchr(current->value, '*'))
			process_wildcard_token(&current, &prev, &head);
		else
		{
			prev = current;
			current = current->next;
		}
	}
	return (head);
}
```

### 🧠 **Análisis línea por línea**

1. **`head = tokens;`** - **REFERENCIA INICIAL:** Mantiene puntero al primer token
2. **`prev = NULL;`** - **PUNTERO ANTERIOR:** Para reenlazar lista después de expansión
3. **`current = tokens;`** - **ITERADOR:** Recorre toda la lista de tokens
4. **`while (current)`** - **BUCLE PRINCIPAL:** Procesa cada token
5. **`if (current->type == TOKEN_WORD && ft_strchr(current->value, '*'))`** - **DETECCIÓN:** ¿Token contiene wildcard?
6. **`process_wildcard_token(&current, &prev, &head);`** - **EXPANSIÓN:** Reemplaza token con matches
7. **`else { prev = current; current = current->next; }`** - **AVANCE NORMAL:** Token sin wildcard
8. **`return (head);`** - **RETORNA:** Lista expandida (posiblemente modificada)

### 🎯 **Propósito de la función**

**Expansor de patrones wildcard.** Recorre la lista de tokens, detecta aquellos que contienen `*` y los reemplaza por todos los archivos que coinciden con el patrón en el directorio actual.

### 📊 **Función auxiliar crítica: `process_wildcard_token()`**

```c
static void	process_wildcard_token(t_token **current_ptr, t_token **prev_ptr,
									t_token **head_ptr)
{
	t_token	*matches;
	t_token	*current;
	t_token	*prev;

	current = *current_ptr;
	prev = *prev_ptr;
	matches = create_match_tokens(current->value);
	if (matches)
	{
		prev = replace_token_with_matches(prev, current, matches);
		if (!(*head_ptr) || *head_ptr == current)
			*head_ptr = matches;
		*current_ptr = prev;
	}
	*prev_ptr = *current_ptr;
	*current_ptr = (*current_ptr)->next;
}
```

**Proceso de reemplazo de token:**
1. **`create_match_tokens()`:** Busca archivos que coinciden con el patrón
2. **`replace_token_with_matches()`:** Reemplaza token original con lista de matches
3. **Actualiza punteros:** Mantiene integridad de la lista enlazada
4. **Actualiza `head`:** Si el primer token era wildcard

### 📊 **Función auxiliar: `create_match_tokens()`**

```c
static t_token	*create_match_tokens(const char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	t_token			*matches_head;
	t_token			*matches_curr;
	t_token			*new_token;

	matches_head = NULL;
	matches_curr = NULL;
	dir = opendir(".");
	if (!dir)
		return (NULL);
	entry = readdir(dir);
	while (entry != NULL)
	{
		if (match_file(pattern, entry->d_name))
		{
			new_token = create_token(TOKEN_WORD, ft_strdup(entry->d_name));
			if (!new_token)
				break ;
			add_token(&matches_head, &matches_curr, new_token);
		}
		entry = readdir(dir);
	}
	closedir(dir);
	return (matches_head);
}
```

**Algoritmo de matching:**
1. **`opendir(".")`:** Abre directorio actual
2. **`readdir(dir)`:** Lee cada entrada del directorio
3. **`match_file(pattern, entry->d_name)`:** Verifica si coincide con patrón
4. **Construye lista:** Crea tokens para cada match encontrado

### 📊 **Función auxiliar: `match_wildcard()`** (wildcard_utils.c)

```c
int	match_wildcard(const char *str, const char *pattern)
{
	if (*pattern == '\0')
		return (*str == '\0');
	if (*pattern == '*')
	{
		if (*(pattern + 1) == '\0')
			return (1);
		while (*str)
		{
			if (match_wildcard(str, pattern + 1))
				return (1);
			str++;
		}
		return (match_wildcard(str, pattern + 1));
	}
	if (*str == *pattern)
		return (match_wildcard(str + 1, pattern + 1));
	return (0);
}
```

**Algoritmo recursivo de matching:**
- **Caso base:** Patrón vacío → string debe estar vacío
- **Wildcard `*`:** Prueba todas las posibles subsecuencias
- **Carácter literal:** Debe coincidir exactamente
- **Backtracking:** Si falla, prueba siguiente posición

### 🔄 **Ejemplo de expansión `*.c`**

```
ANTES: [echo] [*.c]
        ↓
opendir(".") → {main.c, utils.c, test.h}
        ↓
match_wildcard("main.c", "*.c") → ✓
match_wildcard("utils.c", "*.c") → ✓  
match_wildcard("test.h", "*.c") → ✗
        ↓
DESPUÉS: [echo] [main.c] [utils.c]
```

### 📊 **Función auxiliar: `match_file()`**

```c
static int	match_file(const char *pattern, const char *filename)
{
	if ((pattern[0] == '.' || filename[0] != '.')
		&& match_wildcard(filename, pattern))
		return (1);
	else
		return (0);
}
```

**Reglas de archivos ocultos:**
- **Patrón inicia con `.`:** Puede hacer match con archivos ocultos
- **Patrón sin `.`:** NO hace match con archivos ocultos (`.bashrc`)
- **Comportamiento bash:** `*` no incluye archivos ocultos, `.*` sí

### 🛡️ **Cobertura de errores**

✅ **Directorio inaccesible:** `opendir()` retorna NULL → no expansión  
✅ **Sin matches:** Token original se mantiene (bash compatible)  
✅ **Memoria insuficiente:** Falla gracefully en `create_token()`  
✅ **Archivos ocultos:** Respeta convenciones de bash  
❌ **Patrones complejos:** Solo soporta `*`, no `?` o `[ranges]`  

### 🔄 **Casos especiales de wildcard**

| Patrón | Archivos | Resultado |
|--------|----------|-----------|
| `*.txt` | file.txt, doc.txt | `file.txt doc.txt` |
| `*` | file.txt, .hidden | `file.txt` (sin ocultos) |
| `.*` | .hidden, .bashrc | `.hidden .bashrc` |
| `no_match*` | (ninguno) | `no_match*` (sin cambio) |

---

## ⭐ Función: `create_pipe_child()` (Análisis adicional)

```c
pid_t	create_pipe_child(t_ast_node *node, t_pipe_config *config,
		int *heredoc_id_ptr, t_shell_context *shell_context)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (perror("fork"), -1);
	if (pid == 0)
		child_process_routine(node, config, heredoc_id_ptr, shell_context);
	return (pid);
}
```

### 🧠 **Análisis línea por línea**

1. **`pid = fork();`** - **BIFURCACIÓN:** Crea proceso hijo para comando en pipe
2. **`if (pid == -1)`** - **ERROR DE SISTEMA:** No se pudo crear proceso
3. **`return (perror("fork"), -1);`** - Reporta error y retorna -1
4. **`if (pid == 0)`** - **CÓDIGO DEL HIJO:** Solo se ejecuta en proceso hijo
5. **`child_process_routine(...);`** - **DELEGACIÓN:** Configura redirecciones y ejecuta
6. **`return (pid);`** - **PROCESO PADRE:** Retorna PID del hijo creado

### 📊 **Función auxiliar: `child_process_routine()`**

```c
static void	child_process_routine(t_ast_node *node,	t_pipe_config *config,
			int *heredoc_id_ptr, t_shell_context *shell_context)
{
	int	exit_code;

	setup_child_signals();
	if (config->is_left)
	{
		if (config->heredoc_fd != -1)
			dup2(config->heredoc_fd, STDIN_FILENO);
		dup2(config->pipe_fd[1], STDOUT_FILENO);
	}
	else
		dup2(config->pipe_fd[0], STDIN_FILENO);
	close(config->pipe_fd[0]);
	close(config->pipe_fd[1]);
	if (config->heredoc_fd != -1)
		close(config->heredoc_fd);
	exit_code = execute_ast(node, heredoc_id_ptr, shell_context);
	exit(exit_code);
}
```

**Configuración del proceso hijo:**
1. **`setup_child_signals()`:** Restaura señales por defecto
2. **Redirecciones según posición:**
   - **Comando izquierdo:** STDOUT → pipe, STDIN desde heredoc si existe
   - **Comando derecho:** STDIN desde pipe anterior
3. **Cierra descriptores:** Evita file descriptor leaks
4. **`execute_ast()`:** Ejecuta el nodo AST
5. **`exit(exit_code)`:** **CRÍTICO:** El hijo nunca retorna

### 🎯 **Propósito de la función**

**Factor de procesos hijo especializados.** Crea procesos hijo configurados específicamente para ejecutar comandos en pipelines, con las redirecciones apropiadas según su posición en el pipe.

---

## 📝 **Notas clave para evaluación oral**

**¿Cómo gestiona minishell pipelines complejos como `cmd1 | cmd2 | cmd3`?**
- **Creación secuencial:** Un proceso por comando, conectados por pipes
- **Estado centralizado:** `t_pipe_state` mantiene PIDs y descriptores
- **Sincronización:** `wait_for_all_children()` espera a todos antes de continuar

**¿Por qué `cd` debe actualizarse PWD y OLDPWD?**
- **Compatibilidad bash:** Scripts dependen de estas variables
- **Navegación con `cd -`:** Requiere OLDPWD para funcionar
- **Procesos hijo:** Heredan el directorio correcto del padre

**¿Cómo maneja wildcards sin coincidencias?**
- **Comportamiento bash:** Deja el patrón original sin expandir
- **No es error:** `ls *.inexistente` pasa `*.inexistente` al comando
- **Detección temprana:** Solo expande si hay al menos un match

**¿Por qué archivos ocultos necesitan tratamiento especial?**
- **Convención UNIX:** `*` no debe incluir archivos que empiezan con `.`
- **Seguridad:** Evita exponer accidentalmente configuraciones (`.ssh/`)
- **Patrón explícito:** Solo `.*` puede hacer match con archivos ocultos

**¿Qué sucede si fallan los pipes en un pipeline?**
- **Error temprano:** Si `pipe()` falla, no se crean procesos hijo
- **Limpieza automática:** Descriptores se cierran, memoria se libera
- **Código de salida:** Retorna error sin ejecutar comandos pendientes

---

## 🎯 **Análisis completo finalizado**

El análisis detallado de **minishell** está ahora completo, cubriendo:

### 📋 **Funciones principales documentadas:**
1. **Inicialización:** `main()`, `init_shell_environment()`, `cleanup_shell_context()`
2. **Bucle principal:** `run_shell_loop()`, `get_user_input()`, `handle_input_line()`
3. **Parser:** `tokenize()`, `parse()`, `parse_logical_expression()`
4. **Ejecutor:** `execute_ast()`, `execute_simple_command()`, `apply_redirections()`
5. **Procesos:** `handle_simple_command_fork()`, `launch_command()`, `execute_builtin()`
6. **Heredocs:** `preprocess_heredocs()`, `execute_heredoc()`, `create_heredoc_file()`
7. **Pipelines:** `execute_pipe_line()`, `create_pipe_child()`, `wait_for_all_children()`
8. **Builtins:** `builtin_cd()`, `builtin_echo()`, `builtin_export()`
9. **Wildcards:** `expand_wildcards()`, `match_wildcard()`, `create_match_tokens()`

### 🎓 **Preparación para defensa oral:**
- ✅ **Análisis línea por línea** de cada función crítica
- ✅ **Flujos de ejecución** con diagramas detallados  
- ✅ **Manejo de errores** y casos edge documentados
- ✅ **Compatibilidad bash** explicada en cada componente
- ✅ **Gestión de memoria** y prevención de leaks
- ✅ **Preguntas típicas** de evaluación con respuestas preparadas

¡El análisis está listo para una defensa oral exitosa! 🚀

---
