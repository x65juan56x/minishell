# Minishell Flowcharts

These flowcharts reflect the minishell logic from input to execution, including tokenization, expansion, parsing/AST, heredoc preprocessing, and execution (commands, pipelines, logical operators), as well as redirections and child status translation.

---

## 1) Main REPL Loop

```mermaid
flowchart TD
    A[Start minishell] --> B[Init context, env copy, signals]
    B --> C{readline #40;prompt#41;}
    C -->|NULL &#40;EOF&#41;| Z[print &quot;exit&quot;; return last status]
    C --> D{empty/space only?}
    D -->|Yes| C
    D -->|No| E{Needs multiline?}
    E -->|Yes| F[read continuation lines]
    F -->|Canceled/EOF| C
    E -->|No| G[tokenize #40;line#41;]
    G --> H{tokenization ok?}
    H -->|No| C
    H -->|Yes| I[expand #40;tokens#41;]
    I --> J[parse #40;tokens#41;-&gt;AST]
    J --> K{parse ok and at EOF?}
    K -->|No| C
    K -->|Yes| L[preprocess heredocs #40;AST#41;]
    L --> M{ok?}
    M -->|No #40error/SIGINT#41| C
    M -->|Yes| N[status = execute #40;AST#41;]
    N --> O[ctx.#36;? = status]
    O --> C
```

---

## 2) Tokenizer

```mermaid
flowchart TD
    S[Start tokenization] --> T[skip spaces]
    T --> U{end of input?}
    U -->|Yes| V[append EOF token; done]
    U -->|No| W{operator char or paren?}
    W -->|Yes| X[read operator token &#40#124;, #124;#124;, &amp;&amp;, &lt;, &gt;, &gt;&gt;, &lt;&lt;, &#40, &#41&#41]
    W -->|No| Y[read word token respecting quotes]
    X --> Z[append token]
    Y --> Z
    Z --> T
```

Notes:
- Quotes are preserved in WORD tokens for later expansion decisions.

---

## 3) Expansion

```mermaid
flowchart TD
    A[Start expansion] --> B[for each token]
    B --> C{token is WORD?}
    C -->|No| B
    C -->|Yes| D[analyze quote context]
    D --> E{parameter expansion allowed?}
    E -->|Yes| F[expand $VAR, $&#123VAR&#125, $?, $$]
    E -->|No| G[keep literal]
    F --> H{globbing allowed and contains *?}
    G --> H
    H -->|Yes| I[glob files; replace with multiple tokens if matches]
    H -->|No| J[keep as single WORD]
    I --> K[next token]
    J --> K
    K -->|more?| B
    K -->|done| L[Finish]
```

---

## 4) Parser (Recursive Descent)

```mermaid
flowchart TD
    A[parse&#40;tokens&#41;] --> B[parse_logical_expr]
    B --> C{next is &amp;&amp; or #124;#124;?}
    C -->|Yes| D[build AND/OR node; parse next pipeline]
    C -->|No| E[return pipeline]
    D --> C

    subgraph Pipeline
        P1[parse_pipeline] --> P2[parse_command]
    P2 --> P3{next is #124; ?}
        P3 -->|Yes| P4[build PIPE node; parse next command]
        P3 -->|No| P5[return command]
        P4 --> P3
    end

    subgraph Command
        C1[start simple command]
        C1 --> C2{next token}
        C2 -->|WORD| C3[append arg]
    C2 -->|REDIR| C4[parse redirect: &lt;, &gt;, &gt;&gt;, &lt;&lt; + WORD]
    C2 -->| | C5[parse parenthesis -&gt; SUBSHELL]
        C2 -->|other| C6[stop]
        C3 --> C2
        C4 --> C2
        C5 --> C2
    end

    E --> F{at EOF?}
    F -->|No| G[error: leftover tokens; cleanup and fail]
    F -->|Yes| H[return AST]
```

---

## 5) Heredoc Preprocessing

```mermaid
sequenceDiagram
    participant Shell
    participant HD as Heredoc-Child
    Note over Shell: Traverse AST and for each heredoc delimiter
    Shell->>HD: fork
    HD->>HD: set heredoc-specific signal handlers
    loop Read until delimiter
        HD->>HD: read line
        HD->>HD: expand if allowed
        HD->>HD: append to temp file
    end
    HD-->>Shell: exit 0 (ok) or 130 (SIGINT)
    alt child ok
        Shell->>Shell: replace heredoc with temp-file input redirection
    else error or SIGINT
        Shell->>Shell: abort preprocessing and set status
    end
```

---

## 6) Execution — Node Dispatcher

```mermaid
flowchart TD
    A[execute #40;node#41;] --> B{type}
    B -->|LOGICAL_AND| C[run left; if 0 then run right]
    B -->|LOGICAL_OR| D[run left; if !=0 then run right]
    B -->|PIPELINE| E[execute pipeline]
    B -->|COMMAND| F[execute simple command]
    B -->|SUBSHELL| G[run subtree in child]
    C --> H[return status]
    D --> H
    E --> H
    F --> H
    G --> H
```

### 6.a) Execute Simple Command

```mermaid
flowchart TD
    A[collect redirects + argv] --> B{argv empty?}
    B -->|Yes| C[apply redirects; restore; return]
    B -->|No| D{is builtin?}
    D -->|Yes &amp; standalone| E[backup stdio; apply redirects; run builtin in parent; restore]
    D -->|Yes &amp; in pipe/subshell| F[fork child -&gt; apply redirects -&gt; run builtin -&gt; exit]
    D -->|No &#40external&#41| G[fork child -&gt; apply redirects -&gt; execve]
    E --> H[status]
    F --> I[waitpid -> status]
    G --> I
    H --> J[return status]
    I --> J
```

### 6.b) Execute Pipeline

```mermaid
flowchart TD
    A[count commands] --> B[create N-1 pipes]
    B --> C[for i in 0..N-1 fork child]
    C --> D{child i}
    D -->|i&gt;0| E[dup prev.read -&gt; STDIN]
    D -->|i&lt;N-1| F[dup curr.write -&gt; STDOUT]
    E --> G[close all pipe fds]
    F --> G
    G --> H[exec node i]
    C --> I[parent closes all pipes]
    I --> J[wait for all children]
    J --> K[derive status from last child]
```

---

## 7) Redirections Apply

```mermaid
flowchart TD
    A[for each redirection] --> B{type}
    B -->|&lt;| C[open read-only; dup2 -&gt; STDIN]
    B -->|&gt;| D[open O_TRUNC; dup2 -&gt; STDOUT]
    B -->|&gt;&gt;| E[open O_APPEND; dup2 -&gt; STDOUT]
    B -->|&lt;&lt;| F[handled during heredoc preprocessing]
    C --> G[close fd]
    D --> G
    E --> G
    F --> G
    G --> H[next or done]
```

---

## 8) Child Status Translation and Signals

```mermaid
flowchart TD
    A[waitpid #40;status#41;] --> B{WIFEXITED?}
    B -->|Yes| C[rc = WEXITSTATUS]
    B -->|No| D{WIFSIGNALED?}
    D -->|Yes| E[sig = WTERMSIG; maybe print; rc=128+sig]
    D -->|No| F[rc = 1]
    C --> G[return rc]
    E --> G
    F --> G
```

---

These diagrams match the behavior described in `MINISHELL_FLOW.md` and `CONCEPTUAL_GUIDE.md`.