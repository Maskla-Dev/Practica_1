set terminal png size 1424,669
set output 'Resultados Algoritmos Busqueda Mejor Caso.png'
set title 'Resultados Algoritmos Busqueda Mejor Caso'
set xlabel 'Muestras'
set ylabel 'Tiempo'
set key outside
plot 'tabla_mejor_bubble.txt' with line title 'Mejor Bubble Sort', 'tabla_mejor_insertion.txt' with line title 'Mejor Insertion Sort', 'tabla_mejor_merge.txt' with line title 'Mejor Merge Sort'