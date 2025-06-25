Tabla de Contenidos

¿Qué es el Executor?
Arquitectura del Executor
Flujo de Ejecución
Análisis Detallado por Archivos
Gestión de Procesos
Manejo de File Descriptors
Exit Status y Códigos de Error
Ejemplos de Ejecución
Ventajas de esta Arquitectura


¿Qué es el Executor?
El executor es el tercer y último paso en el procesamiento de comandos de una shell. Toma el AST (Abstract Syntax Tree) generado por el parser y lo ejecuta creando procesos, conectando pipes, y manejando redirects.

Flujo Completo:
Input String → Tokenizer → Tokens → Parser → AST → Executor → Procesos Ejecutándose

Responsabilidades del Executor:
-Crear procesos usando fork() y execve()
-Conectar pipes entre comandos
-Manejar redirects manipulando file descriptors
-Coordinar ejecución respetando la estructura del AST
-Recoger exit status de procesos hijos


Arquitectura del Executor
-Filosofía de Diseño: Recursive AST Traversal
-El executor recorre el AST de manera recursiva, ejecutando cada nodo según su tipo. La estructura del árbol determina el orden y forma de ejecución.

Separación de Responsabilidades:

src/executor/
├── executor.c           # Coordinación principal y dispatcher
├── cmd_executor.c       # Ejecución de comandos individuales
├── pipe_executor.c      # Gestión de pipes entre procesos
├── redirect_executor.c  # Manejo de redirects de entrada/salida
└── find_the_path.c      # Búsqueda de binarios en PATH

Tipos de Nodos que Maneja:
•NODE_COMMAND: Comandos simples (echo hello)
•NODE_PIPE: Pipes (cmd1 | cmd2)
•NODE_REDIRECT_*: Redirects (cmd > file, cmd < file)


Flujo de Ejecución
Función Principal:
int execute_ast(t_ast_node *ast, char **envp)
{
    if (!ast)
        return (0);
    if (ast->type == NODE_COMMAND)
        return (execute_command_node(ast, envp));
    if (ast->type == NODE_PIPE)
        return (execute_pipe_node(ast, envp));
    if (ast->type == NODE_REDIRECT_*)
        return (execute_redirect_node(ast, envp));
    return (1);
}

Patrón de Decisión (Dispatcher):
-Verificación: Valida que el AST no sea NULL
-Tipo de nodo: Determina qué tipo de ejecución realizar
-Delegación: Llama a la función especializada correspondiente
-Retorno: Propaga el exit status hacia arriba


Análisis Detallado por Archivos

1. executor.c - Coordinador Principal

**Función: execute_ast(t_ast_node *ast, char **envp)
•Responsabilidad: Punto de entrada principal del executor.
•Estrategia:
-Dispatcher pattern: Determina el tipo de nodo y delega
-Sin estado global: Todo se maneja por valores de retorno
-Recursión implícita: Las funciones delegadas pueden llamar de vuelta a execute_ast()

**Función: execute_command_node(t_ast_node *node, char **envp)
•Responsabilidad: Ejecutar un comando simple en un proceso hijo.
•Algoritmo detallado:
static int execute_command_node(t_ast_node *node, char **envp)
{
    pid_t   pid;
    int     status;

    if (!node || !node->args || !node->args[0])  // Validación
        return (1);
    pid = fork();                                // Crear proceso hijo
    if (pid == -1)
        return (1);                              // Error en fork
    if (pid == 0)                                // Proceso hijo
    {
        run_cmd_from_args(node->args, envp);     // Ejecutar comando
        exit(127);                               // Fallback si execve falla
    }
    waitpid(pid, &status, 0);                    // Esperar terminación
    // Procesar exit status...
}

¿Por qué fork para comandos simples?
-Aislamiento: El comando no puede afectar el proceso principal
-Control: Podemos capturar el exit status
-Consistencia: Mismo patrón que pipes y redirects

**Función: execute_pipe_node(t_ast_node *node, char **envp)
•Responsabilidad: Coordinar la ejecución de un pipe.
•Proceso:
-Crear pipe: pipe(pipe_fd) crea el canal de comunicación
-Crear hijos: Un proceso para cada lado del pipe
-Cerrar descriptors: El proceso padre cierra ambos extremos
-Esperar: Recoge exit status de ambos procesos

