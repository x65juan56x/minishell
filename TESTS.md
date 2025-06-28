Preparación (Ejecuta esto primero en tu terminal)
'''bash
# Crear archivos de prueba
echo "Línea 1 de input.txt" > input.txt
echo "Línea 2 de input.txt" >> input.txt
echo "Contenido inicial" > para_sobrescribir.txt
echo "Contenido inicial" > para_append.txt
touch archivo_vacio.txt
'''
1. Redirección de Salida Simple (>)  
Estos tests verifican la creación y sobrescritura de archivos.
'''bash
# --- Test 1.1: Básico ---
# Descripción: El contenido de 'ls -l' debe ir a 'listado.txt'
ls -l > listado.txt

# --- Test 1.2: Sobrescribir ---
# Descripción: 'para_sobrescribir.txt' debe contener "Nuevo contenido"
echo "Nuevo contenido" > para_sobrescribir.txt

# --- Test 1.3: Crear archivo vacío ---
# Descripción: Debe crear un archivo 'nuevo_vacio.txt' de 0 bytes
> nuevo_vacio.txt

# --- Test 1.4: Orden flexible (Redirección al principio) ---
# Descripción: 'orden1.txt' debe contener "hola mundo"
> orden1.txt echo hola mundo

# --- Test 1.5: Orden flexible (Redirección en medio) ---
# Descripción: 'orden2.txt' debe contener "hola mundo"
echo hola > orden2.txt mundo

# --- Test 1.6: Múltiples redirecciones de salida (la última gana) ---
# Descripción: 'final.txt' debe contener "contenido final". 'temp.txt' debe estar vacío.
echo "contenido final" > temp.txt > final.txt
'''
2. Redirección de Salida en Modo Append (>>)
Estos tests verifican que el contenido se añade al final del archivo.
'''bash
# --- Test 2.1: Básico ---
# Descripción: Añade "Nueva línea" a 'para_append.txt'
echo "Nueva línea" >> para_append.txt

# --- Test 2.2: Append a archivo no existente ---
# Descripción: Debe crear 'nuevo_append.txt' y escribir "contenido"
echo "contenido" >> nuevo_append.txt

# --- Test 2.3: Orden flexible (Redirección al principio) ---
# Descripción: Añade "mas contenido" a 'orden1.txt'
>> orden1.txt echo mas contenido

# --- Test 2.4: Múltiples appends (la última gana) ---
# Descripción: 'append_final.txt' debe contener "linea". 'append_temp.txt' no se crea.
echo "linea" >> append_temp.txt >> append_final.txt
'''
3. Redirección de Entrada (<)
Estos tests verifican que un comando lee desde un archivo en lugar de la entrada estándar.
'''bash
# --- Test 3.1: Básico ---
# Descripción: 'cat' debe mostrar el contenido de 'input.txt'
cat < input.txt

# --- Test 3.2: Con un comando que procesa stdin ---
# Descripción: 'wc -l' debe contar las líneas de 'input.txt' (debería ser 2)
wc -l < input.txt

# --- Test 3.3: Orden flexible (Redirección al principio) ---
# Descripción: Igual que el 3.1, pero con diferente orden
< input.txt cat

# --- Test 3.4: Error (archivo no existe) ---
# Descripción: Debe mostrar un error como "No such file or directory"
cat < archivo_que_no_existe.txt
'''
4. Here-Document (<<)
Estos tests verifican la lectura de entrada hasta un delimitador.
'''bash
# --- Test 4.1: Básico ---
# Descripción: 'cat' debe imprimir las dos líneas de texto
cat << EOF
hola
mundo
EOF

# --- Test 4.2: Con un comando que procesa stdin ---
# Descripción: 'grep' debe encontrar y mostrar la línea que contiene "linea2"
grep "linea2" << DELIMITADOR
linea1
linea2
linea3
DELIMITADOR

# --- Test 4.3: Delimitador sin saltos de línea ---
# Descripción: 'wc -c' debe contar los caracteres de "abc" (debería ser 4, incluyendo el \n)
wc -c << STOP_WORD
abc
STOP_WORD
'''
5. Combinaciones de Redirecciones
Estos tests mezclan los diferentes tipos de redirecciones.
'''bash
# --- Test 5.1: Entrada y Salida ---
# Descripción: Copia el contenido de 'input.txt' a 'output1.txt'
cat < input.txt > output1.txt

# --- Test 5.2: Orden mixto de entrada y salida ---
# Descripción: Idéntico al 5.1, pero prueba la flexibilidad del parser
> output2.txt cat < input.txt

# --- Test 5.3: Here-Document y Salida ---
# Descripción: Escribe el contenido del here-doc en 'output3.txt'
cat << EOF > output3.txt
contenido del heredoc
EOF

# --- Test 5.4: Entrada y Append ---
# Descripción: Añade el contenido de 'input.txt' al final de 'para_append.txt'
cat < input.txt >> para_append.txt

# --- Test 5.5: Múltiples redirecciones mixtas ---
# Descripción: 'wc' lee de 'input.txt', la salida va a 'count.txt' y 'final_count.txt' se sobrescribe vacío.
# El resultado final es que 'count.txt' contiene el conteo de palabras de 'input.txt'.
wc -w < input.txt > count.txt > final_count.txt
'''
6. Combinaciones con Pipes (|)
Estos tests verifican la interacción entre pipes y redirecciones.
'''bash
# --- Test 6.1: Redirección de salida después de un pipe ---
# Descripción: La salida de 'sort' (que recibe de 'ls') va a 'sorted_ls.txt'
ls | sort > sorted_ls.txt

# --- Test 6.2: Redirección de entrada antes de un pipe ---
# Descripción: 'grep' filtra el contenido de 'input.txt' y lo pasa a 'wc -l'
cat < input.txt | grep "Línea" | wc -l

# --- Test 6.3: Here-Document antes de un pipe ---
# Descripción: 'sort' ordena el contenido del here-doc y lo muestra
cat << EOF | sort
c
a
b
EOF

# --- Test 6.4: Combinación compleja ---
# Descripción: Lee de 'input.txt', filtra, ordena y guarda el resultado en 'complex.txt'
< input.txt grep "Línea" | sort -r > complex.txt
bash
