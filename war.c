#include <stdio.h>
#include <string.h>

#define NUM_TERRITORIOS 5
#define MAX_STR 64

typedef struct {
    char nome[MAX_STR];
    char cor[MAX_STR];
    int tropas;
} Territorio;

static void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void removerNovaLinha(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') s[len - 1] = '\0';
}

int main(void) {
    Territorio mapa[NUM_TERRITORIOS];

    printf("Cadastro de %d territórios\n\n", NUM_TERRITORIOS);
    for (int i = 0; i < NUM_TERRITORIOS; ++i) {
        printf("=== Território %d ===\n", i + 1);

        printf("Nome: ");
        if (fgets(mapa[i].nome, sizeof(mapa[i].nome), stdin) == NULL) {
            mapa[i].nome[0] = '\0';
        }
        removerNovaLinha(mapa[i].nome);

        printf("Cor do Exército dominante: ");
        if (fgets(mapa[i].cor, sizeof(mapa[i].cor), stdin) == NULL) {
            mapa[i].cor[0] = '\0';
        }
        removerNovaLinha(mapa[i].cor);

        printf("Número de Tropas: ");
        while (scanf("%d", &mapa[i].tropas) != 1) {
            limparBufferEntrada();
            printf("Entrada inválida. Digite um número inteiro para tropas: ");
        }
        limparBufferEntrada(); /* remove o '\n' deixado pelo scanf */

        printf("\n");
    }

    printf("Estado atual do mapa:\n");
    printf("%-4s %-30s %-20s %-8s\n", "No.", "Nome", "Cor", "Tropas");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < NUM_TERRITORIOS; ++i) {
        printf("%-4d %-30s %-20s %-8d\n",
               i + 1,
               mapa[i].nome[0] ? mapa[i].nome : "(vazio)",
               mapa[i].cor[0] ? mapa[i].cor : "(vazio)",
               mapa[i].tropas);
    }

    return 0;
}