Tabla de Contenidos
¿Qué es el Parsing?
Concepto de AST
Arquitectura del Parser
Estructuras de Datos
Análisis Detallado por Archivos
Precedencia y Asociatividad
Manejo de Errores
Ejemplos de Parsing
Ventajas de esta Arquitectura
¿Qué es el Parsing?
El parsing (análisis sintáctico) es el segundo paso en el procesamiento de comandos de una shell. Toma la lista de tokens generada por el tokenizador y construye un Abstract Syntax Tree (AST) que representa la estructura jerárquica del comando.

Flujo de Datos:
Input String → Tokenizer → Lista de Tokens → Parser → AST → Executor

Diferencia con Tokenización:
Tokenización: Divide el input en unidades significativas (palabras)
Parsing: Analiza las relaciones y estructura entre esas unidades (gramática)
Concepto de AST
Un AST (Abstract Syntax Tree) es una representación en árbol de la estructura sintáctica del comando. Cada nodo representa una operación, y los hijos representan los operandos.

Ejemplo Visual Completo:

Input:

echo hello | grep test > output.txt

Tokens producidos:

TOKEN_WORD: "echo"
TOKEN_WORD: "hello" 
TOKEN_PIPE: "|"
TOKEN_WORD: "grep"
TOKEN_WORD: "test"
TOKEN_REDIRECT_OUT: ">"
TOKEN_WORD: "output.txt"
TOKEN_EOF: NULL

AST resultante:

			NODE_REDIRECT_OUT
		   /                \
	  NODE_PIPE           file: "output.txt"
	 /         \
NODE_COMMAND  NODE_COMMAND
	|             |
args:         args:
["echo",      ["grep",
 "hello",      "test",
 NULL]         NULL]


 ¿Por qué un Árbol?
Jerarquía Natural: Los operadores tienen precedencia (redirects > pipes)
Evaluación Recursiva: Se puede ejecutar de manera recursiva
Extensibilidad: Fácil añadir nuevos operadores y estructuras
Optimización: Permite optimizaciones futuras

Arquitectura del Parser
Filosofía de Diseño: Recursive Descent Parser
El parser utiliza una estrategia de descenso recursivo donde cada función maneja un nivel de precedencia específico.

Jerarquía de Precedencia (de menor a mayor):

Expresiones lógicas (&&, ||) - BONUS
Pipes (|)
Redirects (<, >, >>, <<)
Comandos simples (palabras y argumentos)


Separación de Responsabilidades:

src/parser/
├── parser.c                  # Coordinación principal
├── ast_utils.c              # Creación y gestión de nodos AST
├── parser_commands.c        # Parsing de comandos simples
└── parser_expressions.c     # Parsing de expresiones (pipes, redirects)


Estructuras de Datos

Tipos de Nodos AST:

typedef enum e_node_type
{
	NODE_COMMAND,         // Comando simple: echo hello
	NODE_PIPE,           // Pipe: cmd1 | cmd2
	NODE_REDIRECT_IN,    // Redirect entrada: cmd < file
	NODE_REDIRECT_OUT,   // Redirect salida: cmd > file
	NODE_REDIRECT_APPEND,// Append: cmd >> file
	NODE_HEREDOC,        // Here document: cmd << EOF
	NODE_AND,            // AND lógico: cmd1 && cmd2 (BONUS)
	NODE_OR              // OR lógico: cmd1 || cmd2 (BONUS)
} t_node_type;

Nodo AST:

typedef struct s_ast_node
{
	t_node_type         type;    // Tipo de nodo
	char                **args;  // Para comandos: ["echo", "hello", NULL]
	char                *file;   // Para redirects: "output.txt"
	struct s_ast_node   *left;   // Hijo izquierdo
	struct s_ast_node   *right;  // Hijo derecho
} t_ast_node;

Estado del Parser:

typedef struct s_parser
{
	t_token     *tokens;   // Lista original de tokens
	t_token     *current;  // Token actual siendo procesado
	int         error;     // Flag de error
} t_parser;


Análisis Detallado por Archivos

1. parser.c - Coordinador Principal

**Función: parse(t_token *tokens)
•Responsabilidad: Punto de entrada principal del parser.
•Flujo:
-Validación: Verifica que la lista de tokens no sea NULL
-Inicialización: Configura el estado del parser
-Parsing: Llama al nivel más alto de precedencia
-Validación final: Verifica que no haya errores
-Cleanup: Limpia memoria en caso de error

Funciones Auxiliares:

**Función: init_parser(t_parser *parser, t_token *tokens)
•Propósito: Inicializar el estado del parser
•Configuración: Establece posición inicial y resetea flags de error

**Función: advance_token(t_parser *parser)
•Propósito: Avanzar al siguiente token
•Protección: No avanza más allá del token EOF

