# Plan de Pruebas para Minishell

## 1. Comandos simples y la variable global

### Descripción
Ejecuta comandos básicos y analiza el uso de variables globales.

### Pruebas y Salidas Esperadas

- **Comando simple con ruta absoluta:**
  ```bash
  ✅ MiniShell $ /bin/ls 
  ```
  - **Salida esperada:** El listado de archivos y directorios en la carpeta actual, seguido de un nuevo prompt.

- **Otros comandos con ruta absoluta:**
  ```bash
  ✅ MiniShell $ /bin/pwd
  ```
  - **Salida esperada:** La ruta del directorio actual.

  ```bash
  ✅ MiniShell $ /usr/bin/whoami
  ```
  - **Salida esperada:** Tu nombre de usuario.

- **Comando vacío:**
  ```bash
  ✅ MiniShell $ 
  ```
  - **Salida esperada:** Simplemente un nuevo prompt, sin errores.

- **Solo espacios o tabuladores:**
  ```bash
  ✅ MiniShell $      
  ```
  - **Salida esperada:** Igual que un comando vacío, debe mostrar un nuevo prompt.

### Análisis de Variables Globales
- **¿Cuántas variables globales utiliza?**
  - Tu proyecto utiliza una sola variable global: `volatile sig_atomic_t g_signal_status`.
- **¿Por qué es necesaria?**
  - Es necesaria para manejar las señales de forma segura. Cuando el usuario presiona `Ctrl+C`, se ejecuta una función especial llamada "manejador de señales" (`interactive_sigint_handler` en `signals.c`).
  - Las funciones que se pueden llamar de forma segura dentro de un manejador son muy limitadas. No se puede llamar a `readline`, `printf`, ni `malloc`, por ejemplo.
  - La variable global `g_signal_status` actúa como un puente. El manejador de señales (que se ejecuta de forma asíncrona) simplemente cambia el valor de esta variable a `SIGINT`. Luego, el bucle principal del shell, en un punto seguro de su ejecución, puede comprobar el valor de `g_signal_status` y actuar en consecuencia (por ejemplo, reiniciando el prompt y actualizando el código de salida a 130).
  - Es `volatile` para que el compilador no optimice su acceso, y `sig_atomic_t` para garantizar que las lecturas y escrituras en ella sean atómicas (indivisibles), evitando condiciones de carrera.

---

## 2. Argumentos

### Descripción
Ejecuta comandos con múltiples argumentos, sin usar comillas.

### Pruebas y Salidas Esperadas
- **Comando con argumentos:**
  ```bash
  ✅ MiniShell $ /bin/ls -l
  ```
  - **Salida esperada:** El listado detallado de archivos en el directorio actual.

- **Comando con múltiples argumentos:**
  ```bash
  ✅ MiniShell $ /bin/ls -la /tmp
  ```
  - **Salida esperada:** El listado detallado de todos los archivos (incluidos los ocultos) en `/tmp`.

- **Comando echo con argumentos:**
  ```bash
  ✅ MiniShell $ /bin/echo hello world
  ```
  - **Salida esperada:**
    ```
    hello world
    ```

- **Comando de conteo de líneas:**
  ```bash
  ❓✅ MiniShell $ /usr/bin/wc -l /etc/passwd
  ```
  - **Salida esperada:** El número de líneas del archivo `/etc/passwd`.

- **Comando cat con archivo:**
  ```bash
  ✅ MiniShell $ /bin/cat /etc/hosts
  ```
  - **Salida esperada:** El contenido del archivo `/etc/hosts`.

- **Comando grep con patrón:**
  ```bash
  ✅ MiniShell $ /bin/grep root /etc/passwd
  ```
  - **Salida esperada:** Las líneas que contienen "root" en `/etc/passwd`.

---

## 3. `echo`

### Descripción
Prueba el builtin `echo` con y sin la opción `-n`.

### Pruebas y Salidas Esperadas
- **`echo` sin argumentos:**
  ```bash
  ✅ MiniShell $ echo
  ```
  - **Salida esperada:** Un salto de línea.
    ```

    ```

- **`echo` con un argumento:**
  ```bash
  ✅ MiniShell $ echo hello
  ```
  - **Salida esperada:** `hello` seguido de un salto de línea.

- **`echo` con múltiples argumentos:**
  ```bash
  ✅ MiniShell $ echo hello world
  ```
  - **Salida esperada:** `hello world` seguido de un salto de línea.

- **`echo` con la opción `-n`:**
  ```bash
  ✅ MiniShell $ echo -n hello
  ```
  - **Salida esperada:** `hello` sin salto de línea. El siguiente prompt aparecerá en la misma línea.

- **`echo` con `-n` y múltiples argumentos:**
  ```bash
  ✅ MiniShell $ echo -n hello world
  ```
  - **Salida esperada:** `hello world` sin salto de línea.

- **`echo` solo con `-n`:**
  ```bash
  ✅ MiniShell $ echo -n
  ```
  - **Salida esperada:** Ninguna salida, el prompt aparece en la misma línea.

- **`echo` con `-n` en medio:**
  ```bash
  ✅ MiniShell $ echo hello -n world
  ```
  - **Salida esperada:** `hello -n world` seguido de un salto de línea (el `-n` no se interpreta si no está al principio).

- **`echo` con múltiples `-n`:**
  ```bash
  ✅ MiniShell $ echo -n -n hello
  ```
  - **Salida esperada:** `-n hello` sin salto de línea (solo el primer `-n` se interpreta).
  - ⚠️ ** BASH: comparar**

---

## 4. `exit`

### Descripción
Prueba el builtin `exit` con diferentes argumentos para verificar los códigos de salida y el manejo de errores.

### Pruebas y Salidas Esperadas
- **`exit` sin argumentos:**
  ```bash
  ✅MiniShell $ exit
  ```
  - **Salida esperada:** El shell imprime `exit` y termina. El código de salida es el del último comando ejecutado (o 0 si no hubo ninguno).

