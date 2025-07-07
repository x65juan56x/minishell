## Análisis del Flujo de Ejecución de Minishell
### 1. Inicio del Programa - main() en minishell.c
```c
int	main(int ac, char **av, char **envp)
{
    char		**envp_copy;
    int			exit_status;

    (void)ac;
    (void)av;
    envp_copy = init_shell_environment(envp);
    exit_status = run_shell_loop(&envp_copy);
    ft_freearr(envp_copy);
    rl_clear_history();
    return (exit_status);
}
```
Análisis línea por línea:  
1.char **envp_copy; - Declara un puntero para almacenar una copia del entorno  
2.int exit_status; - Variable para el código de salida final del shell  
3.(void)ac; (void)av; - Suprime warnings del compilador por parámetros no usados  
4.envp_copy = init_shell_environment(envp); - CLAVE: Crea una copia del entorno  
5.exit_status = run_shell_loop(&envp_copy); - CLAVE: Ejecuta el bucle principal  
6.ft_freearr(envp_copy); - Libera la memoria del entorno copiado  
7.rl_clear_history(); - Limpia el historial de readline al salir  
8.return (exit_status); - Retorna el código de salida al sistema

### 2. Inicialización del Entorno - init_shell_environment() en shell_init.c
```c
char	**init_shell_environment(char **envp)
{
    char	**envp_copy;

    envp_copy = copy_envp(envp);
    if (!envp_copy)
    {
        ft_putstr_fd("minishell: failed to initialize environment\n", STDERR_FILENO);
        exit(1);
    }
    return (envp_copy);
}
```
Análisis:  
1.envp_copy = copy_envp(envp); - Llama a la función que duplica el entorno  
2.if (!envp_copy) - Verificación de error de memoria  
3.Error handling - Si falla, imprime error y termina el programa  
4.return (envp_copy); - Retorna la copia exitosa
Función auxiliar copy_envp():
```c
static char	**copy_envp(char **envp)
{
    char	**new_envp;
    int		i;
    int		count;

    count = 0;
    while (envp[count])         // Cuenta las variables de entorno
        count++;
    new_envp = malloc(sizeof(char *) * (count + 1));  // Aloca array de punteros
    if (!new_envp)
        return (NULL);
    i = -1;
    while (++i < count)
    {
        new_envp[i] = ft_strdup(envp[i]);   // Duplica cada string
        if (!new_envp[i])
            return (ft_freearr(new_envp), NULL);
    }
    new_envp[i] = NULL;         // Termina con NULL
    return (new_envp);
}
```
El entorno se copia porque el shell puede modificar variables (con export/unset) sin afectar el proceso padre (la shell que ejecuta la minishell).
Escenario típico:
```bash
bash$ ./minishell        # ← bash es el proceso padre
MiniShell $ export VAR=valor
MiniShell $ exit
bash$ echo $VAR          # ← VAR no existe aquí
```
¿Cómo se vería afectado si no copiáramos envp?  
Si modificáramos el envp original directamente:
```c
// PELIGROSO: Modificar envp directamente
int main(int ac, char **av, char **envp)
{
    // Si hiciéramos esto...
    envp[0] = "PATH=/mi/nuevo/path";  // Modificación directa
    // ...
}
```
Consecuencias:  
1.Corrupción de memoria: El envp original no fue alocado por tu proceso  
2.Segmentation fault: Intentar escribir en memoria de solo lectura  
3.Comportamiento indefinido: El sistema podría crashear
La copia nos permite:  
-Modificar variables sin afectar al proceso padre  
-Añadir/quitar variables con export/unset  
-Gestionar memoria de forma segura  
-Mantener aislamiento entre procesos  

### 3. Bucle Principal del Shell - run_shell_loop() en shell_loop.c
```c
int	run_shell_loop(char ***envp_ptr)
{
    char	*input;
    int		exit_status;
    int		should_exit;

    exit_status = 0;
    while (1)
    {
        //signals_parent();
        input = get_user_input();
        if (!input)
            break ;
        should_exit = handle_input_line(input);
        if (should_exit)
        {
            free(input);
            break ;
        }
        if (*input)
            exit_status = process_command_line(input, envp_ptr);
        
        free(input);
    }
    return (exit_status);
}
```
Análisis línea por línea:  
1.exit_status = 0; - Inicializa el código de salida  
2.while (1) - Bucle infinito hasta que se indique salir  
3.input = get_user_input(); - CLAVE: Obtiene entrada del usuario  
4.if (!input) break; - Si es NULL (Ctrl+D), sale del bucle  
5.should_exit = handle_input_line(input); - Procesa línea para comandos especiales  
6.if (should_exit) - Si es comando exit, sale del bucle  
7.if (*input) - Si la línea no está vacía, la procesa  
8.exit_status = process_command_line(input, envp_ptr); - CLAVE: Procesa el comando  
9.free(input); - Libera la memoria de la entrada  

### 4. Obtener Entrada del Usuario - get_user_input() en shell_input.c
```c
char	*get_user_input(void)
{
    char	*input;

    signals_parent();
    input = readline(PROMPT);
    if (!input)
    {
        printf("exit\n");
        return (NULL);
    }
    return (input);
}
```
Análisis:  
1.signals_parent(); - Configura el manejo de señales para modo interactivo  
2.input = readline(PROMPT); - CLAVE: Llama a readline con el prompt    
3.if (!input) - Si readline devuelve NULL (EOF/Ctrl+D)  
4.printf("exit\n"); - Imprime "exit" al salir con Ctrl+D  
5.return (input); - Retorna la línea leída (o NULL)  