**Función: parse_expression(t_parser *parser)
•Propósito: Punto de entrada al parsing de expresiones
•Delegación: Llama al nivel más alto de precedencia (pipes)


2. ast_utils.c - Gestión de Nodos AST

**Función: create_ast_node(t_node_type type)
•Responsabilidad: Crear un nodo AST básico con inicialización segura.

¿Por qué inicialización completa?

-Previene accesos a memoria no inicializada
-Facilita el debugging
-Hace el cleanup más seguro

**Función: create_binary_node(t_token_type op_type, t_ast_node *left, t_ast_node *right)
•Responsabilidad: Crear nodos para operadores binarios (pipes, lógicos).

Uso típico:
// Para: cmd1 | cmd2
pipe_node = create_binary_node(TOKEN_PIPE, cmd1_node, cmd2_node);

**Función: create_redirect_node(t_token_type redirect_type, t_ast_node *cmd, char *file)
•Responsabilidad: Crear nodos específicos para redirects.
•Características especiales:
-El comando va en left
-El archivo va en file (string)
-right queda NULL (estructura específica para redirects)

**Función: cleanup_ast(t_ast_node *node)
•Responsabilidad: Liberar recursivamente todo el árbol AST.
•Algoritmo:
-Verificación NULL: Protección contra nodos nulos
-Liberar contenido: Arrays de argumentos y strings de archivos
-Recursión: Limpiar hijos izquierdo y derecho
-Liberar nodo: Finalmente liberar la estructura

void cleanup_ast(t_ast_node *node)
{
	if (!node)
		return ;
	if (node->args)         // Liberar array de argumentos
		ft_freearr(node->args);
	if (node->file)         // Liberar string de archivo
		free(node->file);
	cleanup_ast(node->left);    // Recursión izquierda
	cleanup_ast(node->right);   // Recursión derecha
	free(node);             // Liberar estructura
}


3. parser_commands.c - Parsing de Comandos Simples

**Función: parse_command(t_parser *parser)
•Responsabilidad: Parsear un comando simple con sus argumentos.
•Proceso:
-Validación: Verifica que el token actual sea una palabra
-Creación: Crea un nodo de tipo NODE_COMMAND
-Recolección: Recolecta todos los argumentos consecutivos
-Validación final: Verifica que se hayan recolectado argumentos

**Función: collect_command_args(t_parser *parser)
•Responsabilidad: Recoger todos los argumentos de un comando.
•Algoritmo detallado:
-Conteo: Cuenta cuántos tokens consecutivos son palabras
-Reserva: Aloca memoria para array de strings (count + 1 para NULL final)
-Recolección: Consume tokens y copia sus valores
-Terminación: Añade NULL al final del array

char **collect_command_args(t_parser *parser)
{
	char    **args;
	int     count;
	int     i;
	t_token *token;

	count = count_command_args(parser);     // Contar argumentos
	if (count == 0)
		return (NULL);
	args = malloc(sizeof(char *) * (count + 1));  // Alocar array
	if (!args)
		return (NULL);
	i = 0;
	while (i < count)
	{
		token = consume_token(parser, TOKEN_WORD);  // Consumir token
		if (!token)
			return (ft_freearr(args), NULL);
		args[i] = ft_strdup(token->value);          // Copiar valor
		if (!args[i])
			return (ft_freearr(args), NULL);
		i++;
	}
	args[i] = NULL;                         // Terminar array
	return (args);
}

**Función: consume_token(t_parser *parser, t_token_type expected)
•Responsabilidad: Consumir un token específico y avanzar el parser.
•Características:
-Validación: Verifica que el token actual sea del tipo esperado
-Avance: Mueve el parser al siguiente token
-Error handling: Establece flag de error si falla


4. parser_expressions.c - Parsing de Expresiones

**Función: parse_pipe_expression(t_parser *parser)
•Responsabilidad: Parsear expresiones con pipes (precedencia más baja).
•Algoritmo (Left-Associative):

t_ast_node *parse_pipe_expression(t_parser *parser)
{
	t_ast_node *left;
	t_ast_node *right;
	t_token    *pipe_token;

	left = parse_redirect_expression(parser);  // Parsear lado izquierdo
	if (!left)
		return (NULL);
	
	// Mientras haya pipes
	while (parser->current && parser->current->type == TOKEN_PIPE)
	{
		pipe_token = consume_token_type(parser, TOKEN_PIPE);
		if (!pipe_token)
		{
			cleanup_ast(left);
			return (NULL);
		}
		right = parse_redirect_expression(parser);  // Parsear lado derecho
		if (!right)
		{
			cleanup_ast(left);
			return (NULL);
		}
		// Crear nodo pipe con left y right
		left = create_binary_node(TOKEN_PIPE, left, right);
		if (!left)
		{
			cleanup_ast(right);
			return (NULL);
		}
	}
	return (left);
}

