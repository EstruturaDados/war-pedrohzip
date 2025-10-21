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

typedef enum {
    DESTRUIR_EXERCITO_VERDE,
    CONQUISTAR_TRES_TERRITORIOS
} TipoMissao;

typedef struct {
    TipoMissao tipo;
    const char* descricao;
} Missao;

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

static void inicializarTerritorios(Territorio *mapa) {
    const char *nomes[] = {"Brasil", "Argentina", "Peru", "Colombia", "Chile"};
    const char *cores[] = {"Vermelho", "Verde", "Azul", "Amarelo", "Verde"};
    const int tropas[] = {3, 2, 4, 2, 3};

    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        strncpy(mapa[i].nome, nomes[i], MAX_STR - 1);
        strncpy(mapa[i].cor, cores[i], MAX_STR - 1);
        mapa[i].tropas = tropas[i];
        mapa[i].nome[MAX_STR - 1] = '\0';
        mapa[i].cor[MAX_STR - 1] = '\0';
    }
}

static void mostrarMapa(const Territorio *mapa) {
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
}

static int verificarMissao(const Territorio *mapa, const Missao missao) {
    if (missao.tipo == DESTRUIR_EXERCITO_VERDE) {
        int exercito_verde_encontrado = 0;
        for (int i = 0; i < NUM_TERRITORIOS; i++) {
            if (strcmp(mapa[i].cor, "Verde") == 0 && mapa[i].tropas > 0) {
                exercito_verde_encontrado = 1;
                break;
            }
        }
        return !exercito_verde_encontrado;
    } else if (missao.tipo == CONQUISTAR_TRES_TERRITORIOS) {
        const char *cor_jogador = mapa[0].cor; // Assumimos que o jogador começa com o primeiro território
        int territorios_conquistados = 0;
        for (int i = 0; i < NUM_TERRITORIOS; i++) {
            if (strcmp(mapa[i].cor, cor_jogador) == 0) {
                territorios_conquistados++;
            }
        }
        return territorios_conquistados >= 3;
    }
    return 0;
}

static void realizarAtaque(Territorio *mapa) {
    mostrarMapa(mapa);

    int atk, def;
    printf("\nDigite o número do território atacante (1-%d): ", NUM_TERRITORIOS);
    if (scanf("%d", &atk) != 1) {
        limparBufferEntrada();
        printf("Entrada inválida. Tente novamente.\n");
        return;
    }
    
    if (atk < 1 || atk > NUM_TERRITORIOS) {
        printf("Atacante inválido. Escolha entre 1 e %d.\n", NUM_TERRITORIOS);
        return;
    }

    printf("Digite o número do território defensor (1-%d): ", NUM_TERRITORIOS);
    if (scanf("%d", &def) != 1) {
        limparBufferEntrada();
        printf("Entrada inválida. Tente novamente.\n");
        return;
    }

    if (def < 1 || def > NUM_TERRITORIOS) {
        printf("Defensor inválido. Escolha entre 1 e %d.\n", NUM_TERRITORIOS);
        return;
    }

    if (atk == def) {
        printf("Atacante e defensor não podem ser o mesmo território.\n");
        return;
    }

    const int atk_idx = atk - 1;
    const int def_idx = def - 1;

    if (mapa[atk_idx].tropas <= 1) {
        printf("Território atacante precisa ter pelo menos 2 tropas para atacar.\n");
        return;
    }

    limparBufferEntrada();
    simulate_attack(atk_idx, def_idx, mapa);

    printf("Resultado atualizado:\n");
    printf("%s: %d tropa(s)    %s: %d tropa(s)\n",
           mapa[atk_idx].nome, mapa[atk_idx].tropas,
           mapa[def_idx].nome, mapa[def_idx].tropas);
}

static void mostrarMenu(void) {
    printf("\nMenu Principal:\n");
    printf("1 - Atacar\n");
    printf("2 - Verificar Missão\n");
    printf("0 - Sair\n");
    printf("Escolha uma opção: ");
}

int main(void) {
    Territorio *mapa = calloc(NUM_TERRITORIOS, sizeof(Territorio));
    if (!mapa) {
        fprintf(stderr, "Erro ao alocar memória para o mapa.\n");
        return 1;
    }

    srand((unsigned)time(NULL));
    
    // Inicialização automática dos territórios
    inicializarTerritorios(mapa);
    
    // Definição aleatória da missão
    const Missao missoes[] = {
        {DESTRUIR_EXERCITO_VERDE, "Destruir o exército Verde"},
        {CONQUISTAR_TRES_TERRITORIOS, "Conquistar 3 territórios"}
    };
    const Missao missao_atual = missoes[rand() % 2];
    
    printf("Sua missão: %s\n", missao_atual.descricao);

    int opcao;
    do {
        mostrarMenu();
        
        if (scanf("%d", &opcao) != 1) {
            limparBufferEntrada();
            printf("Opção inválida!\n");
            continue;
        }
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                realizarAtaque(mapa);
                if (verificarMissao(mapa, missao_atual)) {
                    printf("\nParabéns! Você completou sua missão: %s\n", missao_atual.descricao);
                    opcao = 0; // Encerra o jogo
                }
                break;
            
            case 2:
                printf("\nSua missão atual: %s\n", missao_atual.descricao);
                if (verificarMissao(mapa, missao_atual)) {
                    printf("Status: COMPLETA!\n");
                } else {
                    printf("Status: Em andamento...\n");
                }
                break;
            
            case 0:
                printf("Encerrando o jogo...\n");
                break;
            
            default:
                printf("Opção inválida!\n");
                break;
        }
    } while (opcao != 0);

    free(mapa);
    return 0;
}