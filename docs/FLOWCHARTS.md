# Diagramas de Flujo - Minishell

## Diagrama Principal
```mermaid
flowchart TD
  subgraph "Definiciones"
    START([START MINISHELL<br/>main loop])
    INPUT[INPUT<br/>input = readline(PROMPT)<br/>ej: echo hello &#124; grep test]
    NULL_CHECK{input == NULL?}
    EMPTY_CHECK{input empty?}
    HISTORY[add_history(input)]
    EXIT_CHECK{input == "exit"?}
    TOKENIZE[tokenize()<br/>generate_token_list<br/>add EOF token]
    TOKEN_ERROR{tokens == NULL?}
    TOKENS_LIST[TOKENS LIST<br/>…→ TOKEN_EOF]
    PARSE[parse(tokens)]
    PARSE_ERROR{ast == NULL?}
    AST_TREE[AST TREE<br/>…]
    EXECUTE[execute_ast(ast, envp)]
    EXIT_STATUS[exit_status = code]
    CLEANUP[CLEANUP<br/>cleanup_ast<br/>cleanup_tokens<br/>free input]
    PRINT_NULL([PRINT NEWLINE<br/>printf "\n"<br/>break])
    EXIT_PROGRAM([EXIT PROGRAM<br/>rl_clear_history()<br/>return exit_status])
    ERROR_TOKEN([ERROR TOKENIZER<br/>return to loop])
    ERROR_PARSE([ERROR PARSER<br/>print syntax error])
  end

  START --> INPUT
  INPUT --> NULL_CHECK
  NULL_CHECK -->|YES| PRINT_NULL
  NULL_CHECK -->|NO| EMPTY_CHECK
  EMPTY_CHECK -->|YES| INPUT
  EMPTY_CHECK -->|NO| HISTORY
  HISTORY --> EXIT_CHECK
  EXIT_CHECK -->|YES| EXIT_PROGRAM
  EXIT_CHECK -->|NO| TOKENIZE

  TOKENIZE --> TOKEN_ERROR
  TOKEN_ERROR -->|YES| ERROR_TOKEN
  TOKEN_ERROR -->|NO| TOKENS_LIST
  ERROR_TOKEN --> CLEANUP

  TOKENS_LIST --> PARSE
  PARSE --> PARSE_ERROR
  PARSE_ERROR -->|YES| ERROR_PARSE
  PARSE_ERROR -->|NO| AST_TREE
  ERROR_PARSE --> CLEANUP

  AST_TREE --> EXECUTE
  EXECUTE --> EXIT_STATUS
  EXIT_STATUS --> CLEANUP
  CLEANUP --> INPUT
  PRINT_NULL --> EXIT_PROGRAM