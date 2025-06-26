Una shell es un programa que actúa como interfaz entre el usuario y el sistema operativo. Interpreta comandos del usuario y los ejecuta.

Usuario escribe comando → Shell interpreta → Sistema ejecuta → Shell muestra resultado

Ejemplo:
```bash
$ ls -l
# Shell:
# 1. Lee "ls -l"
# 2. Busca el programa "ls"
# 3. Lo ejecuta con argumento "-l"
# 4. Muestra la salida al usuario
```

## Variables de Entorno
¿Qué son?  
Las variables de entorno son valores que están disponibles para todos los procesos del sistema. Almacenan información como rutas, configuraciones, etc.

Variables Importantes

**PATH**
```bash
echo $PATH
# /usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin
```
Propósito: Lista de directorios donde buscar comandos  
Uso en minishell: Cuando escribes ls, la shell busca el ejecutable en estos directorios

**HOME**
```bash
echo $HOME
# /Users/username
```
Propósito: Directorio home del usuario  
Uso: Para comandos como cd sin argumentos

**PWD** (Directorio actual de trabajo)
```bash
echo $PWD
# /current/working/directory
```

Implementación en la Minishell
```c
// En find_the_path.c
static char *paths_in_env(char **envp)
{
    int i = 0;
    if (!envp)
        return (NULL);
    while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
        i++;
    if (!envp[i])
        return (NULL);
    return (envp[i] + 5);  // Retorna después de "PATH="
}
```

Expansión de Variables
```bash
echo $HOME          # Se expande a /Users/username
echo "$USER says hi" # Se expande a "john says hi"
echo '$USER says hi' # NO se expande: $USER says hi
```

## Built-ins vs Comandos Externos

Comandos Externos (Normales)
```bash
ls -l    # Ejecuta /bin/ls en un proceso separado
grep     # Ejecuta /usr/bin/grep en un proceso separado
```

Built-ins (Internos)
```bash
cd /home      # NO crea proceso, la shell misma cambia directorio
export VAR=val # NO crea proceso, modifica variables de la shell
exit          # NO crea proceso, la shell misma termina
```
Razón Fundamental: Estado de la Shell

Problema con cd como comando externo:
```bash
# Si cd fuera un comando externo:
cd /tmp  # Proceso hijo cambia a /tmp, luego termina
pwd      # Sigues en el directorio original
```
Esto es porque cada proceso tiene su propio working directory. Cuando el proceso cd termina, sus cambios se pierden.

Solución: Built-in
```bash
cd /tmp  # La shell misma cambia su directorio
pwd      # Ahora estás en /tmp
```

Built-ins Obligatorios en Minishell:
◦ echo with option -n (Print text)  
◦ cd with only a relative or absolute path (Change directory)  
◦ pwd with no options (Print working directory)  
◦ export with no options (Set enviroment variables)  
◦ unset with no options (Remove enviroment variables)  
◦ env with no options or arguments (Display enviroment)  
◦ exit with no options (Exit Minishell)


## File Descriptors y Redirección

Un file descriptor es un número que identifica un archivo abierto en un proceso.

File Descriptors Estándar
```c
#define STDIN_FILENO  0  // Entrada estándar (teclado)
#define STDOUT_FILENO 1  // Salida estándar (pantalla)
#define STDERR_FILENO 2  // Error estándar (pantalla)
```

### Redirección de Salida (>)
Sin Redirección
```bash
echo "hello"  # Salida va a la pantalla (stdout)
```

Con Redirección
```bash
echo "hello" > file.txt  # Salida va al archivo
```

Implementación
```c
// En redirect_executor.c
int fd = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);  // stdout ahora apunta al archivo
close(fd);
// Ahora cualquier printf() va al archivo
```

### Redirección de Entrada (<)
```c
sort < input.txt  # sort lee desde input.txt en lugar del teclado
```
```c
int fd = open("input.txt", O_RDONLY);
dup2(fd, STDIN_FILENO);   // stdin ahora lee del archivo
close(fd);
```

### Redirección Append (>>)
```bash
echo "new line" >> file.txt  # Añade al final del archivo
```
```c
int fd = open("file.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
dup2(fd, STDOUT_FILENO);
close(fd);
```