2. cmd_executor.c - Ejecución de Comandos

**Función: run_cmd_from_args(char **args, char **envp)
•Responsabilidad: Ejecutar un comando dado como array de argumentos.
•Flujo completo:
void run_cmd_from_args(char **args, char **envp)
{
    char *path;

    if (!args || !args[0] || args[0][0] == '\0')  // Validación
        ft_cmd_not_found_exit("");
    path = get_path(args[0], envp);               // Buscar binario
    if (!path)
        handle_cmd_error(args, path);             // No encontrado
    execve(path, args, envp);                     // Ejecutar
    handle_execve_error(args, path);              // Error en execve
}

•Casos de error manejados:
-Comando vacío: ft_cmd_not_found_exit("")
-Comando no encontrado: ft_cmd_not_found_exit(cmd_name) con exit 127
-Sin permisos de ejecución: ft_permission_denied_exit(cmd_name) con exit 126

Funciones de Error:

**Función: handle_cmd_error(char **args, char *path)
-Uso: Cuando get_path() retorna NULL
-Limpieza: Libera memory antes de terminar
-Exit code: 127 (command not found)

**Función: handle_execve_error(char **args, char *path)
-Uso: Cuando execve() falla (permisos, archivo corrupto)
-Limpieza: Libera memory antes de terminar
-Exit code: 126 (permission denied)

3. pipe_executor.c - Gestión de Pipes

**Función: create_pipe_child(t_ast_node *node, int *pipe_fd, int is_left, char **envp)
•Responsabilidad: Crear un proceso hijo para un lado del pipe.
•Parámetros explicados:
-node: Nodo AST a ejecutar en el hijo
-pipe_fd: Array con descriptors del pipe [read_end, write_end]
-is_left: 1 si es lado izquierdo (escribe), 0 si es derecho (lee)
-envp: Variables de entorno
•Algoritmo:
pid_t create_pipe_child(t_ast_node *node, int *pipe_fd, int is_left, char **envp)
{
    pid_t pid;

    pid = fork();
    if (pid == -1)                               // Error handling
    {
        if (is_left)
            close(pipe_fd[1]);                   // Cerrar write end
        else  
            close(pipe_fd[0]);                   // Cerrar read end
        return (-1);
    }
    if (pid == 0)                                // Proceso hijo
    {
        setup_pipe_child(pipe_fd, is_left);      // Configurar redirects
        execute_node_in_child(node, envp);       // Ejecutar nodo
    }
    return (pid);                                // Retornar PID al padre
}

**Función: setup_pipe_child(int *pipe_fd, int is_left_child)
•Responsabilidad: Configurar file descriptors en el proceso hijo.
•Para lado izquierdo (escritor):
if (is_left_child)
{
    close(pipe_fd[0]);                    // No necesita leer
    dup2(pipe_fd[1], STDOUT_FILENO);      // stdout → pipe write end
    close(pipe_fd[1]);                    // Cerrar descriptor original
}

•Para lado derecho (lector):
else
{
    close(pipe_fd[1]);                    // No necesita escribir
    dup2(pipe_fd[0], STDIN_FILENO);       // stdin ← pipe read end  
    close(pipe_fd[0]);                    // Cerrar descriptor original
}

¿Por qué cerrar descriptors?

•Evitar deadlocks: Si un proceso mantiene abierto el write end, el reader nunca recibe EOF
•Limpieza: Liberar resources del sistema
•Corrección: Solo mantener los descriptors necesarios

**Función: wait_pipe_children(pid_t left_pid, pid_t right_pid)
•Responsabilidad: Esperar a que terminen ambos procesos del pipe.
•Estrategia de exit status:
-Prioridad al lado derecho: En bash, el exit status de un pipe es el del último comando
-Fallback al izquierdo: Si el derecho falla, usar el izquierdo
-Manejo de señales: Detectar terminación por señal vs exit normal

4. redirect_executor.c - Manejo de Redirects

