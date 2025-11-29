/**********************************************************************************
 Fichero: imagen.c
 Se aplica a una imagen en formato PGM (P5: binario) el siguiente tratamiento
 para resaltar los contornos de la imagen:
 -- una mascara de laplace de 3 x 3
 -- unos umbrales maximo y minimo
 El proceso se repite NUM_ITER (2) veces.
 De la imagen final se calcula un histograma por filas y otro por columnas,
 para contar el numero de pixeles distintos de 0 (negro).
 Ficheros de entrada: xx.pgm (imagen en formato pgm)
 Ficheros de salida: xx_imagconproy.pgm imagen final e histogramas
 xx_lp1 / xx_lp2 ... imagenes parciales de cada iteracion
 xx_proy_hori.pgm histograma por filas
 xx_proy_vert.pgm histograma por columnas

 Compilar el programa junto con pixmap.c
**********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "pixmap.h"
#define NEGRO 0
#define BLANCO 255
#define MAX_VAL 256 /* Rango de posibles valores de grises */
#define NUM_ITER 2 /* Numero de repeticiones del proceso de la imagen */
#define TAM_PROY 150 /* Los histogramas son de 150 * h/w pixeles */
struct timeval t0,t1;
double tej;
/* FUNCIONES DE GESTION DE IMAGENES */
/* 1: Copia la imagen iimagen en oimagen. Reserva memoria */
/*********************************************************************************/
void copiar_imagen (imagen *oimagen, imagen iimagen) {
    int i, h, w;
    h = iimagen.h;
    w = iimagen.w;
    if ((oimagen->dat = (unsigned char *) malloc(w*h * sizeof(unsigned char))) == NULL)
    {
    fprintf(stderr, "Funcion copiar_imagen: malloc error\n");
    exit(1);
    }
    oimagen->h = h;
    oimagen->w = w;
    oimagen->im = (unsigned char **) malloc(h * sizeof(unsigned char *));
    for (i=0; i<h; i++) oimagen->im[i] = &oimagen->dat[i*w];
    memcpy(oimagen->dat, iimagen.dat, h*w);
    return;
}

/* 2: Crea una imagen y la inicializa a un valor constante */
/***********************************************************************************/
void generar_imagen(imagen *oimagen, int h, int w, unsigned char val) {
    int i, j;
    oimagen->h = h;
    oimagen->w = w;
    // asignar memoria a la imagen
    if ((oimagen->dat = (unsigned char *) malloc(w*h * sizeof(unsigned char))) == NULL) {
        fprintf(stderr, "Funcion generar_imagen: malloc error\n");
        exit(1);
    }
    // crear la estructura de punteros a las filas
    if ((oimagen->im = (unsigned char **) malloc(h * sizeof(unsigned char *))) == NULL) {
        fprintf(stderr, "Funcion generar_imagen: malloc error\n");
        exit(1);
    }
    for(i=0; i<h; i++) oimagen->im[i] = &oimagen->dat[i*w];
    // inicializar los pixeles de la imagen
    memset(oimagen->dat, val, h*w);
    return;
}
/* 3: Libera memoria de una imagen */
/***********************************************************************************/
void liberar_imagen(imagen iimagen) {
    free(iimagen.dat);
    free(iimagen.im);
    return;
}

/* FUNCIONES DE TRATAMIENTO DE IMAGENES */
/*4: Calcula un vector con la proyeccion vertical (reserva memoria) */
/***********************************************************************************/
void calcular_proyeccion_vertical(int **proy, imagen iimagen) {
    int i, j, h, w;

    h = iimagen.h;
    w = iimagen.w;
    // Reservar memoria para el vector proyeccion
    if ( (*proy = (int *) malloc(w * sizeof(int))) == NULL ) {
        fprintf(stderr, "Funcion calcular_proyeccion_vertical: malloc error\n");
        exit(1);
    }

    for (j=0; j<w; j++) (*proy)[j] = 0;

    for (i=0; i<h; i++)
        for (j=0; j<w; j++)
            if (iimagen.im[i][j] != NEGRO) (*proy)[j]++;
    return;
}
/* 5: Crea una imagen con la proyeccion vertical (reserva memoria) */
/***********************************************************************************/
void crear_imagen_proyeccion_vertical(imagen *proyimagen,int *proy, int longi) {
    int maximo, valor;
    int i, j;
    // Crea una imagen para la proyeccion vertical, toda en negro
    generar_imagen(proyimagen, TAM_PROY, longi, NEGRO);
    // calcula el valor maximo para escalar la imagen del histo a 150 pixeles
    maximo = -1;
    for (i=0; i<longi; i++) if(proy[i] > maximo) maximo = proy[i];

    for (j=0; j<longi; j++) {
        if (maximo == 0) valor = 0; // por si max fuera 0
        else valor = proy[j] * TAM_PROY/maximo; // escalar al valor maximo
        // deja una linea en negro, de tamaino proporcional al valor correspondiente

        for (i=0; i < TAM_PROY-valor; i++) proyimagen->im[i][j] = BLANCO;
    }
    return;
}
/* 6: Calcula un vector con la proyeccion horizontal (reserva memoria) */
/***********************************************************************************/
void calcular_proyeccion_horizontal(int **proy, imagen iimagen) {
    int i, j, h, w;
    h = iimagen.h;
    w = iimagen.w;
    // Reservar memoria para el vector proyeccion
    if ( (*proy = (int *) malloc(h * sizeof(int))) == NULL ) {
        fprintf(stderr, "Funcion calcular_proyeccion_horizontal: malloc error\n");
        exit(1);
    }
    for (i=0; i<h; i++) (*proy)[i] = 0;

    for (i=0; i<h; i++)
        for (j=0; j<w; j++)
            if (iimagen.im[i][j] != NEGRO) (*proy)[i]++;
    return;
}

