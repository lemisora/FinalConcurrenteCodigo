#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pixmap.h"

// Función para saltar comentarios en cabeceras PGM
void skip_comments(FILE *fp) {
    int ch;
    char line[1024];
    while ((ch = fgetc(fp)) != EOF && isspace(ch));
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        skip_comments(fp);
    } else {
        fseek(fp, -1, SEEK_CUR);
    }
}

int load_pixmap(char *filename, imagen *img) {
    FILE *fp;
    char buf[16];
    int c, maxval;

    if ((fp = fopen(filename, "rb")) == NULL) return 0;

    // Leer número mágico "P5"
    if (!fgets(buf, sizeof(buf), fp)) { fclose(fp); return 0; }
    if (buf[0] != 'P' || buf[1] != '5') { 
        fprintf(stderr, "Error: No es un formato PGM P5 valido\n");
        fclose(fp); 
        return 0; 
    }

    skip_comments(fp);
    if (fscanf(fp, "%d %d", &img->w, &img->h) != 2) { fclose(fp); return 0; }
    
    skip_comments(fp);
    if (fscanf(fp, "%d", &maxval) != 1) { fclose(fp); return 0; }
    
    // Consumir el único carácter de espacio en blanco tras el maxval
    fgetc(fp); 

    // Asignar memoria (Lógica basada en source 1545)
    img->dat = (unsigned char *) malloc(img->w * img->h);
    img->im = (unsigned char **) malloc(img->h * sizeof(unsigned char *));
    
    for (int i = 0; i < img->h; i++) 
        img->im[i] = &img->dat[i * img->w];

    // Leer datos binarios
    if (fread(img->dat, 1, img->w * img->h, fp) != img->w * img->h) {
        fprintf(stderr, "Error leyendo datos de imagen\n");
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1; // Éxito
}

void store_pixmap(char *filename, imagen img) {
    FILE *fp;
    if ((fp = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "Error abriendo fichero para escritura\n");
        return;
    }

    // Escribir cabecera PGM P5
    fprintf(fp, "P5\n%d %d\n255\n", img.w, img.h);
    
    // Escribir datos binarios
    fwrite(img.dat, 1, img.w * img.h, fp);
    
    fclose(fp);
}