- **`exit` con código 0:**
  ```bash
  ✅MiniShell $ exit 0
  ```
  - **Salida esperada:** El shell imprime `exit` y termina con código 0.

- **`exit` con código 1:**
  ```bash
  ✅MiniShell $ exit 1
  ```
  - **Salida esperada:** El shell imprime `exit` y termina con código 1.

- **`exit` con código 42:**
  ```bash
  ✅MiniShell $ exit 42
  ```
  - **Salida esperada:** El shell imprime `exit` y termina con código 42.

- **`exit` con código 255:**
  ```bash
  ✅MiniShell $ exit 255
  ```
  - **Salida esperada:** El shell imprime `exit` y termina con código 255.

- **`exit` con código mayor a 255:**
  ```bash
  ✅MiniShell $ exit 256
  ```
  - **Salida esperada:** El shell imprime `exit` y termina con código 0 (256 % 256 = 0).

- **`exit` con código negativo:**
  ```bash
  ✅MiniShell $ exit -1
  ```
  - **Salida esperada:** El shell imprime `exit` y termina con código 255 ((-1) % 256 = 255).

- **`exit` con argumento no numérico:**
  ```bash
  ✅MiniShell $ exit abc
  ```
  - **Salida esperada:** El shell imprime un error y termina con código 255.
    ```
    exit
    minishell: exit: abc: numeric argument required
    ```

- **`exit` con demasiados argumentos:**
  ```bash
  ✅MiniShell $ exit 42 extra_arg
  ```
  - **Salida esperada:** El shell imprime un error y **no termina**.
    ```
    exit
    minishell: exit: too many arguments
    MiniShell $ 
    ```

---

## 5. Valor de retorno de un proceso (`$?`)

### Descripción
Verifica que la variable especial `$?` se expande correctamente al código de salida del último comando ejecutado.

### Pruebas y Salidas Esperadas
- **Comando exitoso:**
  ```bash
  ✅MiniShell $ /bin/ls
  ✅MiniShell $ echo $?
  ```
  - **Salida esperada:** El listado de `ls`, y en la siguiente línea un `0`.

- **Comando exitoso con directorio:**
  ```bash
  ✅MiniShell $ /bin/ls /tmp
  ✅MiniShell $ echo $?
  ```
  - **Salida esperada:** El listado de `/tmp`, y en la siguiente línea un `0`.

- **Comando fallido:**
  ```bash
  ✅MiniShell $ /bin/ls archivo_que_no_existe
  ✅MiniShell $ echo $?
  ```
  - **Salida esperada:** Un mensaje de error de `ls` (ej: `No such file or directory`), y en la siguiente línea un código de error distinto de cero (bash suele dar `2`).

- **Comando que retorna falso:**
  ```bash
  ✅MiniShell $ /bin/false
  ✅MiniShell $ echo $?
  ```
  - **Salida esperada:** En la línea siguiente un `1`.

- **Comando que retorna verdadero:**
  ```bash
  ✅MiniShell $ /bin/true
  ✅MiniShell $ echo $?
  ```
  - **Salida esperada:** En la línea siguiente un `0`.

- **Expresión con `$?`:**
  ```bash
  ✅MiniShell $ /bin/false
  ✅MiniShell $ expr $? + $?
  ```
  - **Salida esperada:** `2` (1 + 1, ya que `false` devuelve 1 y `$?` se evalúa dos veces con el mismo valor).

---

## 6. Señales (`Ctrl+C`, `Ctrl+D`, `Ctrl+\`)

### Descripción
Verifica el comportamiento correcto de las señales en diferentes contextos.

### Pruebas y Salidas Esperadas
- **`Ctrl+C` en prompt vacío:** Muestra un nuevo prompt en una nueva línea. El código de salida debería ser 130.
  ```bash
  ✅MiniShell $ ^C
  ✅MiniShell $ echo $?
  130
  ```
- **`Ctrl+\` en prompt vacío:** No debe hacer nada.✅
- **`Ctrl+D` en prompt vacío:** El shell imprime `exit` y termina.✅
- **`Ctrl+C` con texto en el prompt:** Aborta la línea actual y muestra un nuevo prompt limpio.✅
  ```bash
  MiniShell $ hola que tal^C
  MiniShell $ 
  ```
- **`Ctrl+C` durante un comando bloqueante (ej: `cat`):** El comando `cat` termina, se imprime un salto de línea y se muestra un nuevo prompt. El código de salida `$?` debería ser 130.✅
- **`Ctrl+\` durante un comando bloqueante (ej: `cat`):** El comando `cat` termina y el shell muestra `Quit (core dumped)`. El código de salida `$?` debería ser 131.✅
- **`Ctrl+D` durante un comando bloqueante (ej: `cat`):** Envía un `EOF` al `stdin` del comando. `cat` terminará y el shell mostrará un nuevo prompt.✅

### Comandos de prueba para señales:
```bash
✅MiniShell $ cat
# Probar Ctrl+C, Ctrl+\, Ctrl+D

✅MiniShell $ grep "algo"
# Probar Ctrl+C, Ctrl+\, Ctrl+D

✅MiniShell $ sleep 10
# Probar Ctrl+C, Ctrl+\, Ctrl+D
```

**Comportamiento esperado:**
- Con `cat`: Espera entrada del usuario hasta que se presione una señal
- Con `grep "algo"`: Espera entrada del usuario para buscar el patrón
- Con `sleep 10`: Espera 10 segundos antes de terminar (a menos que se interrumpa)

---

## 7. Comillas dobles (`"`)

### Descripción
Las comillas dobles deben preservar los espacios y caracteres especiales como parte de un único argumento, pero deben permitir la expansión de variables (`$`).

### Pruebas y Salidas Esperadas
- **Preservación de espacios:**
  ```bash
  ✅MiniShell $ echo "hello world"
  ```
  - **Salida esperada:** `hello world` (espacios preservados).

- **Preservación de caracteres especiales:**
  ```bash
  ✅MiniShell $ echo "cat lol.c | cat > lol.c"
  ```
  - **Salida esperada:** La cadena literal `cat lol.c | cat > lol.c`, no una ejecución de pipes.

