# Diagramas de Flujo - Minishell

## Diagrama Principal
```mermaid
flowchart TD
    subgraph "Bucle Principal (main.c)"
        START([START MINISHELL])
        INPUT[/"input = readline(PROMPT)"/]
        NULL_CHECK{input == NULL?}
        EMPTY_CHECK{*input == '\\0'?}
        HISTORY[add_history(input)]
        EXIT_CHECK{"ft_strcmp(input, 'exit') == 0"}
        TOKENIZE[tokenize(input)]
        TOKEN_ERROR{tokens == NULL?}
        PARSE[parse(tokens)]
        PARSE_ERROR{ast == NULL?}
        EXECUTE[exit_status = execute_ast(ast)]
        CLEANUP[CLEANUP<br/>cleanup_ast()<br/>cleanup_tokens()<br/>free(input)]
        EXIT_PROGRAM([EXIT PROGRAM<br/>rl_clear_history()<br/>return exit_status])
        PRINT_EXIT_MSG[printf "exit"]
        SYNTAX_ERROR[printf "syntax error"]
    end

    %% Flujo
    START --> INPUT
    INPUT --> NULL_CHECK
    NULL_CHECK -- SÍ --> PRINT_EXIT_MSG --> EXIT_PROGRAM
    NULL_CHECK -- NO --> EMPTY_CHECK
    EMPTY_CHECK -- SÍ --> INPUT
    EMPTY_CHECK -- NO --> HISTORY
    HISTORY --> EXIT_CHECK
    EXIT_CHECK -- SÍ --> CLEANUP --> EXIT_PROGRAM
    EXIT_CHECK -- NO --> TOKENIZE
    TOKENIZE --> TOKEN_ERROR
    TOKEN_ERROR -- SÍ --> CLEANUP --> INPUT
    TOKEN_ERROR -- NO --> PARSE
    PARSE --> PARSE_ERROR
    PARSE_ERROR -- SÍ --> SYNTAX_ERROR --> CLEANUP --> INPUT
    PARSE_ERROR -- NO --> EXECUTE
    EXECUTE --> CLEANUP

    %% Estilos
    classDef startEnd fill:#4CAF50,stroke:#2E7D32,color:#fff
    classDef process fill:#2196F3,stroke:#1565C0,color:#fff
    classDef decision fill:#FF9800,stroke:#E65100,color:#fff
    classDef io fill:#9C27B0,stroke:#6A1B9A,color:#fff
    classDef cleanup fill:#795548,stroke:#3E2723,color:#fff

    class START,EXIT_PROGRAM startEnd
    class TOKENIZE,PARSE,EXECUTE,HISTORY process
    class NULL_CHECK,EXIT_CHECK,EMPTY_CHECK,TOKEN_ERROR,PARSE_ERROR decision
    class INPUT,PRINT_EXIT_MSG,SYNTAX_ERROR io
    class CLEANUP cleanup
```

## Tokenizer Detallado
```mermaid
flowchart TD
    subgraph "tokenize()"
        TOK_START([START tokenize(input)])
        TOK_INIT[head = NULL, current = NULL]
        GEN_LIST[generate_token_list()]
        ADD_EOF[create_token(TOKEN_EOF)]
        TOK_SUCCESS([return head])
        TOK_ERROR([cleanup_tokens(head)<br/>return NULL])
    end

    subgraph "generate_token_list()"
        LOOP{input[i] != '\\0'?}
        SKIP_WS[skip whitespace]
        IS_OP{is_operator_char?}
        PROC_OP[process_operator()]
        PROC_WORD[process_word()]
        ADD_TOKEN[add_token()]
    end

    %% Flujo
    TOK_START --> TOK_INIT --> GEN_LIST
    GEN_LIST --> LOOP
    LOOP -- SÍ --> SKIP_WS --> IS_OP
    IS_OP -- SÍ --> PROC_OP
    IS_OP -- NO --> PROC_WORD
    PROC_OP --> ADD_TOKEN
    PROC_WORD --> ADD_TOKEN
    PROC_OP -- Falla --> TOK_ERROR
    PROC_WORD -- Falla --> TOK_ERROR
    ADD_TOKEN --> LOOP
    LOOP -- NO --> ADD_EOF
    ADD_EOF -- Falla --> TOK_ERROR
    ADD_EOF --> TOK_SUCCESS

    %% Estilos
    classDef process fill:#00BCD4,stroke:#00838F,color:#fff
    classDef decision fill:#FFC107,stroke:#FFA000,color:#000
    classDef startEnd fill:#8BC34A,stroke:#558B2F,color:#fff
    classDef error fill:#F44336,stroke:#C62828,color:#fff

    class TOK_START,TOK_SUCCESS startEnd
    class TOK_ERROR error
    class TOK_INIT,GEN_LIST,ADD_TOKEN,ADD_EOF,PROC_OP,PROC_WORD,SKIP_WS process
    class LOOP,IS_OP decision
```

