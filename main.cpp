#include <iostream>
#include <chrono>
#include <array>
#include <vector>
#include <fstream>
#include <string>

const int MAX_MUESTRA_BUSQUEDA = 5000;
const int MAX_MUESTRA_ORDENAMIENTO = MAX_MUESTRA_BUSQUEDA * 2;

//Indices de apoyo, mejora la lectura de acceso y evita confusiones
#define INDICE_CASO_PROMEDIO 0
#define INDICE_MEJOR_CASO 1
#define INDICE_PEOR_CASO 2

#define INDICE_INSERTION_SORT INDICE_CASO_PROMEDIO
#define INDICE_BUBBLE_SORT INDICE_MEJOR_CASO
#define INDICE_MERGE_SORT INDICE_PEOR_CASO

//Apoyos formato .tex
#define INICIO_TABLA_TEX "\\begin{table}[!ht]\n\\centering\n\\begin{tabular}"
#define FIN_TABLA_TEX "\\end{tabular}\n\\caption{}\n\\end{table}"
#define TEXTO_NEGRITAS_TEX "\\textbf"
#define SALTO_LINEA_TEX "\\\\"
#define LINEA_HORIZONTAL_TEX "\\hline\n"

//Renombramiento de los objetos, hacen la nomenclatura mas sencilla
using micro_segundos = std::chrono::duration<double,std::micro>;
using mili_segundos = std::chrono::duration<double,std::milli>;
using Renglon_Tiempo_micras = std::vector<micro_segundos>;
using Renglon_Tiempo_milis = std::vector<mili_segundos>;
using Arreglo = std::array<int, 5000>;
using Tiempo_Casos = std::array<Renglon_Tiempo_milis, 3>;
using Numeros_Ordenables = std::vector<int>;
using Reloj = std::chrono::system_clock;

//Estructura de las tablas
typedef struct tabla_resultados_busqueda{
    Renglon_Tiempo_micras tiempo_ejecucion_recursivo;
    Renglon_Tiempo_micras tiempo_ejecucion_iterativo;
    std::vector<int> muestra;
}Tabla_Resultados_Busqueda;

typedef struct tabla_resultados_ordenamiento{
    std::array<Tiempo_Casos, 3> algoritmos; //Arreglo dos dimensiones, primer dimension indice algoritmos, segunda dimension indice casos. El contenido es un arreglo dinamico
    std::vector<int> muestra;
} Tabla_Resultados_Ordenamiento;

//Prototipos de las funciones
void imprimirTablaOrdenamiento(Tabla_Resultados_Ordenamiento&);
void registrarTiempos(Tabla_Resultados_Ordenamiento&, Numeros_Ordenables&, int, int); //Registro tiempos segun algoritmo y caso
void pruebaOrdenamiento(int, Tabla_Resultados_Ordenamiento&, Numeros_Ordenables&, int); //Inicia pruebas para los algoritmos de ordenamiento para el maximo numero de muestras
void probarAlgoritmosOrdenamiento(Tabla_Resultados_Ordenamiento&, Numeros_Ordenables&, int); //Inicia prueba algoritmos de ordenamiento
void inicializarLista(Numeros_Ordenables&, int); //Inicializa lista vector dinamico
void pruebaBusqueda(int, Tabla_Resultados_Busqueda&, Arreglo&, int); //Inicia pruebas para algoritmos de busqueda
void imprimirResultadosBusqueda(Tabla_Resultados_Busqueda &); //Imprime en pantalla los resultado de la primera prueba
void probarAlgoritmosBusqueda(Tabla_Resultados_Busqueda&, Arreglo&, int); //Inicia prueba para algoritmo 1 y Algoritmo 2
int random(int, int);   //Funcion, obtiene retorna un numero aleatorio dependiendo el rango indicado
int busquedaAlgoritmoIterativo(Arreglo&, int); //Busqueda, algoritmo 1 iterativo
int busquedaAlgoritmoRecursivo(Arreglo&, int, int); //Busqueda, algoritmo 2 recursivo
void insertionSort(Numeros_Ordenables&); //Metodo insertion sort
void bubbleSort(Numeros_Ordenables&); //Metodo bubble sort
void unirArreglos(Numeros_Ordenables&, int, int, int); //"Une" dos arreglos
void mergeSort(Numeros_Ordenables&, int, int); //Metodo merge sort
void crearTablaLATEX(Tabla_Resultados_Ordenamiento& tabla2, Tabla_Resultados_Busqueda& tabla1); //Crea tabla en formato LATEX de los resultados, guarda en archivo
void crearTablaGNUPLOT(Tabla_Resultados_Ordenamiento& tabla2, Tabla_Resultados_Busqueda& tabla1); //Crea tabla con formato que GNUPLOT pueda graficar de los resultados, guarda en archivo

