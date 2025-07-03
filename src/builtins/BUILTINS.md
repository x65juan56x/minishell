¿Por qué char ***envp_ptr en lugar de char **envp?
El Problema con char **envp
Con char **envp, solo tenemos acceso de lectura al array de variables de entorno. No se puede modificar el puntero original que apunta al array.
```c
// Con char **envp - SOLO LECTURA
int function(char **envp)
{
    envp = new_array; // ❌ Solo cambia la copia local
    // El envp original en main() NO se modifica
}
```
La Solución con char ***envp_ptr
Con char ***envp_ptr, tienes acceso de escritura al puntero original, permitiendo modificar el array de variables de entorno.
```c
// Con char ***envp_ptr - LECTURA Y ESCRITURA
int function(char ***envp_ptr)
{
    *envp_ptr = new_array; // ✅ Modifica el puntero original
    // El envp en main() SI se modifica
}
```
¿Por qué es Necesario?
Los built-ins export y unset necesitan modificar permanentemente las variables de entorno de la shell:
```bash
MiniShell $ export NEW_VAR=hello
MiniShell $ echo $NEW_VAR    # Debe mostrar "hello"
MiniShell $ unset NEW_VAR
MiniShell $ echo $NEW_VAR    # Debe estar vacío
```

char *getcwd(char *buf, size_t size);

These functions return a null-terminated string containing an absolute pathname that is the current working directory of the calling process. The pathname is returned as the function result and via the argument buf, if present.

The getcwd() function copies an absolute pathname of the current working directory to the array pointed to by buf, which is of length size.

If the length of the absolute pathname of the current working directory, including the terminating null byte, exceeds size bytes, NULL is returned, and errno is set to ERANGE; an application should check for this error, and allocate a larger buffer if necessary.

As an extension to the POSIX.1-2001 standard, Linux (libc4, libc5, glibc) getcwd() allocates the buffer dynamically using malloc(3) if buf is NULL. In this case, the allocated buffer has the length size unless size is zero, when buf is allocated as big as necessary. The caller should free(3) the returned buffer.


int chdir(const char *path);

Description
The chdir() function shall cause the directory named by the pathname pointed to by the path argument to become the current working directory; that is, the starting point for path searches for pathnames not beginning with '/' .

Return Value
Upon successful completion, 0 shall be returned. Otherwise, -1 shall be returned, the current working directory shall remain unchanged, and errno shall be set to indicate the error.

The chdir() function only affects the working directory of the current process.


Las nuevas funciones en la libft ft_putstrplus_fd() y ft_putcharplus_fd() implementan la gestión de errores en caso de que falle la escritura, las usamos en builtin_env para que sea mas robusta