- **Múltiples espacios:**
  ```bash
  ✅MiniShell $ echo "  hello   world  "
  ```
  - **Salida esperada:** `  hello   world  ` (preservando todos los espacios).

- **Comillas dobles vacías:**
  ```bash
  ✅MiniShell $ echo ""
  ```
  - **Salida esperada:** Una línea en blanco (argumento vacío).

- **Comillas dobles escapadas:**
  ```bash
  ❌MiniShell $ echo "hello \"world\""
  ```
  - **Salida esperada:** `hello "world"` (las comillas internas se escapan).

- **Tabuladores en comillas:**
  ```bash
  ✅MiniShell $ echo "tab	here"
  ```
  - **Salida esperada:** `tab	here` (con tabulador preservado).

- **Salto de línea en comillas:**
  ```bash
  ✅MiniShell $ echo "newline
  here"
  ```
  - **Salida esperada:**
    ```
    newline
    here
    ```

---

## 8. Comillas simples (`'`)

### Descripción
Las comillas simples deben preservar literalmente todo su contenido. No se interpreta nada, ni variables, ni escapes.

### Pruebas y Salidas Esperadas
- **Preservación de espacios:**
  ```bash
  ✅MiniShell $ echo 'hello world'
  ```
  - **Salida esperada:** `hello world`.

- **Sin expansión de variables:**
  ```bash
  ✅MiniShell $ echo '$USER'
  ```
  - **Salida esperada:** La cadena literal `$USER`.

- **Caracteres especiales:**
  ```bash
  ✅MiniShell $ echo '| > < >> <<'
  ```
  - **Salida esperada:** La cadena literal `| > < >> <<`.

- **Argumento vacío:**
  ```bash
  ✅MiniShell $ echo ''
  ```
  - **Salida esperada:** Una línea en blanco (un argumento vacío que `echo` imprime como nada, seguido de un salto de línea).

- **Comillas dobles dentro de simples:**
  ```bash
  ✅MiniShell $ echo 'hello "world"'
  ```
  - **Salida esperada:** `hello "world"` (las comillas dobles se tratan como literales).

- **Variable PATH sin expansión:**
  ```bash
  ✅MiniShell $ echo '$PATH'
  ```
  - **Salida esperada:** La cadena literal `$PATH`.

- **Comando sin expansión:**
  ```bash
  ✅MiniShell $ echo '$(ls)'
  ```
  - **Salida esperada:** La cadena literal `$(ls)` (sin ejecutar el comando).

---

## 9. `export`

### Descripción
Prueba la creación, modificación y visualización de variables de entorno.

### Pruebas y Salidas Esperadas
- **`export` sin argumentos:**
  ```bash
  ✅MiniShell $ export
  ```
  - **Salida esperada:** Una lista de todas las variables de entorno, formateadas como `declare -x VAR="valor"`.

- **Crear una nueva variable:**
  ```bash
  ✅MiniShell $ export TEST_VAR=hello
  ✅MiniShell $ env | grep TEST_VAR
  ```
  - **Salida esperada:** `TEST_VAR=hello`.

- **Modificar PATH:**
  ```bash
  ✅MiniShell $ export PATH=/bin:/usr/bin
  ✅MiniShell $ echo $PATH
  ```
  - **Salida esperada:** `/bin:/usr/bin`.

- **Modificar USER:**
  ```bash
  ✅MiniShell $ export USER=new_user
  ✅MiniShell $ env | grep USER
  ```
  - **Salida esperada:** `USER=new_user`.

- **Variable vacía:**
  ```bash
  ✅MiniShell $ export EMPTY_VAR=""
  ✅MiniShell $ env | grep EMPTY_VAR
  ```
  - **Salida esperada:** `EMPTY_VAR=`.

- **Variable con nombre válido:**
  ```bash
  ✅MiniShell $ export VALID_123=value
  ✅MiniShell $ env | grep VALID_123
  ```
  - **Salida esperada:** `VALID_123=value`.

- **Variable con nombre inválido:**
  ```bash
  ✅MiniShell $ export 123INVALID=value
  ```
  - **Salida esperada:** Error: `minishell: export: '123INVALID': not a valid identifier`.

---

## 10. `unset`

### Descripción
Prueba la eliminación de variables de entorno.

### Pruebas y Salidas Esperadas
- **Crear y eliminar variable:**
  ```bash
  ✅MiniShell $ export TEST_VAR=hello
  ✅MiniShell $ env | grep TEST_VAR
  ✅MiniShell $ unset TEST_VAR
  ✅MiniShell $ env | grep TEST_VAR
  ```
  - **Salida esperada:** Primera línea: `TEST_VAR=hello`. Última línea: no debe producir salida.

- **Eliminar PATH:**
  ```bash
  ✅MiniShell $ unset PATH
  ✅MiniShell $ env | grep PATH
  ```
  - **Salida esperada:** No debe mostrar PATH en la salida.

- **Eliminar USER:**
  ```bash
  ✅MiniShell $ unset USER
  ✅MiniShell $ env | grep USER
  ```
  - **Salida esperada:** No debe mostrar USER en la salida.

- **Intentar eliminar una variable que no existe:**
  ```bash
  ✅MiniShell $ unset NONEXISTENT_VAR
  ```
  - **Salida esperada:** No debe ocurrir ningún error, simplemente no hace nada.

---

## 11. `cd`

### Descripción
Prueba el cambio de directorios.

### Pruebas y Salidas Esperadas
- **Ir a un directorio:**
  ```bash
  ✅MiniShell $ cd /tmp
  ✅MiniShell $ /bin/ls
  ```
  - **Salida esperada:** Cambio a `/tmp`, luego listado de archivos en `/tmp`.

- **Directorio padre (`..`):**
  ```bash
  ✅MiniShell $ cd ..
  ✅MiniShell $ /bin/ls
  ```
  - **Salida esperada:** Cambio al directorio padre, luego listado de archivos.

