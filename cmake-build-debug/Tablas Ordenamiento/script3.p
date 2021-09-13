set terminal png size 1424,669
set output 'Resultados Algoritmos Busqueda Caso Promedio.png'
set title 'Resultados Algoritmos Busqueda Caso Promedio'
set xlabel 'Muestras'
set ylabel 'Tiempo'
set key outside
plot 'tabla_promedio_bubble.txt' with line title 'Promedio Bubble Sort', 'tabla_promedio_insertion.txt' with line title 'Promedio Insertion Sort', 'tabla_promedio_merge.txt' with line title 'Promedio Merge Sort'