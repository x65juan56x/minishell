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



```c

```


```c

```


```c

```


```c

```