- **Directorio actual (`.`):**
  ```bash
  ✅MiniShell $ cd .
  ✅MiniShell $ /bin/ls
  ```
  - **Salida esperada:** Permanece en el mismo directorio, luego listado de archivos.

- **Ir al directorio `HOME`:**
  ```bash
  ✅MiniShell $ cd
  ✅MiniShell $ /bin/ls
  ```
  - **Salida esperada:** Cambio al directorio home, luego listado de archivos del home.

- **Ir al directorio anterior:**
  ```bash
  ✅MiniShell $ cd -
  ✅MiniShell $ /bin/ls
  ```
  - **Salida esperada:** Cambio al directorio anterior, imprime la ruta, luego listado de archivos.

- **Error de directorio no existente:**
  ```bash
  ✅MiniShell $ cd /nonexistent
  ```
  - **Salida esperada:** Un mensaje de error como `minishell: cd: /nonexistent: No such file or directory`.

- **Argumento vacío:**
  ```bash
  ✅MiniShell $ cd ""
  ```
  - **Salida esperada:** Debería comportarse como `cd` sin argumentos (ir al HOME) o mostrar error.

---

## 12. Rutas relativas

### Descripción
Ejecuta comandos usando rutas relativas en lugar de absolutas o nombres de comando.

### Pruebas y Salidas Esperadas
- **Ejecutable en directorio actual:**
  ```bash
  ✅MiniShell $ ./minishell
  ```
  - **Salida esperada:** Se ejecuta una nueva instancia de tu shell.

- **Ruta relativa hacia arriba:**
  ```bash
  ✅MiniShell $ ../bin/ls
  ```
  - **Salida esperada:** Error (probablemente `No such file or directory` a menos que exista).

- **Ruta relativa compleja:**
  ```bash
  ✅MiniShell $ ../../usr/bin/whoami
  ```
  - **Salida esperada:** Tu nombre de usuario (si la ruta es correcta).

- **Desde /tmp:**
  ```bash
  ✅MiniShell $ cd /tmp
  ✅MiniShell $ ../bin/ls
  ```
  - **Salida esperada:** Listado de archivos si `/bin/ls` existe.

- **Desde directorio raíz:**
  ```bash
  ✅MiniShell $ cd /
  ✅MiniShell $ usr/bin/whoami
  ```
  - **Salida esperada:** Tu nombre de usuario.

- **Archivo en directorio actual:**
  ```bash
  ✅MiniShell $ ./etc/passwd
  ```
  - **Salida esperada:** Error de permisos o que no es ejecutable.

---

## 13. La variable de entorno `PATH`

### Descripción
Verifica que los comandos se buscan en los directorios especificados por `PATH`.

### Pruebas y Salidas Esperadas
- **Ejecución normal:**
  ```bash
  ✅MiniShell $ ls
  ```
  - **Salida esperada:** El listado de archivos.

- **Verificar `PATH` actual:**
  ```bash
  ✅MiniShell $ echo $PATH
  ```
  - **Salida esperada:** La lista de directorios separados por `:`.

- **Eliminar `PATH`:**
  ```bash
  ✅MiniShell $ unset PATH
  MiniShell $ ls
  ```
  - **Salida esperada:** `minishell: ls: command not found`.

- **Restaurar `PATH` con /bin:**
  ```bash
  ✅MiniShell $ export PATH="/bin"
  MiniShell $ ls
  ```
  - **Salida esperada:** El listado de archivos de nuevo.

- **Restaurar `PATH` con múltiples directorios:**
  ```bash
  ✅MiniShell $ export PATH="/usr/bin:/bin"
  MiniShell $ ls
  ```
  - **Salida esperada:** El listado de archivos de nuevo.

- **PATH completo:**
  ```bash
  ✅MiniShell $ export PATH="/usr/bin:/bin:/usr/local/bin"
  MiniShell $ ls
  ```
  - **Salida esperada:** El listado de archivos de nuevo.

- **Comprobar orden de búsqueda:**
  ```bash
  ✅MiniShell $ which ls
  ```
  - **Salida esperada:** Debería mostrar la ruta del primer `ls` encontrado en PATH.

---

## 14. Redirecciones (`<`, `>`, `>>`, `<<`)

### Descripción
Prueba la redirección de entrada y salida.

### Pruebas y Salidas Esperadas
- **Redirección de salida `>`:**
  ```bash
  ✅MiniShell $ echo "hello" > file.txt
  ✅MiniShell $ cat file.txt
  ```
  - **Salida esperada:** `hello`. El archivo `file.txt` es creado o sobrescrito.

- **Redirección de entrada `<`:**
  ```bash
  ✅MiniShell $ cat < file.txt
  ```
  - **Salida esperada:** `hello` (contenido del archivo).

- **Redirección de salida `>>` (append):**
  ```bash
  ✅MiniShell $ echo "world" >> file.txt
  ✅MiniShell $ cat file.txt
  ```
  - **Salida esperada:**
    ```
    hello
    world
    ```

- **Redirección de salida con listado:**
  ```bash
  ✅MiniShell $ ls > output.txt
  ✅MiniShell $ cat < output.txt
  ```
  - **Salida esperada:** El listado de archivos del directorio actual.

- **Redirección de entrada con wc:**
  ```bash
  ✅MiniShell $ wc -l < /etc/passwd
  ```
  - **Salida esperada:** El número de líneas del archivo `/etc/passwd`.

- **Múltiples redirecciones:**
  ```bash
  ✅MiniShell $ echo "test" > file1.txt > file2.txt
  ```
  - **Salida esperada:** Solo `file2.txt` debería contener "test" (la última redirección "gana").

- **Redirección con archivo inexistente:**
  ```bash
  ✅MiniShell $ cat < /nonexistent_file
  ```
  - **Salida esperada:** Error: `No such file or directory`.

- **Here Document `<<`:**
  ```bash
  ✅MiniShell $ cat << EOF
  > hello
  > world
  > EOF
  ```
  - **Salida esperada:**
    ```
    hello
    world
    ```

---

## 15. Pipes (`|`)

### Descripción
Prueba la conexión de la salida de un comando con la entrada de otro.