//Recursos apoyo para formato LATEX
std::string definirFilasTablaLATEX(int);
std::string definirMultiColumnasTablaLATEX(int, std::string&&);
std::string crearTextoNegritasLATEX(std::string&& src);

int main() {
    Tabla_Resultados_Busqueda tabla_resultados1; //Tabla perteneciente a los algoritmos de busqueda
    Tabla_Resultados_Ordenamiento tabla_resultados2; //Tabla perteneciente a los algoritmos de ordenamiento

    int incremento_muestra = 200;
    int tam_muestra_inicial = 200;

    Arreglo nums{}; //Arreglo que contiene numeros aleatorios
    Numeros_Ordenables lista;   //Arreglo que contiene numeros aleatorios

    for(int & num : nums)   //Inicializacion arreglo nums
        num = random(0, MAX_MUESTRA_ORDENAMIENTO);

    //Inicia pruebas algoritmos de busqueda
    pruebaBusqueda(tam_muestra_inicial, tabla_resultados1, nums, incremento_muestra);
    imprimirResultadosBusqueda(tabla_resultados1);

    //Inicio pruebas algoritmos ordenamiento
    tam_muestra_inicial = 1000;
    incremento_muestra = 1000;
    pruebaOrdenamiento(tam_muestra_inicial, tabla_resultados2, lista, incremento_muestra);
    imprimirTablaOrdenamiento(tabla_resultados2);

    //Creacion archivos de resultados
    crearTablaLATEX(tabla_resultados2, tabla_resultados1);
    crearTablaGNUPLOT(tabla_resultados2, tabla_resultados1);
    return 0;
}

void crearTablaLATEX(Tabla_Resultados_Ordenamiento& tabla2, Tabla_Resultados_Busqueda& tabla1){
    std::ofstream tabla_ordenamiento("Tablas LATEX/tabla_ordenamiento.tex");
    std::ofstream tabla_busqueda("Tablas LATEX/tabla_busqueda.tex");
    std::string contenido_tabla = "";
    //Creación Tabla 1
    contenido_tabla.append(INICIO_TABLA_TEX);
    contenido_tabla.append(definirFilasTablaLATEX(tabla1.muestra.size() + 1));
    contenido_tabla.append(LINEA_HORIZONTAL_TEX);
    contenido_tabla.append(crearTextoNegritasLATEX("n") + "&" + crearTextoNegritasLATEX("Máximo Iterativo") + "&" +
                                   crearTextoNegritasLATEX("Máximo Recursivo") + SALTO_LINEA_TEX +LINEA_HORIZONTAL_TEX);
    for(int i = 0; i < tabla1.muestra.size(); ++i)
        contenido_tabla.append(std::to_string(
                tabla1.muestra[i]) +
                "&" + std::to_string(tabla1.tiempo_ejecucion_iterativo[i].count()) +
                "&" + std::to_string(tabla1.tiempo_ejecucion_recursivo[i].count()) +
                SALTO_LINEA_TEX + LINEA_HORIZONTAL_TEX);
    contenido_tabla.append(FIN_TABLA_TEX);
    tabla_busqueda << contenido_tabla;
    tabla_busqueda.close();
    contenido_tabla.clear();
    //Creacion Tabla 2
    contenido_tabla.append(INICIO_TABLA_TEX);
    contenido_tabla.append(definirFilasTablaLATEX(tabla2.muestra.size() + 2));
    contenido_tabla.append(LINEA_HORIZONTAL_TEX);
    contenido_tabla.append(" &" + definirMultiColumnasTablaLATEX(3, crearTextoNegritasLATEX("Caso Promedio")) + "&" +
                                   definirMultiColumnasTablaLATEX(3, crearTextoNegritasLATEX("Mejor Caso")) + "&" +
                                   definirMultiColumnasTablaLATEX(3, crearTextoNegritasLATEX("Peor Caso")) +
                                   SALTO_LINEA_TEX + LINEA_HORIZONTAL_TEX);
    contenido_tabla.append(crearTextoNegritasLATEX("n"));
    for(int i = 0; i < 3; ++i)
        contenido_tabla.append("&" + crearTextoNegritasLATEX("Insertion") + "&" + crearTextoNegritasLATEX("Bubble") + "&" +
                                       crearTextoNegritasLATEX("Merge"));
    contenido_tabla.append(SALTO_LINEA_TEX);
    contenido_tabla.append(LINEA_HORIZONTAL_TEX);
    for(int i = 0; i < tabla2.muestra.size(); ++i){
        contenido_tabla.append(std::to_string(tabla2.muestra[i]));
        for(int j = 0; j < 3; ++j)
            for(int k = 0; k < 3; ++k)
                contenido_tabla.append("&" + std::to_string(tabla2.algoritmos[k][j][i].count()));
        contenido_tabla.append(SALTO_LINEA_TEX);
        contenido_tabla.append(LINEA_HORIZONTAL_TEX);
    }
    contenido_tabla.append(FIN_TABLA_TEX);
    tabla_ordenamiento << contenido_tabla;
    tabla_ordenamiento.close();
}

