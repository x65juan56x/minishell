# Documentación del Executor de MiniShell

## Tabla de Contenidos
- [¿Qué es el Executor?](#qué-es-el-executor)  
- [Arquitectura del Executor](#arquitectura-del-executor)  
- [Estructura de Archivos](#estructura-de-archivos)  
- [Recorrido del AST y Flujo de Ejecución](#recorrido-del-ast-y-flujo-de-ejecución)  
- [Análisis Detallado por Archivo](#análisis-detallado-por-archivo)  
  - [executor.c](#executorc)  
  - [cmd_executor.c](#cmd_executorc)  
  - [path_utils.c](#path_utilsc)  
  - [pipe_executor.c](#pipe_executorc)  
  - [redirect_executor.c](#redirect_executorc)  
  - [heredoc_preprocessor.c](#heredoc_preprocessorc)  
  - [heredoc_executor.c](#heredoc_executorc)  
- [Gestión de Procesos y Pipes](#gestión-de-procesos-y-pipes)  
- [Manejo de File Descriptors](#manejo-de-file-descriptors)  
- [Códigos de Salida y Manejo de Errores](#códigos-de-salida-y-manejo-de-errores)  
- [Ejemplos de Ejecución](#ejemplos-de-ejecución)   

---

## ¿Qué es el Executor?
El Executor toma el AST generado por el Parser y lo convierte en procesos reales, estableciendo pipes, redirecciones y llamando a `execve`.  
Flujo global:  
```
Input → Tokenizer → Tokens → Parser → AST → Executor → Procesos Unix
```

---

## Arquitectura del Executor
Se emplea **recorrido recursivo** del AST. Cada nodo delega en la función apropiada según su `type`:  
- `NODE_COMMAND` → comando simple  
- `NODE_PIPE`    → creación de tubería entre dos hijos  
- `NODE_REDIRECT_*` / `NODE_HEREDOC` → redirecciones  

Este patrón garantiza separación de responsabilidades y manejo uniforme de estados.

---

## Estructura de Archivos
```
src/executor/
├── executor.c             # Dispatcher principal
├── cmd_executor.c         # Búsqueda de binario y execve
├── path_utils.c           # Resolución de rutas en $PATH
├── pipe_executor.c        # Gestión de pipes y procesos hijos
├── redirect_executor.c    # Aplicación de redirects (<, >, >>)
├── heredoc_preprocessor.c # Preprocesa here-docs (<<) antes del pipe
└── heredoc_executor.c     # Lectura de here-documents y creación de pipe
```

---

## Recorrido del AST y Flujo de Ejecución
La función central es:
```c
int execute_ast(t_ast_node *ast, char **envp);
```
1. Si `ast == NULL` → retorna 0.  
2. Según `ast->type`:
   - `NODE_COMMAND`  → `execute_command_node()`  
   - `NODE_PIPE`     → `execute_pipe_node()`  
   - `NODE_REDIRECT_*` → `execute_redirect_node()`  
3. Cada llamada propaga el status de salida hasta el padre.

---

## Análisis Detallado por Archivo

### executor.c
```c
int execute_ast(t_ast_node *ast, char **envp);
```
- **Propósito**: Dispatcher que examina `ast->type` y delega la ejecución.  
- **Funciones clave**:
  - `execute_command_node`  
  - `execute_pipe_node`  
  - `execute_redirect_node`  
- **Flujo**:
  1. `fork()` para comandos simples → `launch_command`.  
  2. `pipe()` y dos forks para pipes.  
  3. `fork()` y duplicado de fds para redirects.

### cmd_executor.c
```c
void launch_command(char **args, char **envp);
```
- **Propósito**: Buscar ruta con `find_command_path()`, llamar `execve`.  
- **Errores**:
  - Comando no encontrado → exit 127.  
  - `execve` falla → exit 126.  
- **Helpers**:  
  - `exit_command_not_found()`  
  - `exit_execve_error()`

### path_utils.c
```c
char *find_command_path(char *cmd, char **envp);
```
- **Propósito**:  
  1. Si `cmd` contiene `'/'`, verificar acceso directo.  
  2. Extraer `PATH` del entorno o usar valor por defecto.  
  3. Dividir en directorios (`ft_split`) y buscar ejecutable (`access(X_OK)`).  
- **Retorna**: ruta al comando o `NULL`.

### pipe_executor.c
```c
pid_t create_pipe_child(t_ast_node *node, t_pipe_config *cfg);
int   wait_pipe_children(pid_t left, pid_t right);
```
- **Propósito**:  
  - Crea un hijo para cada extremo del pipe.  
  - Configura `dup2` según `is_left`.  
  - Cierra fds no necesarios.  
- **Estrategia**:  
  - Padre cierra ambos extremos y espera con `wait_pipe_children()`,  
    que retorna el status del **último** comando.

### redirect_executor.c
```c
int execute_redirect_node(t_ast_node *node, char **envp);
```
- **Propósito**:  
  1. `collect_redirect_nodes()` en una lista LIFO.  
  2. `fork()`, en hijo: aplicar cada redirect con `dup2`.  
  3. Ejecutar el comando real con `execute_ast()`.  
- **Tipos**: `<`, `>`, `>>` y `<<` (heredoc via preprocesador).

### heredoc_preprocessor.c
```c
int preprocess_heredocs(t_ast_node *node);
```
- **Propósito**: Recorrer el subárbol izquierdo de un pipe y, si hay `NODE_HEREDOC`,  
  llamar a `execute_heredoc()` para crear un pipe con su contenido.  
- **Retorna**: descriptor de lectura para el stdin del primer comando.

### heredoc_executor.c
```c
int execute_heredoc(char *delimiter);
```
- **Propósito**:  
  1. `pipe()`.  
  2. Leer líneas con `readline("> ")` hasta `delimiter`.  
  3. Escribir cada línea en el extremo de escritura.  
  4. Cerrar el write-end y retornar el read-end.  

---

## Gestión de Procesos y Pipes
- **Comando simple**: 1 `fork()`.  
- **Pipe**: 2 `fork()` y 1 `pipe()`.  
- **Redirect**: 1 `fork()`, abrir archivos, `dup2()`.  
- El árbol de procesos hereda ficheros duplicados y cierra los sobrantes.

---

## Manejo de File Descriptors
- **dup2(oldfd, STDIN/STDOUT)** para redirigir I/O.  
- Cerrar siempre los fds originales tras `dup2`.  
- En pipes, cerrar el extremo opuesto para evitar deadlocks.

---

## Códigos de Salida y Manejo de Errores
- 0   → Éxito  
- 1   → Error general / malloc / fork  
- 126 → `execve` con permisos insuficientes  
- 127 → Comando no encontrado  
- 128+N → Terminación por señal N (p.ej. Ctrl-C)  

Los fragmentos `WIFEXITED` y `WIFSIGNALED` en `waitpid` permiten propagar estos códigos.

---

## Ejemplos de Ejecución

**1. Simple**  
```
$ echo hello
```
- AST: `NODE_COMMAND args:["echo","hello"]`  
- `launch_command` → `execve("/bin/echo",…)`  

**2. Pipe**  
```
$ ls | grep .c
```
- AST: `NODE_PIPE( ls , grep )`  
- `pipe()`, 2 forks, stdout de `ls` → stdin de `grep`.  

**3. Redirect**  
```
$ echo hi > out.txt
```
- AST: `NODE_REDIRECT_OUT(file="out.txt") → NODE_COMMAND`.  
- Fork hijo: `open("out.txt",O_TRUNC)`, `dup2(fd,1)`, exec.

**4. Heredoc + Pipe + Redirect**  
```
$ cat << EOF | grep foo >> result.txt
```
- `heredoc_preprocessor` genera un pipe con el contenido.  
- `create_pipe_and_execute` con `heredoc_fd`.  
- `redirect_executor` apila `>> result.txt` tras el pipe.
