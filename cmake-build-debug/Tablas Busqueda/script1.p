set terminal png size 1424,669
set output 'Resultados Algoritmos Busqueda.png'
set xlabel 'Muestras'
set ylabel 'Tiempo'
plot 'tabla_busqueda_recursiva.txt' with line title 'Busqueda Recursiva', 'tabla_busqueda_iterativo.txt' with line title 'Busqueda Iterativa'