Visualización
```
Normal:
programa → stdout (1) → pantalla

Redirección >:
programa → stdout (1) → archivo

Redirección <:
archivo → stdin (0) → programa
```

## Pipes
Un pipe conecta la salida de un comando con la entrada del siguiente.
```bash
ls | grep .c | wc -l
```

Funcionamiento Interno  
**1. Crear el Pipe**
```c
int pipe_fd[2];  // [0] = read end, [1] = write end
pipe(pipe_fd);
```
**2. Crear Procesos**
```c
pid_t left_pid = fork();   // Para "ls"
pid_t right_pid = fork();  // Para "grep .c"
```
**3. Configurar File Descriptors**  
Proceso Izquierdo (ls):
```c
close(pipe_fd[0]);              // No necesita leer
dup2(pipe_fd[1], STDOUT_FILENO); // Su salida va al pipe
close(pipe_fd[1]);
execve("/bin/ls", args, envp);
```
Proceso Derecho (grep):
```c
close(pipe_fd[1]);              // No necesita escribir
dup2(pipe_fd[0], STDIN_FILENO); // Su entrada viene del pipe
close(pipe_fd[0]);
execve("/usr/bin/grep", args, envp);
```
Proceso Padre:
```c
close(pipe_fd[0]);  // Cerrar ambos extremos
close(pipe_fd[1]);
waitpid(left_pid, &status1, 0);   // Esperar a ambos
waitpid(right_pid, &status2, 0);
```
Diagrama
```
ls → [write_fd] PIPE [read_fd] → grep
```

## Procesamiento de Comandos

Fases del Procesamiento  
**1. Tokenización** - Dividir en Tokens
```bash
echo "hello world" | grep test > output.txt
```
Se convierte en:
```
TOKEN_WORD:        "echo"
TOKEN_WORD:        "hello world"
TOKEN_PIPE:        "|"
TOKEN_WORD:        "grep"
TOKEN_WORD:        "test"
TOKEN_REDIRECT_OUT: ">"
TOKEN_WORD:        "output.txt"
TOKEN_EOF:         NULL
```

**2. Parsing** - Construir Estructura
Los tokens se organizan según precedencia y gramática.

**3. Ejecución** - Ejecutar Comandos
El AST se ejecuta recursivamente.

## Abstract Syntax Tree (AST)
Un Abstract Syntax Tree es una representación en árbol de la estructura sintáctica del comando.

**Ejemplo Completo**
Input:
```bash
echo hello | grep h > output.txt
```
AST Resultante:
```
                  NODE_REDIRECT_OUT
                 /                \
         NODE_PIPE                file: "output.txt"
        /         \
  NODE_COMMAND     NODE_COMMAND
       |                 |
["echo", "hello"]  ["grep", "h"]
```

1. Precedencia Natural  
Los operadores tienen precedencia:  
Redirects (>, <) tienen mayor precedencia que pipes (|)  
Se ejecutan "más cerca" del comando

2. Ejecución Recursiva
```c
int execute_ast(t_ast_node *ast, char **envp)
{
    if (ast->type == NODE_COMMAND)
        return execute_command_node(ast, envp);
    if (ast->type == NODE_PIPE)
        return execute_pipe_node(ast, envp);
    if (ast->type == NODE_REDIRECT_OUT)
        return execute_redirect_node(ast, envp);
    // ...
}
```

3. Composición Infinita  
Se pueden anidar estructuras arbitrariamente:
```bash
(cmd1 | cmd2) > file && (cmd3 < input | cmd4)
```

**Tipos de nodos**
```c
typedef enum e_node_type
{
    NODE_COMMAND,        // Comando simple: echo hello
    NODE_PIPE,           // Pipe: cmd1 | cmd2
    NODE_REDIRECT_OUT,   // Redirect: cmd > file
    NODE_REDIRECT_IN,    // Redirect: cmd < file
    NODE_REDIRECT_APPEND, // Append: cmd >> file
    NODE_HEREDOC,        // Here doc: cmd << EOF
    NODE_AND,            // Lógico AND: cmd1 && cmd2 (Bonus, todavía no implementado)
    NODE_OR              // Lógico OR: cmd1 || cmd2 (Bonus, todavía no implementado)
} t_node_type;
```


## Gestión de Procesos

