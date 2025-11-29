# Códigos del tercer examen parcial de Programación Concurrente

Este repositorio contiene implementaciones de varios algoritmos paralelos utilizando C y la biblioteca OpenMPI.

## Prerrequisitos

Para compilar y ejecutar los códigos de este repositorio, es necesario tener instalado un entorno de OpenMPI en su sistema.

## Compilación y Ejecución de los Programas

A continuación se detallan las instrucciones para compilar y ejecutar cada uno de los programas.

---

### 1. Anillo (`Anillo.c`)

Este programa implementa un algoritmo de comunicación en anillo.

**Compilación:**
Para compilar el programa, utilice el siguiente comando. Se incluye la biblioteca matemática (`-lm`).

```bash
mpicc Anillo.c -o anillo -lm
```

**Ejecución:**
Para ejecutar el programa, utilice `mpirun`. Reemplace `<numero_de_procesos>` con la cantidad de procesos que desea utilizar.

```bash
mpirun -np <numero_de_procesos> ./anillo
```

---

### 2. Pipeline (`Pipeline.c`)

Este programa implementa un patrón de pipeline para procesamiento paralelo.

**Compilación:**
Utilice el siguiente comando para compilar el programa:

```bash
mpicc Pipeline.c -o pipeline
```

**Ejecución:**
Para ejecutar el programa, utilice `mpirun`. Reemplace `<numero_de_procesos>` con la cantidad de procesos que desea utilizar.

```bash
mpirun -np <numero_de_procesos> ./pipeline
```

---

### 3. Cálculo Integral (`calculo_integral.c`)

Este programa calcula una integral de forma paralela.

**Compilación:**
Para compilar el programa, utilice el siguiente comando. Se incluye la biblioteca matemática (`-lm`).

```bash
mpicc calculo_integral.c -o integral -lm
```

**Ejecución:**
Para ejecutar el programa, utilice `mpirun`. Reemplace `<numero_de_procesos>` con la cantidad de procesos que desea utilizar.

```bash
mpirun -np <numero_de_procesos> ./integral
```

---

### 4. Procesamiento de Imagen (`imagen_mpi/`)

Este programa realiza un procesamiento de imágenes en paralelo. El código fuente y sus dependencias se encuentran dentro del directorio `imagen_mpi`.

**Compilación:**
Primero, navegue al directorio `imagen_mpi`. Luego, compile el código fuente.

```bash
cd imagen_mpi
mpicc imagen.c pixmap.c -o imagen_mpi
```

**Ejecución:**
El programa base utiliza una imagen de la subcarpeta `images` y guarda las imágenes procesadas en el mismo lugar. Para ejecutarlo, utilice el siguiente comando desde el directorio `imagen_mpi`:

```bash
mpirun -np <numero_de_procesos> ./imagen_mpi <nombre_de_imagen_sin_extension>
```