void crearTablaGNUPLOT(Tabla_Resultados_Ordenamiento& tabla2, Tabla_Resultados_Busqueda& tabla1){
    std::array<std::ofstream, 9> archivos_ordenamiento{
            std::ofstream("Tablas Ordenamiento/tabla_promedio_insertion.txt"),
            std::ofstream("Tablas Ordenamiento/tabla_mejor_insertion.txt"),
            std::ofstream("Tablas Ordenamiento/tabla_peor_insertion.txt"),
            std::ofstream("Tablas Ordenamiento/tabla_promedio_bubble.txt"),
            std::ofstream("Tablas Ordenamiento/tabla_mejor_bubble.txt"),
            std::ofstream("Tablas Ordenamiento/tabla_peor_bubble.txt"),
            std::ofstream("Tablas Ordenamiento/tabla_promedio_merge.txt"),
            std::ofstream("Tablas Ordenamiento/tabla_mejor_merge.txt"),
            std::ofstream("Tablas Ordenamiento/tabla_peor_merge.txt")
    };
    std::array<std::ofstream, 2> archivos_busqueda{
            std::ofstream("Tablas Busqueda/tabla_busqueda_iterativo.txt"),
            std::ofstream("Tablas Busqueda/tabla_busqueda_recursiva.txt")
    };

    std::string contenido_tabla = "";
    //Tablas busqueda
    for(int j = 0; j < tabla1.muestra.size(); ++j)
        contenido_tabla.append(std::to_string(tabla1.muestra[j]) + "\t" + std::to_string(tabla1.tiempo_ejecucion_iterativo[j].count()) + "\n");
    archivos_busqueda[0] << contenido_tabla;
    contenido_tabla.clear();
    archivos_busqueda[0].close();
    for(int j = 0; j < tabla1.muestra.size(); ++j)
        contenido_tabla.append(std::to_string(tabla1.muestra[j]) + "\t" + std::to_string(tabla1.tiempo_ejecucion_recursivo[j].count()) + "\n");
    archivos_busqueda[1] << contenido_tabla;
    contenido_tabla.clear();
    archivos_busqueda[1].close();
    //Tablas ordenamiento
    for(int i = 0, j = 0; i < archivos_ordenamiento.size(); ++i){
        j += (i == 3 || i == 6) ? 1 : 0;
        for(int k = 0; k < tabla2.muestra.size(); ++k)
            contenido_tabla.append(std::to_string(tabla2.muestra[k]) + "\t" + std::to_string(tabla2.algoritmos[j][i % 3][k].count()) + "\n");
        archivos_ordenamiento[i] << contenido_tabla;
        contenido_tabla.clear();
        archivos_ordenamiento[i].close();
    }
}

std::string definirFilasTablaLATEX(int filas){
    std::string texto;
    texto.append("{|");
    for(int i = 0; i < filas; ++i){
        texto.append("l|");
    }
    texto.append("}");
    return texto;
}

std::string definirMultiColumnasTablaLATEX(int columnas, std::string&& src){
    std::string texto;
    texto.append("\\multicolumn{");
    texto.append(std::to_string(columnas));
    texto.append("}{c|}");
    texto.append("{"+src+"}");
    return texto;
}