**Función: execute_redirect_node(t_ast_node *node, char **envp)
•Responsabilidad: Ejecutar un comando con redirect aplicado.
•Proceso completo:
int execute_redirect_node(t_ast_node *node, char **envp)
{
    pid_t   pid;
    int     status;

    if (!node || !node->left || !node->file)     // Validación
        return (1);
    pid = fork();                                // Crear proceso hijo
    if (pid == -1)
        return (1);
    if (pid == 0)                                // Proceso hijo
    {
        if (setup_redirect(node) != 0)           // Configurar redirect
            exit(1);
        execute_ast(node->left, envp);            // Ejecutar comando
        exit(1);                                 // Fallback
    }
    waitpid(pid, &status, 0);                    // Esperar terminación
    // Procesar exit status...
}

**Función: setup_redirect(t_ast_node *node)
•Responsabilidad: Configurar file descriptors para el redirect.
•Proceso:
-Abrir archivo: Con flags apropiados según tipo de redirect
-Determinar target: stdin para <, stdout para > y >>
-Duplicar descriptor: dup2(fd, target_fd)
-Cerrar original: Liberar el descriptor de archivo

**Función: open_redirect_file(char *file, t_node_type type)
•Responsabilidad: Abrir archivo con flags correctos según tipo de redirect.
•Tipos de redirect:
if (type == NODE_REDIRECT_IN)
    fd = open(file, O_RDONLY);                    // Solo lectura
else if (type == NODE_REDIRECT_OUT)  
    fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Truncar
else if (type == NODE_REDIRECT_APPEND)
    fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644); // Append

•Flags explicados:
O_RDONLY: Solo lectura
O_WRONLY: Solo escritura
O_CREAT: Crear archivo si no existe
O_TRUNC: Truncar archivo existente a tamaño 0
O_APPEND: Escribir al final del archivo
0644: Permisos (owner: rw, group: r, others: r)

5. find_the_path.c - Búsqueda de Binarios

**Función: get_path(char *cmd, char **envp)
•Responsabilidad: Encontrar la ruta completa de un comando.
•Casos manejados:
-Ruta absoluta/relativa: Si contiene /, verificar directamente
-Comando en PATH: Buscar en directorios de la variable PATH
-PATH no definido: Usar PATH por defecto del sistema
•Algoritmo:
char *get_path(char *cmd, char **envp)
{
    char *path;
    char **dirs;
    char *tmp;

    if (!cmd)
        return (NULL);
    if (ft_strchr(cmd, '/') != NULL)              // Ruta con /
    {
        if (access(cmd, X_OK) == 0)               // Verificar permisos
            return (ft_strdup(cmd));
        return (NULL);
    }
    tmp = paths_in_env(envp);                     // Obtener PATH
    if (!tmp)                                     // PATH no definido
        tmp = "/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin";
    dirs = ft_split(tmp, ':');                    // Dividir directorios
    if (!dirs)
        return (NULL);
    path = check_dirs(dirs, cmd);                 // Buscar en directorios
    return (path);
}

**Función: check_dirs(char **dirs, char *cmd)
•Responsabilidad: Buscar comando en lista de directorios.
•Proceso:
-Iterar directorios: Probar cada directorio en orden
-Construir ruta: dir + "/" + cmd
-Verificar acceso: access(full_path, X_OK)
-Retornar primera coincidencia: O NULL si no se encuentra


Gestión de Procesos
Estrategia de Fork:
Comando simple: 1 fork (padre espera al hijo)
Pipe: 2 forks (padre espera a ambos hijos)
Redirect: 1 fork (hijo configura redirect y ejecuta)
Jerarquía de Procesos:

Minishell (padre)
├── Comando simple (hijo)
├── Pipe
│   ├── Lado izquierdo (hijo)
│   └── Lado derecho (hijo)
└── Redirect
    └── Comando con redirect (hijo)

Wait Strategy:
-waitpid(pid, &status, 0): Esperar a proceso específico
-Sin WNOHANG: Espera bloqueante (apropiado para shell interactiva)
-Recoger exit status: Para propagar hacia arriba


Manejo de File Descriptors

Principios Fundamentales:

1. Duplicación con dup2():
dup2(pipe_fd[1], STDOUT_FILENO);  // stdout apunta al pipe
dup2(file_fd, STDIN_FILENO);      // stdin apunta al archivo

