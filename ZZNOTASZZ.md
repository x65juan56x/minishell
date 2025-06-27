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

valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell

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