**fork()** - Crear Proceso
```c
pid_t pid = fork();
if (pid == 0)
{
    // Código del proceso hijo
    execve("/bin/ls", args, envp);
}
else
{
    // Código del proceso padre
    waitpid(pid, &status, 0);
}
```
**execve()** - Reemplazar Proceso
```c
char *args[] = {"ls", "-l", NULL};
char *path = "/bin/ls";
execve(path, args, envp);  // Reemplaza el proceso actual
```
**waitpid()** - Esperar a Proceso Hijo
```c
int status;
waitpid(pid, &status, 0);
if (WIFEXITED(status))
    printf("Exit code: %d\n", WEXITSTATUS(status));
```

**Patrón en Minishell**  
Para Comando Simple:
```c
pid_t pid = fork();
if (pid == 0)
{
    // Hijo: ejecutar comando
    execve(path, args, envp);
    exit(127);  // Si execve falla
}
// Padre: esperar resultado
waitpid(pid, &status, 0);
return WEXITSTATUS(status);
```
Para Pipe:
```c
int pipe_fd[2];
pipe(pipe_fd);

pid_t left = fork();
if (left == 0)
{
    // Configurar salida al pipe
    dup2(pipe_fd[1], STDOUT_FILENO);
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    execve(cmd1_path, cmd1_args, envp);
}

pid_t right = fork();
if (right == 0)
{
    // Configurar entrada desde pipe
    dup2(pipe_fd[0], STDIN_FILENO);
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    execve(cmd2_path, cmd2_args, envp);
}

// Padre cierra pipe y espera
close(pipe_fd[0]);
close(pipe_fd[1]);
waitpid(left, &status1, 0);
waitpid(right, &status2, 0);
```

## Manejo de Comillas

**Comillas Simples (')**
```bash
echo 'Hello $USER'  # Output: Hello $USER (literal)
```
Comportamiento: Todo es literal, no hay expansión  
Uso: Cuando quieres texto exacto

**Comillas Dobles (")**
```bash
echo "Hello $USER"  # Output: Hello john (expansión)
```
Comportamiento: Permite expansión de variables  
Uso: Cuando quieres variables pero preservar espacios

**Procesamiento en el Tokenizer**
```c
// En token_words.c
char *process_quoted_string(const char *s, int start, int end)
{
    char *result = malloc(end - start + 1);
    int j = 0, k = start;
    
    while (k < end)
    {
        if (s[k] == '\'' || s[k] == '"')
        {
            char quote = s[k++];  // Guardar tipo de comilla
            // Copiar contenido hasta comilla de cierre
            while (k < end && s[k] != quote)
                result[j++] = s[k++];
            if (s[k] == quote)
                k++;  // Saltar comilla de cierre
        }
        else
            result[j++] = s[k++];
    }
    result[j] = '\0';
    return result;
}
```


## Here Documents

Los here documents permiten pasar texto multi-línea como entrada.

**Sintaxis**
```bash
cat << EOF
Este es un texto
de múltiples líneas
que termina cuando encuentra
EOF
```
**Funcionamiento**  
Detectar delimiter: EOF en el ejemplo  
Leer líneas: Hasta encontrar una línea que sea exactamente el delimiter  
Crear pipe temporal: Para pasar el texto al comando  
No actualizar historial: Las líneas del here-doc no van al historial  
**Ejemplo de implementación de heredoc**
```c
int create_heredoc(char *delimiter)
{
    int pipe_fd[2];
    pipe(pipe_fd);
    
    pid_t pid = fork();
    if (pid == 0)
    {
        // Hijo: escribir contenido al pipe
        close(pipe_fd[0]);
        char *line;
        while ((line = readline("> ")))
        {
            if (strcmp(line, delimiter) == 0)
            {
                free(line);
                break;
            }
            write(pipe_fd[1], line, strlen(line));
            write(pipe_fd[1], "\n", 1);
            free(line);
        }
        close(pipe_fd[1]);
        exit(0);
    }
    
    // Padre: retornar read end del pipe
    close(pipe_fd[1]);
    waitpid(pid, NULL, 0);
    return pipe_fd[0];  // Para usar como stdin del comando
}
```
```bash

```
```bash

```
```bash

```




```bash

```




















```bash

```








```bash

```
