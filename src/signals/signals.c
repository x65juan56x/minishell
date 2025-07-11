#include "../../include/minishell.h"

volatile sig_atomic_t	g_signal_status = 0;

static void	interactive_sigint_handler(int signum)
{
	g_signal_status = signum;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/**
 * Manejador para SIGINT (Ctrl+C) en modo interactivo.
 * Esta función es llamada por el kernel. Debe ser simple y segura.
 * Establece el estado global, escribe una nueva línea y usa las
 * funciones seguras de readline para refrescar el prompt.
 */

void	setup_interactive_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_RESTART;
	sa_int.sa_handler = interactive_sigint_handler;
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
		perror("sigaction");

	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sa_quit.sa_handler = SIG_IGN;
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
		perror("sigaction");
}
/**
 * Configura las señales para el modo interactivo (el prompt principal).
 * Usa sigaction para un manejo robusto y portable.
 * - SIGINT (Ctrl+C) es capturado para mostrar un nuevo prompt sin salir.
 * - SIGQUIT (Ctrl+\) es ignorado, como pide el subject.
 * - SA_RESTART reinicia llamadas al sistema lentas (como read) si son
 *   interrumpidas por una señal, evitando errores.
 */

void	setup_child_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_DFL; // DFL = Default action
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("sigaction");
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		perror("sigaction");
}
/**
 * Configura las señales para los procesos hijos (comandos externos).
 * Los comandos ejecutados (como `cat`, `sleep`) no deben heredar
 *  el manejo de señales del shell. Deben tener el comportamiento
 *  por defecto (terminar al recibir SIGINT o SIGQUIT).
 */

static void	heredoc_sigint_handler(int signum)
{
	(void)signum;
	exit(130); // Salir con código 130 para indicar interrupción por Ctrl+C
}
/**
 * Manejador de señal para el proceso hijo del heredoc.
 */

void	setup_heredoc_signals(void)
{
	struct sigaction	sa_int;

	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sa_int.sa_handler = heredoc_sigint_handler;
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
		perror("sigaction");
}
/**
 * Configura las señales para el proceso hijo que lee un here-document.
 * Este proceso hijo necesita un manejo especial: al recibir
 *  Ctrl+C, debe salir inmediatamente para que el padre pueda
 *  detectarlo y cancelar la operación.
 */

void	ignore_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN; // IGN = Ignore
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("sigaction");
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		perror("sigaction");
}
/**
 * Ignora las señales temporalmente.
 * El shell principal debe ignorar SIGINT y SIGQUIT mientras
 *  un comando se está ejecutando en primer plano. De esta forma,
 *  Ctrl+C solo afecta al hijo y no al shell.
 */
