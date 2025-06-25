#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* input_filename = "Entradas/Odisea.txt";
    char* output_filename = "Salida.bin";
    char* key = "Hola";


    FILE *in = fopen(input_filename, "rb");
    if (!in) {
        perror("No se pudo abrir el archivo de entrada");
        return 1;
    }

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("No se pudo crear el archivo de salida");
        fclose(in);
        return 1;
    }

    int key_len = strlen(key);
    int ch, i = 0;

    while ((ch = fgetc(in)) != EOF) {
        fputc(ch ^ key[i % key_len], out);
        i++;
    }

    fclose(in);
    fclose(out);

    printf("Archivo '%s' cifrado exitosamente a '%s'\n", input_filename, output_filename);

    return 0;
}
