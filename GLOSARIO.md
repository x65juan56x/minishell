
## Macros externas usadas en minishell

### 1. Señales y manejo de procesos (`signal.h`, `sys/wait.h`)

| Macro                | Librería         | Descripción                                                                 |
|----------------------|------------------|-----------------------------------------------------------------------------|
| SIGINT               | `<signal.h>`     | Señal de interrupción (Ctrl+C)                                              |
| SIGQUIT              | `<signal.h>`     | Señal de salida forzada (Ctrl+\)                                           |
| SIGTSTP              | `<signal.h>`     | Señal para parar proceso (Ctrl+Z)                                           |
| WIFEXITED(status)    | `<sys/wait.h>`   | Verdadero si el hijo terminó normalmente                                    |
| WEXITSTATUS(status)  | `<sys/wait.h>`   | Código de salida del hijo si terminó normalmente                            |
| WIFSIGNALED(status)  | `<sys/wait.h>`   | Verdadero si el hijo terminó por una señal                                  |
| WTERMSIG(status)     | `<sys/wait.h>`   | Número de la señal que terminó el proceso hijo                              |

### 2. Entrada/Salida y archivos (`unistd.h`, `fcntl.h`)

| Macro                | Librería         | Descripción                                                                 |
|----------------------|------------------|-----------------------------------------------------------------------------|
| STDIN_FILENO         | `<unistd.h>`     | Descriptor estándar de entrada (0)                                          |
| STDOUT_FILENO        | `<unistd.h>`     | Descriptor estándar de salida (1)                                           |
| STDERR_FILENO        | `<unistd.h>`     | Descriptor estándar de error (2)                                            |
| O_RDONLY             | `<fcntl.h>`      | Abrir archivo solo lectura                                                  |
| O_WRONLY             | `<fcntl.h>`      | Abrir archivo solo escritura                                                |
| O_CREAT              | `<fcntl.h>`      | Crear archivo si no existe                                                  |
| O_TRUNC              | `<fcntl.h>`      | Truncar archivo a cero                                                      |
| O_APPEND             | `<fcntl.h>`      | Añadir al final del archivo                                                 |

### 3. Otros

| Macro                | Librería         | Descripción                                                                 |
|----------------------|------------------|-----------------------------------------------------------------------------|
| NULL                 | `<stddef.h>`/`<stdlib.h>` | Puntero nulo                                                      |
| EXIT_SUCCESS         | `<stdlib.h>`     | Código estándar de éxito (0)                                                |
| EXIT_FAILURE         | `<stdlib.h>`     | Código estándar de error (1)                                                |
