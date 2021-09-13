set terminal png size 1424,669
set output 'Resultados Algoritmos Busqueda Peor Caso.png'
set title 'Resultados Algoritmos Busqueda Peor Caso'
set xlabel 'Muestras'
set ylabel 'Tiempo'
set key outside
plot 'tabla_peor_bubble.txt' with line title 'Peor Bubble Sort', 'tabla_peor_insertion.txt' with line title 'Peor Insertion Sort', 'tabla_peor_merge.txt' with line title 'Peor Merge Sort'