### Pruebas y Salidas Esperadas
- **Pipe simple con grep:**
  ```bash
  ✅MiniShell $ ls | grep txt
  ```
  - **Salida esperada:** Solo las líneas que contienen "txt".

- **Pipe con passwd:**
  ```bash
  ✅MiniShell $ cat /etc/passwd | grep root
  ```
  - **Salida esperada:** Las líneas que contienen "root" en `/etc/passwd`.

- **Pipe con conteo de palabras:**
  ```bash
  ✅MiniShell $ echo "hello world" | wc -w
  ```
  - **Salida esperada:** `2` (número de palabras).

- **Pipe con conteo de líneas:**
  ```bash
  ✅MiniShell $ ls | wc -l
  ```
  - **Salida esperada:** El número de archivos en el directorio actual.

- **Pipe múltiple:**
  ```bash
  ✅MiniShell $ cat /etc/passwd | grep root | wc -l
  ```
  - **Salida esperada:** El número de líneas que contienen "root".

- **Pipe con comando que falla:**
  ```bash
  ✅MiniShell $ ls nonexistent | grep something | wc -l
  ```
  - **Salida esperada:** Un mensaje de error de `ls` a `stderr`, y `0` de `wc -l`.

- **Pipe múltiple con cat:**
  ```bash
  ✅MiniShell $ echo "test" | cat | cat | cat
  ```
  - **Salida esperada:** `test`.

- **Pipe con redirección:**
  ```bash
  ✅MiniShell $ ls | grep txt > output.txt
  ✅MiniShell $ cat output.txt
  ```
  - **Salida esperada:** Los archivos que contienen "txt" guardados en `output.txt`.

- **Pipe con redirección de entrada:**
  ```bash
  ✅MiniShell $ cat file.txt | grep pattern < input.txt
  ```
  - **Salida esperada:** Comportamiento complejo - normalmente `grep` ignoraría el pipe y leería de `input.txt`.

---

## 16. Vuélvete loco, y el historial

### Descripción
Pruebas de robustez y características de la terminal.

### Pruebas y Salidas Esperadas
- **Navegación del historial:** Presiona las flechas arriba y abajo.✅
  - **Salida esperada:** Debes poder navegar por los comandos previamente introducidos.

- **Comando inválido:**
  ```bash
  ✅MiniShell $ wjkgjrgwg4g43go34o
  ```
  - **Salida esperada:** `minishell: wjkgjrgwg4g43go34o: command not found`. El shell no debe cerrarse.

- **Pipe colgado:**
  ```bash
  ✅MiniShell $ cat | cat | ls
  ```
  - **Salida esperada:** El shell debe esperar una entrada de `stdin`. Si escribes algo y presionas `Ctrl+D`, `ls` debería ejecutarse.

- **Comando muy largo:**
  ```bash
  ✅MiniShell $ echo this is a very long command with many many many arguments and it should work fine
  ```
  - **Salida esperada:** El echo completo de todos los argumentos.

- **Comando inexistente:**
  ```bash
  ✅MiniShell $ invalidcommand
  ```
  - **Salida esperada:** `minishell: invalidcommand: command not found`. El shell no debe cerrarse.

- **Ruta inexistente:**
  ```bash
  ✅MiniShell $ /bin/nonexistent
  ```
  - **Salida esperada:** `minishell: /bin/nonexistent: No such file or directory`. El shell no debe cerrarse.

- **Buffer vacío después de `Ctrl+C`:**
  ```bash
  ✅MiniShell $ echo hola^C
  ✅MiniShell $ [presionar Enter]
  ```
  - **Salida esperada:** No debería ejecutarse nada, solo mostrar un nuevo prompt.

---

## 17. Variables de entorno

### Descripción
Verifica la correcta expansión de variables de entorno.

### Pruebas y Salidas Esperadas
- **Expansión simple:**
  ```bash
  ✅MiniShell $ echo $USER
  ```
  - **Salida esperada:** Tu nombre de usuario.

- **Expansión de PATH:**
  ```bash
  ✅MiniShell $ echo $PATH
  ```
  - **Salida esperada:** La lista de directorios separados por `:`.

- **Expansión de HOME:**
  ```bash
  ✅MiniShell $ echo $HOME
  ```
  - **Salida esperada:** La ruta de tu directorio home.

- **Expansión de PWD:**
  ```bash
  ✅MiniShell $ echo $PWD
  ```
  - **Salida esperada:** La ruta del directorio actual.

- **Variable inexistente:**
  ```bash
  ✅MiniShell $ echo $NONEXISTENT
  ```
  - **Salida esperada:** Una línea en blanco.

- **Expansión en comillas dobles:**
  ```bash
  ✅MiniShell $ echo "$USER"
  ```
  - **Salida esperada:** Tu nombre de usuario.

- **Sin expansión en comillas dobles:**
  ```bash
  ❌MiniShell $ echo $"USER"
  ```
  - **Salida esperada:** `USER`.

- **Sin expansión en comillas simples:**
  ```bash
  ✅MiniShell $ echo '$USER'
  ```
  - **Salida esperada:** `$USER`.

- **Concatenación de variables:**
  ```bash
  ✅MiniShell $ echo $USER$HOME
  ```
  - **Salida esperada:** Tu nombre de usuario seguido de tu directorio home (sin espacio).

- **Expansión con llaves:**
  ```bash
  ✅MiniShell $ echo ${USER}
  ```
  - **Salida esperada:** Tu nombre de usuario.

- **Variable personalizada:**
  ```bash
  ✅MiniShell $ export TEST=hello
  ✅MiniShell $ echo $TEST
  ```
  - **Salida esperada:** `hello`.

---

## 18. `&&` y `||` (Bonus)

### Descripción
Prueba los operadores lógicos AND y OR.

### Pruebas y Salidas Esperadas
- **AND (`&&`) - éxito:**
  ```bash
  ✅MiniShell $ echo hello && echo world
  ```
  - **Salida esperada:**
    ```
    hello
    world
    ```