### 5. Manejo de Línea de Entrada - handle_input_line() en shell_input.c
```c
int	handle_input_line(char *input)
{
    if (!*input)
        return (0);
    add_history(input);
    if (ft_strcmp(input, "exit") == 0)
        return (1);
    return (0);
}
```
Análisis:  
1.if (!*input) - Si la línea está vacía (solo \n), no hace nada  
2.add_history(input); - Añade la línea al historial de readline  
3.if (ft_strcmp(input, "exit") == 0) - Detecta comando exit hardcodeado  
4.return (1); - Indica que debe salir del shell  
5.return (0); - Indica que debe continuar  

### 6. Procesamiento de Comando - process_command_line() en shell_process.c
```c
int	process_command_line(char *input, char ***envp_ptr)
{
    t_token		*tokens;
    t_ast_node	*ast;
    int			exit_status;

    tokens = tokenize(input);
    if (!tokens)
        return (0);
    
    ast = parse(tokens);
    if (!ast)
    {
        exit_status = handle_parsing_error(tokens);
        cleanup_tokens(tokens);
        return (exit_status);
    }
    
    exit_status = execute_ast(ast, envp_ptr);
    cleanup_ast(ast);
    cleanup_tokens(tokens);
    rl_on_new_line();
    return (exit_status);
}
```
Análisis línea por línea:  
1.tokens = tokenize(input); - PUNTO CLAVE: Aquí llega al tokenizador  
2.if (!tokens) return (0); - Si tokenización falla, retorna 0  
3.ast = parse(tokens); - Convierte tokens en AST  
4.Error handling para parsing - Si falla, limpia y retorna error  
5.exit_status = execute_ast(ast, envp_ptr); - Ejecuta el AST  
6.Limpieza de memoria - Libera AST y tokens  
7.rl_on_new_line(); - Informa a readline que estamos en nueva línea  
8.return (exit_status); - Retorna código de salida del comando  