std::string crearTextoNegritasLATEX(std::string&& src){
    std::string texto;
    texto.append(TEXTO_NEGRITAS_TEX);
    texto.append("{"+src+"}");
    return texto;
}

void imprimirTablaOrdenamiento(Tabla_Resultados_Ordenamiento& tabla){
    std::cout << "\tInsertion Sort\t\t\tBubble Sort\t\t\tMergesort" << std::endl;
    std::cout << "n\t";
    for(int i = 0; i < 3; ++i)
        std::cout << "=\t+\t-\t\t";
    std::cout << std::endl;
    for(int i = 0; i < tabla.muestra.size(); ++i){
        std::cout << tabla.muestra[i] << "\t";
        for(int j = 0; j < tabla.algoritmos.size(); ++j){
            std::cout << tabla.algoritmos[j][0][i].count() << "\t" << tabla.algoritmos[j][1][i].count() << "\t" << tabla.algoritmos[j][2][i].count();
            std::cout << "\t\t";
        }
        std::cout << std::endl;
    }
}

void pruebaOrdenamiento(int tam_muestra, Tabla_Resultados_Ordenamiento & tabla, Numeros_Ordenables& lista, int incremento_muestra){
    while(tam_muestra <= MAX_MUESTRA_ORDENAMIENTO){
        probarAlgoritmosOrdenamiento(tabla, lista, tam_muestra);
        tam_muestra += incremento_muestra;
    }
}

void probarAlgoritmosOrdenamiento(Tabla_Resultados_Ordenamiento& tabla, Numeros_Ordenables& lista, int tam_muestra){
    //Caso promedio insertion sort
    tabla.muestra.push_back(tam_muestra);
    //Inicia evaluacion algoritmo insertion sort, termina con merge sort
    for(int indice_algoritmo = INDICE_INSERTION_SORT; indice_algoritmo <= INDICE_MERGE_SORT ; ++indice_algoritmo){
        //Limpia y vuelve a llenar la lista con nuevos elementos
        inicializarLista(lista, tam_muestra);
        //Primera evaluacion caso promedio, termina evaluando mejor caso
        registrarTiempos(tabla, lista, indice_algoritmo, INDICE_CASO_PROMEDIO);
        registrarTiempos(tabla, lista, indice_algoritmo, INDICE_MEJOR_CASO);
        //Invierte el listado de numeros
        std::reverse(lista.begin(), lista.end());
        registrarTiempos(tabla, lista, indice_algoritmo, INDICE_PEOR_CASO);
    }
}

void registrarTiempos(Tabla_Resultados_Ordenamiento& tabla, Numeros_Ordenables& lista, int algoritmo, int caso){
    std::chrono::system_clock::time_point inicio, final;
    switch(algoritmo){
        case INDICE_INSERTION_SORT:
            inicio = Reloj::now();
            insertionSort(lista);
            break;
        case INDICE_BUBBLE_SORT:
            inicio = Reloj::now();
            bubbleSort(lista);
            break;
        case INDICE_MERGE_SORT:
            inicio = Reloj::now();
            mergeSort(lista, 0, lista.size() - 1);
            break;
    }
    final = Reloj::now();
    tabla.algoritmos[algoritmo][caso].push_back(final - inicio);
}

void inicializarLista(Numeros_Ordenables& lista, int tam){
    if(!lista.empty())
        lista.clear();
    for(int i = 0; i < tam; ++i)
        lista.push_back(random(0, MAX_MUESTRA_ORDENAMIENTO));
}

void pruebaBusqueda(int tam_muestra_inicial, Tabla_Resultados_Busqueda& tabla_resultados1, Arreglo& nums, int incremento_muestra){
    while(tam_muestra_inicial <= MAX_MUESTRA_BUSQUEDA){
        probarAlgoritmosBusqueda(tabla_resultados1, nums, tam_muestra_inicial);
        tam_muestra_inicial += incremento_muestra;
    }
}

void imprimirResultadosBusqueda(Tabla_Resultados_Busqueda &tabla){
    std::cout << "Muestra\t\t\tIterativo\t\tRecursivo" << std::endl;
    for(int i = 0; i < tabla.muestra.size(); ++i)
        std::cout << tabla.muestra[i] << "\t\t\t" << tabla.tiempo_ejecucion_iterativo[i].count() << "\t\t\t" << tabla.tiempo_ejecucion_recursivo[i].count() << std::endl;
}