- **OR (`||`) - éxito:**
  ```bash
  ✅MiniShell $ echo hello || echo world
  ```
  - **Salida esperada:** `hello` (no se ejecuta el segundo comando).

- **AND (`&&`) - fallo:**
  ```bash
  ✅MiniShell $ false && echo world
  ```
  - **Salida esperada:** No se imprime "world" porque `false` falla.

- **OR (`||`) - fallo:**
  ```bash
  ✅MiniShell $ false || echo world
  ```
  - **Salida esperada:** `world` (se ejecuta porque `false` falla).

- **AND con éxito:**
  ```bash
  ✅MiniShell $ true && echo success
  ```
  - **Salida esperada:** `success`.

- **OR con éxito:**
  ```bash
  ✅MiniShell $ true || echo failure
  ```
  - **Salida esperada:** No se imprime "failure" porque `true` tiene éxito.

- **Combinación con paréntesis:**
  ```bash
  ✅MiniShell $ (echo hello && echo world) || echo failed
  ```
  - **Salida esperada:**
    ```
    hello
    world
    ```

- **Combinación compleja:**
  ```bash
  ✅MiniShell $ echo test && (echo nested && echo commands)
  ```
  - **Salida esperada:**
    ```
    test
    nested
    commands
    ```

---

## 19. Wildcards (`*`) (Bonus)

### Descripción
Prueba la expansión de `*` a los nombres de archivo del directorio actual.

### Pruebas y Salidas Esperadas
- **Wildcard con echo:**
  ```bash
  ✅MiniShell $ echo *
  ```
  - **Salida esperada:** Una lista de todos los archivos y directorios en el directorio actual, separados por espacios.

- **Wildcard con ls:**
  ```bash
  ✅MiniShell $ ls *.txt
  ```
  - **Salida esperada:** Listado de todos los archivos `.txt`.

- **Wildcard con cat:**
  ```bash
  ✅MiniShell $ cat *.c
  ```
  - **Salida esperada:** Contenido de todos los archivos `.c` concatenados.

- **Wildcard con echo para headers:**
  ```bash
  ✅MiniShell $ echo *.h
  ```
  - **Salida esperada:** Una lista de todos los archivos `.h`, separados por espacios.

- **Wildcard con rm:**
  ```bash
  ✅MiniShell $ rm *.tmp
  ```
  - **Salida esperada:** Elimina todos los archivos `.tmp` (si existen).

- **Wildcard con patrón:**
  ```bash
  ✅MiniShell $ ls *test*
  ```
  - **Salida esperada:** Listado de todos los archivos que contengan "test" en su nombre.

- **Wildcard con prefijo:**
  ```bash
  ✅MiniShell $ echo file*
  ```
  - **Salida esperada:** Todos los archivos que empiecen con "file".

---

## 20. ¡Sorpresa! (O no...)

### Descripción
Casos de comillas anidadas y su interpretación.

### Pruebas y Salidas Esperadas
- **Establecer variable USER:**
  ```bash
  MiniShell $ export USER=testuser
  ```
  - **Salida esperada:** No hay salida visible.

- **Comillas simples dentro de dobles:**
  ```bash
  ✅MiniShell $ echo "'$USER'"
  ```
  - **Salida esperada:** `'testuser'`. Las comillas dobles exteriores se eliminan, las comillas simples interiores se tratan como caracteres literales, y `$USER` se expande.

- **Comillas dobles dentro de simples:**
  ```bash
  ✅MiniShell $ echo '"$USER"'
  ```
  - **Salida esperada:** `"$USER"`. Las comillas simples exteriores se eliminan, y todo lo de adentro, incluidas las comillas dobles y el `$`, se trata como literal.

- **Expansión normal:**
  ```bash
  ✅MiniShell $ echo "$USER"
  ```
  - **Salida esperada:** `testuser`.

- **Sin expansión:**
  ```bash
  ✅MiniShell $ echo '$USER'
  ```
  - **Salida esperada:** `$USER`.

- **Caso complejo con comillas simples:**
  ```bash
  ✅MiniShell $ echo "hello '$USER' world"
  ```
  - **Salida esperada:** `hello 'testuser' world`.

- **Caso complejo con comillas dobles:**
  ```bash
  ✅MiniShell $ echo 'hello "$USER" world'
  ```
  - **Salida esperada:** `hello "$USER" world`.

---

## Comandos útiles para comparar con bash

Para verificar que tu minishell se comporta como bash:

```bash
# Ejecutar el mismo comando en bash y comparar
bash -c "comando_a_probar"

# Verificar código de salida en bash
bash -c "comando_a_probar"; echo $?

# Probar variables de entorno
bash -c 'echo $USER'

# Probar redirecciones
bash -c 'echo "test" > file.txt; cat file.txt'
```

---

## Apéndice: Casos Edge y Pruebas Malintencionadas

### A1. Casos Edge de Comillas

#### Comillas no cerradas (debería fallar)
```bash
✅MiniShell $ echo "hello world
```
- **Salida esperada:** El shell debería pedir más entrada con `>` o mostrar error de sintaxis.

#### Comillas vacías múltiples
```bash
✅MiniShell $ echo "" "" ""
```
- **Salida esperada:** Dos líneas en blanco (tres argumentos vacíos).

#### Comillas anidadas complejas
```bash
❌MiniShell $ echo "He said: \"She said: 'Hello world'\""
```
- **Salida esperada:** `He said: "She said: 'Hello world'"`.

#### Comillas simples dentro de comillas dobles con variables
```bash
✅MiniShell $ export TEST="world"
✅MiniShell $ echo "'Hello $TEST'"
```
- **Salida esperada:** `'Hello world'`.

### A2. Variables de Entorno Extremas

#### Variables con nombres válidos pero raros
```bash
✅MiniShell $ export _VAR=value
✅MiniShell $ export VAR_123_ABC=test
✅MiniShell $ export ___=empty
✅MiniShell $ echo $_VAR $VAR_123_ABC $___
```
- **Salida esperada:** `value test empty`.

