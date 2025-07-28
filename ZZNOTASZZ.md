Cosas a probar y ver como se gestionan:

(ls | wc -l) > out.txt

cmd > a < b

minishell git:(main) bash
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ >holi echo papafrita
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ cat holi 
papafrita
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ echo papafrita <123123
bash: 123123: No such file or directory
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ echo >holi papafrita con extra ketchup
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ cat holi 
papafrita con extra ketchup
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ echo papafrita >holi >holidenuevo
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ cat holi
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ cat holidenuevo 
papafrita
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ echo papafrita >holi con >holidenuevo ketcup
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ cat holidenuevo 
papafrita con ketcup
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ cat holidenuevo <<hola
> ^C
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ <hola
bash: hola: No such file or directory
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ >hoal
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ ^C
jmondon@c1r14s4:~/Documents/Git Personal/minishell$ ./minishell 
MiniShell $ >holi
Error: Failed to parse tokens
MiniShell $ <<holi
Error: Failed to parse tokens
MiniShell $ cat <<holi
holi: Success
MiniShell $ cat << holi
holi: Success
MiniShell $ <<holi cat
Error: Failed to parse tokens
MiniShell $ 

echo $?

exit 5

echo $$  

echo $SHELL    

ls /proc/$$    

ls /proc/$$/fd

ls /proc/self/fd

PATH="" ./minishell

yes ls | ./minishell

yes | ./minishell    

./minishell 1 12312312312312

./minishell 1



Parte Obligatoria  
✅ Completado:  

Mostrar un prompt: Implementado en minishell.c con readline(PROMPT).  
Tener un historial funcional: Implementado con add_history() y rl_clear_history() en minishell.c.  
Buscar y lanzar ejecutables (PATH, relativo, absoluto): Perfectamente implementado en path_utils.c y cmd_executor.c. La lógica de find_command_path maneja correctamente los tres casos.  
Redirección de salida >: Implementado. redirect_executor.c abre el fichero con O_TRUNC.  
Redirección de salida en modo append >>: Implementado. redirect_executor.c abre el fichero con O_APPEND.  
Redirección de entrada <: Implementado. redirect_executor.c abre el fichero con O_RDONLY.  
Implementar pipes |: Implementado. La lógica en pipe_executor.c y executor.c crea dos procesos hijos y conecta su stdin/stdout correctamente.  
Manejar << (Here Document): Implementado. heredoc_executor.c lee la entrada hasta el delimitador y heredoc_preprocessor.c lo integra correctamente en los pipes.  
Manejar ctrl-C, ctrl-D y ctrl-\:  
	ctrl-D funciona (porque readline devuelve NULL), pero el resto del manejo de señales (signal, sigaction) no está implementado.  
	Durante ejecución del shell (modo prompt)  
 		Ignorar SIGQUIT (^\\) → No debe cerrar el shell.  
		Capturar SIGINT (^C) → Debe: Cancelar la línea actual.Mostrar un nuevo prompt limpio.No salir del shell.  
	Durante ejecución de un proceso hijo (comando externo)  
 		SIGINT debe terminar el proceso hijo, y el shell debe mostrar un salto de línea.  
		SIGQUIT debe mostrar el mensaje Quit (core dumped) si aplica, y finalizar el hijo.  
Builtins: echo, pwd, env, exit, cd, export, unset.
La expansión de variables ($VAR) dentro de las comillas dobles pero no en las simples.



🟡 Parcialmente Implementado / Requiere Ajustes:





❌ Pendiente de Implementar:  
Manejar $?: No hay implementación para expandir $? al código de salida del último comando. Necesitas una variable (quizás en una estructura principal) para almacenar exit_status y un mecanismo de expansión que la consulte.  

Parte Bonus  

❌ Pendiente de Implementar:  

Wildcards *: No hay ninguna implementación para la expansión de wildcards. Esto requeriría una nueva función que, antes de execve, revise los argumentos, y si encuentra un *, lea el directorio actual (opendir, readdir) y reemplace el argumento por los ficheros que coincidan.



VALGRIND
valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell


checkear:

 cat << EOF | sort
	ctrl + C ❌
	ctrl + C mientras esta en heredoc ❌
cat << EOF
	ctrl + C no hay salto de linea, pero si promt
cat
	ctrl + C ✅
	ctrl + \ ✅


Causa Principal: Corrupción de Punteros
El problema crítico estaba en que calculabas last_match después de llamar a replace_token_with_matches. Esta función modifica la estructura de la lista enlazada y libera el token original (current), lo que puede dejar matches en un estado inconsistente.

Secuencia Problemática:
replace_token_with_matches modifica la lista y libera memoria
Después intentabas recalcular last_match = matches
matches podía estar corrompido o apuntar a memoria inválida
last_match->next devolvía NULL en lugar del siguiente token (*.txt)
El bucle terminaba prematuramente sin procesar *.txt
Causa Secundaria: Salto de Archivos en Directorio
El primer cambio en create_match_tokens solucionó un problema menor donde se saltaban archivos del directorio por la doble llamada a readdir(), pero no era la causa principal del fallo con múltiples wildcards.

Por Qué Ahora Funciona
Preservación de Referencias: Al calcular last_match antes de replace_token_with_matches, conservas la referencia correcta al último nodo de la lista de matches.

Punteros Consistentes: last_match->next ahora apunta correctamente al siguiente token en la lista original (*.txt), permitiendo que el bucle continúe.

Procesamiento Secuencial: Cada wildcard se procesa correctamente en orden, manteniendo la integridad de la lista enlazada.

Lección Aprendida
El orden de las operaciones es crítico cuando trabajas con listas enlazadas que se modifican dinámicamente. Siempre conserva las referencias que necesitas antes de realizar operaciones que modifiquen la estructura de datos.