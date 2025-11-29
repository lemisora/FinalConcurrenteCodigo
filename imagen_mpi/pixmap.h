#ifndef PIXMAP_H
#define PIXMAP_H

// Estructura definida en el PDF 
typedef struct {
    int w;              /* numero de columnas */
    int h;              /* numero de filas */
    unsigned char *dat; /* imagen almacenada en bytes [0..255] */
    unsigned char **im; /* vector de punteros a filas (acceso bidimensional) */
} imagen;

// Prototipos de funciones usadas en imagen.c [cite: 1790, 1828]
int load_pixmap(char *filename, imagen *img);
void store_pixmap(char *filename, imagen img);

#endif