#### Variables que no existen seguidas de texto
```bash
✅MiniShell $ echo $NOEXISTEtest
```
- **Salida esperada:** `test` (solo se expande `$NOEXISTE` a vacío).

#### Variables con caracteres especiales en el valor
```bash
✅MiniShell $ export SPECIAL='!@#$%^&*()[]{}|;:"<>?'
✅MiniShell $ echo $SPECIAL
```
- **Salida esperada:** `!@#$%^&*()[]{}|;:"<>?`.

#### Variable $? después de comandos con pipes
```bash
✅MiniShell $ ls | grep nonexistent
✅MiniShell $ echo $?
```
- **Salida esperada:** `1` (código de salida de `grep`, no de `ls`).

### A3. Redirecciones Malintencionadas

#### Múltiples redirecciones del mismo tipo
```bash
✅MiniShell $ echo "test" > file1 > file2 > file3
```
- **Salida esperada:** Solo `file3` debería contener "test".

#### Redirección a archivo sin permisos
```bash
✅MiniShell $ echo "test" > /root/forbidden.txt
```
- **Salida esperada:** `Permission denied`.

#### Redirección desde archivo que no existe
```bash
✅MiniShell $ cat < /file/that/does/not/exist
```
- **Salida esperada:** `No such file or directory`.

#### Redirección con nombre de archivo vacío
```bash
✅MiniShell $ echo "test" > ""
```
- **Salida esperada:** Error de sintaxis o `No such file or directory`.

#### Here document con delimitador que aparece en el contenido
```bash
✅MiniShell $ cat << EOF
This line contains EOF in it
EOF
```
- **Salida esperada:** `This line contains EOF in it`.

#### Here document con delimitador vacío
```bash
✅MiniShell $ cat << ""
test
""
```
- **Salida esperada:** El contenido hasta encontrar una línea vacía.

### A4. Pipes Complejos

#### Pipe con comando que no existe
```bash
✅MiniShell $ commandnotfound | cat
```
- **Salida esperada:** Error de `commandnotfound`, `cat` no debería recibir entrada.

#### Pipe con redirección compleja
```bash
✅MiniShell $ echo "test" | cat > output.txt | wc -l
```
- **Salida esperada:** Comportamiento indefinido, posible error de sintaxis.

#### Pipe al inicio de línea
```bash
✅MiniShell $ | cat
```
- **Salida esperada:** Error de sintaxis.

#### Pipe al final de línea
```bash
✅MiniShell $ echo "test" |
```
- **Salida esperada:** Error de sintaxis o prompt secundario.

### A5. Casos Edge de Builtin Commands

#### cd con múltiples argumentos
```bash
❌MiniShell $ cd /tmp /home
```
- **Salida esperada:** `too many arguments`.

#### cd con guión pero sin OLDPWD
```bash
✅MiniShell $ unset OLDPWD
✅MiniShell $ cd -
```
- **Salida esperada:** `OLDPWD not set`.

#### export con igual al principio
```bash
✅MiniShell $ export =value
```
- **Salida esperada:** `not a valid identifier`.

#### export con nombre que contiene espacios
```bash
✅MiniShell $ export "VAR WITH SPACES"=value
```
- **Salida esperada:** `not a valid identifier`.

#### unset con argumentos especiales
```bash
❌MiniShell $ unset ""
✅MiniShell $ unset PATH HOME USER
```
- **Salida esperada:** Primera línea posible error, segunda línea elimina las tres variables.

### A6. Casos Edge de Señales

#### Ctrl+C durante here document
```bash
✅MiniShell $ cat << EOF
line1
^C
```
- **Salida esperada:** Debería cancelar el here document y mostrar nuevo prompt.

#### Ctrl+D durante here document
```bash
✅MiniShell $ cat << EOF
line1
^D
```
- **Salida esperada:** Debería completar el here document con EOF implícito.

#### Múltiples Ctrl+C consecutivos
```bash
✅MiniShell $ ^C^C^C
```
- **Salida esperada:** Múltiples nuevas líneas, prompt limpio.

### A7. Casos Edge de Argumentos y Espacios

#### Argumentos con solo espacios
```bash
✅MiniShell $ echo "   " "   " "   "
```
- **Salida esperada:** `         ` (tres grupos de espacios).

#### Tabuladores vs espacios
```bash
✅MiniShell $ echo "	" "    "
```
- **Salida esperada:** Un tabulador seguido de cuatro espacios.

#### Argumentos extremadamente largos
```bash
❌MiniShell $ echo $(python3 -c "print('a'*10000)")
```
- **Salida esperada:** 10000 caracteres 'a' (si soporta expansión de comandos).

### A8. Casos Edge de PATH y Ejecución

#### PATH con directorio inexistente
```bash
✅MiniShell $ export PATH="/nonexistent:/bin"
✅MiniShell $ ls
```
- **Salida esperada:** Funciona (encuentra `ls` en `/bin`).

#### PATH vacío
```bash
✅MiniShell $ export PATH=""
✅MiniShell $ ls
```
- **Salida esperada:** `command not found`.

#### PATH con solo dos puntos
```bash
✅MiniShell $ export PATH=":"
✅MiniShell $ ls
```
- **Salida esperada:** `command not found`.

#### Comando con ruta que contiene espacios
```bash
✅MiniShell $ "/bin/echo with spaces"
```
- **Salida esperada:** `No such file or directory`.

### A9. Casos Edge de Expansión de Variables

#### Variable seguida inmediatamente de dígitos
```bash
✅MiniShell $ export TEST=hello
✅MiniShell $ echo $TEST123
```
- **Salida esperada:** Vacío (busca la variable `TEST123`).

#### Variable con llaves vacías
```bash
❌MiniShell $ echo ${}
```
- **Salida esperada:** `${}` literal o error de sintaxis.

#### Variable con llaves sin cerrar
```bash
❌MiniShell $ echo ${USER
```
- **Salida esperada:** Error de sintaxis.

#### Múltiples signos de dólar
```bash
✅MiniShell $ echo $$
```
- **Salida esperada:** El PID del shell (si soporta `$$`).

### A10. Casos Edge de Wildcards (si implementado)

