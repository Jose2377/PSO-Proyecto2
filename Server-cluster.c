#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Diccionario para las palabras
typedef struct {
    char palabra[100]; // Palabra
    int contador;      // Cantidad de apariciones

} PalabraContador;

// Arrays a usar
PalabraContador array_envio[1000];
PalabraContador array_recep[1000];

int main(int argc, char** argv) {
    MPI_Init(NULL,NULL);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // ID del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Total de procesos
    
    // MPI_Datatype
    MPI_Datatype mpi_palabra_contador;

    // Configuracion del MPI_Datatype
    int lengths[2] = {100, 1};  // 100 chars y 1 int
    MPI_Aint displacements[2];
    MPI_Datatype types[2] = {MPI_CHAR, MPI_INT};

    // Diccionario temporal como constructor
    PalabraContador temp;

    // Direcciones del MPI_Datatype 
    MPI_Aint base_address;
    MPI_Get_address(&temp, &base_address);
    MPI_Get_address(&temp.palabra, &displacements[0]);
    MPI_Get_address(&temp.contador, &displacements[1]);
    displacements[0] = displacements[0] - base_address;
    displacements[1] = displacements[1] - base_address + 8;

    // Creacion del datatype
    MPI_Type_create_struct(2, lengths, displacements, types, &mpi_palabra_contador);
    MPI_Type_commit(&mpi_palabra_contador);
    
    if (rank == 0){
        // Nodo servidor
        printf("Servidor en proceso %d\n", rank);
      
        FILE *inputFile, *outputFile;
        char inputFileName[] = "Salida.bin";
        char outputFileName[] = "Divisiones/Encriptacion.bin";
        char byte;
        int lineas;
        size_t bytes_leidos;

        inputFile = fopen(inputFileName, "rb");
        outputFile = fopen(outputFileName, "wb");
        if (inputFile == NULL) {
            perror("Error al abrir el archivo de entrada");
            return 1;
        }

        while ((bytes_leidos = fread(&byte, 1, 1, inputFile)) == 1) {
            fputs(&byte, outputFile);
            lineas++;
        }
        
        fclose(inputFile);
        printf("Bytes: %d.\n", lineas);
        
        for (int i=1; i<4; i++){
            MPI_Send(&lineas,1,MPI_INT,i,0,MPI_COMM_WORLD);
            MPI_Send(&lineas,1,MPI_INT,i,0,MPI_COMM_WORLD);
            MPI_Send(&lineas,1,MPI_INT,i,0,MPI_COMM_WORLD);
        }
        
        //int amount;
        //MPI_Recv(&amount,1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        //MPI_Recv(array_recep,amount, mpi_palabra_contador, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //for (int i=0;i<amount;i++){
        //    printf("Mensaje %s, valor %d\n", array_recep[i].palabra, array_recep[i].contador);
        //}
        printf("Finalizado\n");
           
    } else{
        // Nodo cluster
        char DirectionInput[100];
        sprintf(DirectionInput, "Salida.bin");
        char DirectionOutput[100];
        sprintf(DirectionOutput, "Divisiones/Desencriptada%d.txt", rank);
        
        char* key = "Hola";
        FILE *in = fopen(DirectionInput, "rb");
        FILE *out = fopen(DirectionOutput, "wb");
        
        int key_len = strlen(key);
        int i, j = 0;
        char byte;
        
        int amount;
        MPI_Recv(&amount,1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        for (int k = 0; (k < (amount/3 + 1)*(rank-1));k++){
            fread(&byte, 1, 1, in);
            i++;
        }
        for (int k = 0; k < amount/3 + 1;k++){
            fread(&byte, 1, 1, in);
            fputc(byte ^ key[i % key_len],out);
            i++;
        }
        
        fclose(in);
        fclose(out);
        
        //-------------------------------------------------------------------------------------------
        
        PalabraContador palabras[10000];
        FILE *archivo = fopen(DirectionOutput, "r");
        if (archivo == NULL) {
            printf("No se pudo abrir el archivo.\n");
        }

        char texto[10000];
        size_t length = fread(texto, 1, 10000 - 1, archivo);
        texto[length] = '\0';  // Aseguramos cadena terminada en '\0'
        
        int total_palabras = 0;
        char *token = strtok(texto, " \n\t,.!?;:-");

        while (token != NULL) {
            int encontrada = 0;
            for (int i = 0; i < total_palabras; i++) {
                if (strcmp(palabras[i].palabra, token) == 0) {
                    palabras[i].contador++;
                    encontrada = 1;
                    break;
                }
            }
            if (!encontrada && total_palabras < 10000) {
                strcpy(palabras[total_palabras].palabra, token);
                palabras[total_palabras].contador = 1;
                total_palabras++;
            }
            token = strtok(NULL, " \n\t,.!?;:-");
        }

        if (total_palabras == 0) {
            printf("No se encontraron palabras en el archivo.\n");
        }

        int max_index = 0;
        for (int i = 1; i < total_palabras; i++) {
            if (palabras[i].contador > palabras[max_index].contador) {
                max_index = i;
            }
        }

        printf("En el nodo numero: %d , la palabra que más aparece es '%s' con %d apariciones.\n",
               rank, palabras[max_index].palabra, palabras[max_index].contador);
               
        printf("En el nodo numero: %d , la cantidad de palabras totales son: %d.\n",
               rank, total_palabras);
        
        for (int i=0;i<total_palabras;i++){
          printf("Nodo %d: %s, valor %d\n", rank, palabras[i].palabra, palabras[i].contador);
        }
        
        //MPI_Send(&total_palabras,1,MPI_INT,0,0,MPI_COMM_WORLD);
        //MPI_Send(palabras,total_palabras,mpi_palabra_contador,0,0,MPI_COMM_WORLD);
        
        fclose(archivo);
    }
    
    MPI_Finalize();
    return 0;
}
