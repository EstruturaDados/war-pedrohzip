#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define NUM_TERRITORIOS 5
#define MAX_STR 64

typedef struct {
    char nome[MAX_STR];
    char cor[MAX_STR];
    int tropas;
} Territorio;

/* Simula um ataque entre dois territórios (índices 0-based)
   Retorna 1 se o atacante venceu a rodada, 0 caso contrário. */
static int simulate_attack(int atk_idx, int def_idx, Territorio *mapa) {
    if (!mapa) return 0;

    /* Gerar 1 dado de ataque e 1 dado de defesa (1..6) */
    int dado_atq = (rand() % 6) + 1;
    int dado_def = (rand() % 6) + 1;

    printf("Dados sorteados -> Atacante: %d  Defesa: %d\n", dado_atq, dado_def);

    /* Empates favorecem o atacante */
    if (dado_atq >= dado_def) {
        /* atacante vence: defensor perde 1 tropa */
        mapa[def_idx].tropas -= 1;
        if (mapa[def_idx].tropas < 0) mapa[def_idx].tropas = 0;
        printf("Atacante venceu a rodada! %s perde 1 tropa.\n", mapa[def_idx].nome[0] ? mapa[def_idx].nome : "(vazio)");

        /* se defensor ficar sem tropas, atacante conquista o território */
        if (mapa[def_idx].tropas == 0) {
            /* transferir 1 tropa do atacante para o novo território (se possível) */
            if (mapa[atk_idx].tropas > 1) {
                mapa[atk_idx].tropas -= 1;
                mapa[def_idx].tropas = 1;
                /* copiar cor do exército atacante para o território conquistado */
                strncpy(mapa[def_idx].cor, mapa[atk_idx].cor, MAX_STR - 1);
                mapa[def_idx].cor[MAX_STR - 1] = '\0';
                printf("Território %s conquistado por %s!\n", mapa[def_idx].nome[0] ? mapa[def_idx].nome : "(vazio)", mapa[atk_idx].cor);
            } else {
                /* atacante não tem tropa sobrando para ocupar */
                printf("Defensor eliminado, mas atacante não tem tropas suficientes para ocupar.\n");
            }
        }

        return 1;
    } else {
        /* defensor vence: atacante perde 1 tropa */
        mapa[atk_idx].tropas -= 1;
        if (mapa[atk_idx].tropas < 0) mapa[atk_idx].tropas = 0;
        printf("Defensor venceu a rodada! %s perde 1 tropa.\n", mapa[atk_idx].nome[0] ? mapa[atk_idx].nome : "(vazio)");
        return 0;
    }
}

static void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void removerNovaLinha(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') s[len - 1] = '\0';
}

int main(void) {
    /* alocar dinamicamente o mapa com calloc */
    Territorio *mapa = calloc(NUM_TERRITORIOS, sizeof(Territorio));
    if (!mapa) {
        fprintf(stderr, "Erro ao alocar memória para o mapa.\n");
        return 1;
    }

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

    /* loop interativo para realizar ataques */
    srand((unsigned) time(NULL));

    while (1) {
        printf("\nEstado atual do mapa:\n");
        printf("%-4s %-30s %-20s %-8s\n", "No.", "Nome", "Cor", "Tropas");
        printf("---------------------------------------------------------------\n");
        for (int i = 0; i < NUM_TERRITORIOS; ++i) {
            printf("%-4d %-30s %-20s %-8d\n",
                   i + 1,
                   mapa[i].nome[0] ? mapa[i].nome : "(vazio)",
                   mapa[i].cor[0] ? mapa[i].cor : "(vazio)",
                   mapa[i].tropas);
        }

        int atk, def;
        printf("\nDigite o número do território atacante (1-%d) ou 0 para sair: ", NUM_TERRITORIOS);
        if (scanf("%d", &atk) != 1) {
            limparBufferEntrada();
            printf("Entrada inválida. Tente novamente.\n");
            continue;
        }
        if (atk == 0) break;
        if (atk < 1 || atk > NUM_TERRITORIOS) {
            printf("Atacante inválido. Escolha entre 1 e %d.\n", NUM_TERRITORIOS);
            continue;
        }

        printf("Digite o número do território defensor (1-%d): ", NUM_TERRITORIOS);
        if (scanf("%d", &def) != 1) {
            limparBufferEntrada();
            printf("Entrada inválida. Tente novamente.\n");
            continue;
        }
        if (def < 1 || def > NUM_TERRITORIOS) {
            printf("Defensor inválido. Escolha entre 1 e %d.\n", NUM_TERRITORIOS);
            continue;
        }
        if (atk == def) {
            printf("Atacante e defensor não podem ser o mesmo território.\n");
            continue;
        }

        /* índices 0-based */
        int atk_idx = atk - 1;
        int def_idx = def - 1;

        /* verificar tropas suficientes para atacar */
        if (mapa[atk_idx].tropas <= 1) {
            printf("Território atacante precisa ter pelo menos 2 tropas para atacar.\n");
            continue;
        }

        /* executar uma rodada de ataque */
        limparBufferEntrada(); /* limpar resto da linha antes de imprimir dados */
        simulate_attack(atk_idx, def_idx, mapa);

        printf("Resultado atualizado:\n");
        printf("%s: %d tropa(s)    %s: %d tropa(s)\n",
               mapa[atk_idx].nome[0] ? mapa[atk_idx].nome : "(vazio)", mapa[atk_idx].tropas,
               mapa[def_idx].nome[0] ? mapa[def_idx].nome : "(vazio)", mapa[def_idx].tropas);
    }

    free(mapa);
    printf("Encerrando.\n");
    return 0;
}