2. Cierre de descriptors:
-Inmediato: Cerrar descriptors no necesarios
-Evitar leaks: Todo descriptor abierto debe cerrarse
-Prevenir deadlocks: Cerrar write ends de pipes

3. Orden de operaciones:
-Fork: Crear proceso hijo
-Setup: Configurar redirects en hijo
-Close: Cerrar descriptors originales
-Exec: Ejecutar comando (hereda descriptors configurados)

Diagrama de Pipe:
Proceso Padre:
pipe_fd[0] ← read end
pipe_fd[1] ← write end

Hijo Izquierdo:           Hijo Derecho:
close(pipe_fd[0])         close(pipe_fd[1])
dup2(pipe_fd[1], 1)       dup2(pipe_fd[0], 0)  
close(pipe_fd[1])         close(pipe_fd[0])
stdout → pipe             stdin ← pipe


Exit Status y Códigos de Error

Exit Codes Estándar:
•0: Éxito
•1: Error general
•126: Permiso denegado (execve falló por permisos)
•127: Comando no encontrado
•128+N: Terminado por señal N

Manejo en el Executor:
if (WIFEXITED(status))
    return (WEXITSTATUS(status));        // Exit normal
if (WIFSIGNALED(status))  
    return (128 + WTERMSIG(status));     // Terminado por señal

Propagación de Exit Status:
-Comandos simples: Exit status del proceso hijo
-Pipes: Exit status del último comando (lado derecho)
-Redirects: Exit status del comando ejecutado
-Errores del shell: 1 para errores internos


Ejemplos de Ejecución

Ejemplo 1: Comando Simple
•Input: echo hello world
•AST:
NODE_COMMAND
args: ["echo", "hello", "world", NULL]

•Ejecución:
-execute_ast() → execute_command_node()
-fork() crea proceso hijo
-Hijo llama run_cmd_from_args(["echo", "hello", "world"], envp)
-get_path("echo") → echo
-execve("/bin/echo", ["echo", "hello", "world"], envp)
-Padre espera con waitpid()
-Retorna exit status 0

Ejemplo 2: Pipe Simple
•Input: ls | grep test
•AST:
    NODE_PIPE
   /         \
NODE_COMMAND  NODE_COMMAND
["ls"]        ["grep", "test"]

•Ejecución:
-execute_ast() → execute_pipe_node()
-pipe() crea canal de comunicación
-create_pipe_child(ls_node, pipe_fd, 1):
	Fork para lado izquierdo
	Hijo: dup2(pipe_fd[1], STDOUT_FILENO)
	Hijo: ejecuta ls
-create_pipe_child(grep_node, pipe_fd, 0):
	Fork para lado derecho
	Hijo: dup2(pipe_fd[0], STDIN_FILENO)
	Hijo: ejecuta grep test
-Padre cierra ambos extremos del pipe
-wait_pipe_children() espera a ambos procesos
-Retorna exit status del grep (lado derecho)

Ejemplo 3: Redirect
•Input: echo hello > output.txt
•AST:
NODE_REDIRECT_OUT
    /            \
NODE_COMMAND   file: "output.txt"
["echo", "hello"]

•Ejecución:
-execute_ast() → execute_redirect_node()
-fork() crea proceso hijo
-Hijo: open("output.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644)
-Hijo: dup2(file_fd, STDOUT_FILENO)
-Hijo: close(file_fd)
-Hijo: execute_ast(echo_node) (recursión)
-Ejecuta echo hello con stdout → output.txt
-Padre espera y retorna exit status

Ejemplo 4: Combinación Compleja
•Input: cat file.txt | grep test > result.txt
•AST:
        NODE_REDIRECT_OUT
       /                \
    NODE_PIPE        file: "result.txt"
   /         \
NODE_COMMAND  NODE_COMMAND
["cat",       ["grep",
 "file.txt"]   "test"]

•Ejecución:
-execute_ast() → execute_redirect_node()
-Fork para el redirect
-Hijo configura stdout → result.txt
-Hijo llama execute_ast(pipe_node) (recursión)
-Se ejecuta el pipe cat file.txt | grep test
-La salida del pipe va a result.txt
-Retorna exit status final