#### Wildcard en directorio vacío
```bash
✅MiniShell $ mkdir empty_dir
✅MiniShell $ cd empty_dir
✅MiniShell $ echo *
```
- **Salida esperada:** `*` literal.

#### Wildcard con archivos ocultos
```bash
❌MiniShell $ echo .*
```
- **Salida esperada:** Archivos que empiecen con punto.

#### Wildcard con caracteres especiales en nombres
```bash
✅MiniShell $ touch "file with spaces.txt"
✅MiniShell $ echo *.txt
```
- **Salida esperada:** `file with spaces.txt`.

### A11. Casos Edge de Operadores Lógicos

#### Operadores sin espacios
```bash
MiniShell $ echo hello&&echo world
```
- **Salida esperada:** `hello` seguido de `world` o error de sintaxis.

#### Operadores al principio
```bash
❌MiniShell $ && echo hello
```
- **Salida esperada:** Error de sintaxis.

#### Operadores al final
```bash
❌MiniShell $ echo hello &&
```
- **Salida esperada:** Error de sintaxis o prompt secundario.

### A12. Casos Edge de Memoria y Límites

#### Línea extremadamente larga
```bash
❌MiniShell $ echo "$(python3 -c "print('a'*100000)")"
```
- **Salida esperada:** 100000 caracteres 'a' o error de memoria.

#### Muchos argumentos
```bash
❌MiniShell $ echo $(seq 1 10000)
```
- **Salida esperada:** Números del 1 al 10000 o error.

#### Pipes anidados profundos
```bash
✅MiniShell $ echo test | cat | cat | cat | cat | cat | cat | cat | cat | cat | cat
```
- **Salida esperada:** `test` o error de recursos.

### A13. Casos Edge de Archivos y Permisos

#### Ejecutar archivo sin permisos de ejecución
```bash
✅MiniShell $ touch noexec
✅MiniShell $ chmod 644 noexec
✅MiniShell $ ./noexec
```
- **Salida esperada:** `Permission denied`.

#### Ejecutar directorio
```bash
⚠️MiniShell $ mkdir testdir
⚠️MiniShell $ ./testdir
```
- **Salida esperada:** `Is a directory`.

#### Redirección a dispositivo
```bash
✅MiniShell $ echo "test" > /dev/null
⚠️MiniShell $ cat < /dev/zero | head -1
```
- **Salida esperada:** Primera línea funciona, segunda muestra datos de `/dev/zero`.

### A14. Casos Edge de Sintaxis

#### Punto y coma al final
```bash
❌MiniShell $ echo hello;
```
- **Salida esperada:** `hello` (si soporta `;`) o error de sintaxis.

#### Múltiples espacios y tabuladores
```bash
❌MiniShell $ echo		hello			world
```
- **Salida esperada:** `hello world` (espacios normalizados).

#### Comando con solo espacios
```bash
✅MiniShell $                    
```
- **Salida esperada:** Nuevo prompt sin ejecutar nada.

### A15. Casos Edge de Escape y Caracteres Especiales

#### Backslash al final de línea
```bash
❌MiniShell $ echo hello\
```
- **Salida esperada:** `hello\` o continuación de línea.

#### Caracteres no imprimibles
```bash
❌MiniShell $ echo -e "hello\x00world"
```
- **Salida esperada:** `hello` seguido de caracteres de control.

#### Caracteres Unicode
```bash
✅MiniShell $ echo "Hola 世界 🌍"
```
- **Salida esperada:** `Hola 世界 🌍`.

### A16. Casos Edge de Historial

#### Navegación en historial vacío
- **Prueba:** Iniciar minishell nuevo y usar flechas arriba/abajo.✅
- **Salida esperada:** No debería hacer nada.

#### Comando muy largo en historial
```bash
✅MiniShell $ echo "very long command that should be stored in history and retrievable"
# Usar flecha arriba
```
- **Salida esperada:** El comando debería aparecer completo.

### A17. Casos Maliciosos de Seguridad

#### Inyección de comandos en variables
```bash
✅MiniShell $ export MALICIOUS="test; rm -rf /"
✅MiniShell $ echo $MALICIOUS
```
- **Salida esperada:** `test; rm -rf /` (literal, no ejecutado).

#### Redirección a archivos del sistema
```bash
✅MiniShell $ echo "malicious" > /etc/passwd
```
- **Salida esperada:** `Permission denied`.

#### Paths relativos maliciosos
```bash
✅MiniShell $ ../../../bin/ls
```
- **Salida esperada:** Funciona si la ruta existe.

### A18. Casos Edge de Concurrencia

#### Múltiples procesos hijos
```bash
❌MiniShell $ sleep 1 & sleep 1 & sleep 1
```
- **Salida esperada:** Tres procesos en paralelo (si soporta `&`).

#### Interrupción durante fork
```bash
✅MiniShell $ sleep 10
# Ctrl+C inmediatamente
```
- **Salida esperada:** Proceso interrumpido, código de salida 130.

---

## Notas para el Evaluador

### Criterios de Evaluación Estrictos

1. **Gestión de Memoria**: Verificar que no hay leaks con valgrind.
2. **Señales**: Comportamiento idéntico a bash en todos los casos.
3. **Códigos de Salida**: Deben coincidir exactamente con bash.
4. **Parseo**: Errores de sintaxis deben manejarse correctamente.
5. **Variables**: Expansión debe ser idéntica a bash.
6. **Redirecciones**: Orden de aplicación y manejo de errores.
7. **Pipes**: Propagación correcta de códigos de salida.
8. **Builtins**: Comportamiento exacto según especificaciones.

### Comandos de Verificación

```bash
# Verificar leaks de memoria
valgrind --leak-check=full ./minishell

# Comparar con bash
bash -c "comando" 2>&1; echo "Exit: $?"
./minishell -c "comando" 2>&1; echo "Exit: $?"

# Verificar señales
timeout 5 ./minishell  # Debería terminar con Ctrl+C

# Verificar robustez
echo "comando malicioso" | ./minishell
```