/* 7: Crea una imagen con la proyec. horizontal (reserva memoria) */
/***********************************************************************************/
void crear_imagen_proyeccion_horizontal(imagen *proyimagen,int *proy, int longi) {
    int maximo, valor;
    int i, j;
    // Crea una imagen para la proyeccion vertical, toda en negro
    generar_imagen(proyimagen, longi, TAM_PROY, NEGRO);
    // calcula el valor maximo para escalar la imagen del histo a 150 pixeles
    maximo = -1;
    for (i=0; i<longi; i++) if(proy[i] > maximo) maximo = proy[i];

    for (i=0; i<longi; i++) {
        if (maximo == 0) valor = 0; // por si max fuera 0
        else valor = proy[i]* TAM_PROY/maximo; // escalar al valor maximo
        // deja una linea en negro, de tamaino proporcional al valor correspondiente
        for (j=0; j < TAM_PROY-valor; j++) proyimagen->im[i][j] = BLANCO;
    }
    return;
}
/* 8: Crea una unica imagen con iimagen y sus dos proyecciones */
/***********************************************************************************/
void crear_imagen_con_proyecciones(imagen *oimagen, imagen iimagen, imagen proyvert,
    imagen proyhori) {
    int i, j, w, h;
    h = oimagen->h = iimagen.h + proyvert.h;
    w = oimagen->w = iimagen.w + proyhori.w;
    // Crea la imagen total, inicializada a negro
    generar_imagen(oimagen, h, w, NEGRO);
    // Copia iimagen
    for (i=0; i<iimagen.h; i++)
        for (j=0; j<iimagen.w; j++)
            oimagen->im[i][j] = iimagen.im[i][j];

    // copia a la derecha la proyeccion horizontal
    for (i=0; i<proyhori.h; i++)
        for (j=0; j<proyhori.w; j++)
            oimagen->im[i][iimagen.w + j] = proyhori.im[i][j];

    // copia debajo la proyeccion vertical
    for (i=0; i<proyvert.h; i++)
        for (j=0; j<proyvert.w; j++)
            oimagen->im[i + iimagen.h][j] = proyvert.im[i][j];
    //printf ("\nCreada imagen con proyecciones vertical y horizontal\n");
    return;
}

/* 9: Filtro de laplace (3x3): detecta ejes (iimagen -> oimagen) */
/***********************************************************************************/
void aplicar_laplace_contorno(imagen *oimagen, imagen iimagen) {
    int suma;
    int i, j, k, l, h, w;
    int mask_laplace[3][3];
    // Mascara de laplace
    mask_laplace[0][0] = -1; mask_laplace[0][1] = -1; mask_laplace[0][2] = -1;
    mask_laplace[1][0] = -1; mask_laplace[1][1] = 8; mask_laplace[1][2] = -1;
    mask_laplace[2][0] = -1; mask_laplace[2][1] = -1; mask_laplace[2][2] = -1;
    h = iimagen.h;
    w = iimagen.w;
    generar_imagen(oimagen, h, w, NEGRO);
    // Aplicar mascara y dejar resultado en oimagen
    for (i=0; i<=h-1; i++)
        for (j=0; j<=w-1; j++) {
            if (i==0 || i==h-1) suma = 0; // los bordes de la imagen se dejan en negro (0)
            else if(j==0 || j==w-1) suma = 0;
            else {
                suma = 0;
                for (k=-1; k<=1; k++)
                    for (l=-1; l<=1; l++)
                        suma = suma + ((int)iimagen.im[i+k][j+l] * mask_laplace[k+1][l+1]);
            }
            if (suma<0) suma = 0;
            if (suma>255) suma = 255;
                (oimagen->im)[i][j] = (unsigned char)suma;
        }
    //printf ("\nAplicado laplace_contorno\t\t(w: %d h: %d)\n",w,h);
    return;
}
/* 10: Aplica umbrales minimo y maximo a la imagen */
/***********************************************************************************/
void aplicar_umbrales(imagen *oimagen) {
    int i, j, h, w;
    // Valores umbral: por debajo o por encima, se convierten al minimo o al maximo
    unsigned char umbral_min = 40;
    unsigned char valor_min = 0;
    unsigned char umbral_max = 215;
    unsigned char valor_max = 255;
    h = oimagen->h;
    w = oimagen->w;
    for (i=0; i<=h-1; i++)
        for (j=0; j<=w-1; j++) {
            if (oimagen->im[i][j] <= umbral_min) oimagen->im[i][j] = valor_min;
            else if (oimagen->im[i][j] >= umbral_max) oimagen->im[i][j] = valor_max;
        }
    return;
}