## Parser Detallado (Descenso Recursivo)
```mermaid
flowchart TD
    subgraph "parse()"
        PAR_START([START parse(tokens)])
        PAR_INIT[init t_parser]
        CALL_PIPE[ast = parse_pipe_expression()]
        CHECK_ERR{parser.error == 1?}
        CLEANUP_AST([cleanup_ast(ast)<br/>return NULL])
        PAR_SUCCESS([return ast])
    end

    subgraph "parse_pipe_expression()"
        PIPE_START[call parse_redirect_expression()]
        PIPE_LOOP{current == TOKEN_PIPE?}
        PIPE_RIGHT[call parse_redirect_expression()]
        CREATE_PIPE[create_binary_node(PIPE)]
    end

    subgraph "parse_redirect_expression()"
        REDIR_LEAD[parse_leading_redirects()]
        REDIR_CMD[call parse_command()]
        REDIR_TRAIL[apply_trailing_redirects()]
        REDIR_LINK[link leading & trailing]
    end

    subgraph "parse_command()"
        CMD_CHECK{current == TOKEN_WORD?}
        CREATE_CMD[create_ast_node(COMMAND)]
        COLLECT_ARGS[collect_command_args()]
    end

    %% Flujo
    PAR_START --> PAR_INIT --> CALL_PIPE
    CALL_PIPE --> CHECK_ERR
    CHECK_ERR -- SÍ --> CLEANUP_AST
    CHECK_ERR -- NO --> PAR_SUCCESS

    %% Jerarquía
    CALL_PIPE --> PIPE_START
    PIPE_START --> REDIR_LEAD
    REDIR_LEAD --> REDIR_CMD
    REDIR_CMD --> CMD_CHECK
    CMD_CHECK -- SÍ --> CREATE_CMD --> COLLECT_ARGS --> REDIR_TRAIL
    REDIR_TRAIL --> REDIR_LINK --> PIPE_LOOP
    PIPE_LOOP -- SÍ --> PIPE_RIGHT --> CREATE_PIPE --> PIPE_LOOP
    PIPE_LOOP -- NO --> CALL_PIPE

    %% Estilos
    classDef process fill:#673AB7,stroke:#4527A0,color:#fff
    classDef decision fill:#FF9800,stroke:#E65100,color:#fff
    classDef startEnd fill:#4CAF50,stroke:#2E7D32,color:#fff
    classDef error fill:#F44336,stroke:#C62828,color:#fff
    classDef creation fill:#009688,stroke:#00695C,color:#fff

    class PAR_START,PAR_SUCCESS startEnd
    class PAR_INIT,CALL_PIPE process
    class CHECK_ERR,PIPE_LOOP,CMD_CHECK decision
    class CLEANUP_AST error
    class CREATE_PIPE,CREATE_CMD,creation
```

## Executor Detallado
```mermaid
flowchart TD
    subgraph "execute_ast()"
        EX_START([START execute_ast(ast)])
        EX_CHECK{ast == NULL?}
        NODE_TYPE{ast->type?}
        RETURN_STATUS([return exit_status])
    end

    subgraph "NODE_COMMAND"
        CMD_FORK[fork()]
        CMD_CHILD{pid == 0?}
        CMD_EXEC[execute_command_node()<br/>-> launch_command()<br/>-> execve()]
        CMD_WAIT[waitpid(pid, &status)]
    end

    subgraph "NODE_PIPE"
        PIPE_HDOC[preprocess_heredocs()]
        PIPE_PIPE[pipe(pipe_fd)]
        PIPE_L_CHILD[create_pipe_child(left)]
        PIPE_R_CHILD[create_pipe_child(right)]
        PIPE_CLOSE[close pipe_fd in parent]
        PIPE_WAIT[wait_pipe_children()]
    end

    subgraph "NODE_REDIRECT"
        REDIR_FORK[fork()]
        REDIR_CHILD{pid == 0?}
        REDIR_APPLY[apply_single_redirect()<br/>-> open() / execute_heredoc()<br/>-> dup2()]
        REDIR_EXEC[execute_ast(cmd_node)]
        REDIR_WAIT[waitpid(pid, &status)]
    end

    %% Flujo
    EX_START --> EX_CHECK
    EX_CHECK -- SÍ --> RETURN_STATUS
    EX_CHECK -- NO --> NODE_TYPE

    NODE_TYPE -- COMMAND --> CMD_FORK
    CMD_FORK --> CMD_CHILD
    CMD_CHILD -- SÍ --> CMD_EXEC
    CMD_CHILD -- NO --> CMD_WAIT --> RETURN_STATUS

    NODE_TYPE -- PIPE --> PIPE_HDOC --> PIPE_PIPE --> PIPE_L_CHILD --> PIPE_R_CHILD --> PIPE_CLOSE --> PIPE_WAIT --> RETURN_STATUS

    NODE_TYPE -- REDIRECT --> REDIR_FORK
    REDIR_FORK --> REDIR_CHILD
    REDIR_CHILD -- SÍ --> REDIR_APPLY --> REDIR_EXEC
    REDIR_CHILD -- NO --> REDIR_WAIT --> RETURN_STATUS

    %% Estilos
    classDef process fill:#FF5722,stroke:#D84315,color:#fff
    classDef decision fill:#FF9800,stroke:#E65100,color:#fff
    classDef startEnd fill:#4CAF50,stroke:#2E7D32,color:#fff
    classDef subprocess fill:#E91E63,stroke:#AD1457,color:#fff

    class EX_START,RETURN_STATUS startEnd
    class CMD_FORK,PIPE_PIPE,REDIR_FORK,CMD_WAIT,PIPE_WAIT,REDIR_WAIT process
    class EX_CHECK,NODE_TYPE,CMD_CHILD,REDIR_CHILD decision
    class CMD_EXEC,PIPE_HDOC,PIPE_L_CHILD,PIPE_R_CHILD,PIPE_CLOSE,REDIR_APPLY,REDIR_EXEC subprocess
```