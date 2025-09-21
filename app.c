#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Necessário para strtol

#define IMAGE_WIDTH 740
#define IMAGE_HEIGHT 1109

// As funções readPGM, negativeImage e filterImage não mudam.
// ... (cole as funções readPGM, negativeImage e filterImage aqui) ...

unsigned char* readPGM(const char *filename, int *width, int *height) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erro ao abrir arquivo de entrada");
        return NULL;
    }
    char header[64];
    if (!fgets(header, sizeof(header), file) || strncmp(header, "P5", 2) != 0) {
        fprintf(stderr, "Erro: O arquivo não é um PGM binário (P5).\n");
        fclose(file);
        return NULL;
    }
    do {
        if (!fgets(header, sizeof(header), file)) {
            perror("Erro lendo cabeçalho");
            fclose(file);
            return NULL;
        }
    } while (header[0] == '#');
    sscanf(header, "%d %d", width, height);
    if (!fgets(header, sizeof(header), file)) {
         perror("Erro lendo cabeçalho");
        fclose(file);
        return NULL;
    }
    unsigned char *pixels = malloc((*width) * (*height));
    if (!pixels) {
        perror("Erro alocando memória");
        fclose(file);
        return NULL;
    }
    fread(pixels, 1, (*width) * (*height), file);
    fclose(file);
    return pixels;
}

void negativeImage(unsigned char *imageData, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        imageData[i] = 255 - imageData[i];
    }
}

void filterImage(unsigned char *imageData, int width, int height, int L) {
    for (int i = 0; i < width * height; i++) {
        imageData[i] = (imageData[i] < 128) ? 0 : L;
    }
}


// A função main foi completamente reescrita para usar argc e argv
int main(int argc, char *argv[]) {

    // 1. Validação dos argumentos de entrada
    if (argc != 4) {
        fprintf(stderr, "Uso incorreto!\n");
        fprintf(stderr, "Formato: %s <imagem_entrada.pgm> <imagem_saida.pgm> <valor_L>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // 2. Atribuição dos argumentos a variáveis
    const char *inputFilename = argv[1];
    const char *outputFilename = argv[2];
    
    // 3. Conversão e validação do valor L
    char *endptr;
    long l_long = strtol(argv[3], &endptr, 10);

    // Verifica se a conversão foi bem-sucedida e se o valor está no intervalo
    if (*endptr != '\0' || l_long < 0 || l_long > 255) {
        fprintf(stderr, "Erro: O valor de L deve ser um número inteiro entre 0 e 255.\n");
        return EXIT_FAILURE;
    }
    int L = (int)l_long;

    // 4. Lógica principal do programa (praticamente inalterada)
    int width, height;
    unsigned char *imageData = readPGM(inputFilename, &width, &height);
    if (!imageData) {
        // A função readPGM já imprime o erro específico.
        return EXIT_FAILURE;
    }
    printf("Imagem '%s' carregada com sucesso (%dx%d).\n", inputFilename, width, height);

    filterImage(imageData, width, height, L);

    FILE *outputFile = fopen(outputFilename, "wb");
    if (!outputFile) {
        perror("Erro ao criar arquivo de saída");
        free(imageData);
        return EXIT_FAILURE;
    }

    fprintf(outputFile, "P5\n%d %d\n255\n", width, height);
    fwrite(imageData, 1, width * height, outputFile);

    fclose(outputFile);
    free(imageData);

    printf("Imagem processada e salva como '%s' com L=%d.\n", outputFilename, L);

    return EXIT_SUCCESS;
}