/************************* MAIN **********************/
/***********************************************************************************/
int main(int argc, char **argv) {
    int i, sum=0;
    char *name;
    name = malloc (sizeof(char)*100);
    // Imagenes de salida de cada iteracion; 0 = entrada
    imagen lpimagen[NUM_ITER+1];
    // Vectores de proyecciones e imagenes correspondientes
    int *proy_vert, *proy_hori;
    imagen proyimagevert, proyimagehori, imagenconproyecciones;
    if (argc != 2) {
        printf ("\nUSO: programa imagen\n");
        printf (" [extension .pgm implicita]\n");
        exit (0);
    }
    // Lectura de la imagen de entrada: solo imagenes graylevel en formato .pgm
    strcpy(name, argv[1]);
    strcat(name,".pgm");
    if ( load_pixmap(name, &lpimagen[0]) == 0 ) {
        printf ("\nError en lectura del fichero de entrada: %s\n\n",name);
        exit (0);
    }
    printf("\n --> Procesando imagen de hxw = %dx%d pix.\n", lpimagen[0].h, lpimagen[0].w);

    gettimeofday(&t0, 0);
    /* Proceso imagen: NUM_ITER veces (laplace + umbral) */
    /*********************************************************************************/
    for (i=1; i<=NUM_ITER; i++) {
        aplicar_laplace_contorno(&lpimagen[i], lpimagen[i-1]);
        aplicar_umbrales(&lpimagen[i]);
    }

    /* Calculo de las proyecciones de la imagen final */
    /*********************************************************************************/
    calcular_proyeccion_vertical(&proy_vert, lpimagen[NUM_ITER]);
    calcular_proyeccion_horizontal(&proy_hori, lpimagen[NUM_ITER]);

    /* test de prueba y toma de de tiempos */
    /*********************************************************************************/
    gettimeofday(&t1, 0);
    // test de prueba: sumar el valor de todos los pixeles de la imagen final, modulo 255
    for (i=0; i<lpimagen[NUM_ITER].h * lpimagen[NUM_ITER].w; i++)
        sum = (sum + lpimagen[NUM_ITER].dat[i]) % 255;
    printf("\n Test de prueba sum = %d \n", sum);
    tej = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;
    printf("\n Tej. serie = %1.1f ms\n\n", tej*1000);
    /* Escritura de resultados en disco; liberar memoria */
    /*********************************************************************************/
    // Guardar imagenes de salida lpimagen[i], proyecciones e imagen final conjunta
    for (i=1; i<=NUM_ITER; i++) {
        sprintf(name, "%s_lp%d.pgm", argv[1], i);
        store_pixmap(name, lpimagen[i]);
    }
    strcpy(name, argv[1]);
    name = strcat(name, "_proy_vert.pgm");
    crear_imagen_proyeccion_vertical(&proyimagevert, proy_vert, lpimagen[NUM_ITER].w);
    store_pixmap(name, proyimagevert);
    strcpy(name, argv[1]);
    name = strcat(name, "_proy_hori.pgm");
    crear_imagen_proyeccion_horizontal(&proyimagehori, proy_hori, lpimagen[NUM_ITER].h);
    store_pixmap(name, proyimagehori);
    strcpy(name, argv[1]);
    name=strcat(name, "_imagconproy.pgm");
    crear_imagen_con_proyecciones(&imagenconproyecciones, lpimagen[NUM_ITER],
    proyimagevert, proyimagehori);
    store_pixmap(name, imagenconproyecciones);
    // Liberar memoria de las imagenes y proyecciones
    for (i=0; i<=NUM_ITER; i++) liberar_imagen(lpimagen[i]);
    liberar_imagen(imagenconproyecciones);
    liberar_imagen(proyimagevert);
    liberar_imagen(proyimagehori);
    free(proy_hori);
    free(proy_vert);
    free(name);
    return 0;
}