void probarAlgoritmosBusqueda(Tabla_Resultados_Busqueda& tabla, Arreglo& arreglo, int tam_muestra){
    Reloj::time_point inicio, final;
    tabla.muestra.push_back(tam_muestra);
    //Prueba iterativa
    inicio = Reloj::now();
    busquedaAlgoritmoIterativo(arreglo, tam_muestra);
    final = Reloj::now();
    tabla.tiempo_ejecucion_iterativo.push_back(final - inicio);
    //Prueba recursiva
    inicio = Reloj::now();
    busquedaAlgoritmoRecursivo(arreglo, 0, tam_muestra - 1);
    final = Reloj::now();
    tabla.tiempo_ejecucion_recursivo.push_back(final - inicio);
}

int random(const int min, const int max){
    return ((rand() % (max - min + 1)) + min);
}

int busquedaAlgoritmoIterativo(Arreglo& arreglo, const int tam_muestra) {
    int max = arreglo[0];
    for(int i = 1; i < tam_muestra; ++i){
        if(arreglo[i] > max)
            max = arreglo[i];
    }
    return max;
}

int busquedaAlgoritmoRecursivo(Arreglo& arreglo, int base, int tope){
    int mitad = 0, izquierda = 0, derecha = 0;
    if(base == tope)
        return arreglo[base];
    else{
        mitad = (tope + base) / 2;
        izquierda = busquedaAlgoritmoRecursivo(arreglo, base, mitad);
        derecha = busquedaAlgoritmoRecursivo(arreglo, mitad + 1, tope);
        return (izquierda > derecha) ? izquierda : derecha;
    }
}

void insertionSort(Numeros_Ordenables& arreglo){
    for(int i = 1; i < arreglo.size(); ++i)
        for(int j = i - 1; j >= 1 && arreglo[j] > arreglo[j + 1]; --j)
            std::swap(arreglo[j], arreglo[j+1]);
}

void bubbleSort(Numeros_Ordenables& arreglo){
    for(int i = 0; i < arreglo.size() - 1; ++i)
        for(int j = 0; j < arreglo.size() - i - 1; ++j)
            if(arreglo[j] > arreglo[j + 1])
                std::swap(arreglo[j], arreglo[j + 1]);
}

void unirArreglos(Numeros_Ordenables& arreglo, int inicio, int mitad, int fin){
    int tam_subarreglo1 = mitad - inicio + 1;
    int tam_subarreglo2 = fin - mitad;
    int indice_subarreglo1 = 0;
    int indice_subarreglo2 = 0;
    int indice_arreglounido = inicio;

    Numeros_Ordenables tmp1, tmp2, arreglo_unido;

    for(int i = 0; i < tam_subarreglo1; ++i)
        tmp1.push_back(arreglo[inicio + i]);
    for(int i = 0; i < tam_subarreglo2; ++i)
        tmp2.push_back(arreglo[mitad + 1 + i]);

    while(indice_subarreglo1 < tam_subarreglo1 && indice_subarreglo2 < tam_subarreglo2){
        if(tmp1[indice_subarreglo1] <= tmp2[indice_subarreglo2]){
            arreglo[indice_arreglounido] = tmp1[indice_subarreglo1];
            ++indice_subarreglo1;
        }
        else{
            arreglo[indice_arreglounido] = tmp2[indice_subarreglo2];
            ++indice_subarreglo2;
        }
        ++indice_arreglounido;
    }
    while(indice_subarreglo1 < tam_subarreglo1){
        arreglo[indice_arreglounido] = tmp1[indice_subarreglo1];
        ++indice_subarreglo1;
        ++indice_arreglounido;
    }
    while(indice_subarreglo2 < tam_subarreglo2){
        arreglo[indice_arreglounido] = tmp2[indice_subarreglo2];
        ++indice_subarreglo2;
        ++indice_arreglounido;
    }
}

void mergeSort(Numeros_Ordenables& arreglo, int inicio, int fin){
    int mitad = 0;
    if(inicio < fin){
        mitad = std::floor((inicio + fin) / 2);
        mergeSort(arreglo, inicio, mitad);
        mergeSort(arreglo, mitad + 1, fin);
        unirArreglos(arreglo, inicio, mitad, fin);
    }
}