## Análisis Detallado del Tokenizador
### 7. Función Principal - tokenize() en tokenizer.c
```c
t_token	*tokenize(const char *input)
{
    t_token	*head;
    t_token	*current;
    t_token	*new_token;

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
Análisis línea por línea:  
1.if (!input) return (NULL); - Validación de entrada  
2.head = NULL; current = NULL; - Inicializa punteros de la lista enlazada  
3.if (generate_token_list(input, &head, &current) != 0) - CLAVE: Genera todos los tokens  
4.Error handling - Si falla, limpia memoria parcial y retorna NULL  
5.new_token = create_token(TOKEN_EOF, NULL); - Crea token de fin de archivo  
6.add_token(&head, &current, new_token); - Añade EOF al final  
7.return (head); - Retorna la cabeza de la lista de tokens  
¿Por qué TOKEN_EOF? El parser necesita saber cuándo terminan los tokens. Es como el \0 en las strings.  

### 8. Motor Principal - generate_token_list() en tokenizer.c
```c
static int	generate_token_list(const char *input, t_token **head,
            t_token **current)
{
    int		i;
    t_token	*new_token;

    i = 0;
    while (input[i])
    {
        while (input[i] && input[i] == ' ')
            i++;
        if (!input[i])
            break ;
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
Análisis línea por línea:  
1.i = 0; - Índice para recorrer la cadena de entrada  
2.while (input[i]) - Bucle principal hasta el final de la string  
3.while (input[i] && input[i] == ' ') i++; - Salta espacios en blanco  
4.if (!input[i]) break; - Si llegamos al final después de espacios, termina  
5.if (is_operator_char(input[i])) - Decisión clave: ¿operador o palabra?  
6.new_token = process_operator(input, &i); - Procesa operadores (|, >, etc.)  
7.new_token = process_word(input, &i); - Procesa palabras y argumentos  
8.if (!new_token) return (1); - Error de memoria o sintaxis  
9.add_token(head, current, new_token); - Añade token a la lista enlazada  

Lógica Central:  
El tokenizador funciona como una máquina de estados simple:  
* Estado 1: Saltar espacios
* Estado 2: Identificar tipo (operador vs palabra)
* Estado 3: Procesar según el tipo
* Estado 4: Añadir a la lista y repetir

### 9. Identificación de Operadores - is_operator_char() en src/parser/token_operators.c
```c
int	is_operator_char(char c)
{
    return (c == '|' || c == '<' || c == '>' || c == '&'
        || c == '(' || c == ')');
}
```
Análisis:  
Función que determina si un carácter puede iniciar un operador  
Caracteres soportados: |, <, >, &, (, )  
Uso: Esta función es el dispatcher principal que decide el flujo del tokenizador

### 10. Procesamiento de Operadores - process_operator() en src/parser/tokenizer.c
```c
static t_token	*process_operator(const char *input, int *i)
{
    int				advance;
    t_token_type	type;
    char			*value;

    advance = 0;
    type = get_operator_type(&input[*i], &advance);
    value = ft_substr(input, *i, advance);
    if (!value)
        return (NULL);
    *i += advance;
    return (create_token(type, value));
}
```
Análisis línea por línea:  
1.advance = 0; - Variable para saber cuántos caracteres consume el operador  
2.type = get_operator_type(&input[*i], &advance); - CLAVE: Identifica el operador exacto  
3.value = ft_substr(input, *i, advance); - Extrae la substring del operador  
4.*i += advance; - Avanza el índice principal según los caracteres consumidos  
5.return (create_token(type, value)); - Crea el token con tipo y valor  
Flujo de ejemplo:
```
Entrada: "ls > file.txt"
Posición 3: '>'
1. get_operator_type() → type=TOKEN_REDIRECT_OUT, advance=1
2. ft_substr() → value=">"
3. i avanza de 3 a 4
4. create_token(TOKEN_REDIRECT_OUT, ">")
```
### 11. Identificación Específica - get_operator_type() en token_operators.c
```c
t_token_type	get_operator_type(const char *s, int *advance)
{
    if (s[0] == '|')
        return (get_pipe_operator(s, advance));
    if (s[0] == '<' || s[0] == '>')
        return (get_redirect_operator(s, advance));
    if (s[0] == '&' && s[1] == '&')
    {
        *advance = 2;
        return (TOKEN_AND);
    }
    if (s[0] == '(')
    {
        *advance = 1;
        return (TOKEN_LPAREN);
    }
    if (s[0] == ')')
    {
        *advance = 1;
        return (TOKEN_RPAREN);
    }
    *advance = 1;
    return (TOKEN_WORD);
}
```
Análisis:  
1.if (s[0] == '|') - Delega pipes a función especializada (puede ser | o ||)  
2.if (s[0] == '<' || s[0] == '>') - Delega redirecciones (<, >, <<, >>)  
3.if (s[0] == '&' && s[1] == '&') - Maneja AND lógico (&&) directamente  
4.Paréntesis - Maneja ( y ) para agrupaciones (bonus)  
5.Fallback - Si no es operador conocido, lo trata como palabra  
Funciones auxiliares especializadas:  
get_pipe_operator():
```c
static t_token_type	get_pipe_operator(const char *s, int *advance)
{
    if (s[1] == '|')
    {
        *advance = 2;
        return (TOKEN_OR);
    }
    *advance = 1;
    return (TOKEN_PIPE);
}
```
* || → TOKEN_OR (2 caracteres)
* | → TOKEN_PIPE (1 carácter)  

get_redirect_operator():
```c
static t_token_type	get_redirect_operator(const char *s, int *advance)
{
    if (s[0] == '<')
    {
        if (s[1] == '<')
        {
            *advance = 2;
            return (TOKEN_HEREDOC);
        }
        *advance = 1;
        return (TOKEN_REDIRECT_IN);
    }
    if (s[1] == '>')
    {
        *advance = 2;
        return (TOKEN_REDIRECT_APPEND);
    }
    *advance = 1;
    return (TOKEN_REDIRECT_OUT);
}
```
* << → TOKEN_HEREDOC (2 caracteres)
* < → TOKEN_REDIRECT_IN (1 carácter)
* \>> → TOKEN_REDIRECT_APPEND (2 caracteres)
* \> → TOKEN_REDIRECT_OUT (1 carácter)

### 12. Procesamiento de Palabras - process_word() en tokenizer.c
```c
static t_token	*process_word(const char *input, int *i)
{
    char	*value;

    value = extract_word_token(input, i);
    if (!value)
        return (NULL);
    return (create_token(TOKEN_WORD, value));
}
```
Análisis:  
value = extract_word_token(input, i); - CLAVE: Extrae palabra compleja con comillas  
return (create_token(TOKEN_WORD, value)); - Todas las palabras son TOKEN_WORD  

### 13. Extracción de Palabras Complejas - extract_word_token() en src/parser/token_words.c
```c
char	*extract_word_token(const char *s, int *i)
{
    int	start;
    int	end;

    start = *i;
    end = find_word_end(s, start);
    *i = end;
    return (process_quoted_string(s, start, end));
}
```
Análisis:  
1.start = *i; - Marca el inicio de la palabra  
2.end = find_word_end(s, start); - Encuentra el final (manejando comillas)  
3.*i = end; - Actualiza el índice principal  
4.return (process_quoted_string(s, start, end)); - Procesa y limpia comillas  

Función auxiliar find_word_end():
```c
static int	find_word_end(const char *s, int start)
{
    int		i;
    char	quote;

    i = start;
    while (s[i] && !is_operator_char(s[i]) && s[i] != ' ')
    {
        if (s[i] == '\'' || s[i] == '"')
        {
            quote = s[i];
            i++;
            while (s[i] && s[i] != quote)
                i++;
            if (s[i] == quote)
                i++;
        }
        else
            i++;
    }
    return (i);
}
```
Lógica compleja:  
1.Itera hasta encontrar espacio, operador o fin de string  
2.Si encuentra comilla, salta toda la sección entrecomillada  
3.Maneja comillas anidadas correctamente  
4.Retorna la posición después del último carácter válido

Función auxiliar process_quoted_string():
```c
char	*process_quoted_string(const char *s, int start, int end)
{
    char	*result;
    int		j;
    int		k;
    char	quote;

    result = malloc(end - start + 1);
    if (!result)
        return (NULL);
    j = 0;
    k = start;
    while (k < end)
    {
        if (s[k] == '\'' || s[k] == '"')
        {
            quote = s[k++];
            while (k < end && s[k] != quote)
                result[j++] = s[k++];
            if (k < end && s[k] == quote)
                k++;
        }
        else
            result[j++] = s[k++];
    }
    result[j] = '\0';
    return (result);
}
```
Procesamiento de comillas:

1.Aloca memoria para el resultado procesado  
2.Copia caracteres normales directamente  
3.Para secciones entrecomilladas: copia el contenido sin las comillas  
4.Resultado: String "limpio" sin comillas externas  

Ejemplo de procesamiento:
```
Entrada: "echo \"hello world\" test"
Tokens resultantes:
1. TOKEN_WORD: "echo"
2. TOKEN_WORD: "hello world"  ← Sin comillas externas
3. TOKEN_WORD: "test"
```

### 14. Gestión de Tokens - Funciones en token_utils.c
create_token():
```c
t_token	*create_token(t_token_type type, char *value)
{
    t_token	*token;

    token = malloc(sizeof(t_token));
    if (!token)
    {
        if (value)
            free(value);
        return (NULL);
    }
    token->type = type;
    token->value = value;
    token->next = NULL;
    return (token);
}
```
* Factory function para crear tokens
* Gestión de errores: libera value si malloc falla
* Inicialización completa de la estructura

add_token():
```c
void	add_token(t_token **head, t_token **current, t_token *new_token)
{
    if (!*head)
    {
        *head = new_token;
        *current = new_token;
    }
    else
    {
        (*current)->next = new_token;
        *current = new_token;
    }
}
```
* Añade al final de la lista enlazada
* Mantiene punteros head y current actualizados

### Flujo Visual Completo del Tokenizador
```
Entrada: "cat << EOF | sort > out.txt"

tokenize()
    ↓
generate_token_list()
    ↓
i=0: 'c' → !is_operator_char() → process_word()
    ↓
extract_word_token() → find_word_end() → end=3
    ↓
process_quoted_string() → "cat"
    ↓
create_token(TOKEN_WORD, "cat") → add_token()

i=3: ' ' → skip spaces → i=4

i=4: '<' → is_operator_char() → process_operator()
    ↓
get_operator_type() → get_redirect_operator() 
    ↓
s[1]='<' → TOKEN_HEREDOC, advance=2
    ↓
ft_substr() → "<<"
    ↓
create_token(TOKEN_HEREDOC, "<<") → add_token()

i=6: ' ' → skip spaces → i=7

i=7: 'E' → !is_operator_char() → process_word()
    ↓
extract_word_token() → "EOF"
    ↓
create_token(TOKEN_WORD, "EOF") → add_token()

... (continúa hasta el final)

Resultado final:
[TOKEN_WORD:"cat"] → [TOKEN_HEREDOC:"<<"] → [TOKEN_WORD:"EOF"] → 
[TOKEN_PIPE:"|"] → [TOKEN_WORD:"sort"] → [TOKEN_REDIRECT_OUT:">"] → 
[TOKEN_WORD:"out.txt"] → [TOKEN_EOF:NULL]
```
Estado cuando sale del tokenizador:  
Entrada original: "cat << EOF | sort > out.txt"

Lista de tokens generada:
```c
t_token *tokens = {
    {TOKEN_WORD, "cat", next→},
    {TOKEN_HEREDOC, "<<", next→},
    {TOKEN_WORD, "EOF", next→},
    {TOKEN_PIPE, "|", next→},
    {TOKEN_WORD, "sort", next→},
    {TOKEN_REDIRECT_OUT, ">", next→},
    {TOKEN_WORD, "out.txt", next→},
    {TOKEN_EOF, NULL, NULL}
};
```
Ahora esta lista está lista para ser consumida por el parser, que construirá el AST basándose en la precedencia de operadores y la gramática del shell.

## Análisis Detallado del Parser
### 15. Nivel Superior - parse_or_expression() en parser_expressions.c
```c
t_ast_node *parse_or_expression(t_parser *parser)
{
    t_ast_node *left;
    t_ast_node *right;
    t_ast_node *new_node;

    left = parse_and_expression(parser);
    if (!left)
        return (NULL);
    while (parser->current && parser->current->type == TOKEN_OR)
    {
        consume_token_type(parser, TOKEN_OR);
        if (parser->error)
            return (cleanup_ast(left), NULL);
        right = parse_and_expression(parser);
        if (!right)
            return (cleanup_ast(left), NULL);
        new_node = create_binary_node(TOKEN_OR, left, right);
        if (!new_node)
            return (cleanup_ast(left), cleanup_ast(right), NULL);
        left = new_node;
    }
    return (left);
}
```

**Análisis línea por línea:**
1. `left = parse_and_expression(parser);` - Parsea el lado izquierdo con mayor precedencia
2. `if (!left) return (NULL);` - Error en parsing, propaga NULL
3. `while (parser->current && parser->current->type == TOKEN_OR)` - Busca operadores `||`
4. `consume_token_type(parser, TOKEN_OR);` - Consume y valida el token `||`
5. `if (parser->error)` - Verifica errores de consumo
6. `right = parse_and_expression(parser);` - Parsea el lado derecho
7. `new_node = create_binary_node(TOKEN_OR, left, right);` - Crea nodo OR
8. `left = new_node;` - **CLAVE**: Asociatividad izquierda para OR múltiples

**Ejemplo de precedencia OR:**
```bash
Entrada: "echo fail || echo backup || echo final"
Tokens: [WORD:"echo"] [WORD:"fail"] [OR:"||"] [WORD:"echo"] [WORD:"backup"] [OR:"||"] [WORD:"echo"] [WORD:"final"]

Parsing paso a paso:
1. left = parse_and_expression() → NODE_COMMAND("echo fail")
2. TOKEN_OR encontrado → consume_token_type()
3. right = parse_and_expression() → NODE_COMMAND("echo backup")
4. new_node = create_binary_node(OR, left:"echo fail", right:"echo backup")
5. left = new_node (ahora left es el nodo OR)
6. TOKEN_OR encontrado → consume_token_type()
7. right = parse_and_expression() → NODE_COMMAND("echo final")
8. new_node = create_binary_node(OR, left:(fail||backup), right:"echo final")

AST resultante:
           OR
          /  \
        OR    "echo final"
       /  \
"echo fail" "echo backup"

Ejecución: Si "echo fail" falla (exit ≠ 0), ejecuta "echo backup"
           Si "echo backup" también falla, ejecuta "echo final"
```

### 16. Nivel AND - parse_and_expression() en parser_expressions.c
```c
t_ast_node *parse_and_expression(t_parser *parser)
{
    t_ast_node *left;
    t_ast_node *right;
    t_ast_node *new_node;

    left = parse_pipe_expression(parser);
    if (!left)
        return (NULL);
    while (parser->current && parser->current->type == TOKEN_AND)
    {
        consume_token_type(parser, TOKEN_AND);
        if (parser->error)
            return (cleanup_ast(left), NULL);
        right = parse_pipe_expression(parser);
        if (!right)
            return (cleanup_ast(left), NULL);
        new_node = create_binary_node(TOKEN_AND, left, right);
        if (!new_node)
            return (cleanup_ast(left), cleanup_ast(right), NULL);
        left = new_node;
    }
    return (left);
}
```

**Análisis línea por línea:**
1. `left = parse_pipe_expression(parser);` - Parsea el lado izquierdo (pipes tienen mayor precedencia)
2. `while (parser->current && parser->current->type == TOKEN_AND)` - Busca operadores `&&`
3. `consume_token_type(parser, TOKEN_AND);` - Consume y valida `&&`
4. `if (parser->error)` - Manejo de errores de sintaxis
5. `right = parse_pipe_expression(parser);` - Parsea el lado derecho
6. `new_node = create_binary_node(TOKEN_AND, left, right);` - Crea nodo AND
7. `left = new_node;` - **CLAVE**: Asociatividad izquierda

**Ejemplo de precedencia mixta:**
```bash
Entrada: "mkdir test && cd test || echo 'failed to setup'"
Tokens: [WORD:"mkdir"] [WORD:"test"] [AND:"&&"] [WORD:"cd"] [WORD:"test"] [OR:"||"] [WORD:"echo"] [WORD:"'failed to setup'"]

Parsing (precedencia AND > OR):
1. parse_or_expression() llama a parse_and_expression()
2. parse_and_expression() procesa "mkdir test && cd test" primero
3. parse_or_expression() procesa "(mkdir test && cd test) || echo 'failed to setup'"

AST resultante:
              OR
             /  \
           AND   "echo 'failed to setup'"
          /    \
  "mkdir test" "cd test"

Lógica de ejecución:
1. Ejecuta "mkdir test"
2. Si success (exit = 0), ejecuta "cd test"
3. Si cualquiera falla, ejecuta "echo 'failed to setup'"
```

### 17. Nivel PIPE - parse_pipe_expression() en parser_expressions.c
```c
t_ast_node *parse_pipe_expression(t_parser *parser)
{
    t_ast_node *left;
    t_ast_node *right;
    t_ast_node *new_node;
    t_token *pipe_token;

    left = parse_primary_expression(parser);
    if (!left)
        return (NULL);
    while (parser->current && parser->current->type == TOKEN_PIPE)
    {
        pipe_token = consume_token_type(parser, TOKEN_PIPE);
        if (!pipe_token)
            return (cleanup_ast(left), NULL);
        right = parse_primary_expression(parser);
        if (!right)
            return (cleanup_ast(left), NULL);
        new_node = create_binary_node(TOKEN_PIPE, left, right);
        if (!new_node)
            return (cleanup_ast(left), cleanup_ast(right), NULL);
        left = new_node;
    }
    return (left);
}
```

**Análisis línea por línea:**
1. `left = parse_primary_expression(parser);` - Parsea el lado izquierdo (redirecciones/comandos)
2. `while (parser->current && parser->current->type == TOKEN_PIPE)` - Busca operadores `|`
3. `pipe_token = consume_token_type(parser, TOKEN_PIPE);` - Consume y valida `|`
4. `if (!pipe_token)` - Error de sintaxis en pipe
5. `right = parse_primary_expression(parser);` - Parsea el lado derecho
6. `new_node = create_binary_node(TOKEN_PIPE, left, right);` - Crea nodo PIPE
7. `left = new_node;` - **CLAVE**: Asociatividad izquierda para pipes múltiples

**Ejemplo de pipes múltiples:**
```bash
Entrada: "cat file.txt | grep pattern | sort | head -5"
Tokens: [WORD:"cat"] [WORD:"file.txt"] [PIPE:"|"] [WORD:"grep"] [WORD:"pattern"] [PIPE:"|"] [WORD:"sort"] [PIPE:"|"] [WORD:"head"] [WORD:"-5"]

Parsing paso a paso:
1. left = parse_primary_expression() → "cat file.txt"
2. TOKEN_PIPE → consume
3. right = parse_primary_expression() → "grep pattern"
4. new_node = PIPE(left:"cat file.txt", right:"grep pattern")
5. left = new_node
6. TOKEN_PIPE → consume
7. right = parse_primary_expression() → "sort"
8. new_node = PIPE(left:(cat|grep), right:"sort")
9. left = new_node
10. TOKEN_PIPE → consume
11. right = parse_primary_expression() → "head -5"
12. new_node = PIPE(left:((cat|grep)|sort), right:"head -5")

AST resultante:
                      PIPE
                     /    \
                  PIPE    "head -5"
                 /    \
            PIPE    "sort"
           /    \
"cat file.txt" "grep pattern"

Flujo de datos:
cat file.txt → grep pattern → sort → head -5
```

### 18. Dispatcher - parse_primary_expression() en parser_expressions.c
```c
t_ast_node *parse_primary_expression(t_parser *parser)
{
    if (parser->current && parser->current->type == TOKEN_LPAREN)
        return (parse_parenthesis_expression(parser));
    return (parse_redirect_expression(parser));
}
```

**Análisis:**
1. `if (parser->current && parser->current->type == TOKEN_LPAREN)` - Detecta paréntesis de apertura
2. `return (parse_parenthesis_expression(parser));` - Maneja agrupación con paréntesis
3. `return (parse_redirect_expression(parser));` - Delega a redirecciones por defecto

**Función clave**: Esta es la función que decide entre expresiones agrupadas y expresiones normales.

### 19. Manejo de Paréntesis - parse_parenthesis_expression() en parser_expressions.c
```c
t_ast_node *parse_parenthesis_expression(t_parser *parser)
{
    t_ast_node *expr;

    if (!consume_token_type(parser, TOKEN_LPAREN))
        return (NULL);
    expr = parse_or_expression(parser);
    if (!expr)
        return (NULL);
    if (!consume_token_type(parser, TOKEN_RPAREN))
        return (cleanup_ast(expr), NULL);
    return (expr);
}
```

**Análisis línea por línea:**
1. `if (!consume_token_type(parser, TOKEN_LPAREN))` - Consume `(` y valida sintaxis
2. `expr = parse_or_expression(parser);` - **RECURSIÓN INDIRECTA**: Vuelve al nivel superior
3. `if (!expr)` - Error en la expresión interna
4. `if (!consume_token_type(parser, TOKEN_RPAREN))` - Consume `)` y valida paréntesis balanceados
5. `return (expr);` - Retorna la expresión agrupada

**Ejemplo de paréntesis con precedencia:**
```
Entrada: "(echo hello && echo world) || echo failed"
Tokens: [LPAREN:"("] [WORD:"echo"] [WORD:"hello"] [AND:"&&"] [WORD:"echo"] [WORD:"world"] [RPAREN:")"] [OR:"||"] [WORD:"echo"] [WORD:"failed"]

Parsing:
1. parse_or_expression() llama a parse_and_expression()
2. parse_and_expression() llama a parse_pipe_expression()
3. parse_pipe_expression() llama a parse_primary_expression()
4. parse_primary_expression() detecta LPAREN
5. parse_parenthesis_expression():
   - consume_token_type(LPAREN)
   - parse_or_expression() → parsea "echo hello && echo world"
   - consume_token_type(RPAREN)
6. Continúa con OR: (grupo) || echo failed

AST resultante:
               OR
              /  \
            AND   "echo failed"
           /  \
"echo hello" "echo world"

Sin paréntesis sería: echo hello && (echo world || echo failed)
Con paréntesis es: (echo hello && echo world) || echo failed
```

### 20. Nivel Redirecciones - parse_redirect_expression() en parser_expressions.c
```c
t_ast_node *parse_redirect_expression(t_parser *parser)
{
    t_ast_node *leading_redirects;
    t_ast_node *command_node;
    t_ast_node *final_node;
    t_ast_node *current;

    parse_leading_redirects(parser, &leading_redirects);
    command_node = parse_command(parser);
    if (!command_node && !leading_redirects)
        return (NULL);
    if (!command_node && leading_redirects)
        return (leading_redirects);
    final_node = apply_trailing_redirects(parser, command_node);
    if (!final_node)
        return (cleanup_ast(leading_redirects), NULL);
    if (leading_redirects)
    {
        current = leading_redirects;
        while (current->left && is_redirect_node(current->left->type))
            current = current->left;
        current->left = final_node;
        return (leading_redirects);
    }
    return (final_node);
}
```

**Análisis línea por línea:**
1. `parse_leading_redirects(parser, &leading_redirects);` - Parsea redirecciones **antes** del comando
2. `command_node = parse_command(parser);` - Parsea el comando central
3. `if (!command_node && !leading_redirects)` - Línea vacía o error
4. `if (!command_node && leading_redirects)` - Solo redirecciones (válido: `< file`)
5. `final_node = apply_trailing_redirects(parser, command_node);` - Redirecciones **después** del comando
6. `if (leading_redirects)` - Si hay redirecciones iniciales, conectarlas
7. `current = leading_redirects;` - Buscar el final de la cadena de redirecciones
8. `while (current->left && is_redirect_node(current->left->type))` - Recorrer cadena
9. `current->left = final_node;` - Conectar comando al final de la cadena
10. `return (leading_redirects);` - Retornar el inicio de la cadena

**Ejemplo de redirecciones complejas:**
```bash
Entrada: "< input.txt > output.txt cat | sort >> sorted.txt"
Tokens: [REDIRECT_IN:"<"] [WORD:"input.txt"] [REDIRECT_OUT:">"] [WORD:"output.txt"] [WORD:"cat"] [PIPE:"|"] [WORD:"sort"] [REDIRECT_APPEND:">>"] [WORD:"sorted.txt"]

Parsing hasta el pipe:
1. parse_leading_redirects():
   - REDIRECT_IN + "input.txt" → NODE_REDIRECT_IN
   - REDIRECT_OUT + "output.txt" → NODE_REDIRECT_OUT
   - Cadena: NODE_REDIRECT_IN → NODE_REDIRECT_OUT

2. parse_command(): "cat"

3. apply_trailing_redirects(): no hay (termina en pipe)

4. Conectar: 
   NODE_REDIRECT_IN.left → NODE_REDIRECT_OUT
   NODE_REDIRECT_OUT.left → NODE_COMMAND("cat")

AST parcial (lado izquierdo del pipe):
    NODE_REDIRECT_IN (< input.txt)
         /
    NODE_REDIRECT_OUT (> output.txt)
         /
    NODE_COMMAND("cat")

Flujo de ejecución:
1. Abre input.txt para lectura → STDIN
2. Abre output.txt para escritura → STDOUT  
3. Ejecuta cat (lee de STDIN, escribe a STDOUT)
```

### 21. Redirecciones Iniciales - parse_leading_redirects() en parser_expressions.c
```c
static t_ast_node *parse_leading_redirects(t_parser *parser, t_ast_node **redirects_head)
{
    t_ast_node *redirect_node;
    t_ast_node *current_redirect;
    t_token *redirect_token;
    t_token *file_token;

    *redirects_head = NULL;
    current_redirect = NULL;
    while (parser->current && is_redirect_token(parser->current->type))
    {
        redirect_token = parser->current;
        parser->current = parser->current->next;
        file_token = consume_token(parser, TOKEN_WORD);
        if (!file_token)
            return (cleanup_ast(*redirects_head), NULL);
        redirect_node = create_redirect_node(redirect_token->type, NULL, file_token->value);
        if (!redirect_node)
            return (cleanup_ast(*redirects_head), NULL);
        if (!*redirects_head)
            *redirects_head = redirect_node;
        else
            current_redirect->left = redirect_node;
        current_redirect = redirect_node;
    }
    return (*redirects_head);
}
```

**Análisis línea por línea:**
1. `*redirects_head = NULL;` - Inicializa la cabeza de la lista
2. `current_redirect = NULL;` - Puntero al último nodo añadido
3. `while (parser->current && is_redirect_token(parser->current->type))` - Procesa redirecciones consecutivas
4. `redirect_token = parser->current;` - Guarda el token de redirección
5. `parser->current = parser->current->next;` - Avanza manualmente (no usa consume_token_type)
6. `file_token = consume_token(parser, TOKEN_WORD);` - Obtiene el nombre de archivo
7. `redirect_node = create_redirect_node(redirect_token->type, NULL, file_token->value);` - Crea nodo de redirección
8. `if (!*redirects_head)` - Primer nodo de la lista
9. `else current_redirect->left = redirect_node;` - Conecta a la cadena usando `left`
10. `current_redirect = redirect_node;` - Actualiza el puntero al último

**Ejemplo de múltiples redirecciones iniciales:**
```bash
Entrada: "< in1.txt < in2.txt > out.txt cat"

Parsing:
1. redirect_token = "<", file_token = "in1.txt"
   redirect_node = NODE_REDIRECT_IN(file="in1.txt")
   redirects_head = redirect_node

2. redirect_token = "<", file_token = "in2.txt"  
   redirect_node = NODE_REDIRECT_IN(file="in2.txt")
   current_redirect->left = redirect_node (in1 → in2)

3. redirect_token = ">", file_token = "out.txt"
   redirect_node = NODE_REDIRECT_OUT(file="out.txt")
   current_redirect->left = redirect_node (in2 → out)

Cadena resultante:
NODE_REDIRECT_IN("in1.txt")
    ↓ left
NODE_REDIRECT_IN("in2.txt")  
    ↓ left
NODE_REDIRECT_OUT("out.txt")
    ↓ left
    NULL (será conectado al comando)
```

### 22. Redirecciones Finales - apply_trailing_redirects() en parser_expressions.c
```c
static t_ast_node *apply_trailing_redirects(t_parser *parser, t_ast_node *base_node)
{
    t_ast_node *new_node;
    t_token *redirect_token;
    t_token *file_token;

    while (parser->current && is_redirect_token(parser->current->type))
    {
        redirect_token = parser->current;
        parser->current = parser->current->next;
        file_token = consume_token(parser, TOKEN_WORD);
        if (!file_token)
            return (cleanup_ast(base_node), NULL);
        new_node = create_redirect_node(redirect_token->type, base_node, file_token->value);
        if (!new_node)
            return (cleanup_ast(base_node), NULL);
        base_node = new_node;
    }
    return (base_node);
}
```

**Análisis línea por línea:**
1. `while (parser->current && is_redirect_token(parser->current->type))` - Procesa redirecciones tras el comando
2. `redirect_token = parser->current;` - Guarda operador de redirección
3. `file_token = consume_token(parser, TOKEN_WORD);` - Obtiene archivo destino
4. `new_node = create_redirect_node(redirect_token->type, base_node, file_token->value);` - **CLAVE**: `base_node` se convierte en `left`
5. `base_node = new_node;` - **CLAVE**: Cada redirección "envuelve" a la anterior

**Ejemplo de redirecciones finales:**
```bash
Entrada: "cat file.txt > output.txt 2> error.txt"
(Simplificado sin stderr redirection)

Parsing:
1. base_node = NODE_COMMAND("cat file.txt")
2. redirect_token = ">", file_token = "output.txt"
   new_node = NODE_REDIRECT_OUT(left=NODE_COMMAND, file="output.txt")
   base_node = new_node

AST resultante:
NODE_REDIRECT_OUT (> output.txt)
    ↓ left
NODE_COMMAND("cat file.txt")

Cada redirección "envuelve" al comando o redirección anterior.
```

### 23. Nivel Comando - parse_command() en parser_commands.c
```c
t_ast_node *parse_command(t_parser *parser)
{
    t_ast_node *node;

    if (!parser->current || parser->current->type != TOKEN_WORD)
        return (NULL);
    node = create_ast_node(NODE_COMMAND);
    if (!node)
        return (NULL);
    node->args = collect_command_args(parser);
    if (!node->args)
        return (free(node), NULL);
    return (node);
}
```

**Análisis línea por línea:**
1. `if (!parser->current || parser->current->type != TOKEN_WORD)` - Verifica que hay un comando (TOKEN_WORD)
2. `node = create_ast_node(NODE_COMMAND);` - Crea nodo base para comando
3. `node->args = collect_command_args(parser);` - **CLAVE**: Recolecta todos los argumentos
4. `if (!node->args)` - Error en recolección, libera nodo
5. `return (node);` - Retorna comando completo

### 24. Recolección de Argumentos - collect_command_args() en parser_commands.c
```c
static char **collect_command_args(t_parser *parser)
{
    char **args;
    int count;
    int got;
    t_token *temp;

    count = count_command_args(parser);
    if (count == 0)
        return (NULL);
    args = malloc(sizeof(*args) * (count + 1));
    if (!args)
        return (NULL);
    temp = parser->current;
    got = extract_args(args, count, &temp);
    if (got < 0)
        return (ft_freearr(args), NULL);
    args[got] = NULL;
    while (parser->current && parser->current->type == TOKEN_WORD)
        parser->current = parser->current->next;
    return (args);
}
```

**Análisis línea por línea:**
1. `count = count_command_args(parser);` - **Pre-cuenta** argumentos saltando redirecciones
2. `if (count == 0)` - No hay argumentos (comando vacío)
3. `args = malloc(sizeof(*args) * (count + 1));` - Aloca array con NULL terminator
4. `temp = parser->current;` - Copia temporal del puntero
5. `got = extract_args(args, count, &temp);` - **CLAVE**: Llena array saltando redirecciones
6. `args[got] = NULL;` - Termina array con NULL (para execve)
7. `while (parser->current && parser->current->type == TOKEN_WORD)` - Avanza parser saltando palabras procesadas
8. `return (args);` - Retorna array listo para execve

**Función auxiliar extract_args() (crítica):**
```c
int extract_args(char **args, int max, t_token **token)
{
    int i = 0;
    t_token *temp_token = *token;
    
    while (i < max && temp_token && temp_token->type != TOKEN_PIPE && temp_token->type != TOKEN_EOF)
    {
        if (temp_token->type == TOKEN_WORD)
        {
            args[i] = ft_strdup(temp_token->value);
            i++;
            temp_token = temp_token->next;
        }
        else if (is_redirect_token(temp_token->type))
        {
            // Saltar operador de redirección
            temp_token = temp_token->next;
            // Saltar archivo de redirección
            if (temp_token && temp_token->type == TOKEN_WORD)
                temp_token = temp_token->next;
        }
        else
            break;
    }
    *token = temp_token;
    return (i);
}
```

**Ejemplo de extract_args():**
```bash
Entrada: "cat file1.txt > output.txt file2.txt | grep pattern"
Tokens: [WORD:"cat"] [WORD:"file1.txt"] [REDIRECT_OUT:">"] [WORD:"output.txt"] [WORD:"file2.txt"] [PIPE:"|"] [WORD:"grep"] [WORD:"pattern"]

extract_args() para el comando izquierdo:
1. temp_token = "cat" → TOKEN_WORD → args[0] = "cat", i=1
2. temp_token = "file1.txt" → TOKEN_WORD → args[1] = "file1.txt", i=2  
3. temp_token = ">" → is_redirect_token() → skip
4. temp_token = "output.txt" → skip (archivo de redirección)
5. temp_token = "file2.txt" → TOKEN_WORD → args[2] = "file2.txt", i=3
6. temp_token = "|" → TOKEN_PIPE → break

args resultante: ["cat", "file1.txt", "file2.txt", NULL]
La redirección "> output.txt" se maneja por separado.
```

### 25. Flujo Visual Completo del Parser

```
Entrada: "cat << EOF | sort > out.txt"
Tokens: [WORD:"cat"] [HEREDOC:"<<"] [WORD:"EOF"] [PIPE:"|"] [WORD:"sort"] [REDIRECT_OUT:">"] [WORD:"out.txt"] [EOF]

parse()
    ↓
parse_or_expression() → no OR tokens
    ↓
parse_and_expression() → no AND tokens  
    ↓
parse_pipe_expression()
    ↓
    left = parse_primary_expression()
        ↓
        parse_redirect_expression()
            ↓
            parse_leading_redirects() → encuentra << EOF
                ↓
                redirect_node = NODE_HEREDOC(file="EOF")
            ↓
            command_node = parse_command() → "cat"
            ↓
            apply_trailing_redirects() → no hay
            ↓
            conectar: NODE_HEREDOC.left = NODE_COMMAND("cat")
    ↓
    TOKEN_PIPE encontrado → consume_token_type()
    ↓
    right = parse_primary_expression()
        ↓
        parse_redirect_expression()
            ↓
            parse_leading_redirects() → no hay
            ↓
            command_node = parse_command() → "sort"
            ↓
            apply_trailing_redirects() → encuentra > out.txt
                ↓
                new_node = NODE_REDIRECT_OUT(left=NODE_COMMAND("sort"), file="out.txt")
    ↓
    create_binary_node(TOKEN_PIPE, left, right)
```

### 26. AST Final Construido

```
                NODE_PIPE
               /         \
      NODE_HEREDOC       NODE_REDIRECT_OUT
      (file="EOF")       (file="out.txt")
          /                     /
    NODE_COMMAND          NODE_COMMAND
    (args=["cat"])        (args=["sort"])
```

### 27. Estado cuando sale del parser

**Entrada original:** `"cat << EOF | sort > out.txt"`

**Tokens consumidos:** Todos, hasta TOKEN_EOF

**AST construido:**
```c
t_ast_node *ast = {
    type: NODE_PIPE,
    left: {
        type: NODE_HEREDOC,
        file: "EOF",
        left: {
            type: NODE_COMMAND,
            args: ["cat", NULL]
        }
    },
    right: {
        type: NODE_REDIRECT_OUT,
        file: "out.txt", 
        left: {
            type: NODE_COMMAND,
            args: ["sort", NULL]
        }
    }
};
```

**Precedencia respetada:**
1. **Redirecciones** se asocian fuertemente a sus comandos
2. **Pipes** conectan grupos comando+redirecciones  
3. **AND/OR** operan sobre pipes completos
4. **Paréntesis** pueden alterar precedencia

**El AST está listo** para ser ejecutado por `execute_ast()`, que:
1. Procesará el heredoc creando un pipe temporal
2. Ejecutará `cat` leyendo del heredoc
3. Conectará `cat | sort` via pipe del sistema
4. Redirigirá la salida de `sort` a `out.txt`