¿Por qué Left-Associative? Para cmd1 | cmd2 | cmd3, construye:

	PIPE
   /    \
 PIPE   cmd3
/    \
cmd1  cmd2

Esto permite ejecución secuencial de izquierda a derecha.

**Función: parse_redirect_expression(t_parser *parser)
•Responsabilidad: Parsear expresiones con redirects (precedencia más alta que pipes).
•Características especiales:
-Múltiples redirects: Un comando puede tener varios redirects
-Orden importante: Se procesan de izquierda a derecha
-Anidamiento: Cada redirect crea un nuevo nivel en el árbol
Ejemplo:

echo hello > out.txt 2> err.txt

Construye:

	REDIRECT_OUT (2> err.txt)
	/
REDIRECT_OUT (> out.txt)
	/
 COMMAND
   |
echo hello


** Precedencia y Asociatividad **

Tabla de Precedencia (de menor a mayor):


Operador		Precedencia		Asociatividad	Función Parser
&&,`							`				1
`				`				2				Left
<, >, >>, <<	3				Left			parse_redirect_expression()
comandos		4				N/A				parse_command()


¿Por qué esta Precedencia?
-Lógica de Shell: Coincide con el comportamiento de bash
-Intuición: Los redirects se aplican a comandos individuales antes que los pipes
-Ejecución: Permite optimizaciones en el executor

Ejemplo de Precedencia:

echo hello > temp.txt | grep hello

Parsing:
-echo hello > temp.txt se parsea como redirect
-El resultado se usa como lado izquierdo del pipe
-grep hello se parsea como comando simple

AST resultante:

        PIPE
       /    \
REDIRECT_OUT  COMMAND
    /           |
 COMMAND    args: ["grep", "hello"]
    |
args: ["echo", "hello"]
file: "temp.txt"


Manejo de Errores

Estrategia de Error Handling:

1. Error Flag Pattern:

typedef struct s_parser
{
    t_token *tokens;
    t_token *current;
    int     error;      // Flag global de error
} t_parser;

2. Propagación de Errores:

-Cada función retorna NULL en caso de error
-Se establece parser->error = 1 cuando se detecta un problema
-Las funciones caller verifican el estado antes de continuar

3. Cleanup Automático:

if (!new_token)
{
    cleanup_ast(left);  // Limpiar trabajo parcial
    return (NULL);      // Propagar error
}

Tipos de Errores Detectados:

Errores de Sintaxis:
-Token inesperado (ej: | al inicio)
-Falta archivo después de redirect (echo hello >)
-Comandos vacíos

Errores de Memoria:
-Fallo en malloc
-Fallo en ft_strdup

Errores de Estado:
-Lista de tokens vacía
-Parser en estado inconsistente

Ejemplos de Parsing

*Ejemplo 1: Comando Simple
•Input: echo hello world
•Tokens:
TOKEN_WORD: "echo"
TOKEN_WORD: "hello"
TOKEN_WORD: "world"
TOKEN_EOF: NULL
•AST:
NODE_COMMAND
    |
args: ["echo", "hello", "world", NULL]

*Ejemplo 2: Pipe Simple
•Input: ls | grep test
•AST:
    NODE_PIPE
   /         \
NODE_COMMAND  NODE_COMMAND
    |             |
args:         args:
["ls", NULL]  ["grep", "test", NULL]

*Ejemplo 3: Comando con Redirect
•Input: echo hello > output.txt
•AST:
NODE_REDIRECT_OUT
    /            \
NODE_COMMAND   file: "output.txt"
    |
args: ["echo", "hello", NULL]

*Ejemplo 4: Combinación Compleja
•Input: cat file.txt | grep test > result.txt
•AST:
        NODE_REDIRECT_OUT
       /                \
    NODE_PIPE        file: "result.txt"
   /         \
NODE_COMMAND  NODE_COMMAND
    |             |
args:         args:
["cat",       ["grep",
 "file.txt",   "test",
 NULL]         NULL]


 Ventajas de esta Arquitectura

1. Modularidad:
-Cada nivel de precedencia tiene su propia función
-Fácil añadir nuevos operadores
-Testing individual de componentes
2. Escalabilidad:
-Preparado para el bonus (&&, ||, paréntesis)
-Extensible a nuevas características
-Mantiene compatibilidad hacia atrás
3. Robustez:
-Manejo consistente de errores
-Cleanup automático de memoria
-Validación en cada paso
4. Claridad:
-Estructura refleja la gramática de la shell
-Código auto-documentado
-Fácil debugging y mantenimiento
5. Eficiencia:
-Una sola pasada sobre los tokens
-Construcción directa del AST
-Sin backtracking innecesario
