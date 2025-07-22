# Escenarios para Detectar Memory Leaks en Minishell

Esta lista cubre los casos más propensos a fugas de memoria en un shell. Úsala para probar tu minishell con Valgrind y asegurar que no haya leaks en ninguna ruta de ejecución.

---

## 1. Entrada y Parsing

- `echo ""`
- `echo ''`
- `echo "hola`
- `echo 'hola`
- `echo hola      mundo`
- `echo $NOEXISTE`
- `echo $USER$NOEXISTE`
- `echo $USER$?$$`
- `echo "User: $USER, Home: $HOME"`
- `echo '$USER'`
- `echo "$USER"`

---

## 2. Expansión de Variables

- `echo $FOOBAR`
- `export LONGVAR=$(head -c 10000 /dev/zero | tr '\0' a)`
- `echo $LONGVAR`
- `echo $USER$?$$`
- `echo $USER$?$$$NOEXISTE`
- `echo $USER$USER$USER`
- `echo $USER$USER$USER$USER$USER`

---

## 3. Wildcards

- `ls *.noexiste`
- `touch file{1..100}.txt; ls *.txt`
- `ls a* b*`
- `ls "*.c"`
- `ls *`
- `ls .*`

---

## 4. Redirecciones y Pipes

- `cat < noexiste.txt`
- `echo hola > test.txt`
- `echo hola >> test.txt`
- `cat < test.txt > out.txt`
- `noexiste | echo hola`
- `echo hola | cat | wc -l`
- `echo hola > a > b > c > d`
- `yes | yes | yes | yes | head -n 1`

---

## 5. Here-Document

- `cat << EOF` (luego varias líneas, luego EOF)
- `cat << EOF` (presiona Ctrl+C antes de EOF)
- `cat << ""`
- `cat << EOF | cat`

---

## 6. Builtins

- `export VAR1=hola`
- `unset VAR1`
- `export VAR2=`
- `export 1VAR=hola`
- `unset NOEXISTE`
- `env`
- `cd`
- `cd /noexiste`
- `cd /tmp`
- `exit`
- `exit hola`
- `exit 42`
- `exit 1 2`

---

## 7. Comandos Externos

- `noexiste`
- `ls /noexiste`
- `seq 10000`
- `cat` (luego muchas líneas y Ctrl+D)
- `VAR=hola echo $VAR`
- `export VAR=hola; VAR=adios echo $VAR`

---

## 8. Modo No Interactivo

- Ejecuta un script con muchas líneas:
  ```sh
  echo "echo hola" > script.sh
  for i in {1..100}; do echo "echo $i" >> script.sh; done
  ./minishell < script.sh
  ```
- Redirección de entrada desde un archivo grande:
  ```sh
  ./minishell < /usr/share/dict/words
  ```

---

## 9. Errores y Edge Cases

- `echo $(seq 1000)`
- `echo hola |`
- `echo |`
- `echo hola &&`
- `echo hola ||`
- `echo hola && echo mundo || echo error`
- `echo hola && echo mundo && echo test`
- `echo hola || echo mundo || echo test`
- `echo hola && echo mundo || echo test`
- `echo hola && (echo mundo || echo test)`
- `echo hola && (echo mundo && echo test)`
- `echo hola && (echo mundo && (echo test || echo foo))`
- `VAR=hola`
- `VAR=hola VAR2=adios`
- `VAR=hola VAR2=adios echo $VAR $VAR2`

---

## 10. Señales

- `Ctrl+C en el prompt`
- `Ctrl+D en el prompt`
- `Ctrl+\ en el prompt`
- `Ctrl+C durante ejecución de comando externo (sleep 10)`
- `Ctrl+C durante heredoc`
