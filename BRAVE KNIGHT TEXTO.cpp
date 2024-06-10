#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<locale.h>
#include<time.h>
#include<string.h>
#include<math.h>
#include<windows.h>

#define MAX_MOEDAS 300
#define MAX_BAUS_POR_SALA 1
#define MIN_RECOMPENSA_OURO 5
#define MAX_RECOMPENSA_OURO 20
#define MAX_ITENS 10

// CORES NPC
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_PURPLE "\x1b[35m"
#define ANSI_COLOR_RESET "\x1b[0m"

int salaAtual = 1;
int vidaPersonagem = 100;
int moedas = 0;
int bausAbertosNaSala = 0;
int monstrosDerrotados = 0;
int totalOuroColetado = 0;
int totalOuroMonstros = 0;

// INVENTÁRIO
typedef struct {
    char nome[20];
    int quantidade;
} Item;

//Item inventario[MAX_ITENS];


//JOGADOR 
typedef struct {
    int vidaAtual;
    int vidaMaxima;
    //int moedas;
    Item inventario[MAX_ITENS];
} Jogador;

Jogador jogador = {100, 100, {{"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}}};



// ESPADAS DO JOGO
int espadaBronzeDano = 10;
int espadaFerroDano = 11;
int espadaOuroDano = 15;
int espadaRubiDano = 23;
int espadaAcoNegroDano = 40;
int espadaEquipadaDano = espadaBronzeDano;

int tamanhoTela = 30;
int jogadorX = 15;
int jogadorY = 15;

int bauX, bauY;
int vendedorX, vendedorY;
bool vendedorPresente;

int portaX, portaY;

// MONSTROS
typedef struct {
    char nome[20];
    int vida;
    int dano;
    int posX;
    int posY;
    bool ativo;
} Monstro;

Monstro zumbi = {"Zumbi Arrastado", 35, 8, 0, 0, true};
Monstro zumbi2 = {"Zumbi Guerreiro", 40, 11, 0, 0, true};
Monstro zumbi3 = {"Zumbi Devorador", 50, 14, 0, 0, true};
Monstro esqueleto = {"Esqueleto", 60, 18, 0, 0, true};
Monstro observador = {"Observador", 80, 22, 0, 0, true};
Monstro dragao = {"Dragão", 200, 30, 0, 0, false};
Monstro* monstros[6] = {&zumbi, &zumbi2, &zumbi3, &esqueleto, &observador, &dragao};

int monstroDerrotado = 0;
int totalSalasPassadas = 0;
int totalMonstrosDerrotados = 0;
int totalBausAbertos = 0;

typedef struct {
    int x;
    int y;
    bool presente;
} NPC;

NPC npcAzul = {0, 0, false};
NPC npcRoxo = {0, 0, false};
NPC npcCharada = {0, 0, false};

bool interagiuAzul = false;
bool interagiuRoxo = false;
bool interagiuCharada = false;

void iniciarJogo();
void sairJogo();
void entrarSala();
void interagirElementos();
void movimentarSala();
void abrirBau();
void interagirVendedor();
void verificarCombate();
void combate(Monstro* monstro);
void usarPocao(Jogador *jogador);
void inicializarInventario(Jogador *jogador);
void verificarInventario();
void usarItem(int index);
void verificarVendedor();
void limparBufferEntrada();
void verificarMortePersonagem();

void interagirNPCs();
void interagirNPCAzul();
void interagirNPCRoxo();
void interagirNPCCharada();

int posicaoInicialX;
int posicaoInicialY;

void inicializarPosicoes() {
    srand(time(0));
    bauX = 1 + rand() % (tamanhoTela - 2);
    bauY = 1 + rand() % (tamanhoTela - 2);
    
    vendedorPresente = (rand() % 100 < 60);
    if (vendedorPresente) {
        vendedorX = 1 + rand() % (tamanhoTela - 2);
        vendedorY = 1 + rand() % (tamanhoTela - 2);
    }
    
    if (rand() % 100 < 30) {
        npcAzul.x = 1 + rand() % (tamanhoTela - 2);
        npcAzul.y = 1 + rand() % (tamanhoTela - 2);
        npcAzul.presente = true;
    } else {
        npcAzul.presente = false;
    }
    
     if (rand() % 100 < 30) {
        npcRoxo.x = 1 + rand() % (tamanhoTela - 2);
        npcRoxo.y = 1 + rand() % (tamanhoTela - 2);
        npcRoxo.presente = true;
    } else {
        npcRoxo.presente = false;
    }
    
     if (rand() % 100 < 25) {
        npcCharada.x = 1 + rand() % (tamanhoTela - 2);
        npcCharada.y = 1 + rand() % (tamanhoTela - 2);
        npcCharada.presente = true;
    } else {
        npcCharada.presente = false;
    }
}

void inicializarPosicoesMonstros() {
    for (int i = 0; i < 5; i++) {
        monstros[i]->posX = 1 + rand() % (tamanhoTela - 2);
        monstros[i]->posY = 1 + rand() % (tamanhoTela - 2);
    }
    if (salaAtual >= 12) {
        dragao.ativo = true;
        // Define a posição do dragão (no meio do mapa)
        dragao.posX = 1 + rand() %  tamanhoTela / 2;
        dragao.posY = 1 + rand() %  tamanhoTela / 2;
    }
    
}

void reinicializarMonstros() {
    zumbi.vida = 35;
    zumbi.ativo = true;
    zumbi2.vida = 40;
    zumbi2.ativo = true;
    zumbi3.vida = 50;
    zumbi3.ativo = true;
    esqueleto.vida = 40;
    esqueleto.ativo = true;
    observador.vida = 70;
    observador.ativo = true;
    
    inicializarPosicoesMonstros();
}

void iniciarMapa() {
    char matrizTela[tamanhoTela][tamanhoTela];
    
    char npcAzulChar = 'A'; // Letra para representar NPC azul
    char npcRoxoChar = 'R'; // Letra para representar NPC roxo
    char npcCharadaChar = 'C'; // Letra para representar NPC de charada
    
    // Verificações para determinar se os NPCs estão presentes e suas posições
    int npcAzulX = -1, npcAzulY = -1;
    int npcRoxoX = -1, npcRoxoY = -1;
    int npcCharadaX = -1, npcCharadaY = -1;
    
    if (npcAzul.presente) {
        npcAzulX = npcAzul.x;
        npcAzulY = npcAzul.y;
    }
    
    if (npcRoxo.presente) {
        npcRoxoX = npcRoxo.x;
        npcRoxoY = npcRoxo.y;
    }
    
    if (npcCharada.presente) {
        npcCharadaX = npcCharada.x;
        npcCharadaY = npcCharada.y;
    }
    
   
    // MEXER AQUI 
     
    printf("Sala %d\n", salaAtual);
    //printf("Vida do personagem: %d\n", vidaPersonagem);
    printf("Vida atual: %d / Vida máxima: %d\n", jogador.vidaAtual, jogador.vidaMaxima);
    printf("Moedas: %d\n", moedas);
    printf("Espada equipada: ");
    switch (espadaEquipadaDano) {
        case 10:
            printf("Espada de bronze (10 de dano)\n");
            break;
        case 11:
            printf("Espada de ferro (11 de dano)\n");
            break;
        case 15:
            printf("Espada de ouro (15 de dano)\n");
            break;
        case 23:
            printf("Espada de rubi (23 de dano)\n");
            break;
        case 40:
            printf("Espada de aço negro (40 de dano)\n");
            break;
        default:
            printf("Desconhecida\n");
    }

    for (int i = 0; i < tamanhoTela; i++) {
        for (int j = 0; j < tamanhoTela; j++) {
            matrizTela[i][j] = ' ';
            if (j == 29 || j == 0) {
                matrizTela[i][j] = '|';
            }
            if (i == 29 || i == 0) {
                matrizTela[i][j] = '-';
            }            
            
            for (int k = 0; k < 6; k++) {
                if (monstros[k]->ativo && i == monstros[k]->posX && j == monstros[k]->posY) {
                    matrizTela[i][j] = monstros[k]->nome[0];
                }
            }
            
            if (vendedorPresente && i == vendedorX && j == vendedorY) {
                matrizTela[i][j] = 'V';
            }
            if (i == bauX && j == bauY) {
                matrizTela[i][j] = 'B';
            }
        }
    }
    if (npcAzulX != -1 && npcAzulY != -1) {
        matrizTela[npcAzulX][npcAzulY] = npcAzulChar;
    }
    if (npcRoxoX != -1 && npcRoxoY != -1) {
        matrizTela[npcRoxoX][npcRoxoY] = npcRoxoChar;
    }
    if (npcCharadaX != -1 && npcCharadaY != -1) {
        matrizTela[npcCharadaX][npcCharadaY] = npcCharadaChar;
    }
    

    for (int i = 0; i < tamanhoTela; i++) {
        for (int j = 0; j < tamanhoTela; j++) {
            if (i == jogadorX && j == jogadorY) {
                // Imprimir o jogador "J" em verde
                printf("\033[32mJ\033[0m");
             } else if (matrizTela[i][j] == npcAzulChar) {
                // Imprimir NPC azul em azul
                printf("\033[34m%c\033[0m", npcAzulChar);
            } else if (matrizTela[i][j] == npcRoxoChar) {
                // Imprimir NPC roxo em roxo
                printf("\033[35m%c\033[0m", npcRoxoChar);
            } else if (matrizTela[i][j] == npcCharadaChar) {
                // Imprimir NPC de charada em amarelo
                printf("\033[33m%c\033[0m", npcCharadaChar);
            } else {
                printf("%c", matrizTela[i][j]);
            }
        }
        printf("\n");
    }
    
    interagirNPCs();
    interagirElementos();
}

void interagirNPCs() {
    int posicaoAnteriorX = jogadorX;
    int posicaoAnteriorY = jogadorY;


    if (npcAzul.presente && jogadorX == npcAzul.x && jogadorY == npcAzul.y) {
        interagirNPCAzul();
    }
    if (npcRoxo.presente && jogadorX == npcRoxo.x && jogadorY == npcRoxo.y) {
        interagirNPCRoxo();
    }
    if (npcCharada.presente && jogadorX == npcCharada.x && jogadorY == npcCharada.y) {
        interagirNPCCharada();
    }
    
    // Verifica se a nova posição do jogador está dentro dos limites do mapa
    if (jogadorX < 0 || jogadorX >= tamanhoTela || jogadorY < 0 || jogadorY >= tamanhoTela) {
        // Se estiver fora dos limites, move o jogador de volta para a posição anterior
        jogadorX = posicaoAnteriorX;
        jogadorY = posicaoAnteriorY;
    }
    
    if (jogadorX < 0) jogadorX = 0;
    if (jogadorX >= tamanhoTela) jogadorX = tamanhoTela - 1;
    if (jogadorY < 0) jogadorY = 0;
    if (jogadorY >= tamanhoTela) jogadorY = tamanhoTela - 1;
}
// MEXER AQUI 

void interagirNPCAzul() {
    if (interagiuAzul) {
        printf("Você já interagiu com este NPC. Pressione qualquer tecla para voltar para a sala.\n");
        getchar();
        system("cls");
        iniciarMapa();
        return;
    }

    printf(ANSI_COLOR_BLUE "Você encontrou um NPC azul!\n");
    printf("Ele venderá uma habilidade que aumenta a vida máxima do personagem em 20 HP por 70 moedas.\n" ANSI_COLOR_RESET);
    printf("Deseja comprar a habilidade?\n");
    printf("1 - Sim\n");
    printf("2 - Não\n");

    int escolha;
    scanf("%d", &escolha);
    limparBufferEntrada();

    switch (escolha) {
        case 1:
            if (moedas >= 70) {
                //vidaPersonagem += 20;
                jogador.vidaMaxima += 20;
                moedas -= 70;
                printf("Você comprou a habilidade e sua vida máxima aumentou para %d HP!\n", jogador.vidaMaxima);//vidaPersonagem);
            } else {
                printf("Você não tem moedas suficientes para comprar a habilidade.\n");
            }
            break;
        case 2:
            printf("Você decidiu não comprar a habilidade.\n");
            break; // Não precisa retornar para a sala aqui
        default:
            printf("Escolha inválida!\n");
            break; // Adicionar retorno para evitar loop
    }
    
    jogadorX = posicaoInicialX + 6;
    jogadorY = posicaoInicialY + 6;
    
    printf("Pressione qualquer tecla para voltar para a sala.\n");
    getchar(); // Aguarda a entrada do usuário
    system("cls");
    iniciarMapa();
    interagiuAzul = true;
}

void interagirNPCRoxo() {

    if (interagiuRoxo) {
        printf("Você já interagiu com este NPC. Pressione qualquer tecla para voltar para a sala.\n");
        getchar();
        system("cls");
        iniciarMapa();
        return;
    }

    printf(ANSI_COLOR_PURPLE "Você encontrou um NPC roxo!\n");
    printf("Ele oferece uma espada de aço negro, alegando ser a mais forte de todas, mas isso pode ser uma armadilha.\n" ANSI_COLOR_RESET);
    printf("Deseja trocar sua espada atual por essa suposta espada de aço negro?\n");
    printf("1 - Sim\n");
    printf("2 - Não\n");

    int escolha;
    scanf("%d", &escolha);
    limparBufferEntrada();

    switch (escolha) {
        case 1:
            printf("Você decidiu fazer a troca...\n");
            printf("Ao fazer a troca, o NPC ataca você!\n");
            printf("Você percebe que foi enganado pelo NPC roxo!\n");
            printf("Você está morto!\n");
            sairJogo();
            break;
        case 2:
            printf("Você decidiu não trocar sua espada.\n");
            break; // Não precisa retornar para a sala aqui
        default:
            printf("Escolha inválida!\n");
            break;
    }
    
    jogadorX = posicaoInicialX + 6;
    jogadorY = posicaoInicialY + 6;
    
    printf("Pressione qualquer tecla para voltar para a sala.\n");
    getchar(); // Aguarda a entrada do usuário
    system("cls");
    iniciarMapa();
    interagiuRoxo = true;
}

void interagirNPCCharada() {

    if (interagiuCharada) {
        printf("Você já interagiu com este NPC. Pressione qualquer tecla para voltar para a sala.\n");
        getchar();
        system("cls");
        iniciarMapa();
        return;
    }

    printf("Você encontrou um NPC que apresenta uma charada:\n");
    printf("Tem raízes misteriosas,\n");
    printf("É mais alta que as frondosas,\n");
    printf("Sobe, sobe e também desce,\n");
    printf("Mas não cresce nem descresce.\n");
    printf("Qual é a resposta?\n");

    char resposta[20];
    fgets(resposta, 20, stdin);
    resposta[strcspn(resposta, "\n")] = 0; // Remove o caractere de nova linha

    if (strcmp(resposta, "montanha") == 0) {
        printf("---------------------------------------------------------------------\n");
        printf("Parabéns! Você acertou a charada e conseguiu fugir da dungeon!\n");
        printf("---------------------------------------------------------------------\n");
        getchar(); // Aguarda a entrada do usuário
        exit(0);
    } else {
        printf("Resposta incorreta! Continue sua jornada.\n");
    }
    
    jogadorX = posicaoInicialX + 6;
    jogadorY = posicaoInicialY + 6;
    
    printf("Pressione qualquer tecla para voltar para a sala.\n");
    getchar(); // Aguarda a entrada do usuário
    system("cls");
    iniciarMapa();
    interagiuCharada = true;
}

void entrarSala() {
    bausAbertosNaSala = 0;
    monstroDerrotado = 0;
    monstrosDerrotados = 0;
    inicializarPosicoes();
    reinicializarMonstros();
    iniciarMapa();
}

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void interagirElementos() {
    printf("\nO que você deseja fazer?\n");
    printf("1 - Abrir o baú\n");
    printf("2 - Ir para a próxima sala\n");
    printf("3 - Andar (W A S D)\n");
    printf("4 - Inventário\n");

    char escolha;
    printf("Escolha: ");
    scanf(" %c", &escolha);
    limparBufferEntrada();

    switch (escolha) {
        case '1':
            if (bausAbertosNaSala < MAX_BAUS_POR_SALA) {
                if (salaAtual < 5) {
                    if (monstrosDerrotados >= 2) {
                        printf("Você escolheu abrir o baú.\n");
                        abrirBau();            
                    } else {
                        printf("Você precisa derrotar pelo menos dois monstros para abrir o baú nesta sala!\n");
                        printf("Pressione qualquer tecla para voltar para a sala\n");
                        getchar();
                        system("cls");
                        iniciarMapa();
                    }
                    } else if (salaAtual < 10) { // A partir da sala 5 até a sala 9
                        if (monstrosDerrotados >= 3) {
                            printf("Você escolheu abrir o baú.\n");
                            abrirBau();            
                        } else {
                            printf("Você precisa derrotar pelo menos três monstros para abrir o baú nesta sala!\n");
                            printf("Pressione qualquer tecla para voltar para a sala\n");
                            getchar();
                            system("cls");
                            iniciarMapa();
                        }
                    } else { // A partir da sala 10
                        if (monstrosDerrotados >= 4) {
                        printf("Você escolheu abrir o baú.\n");
                        abrirBau();            
                    } else {
                        printf("Você precisa derrotar pelo menos quatro monstros para abrir o baú nesta sala!\n");
                        printf("Pressione qualquer tecla para voltar para a sala\n");
                        getchar();
                        system("cls");
                        iniciarMapa();
                    }
                }
            } else {
                printf("Você já abriu o baú desta sala!\n");
                interagirElementos();
            }
            break;
        case '2':
            if (monstroDerrotado) {
                if (salaAtual < 5) {
                    if (monstrosDerrotados >= 1) {
                        printf("Você escolheu ir para a próxima sala.\n");
                        movimentarSala();
                    } else {
                        printf("Você precisa derrotar pelo menos um monstro antes de mudar de sala.\n");
                        printf("Pressione qualquer tecla para voltar para a sala\n");
                        getchar();
                        system("cls");
                        iniciarMapa();
                    }
                } else { // A partir da sala 5
                    if (monstrosDerrotados >= 2) {
                        printf("Você escolheu ir para a próxima sala.\n");
                        movimentarSala();
                    } else {
                        printf("Você precisa derrotar pelo menos dois monstros antes de mudar de sala.\n");
                        printf("Pressione qualquer tecla para voltar para a sala\n");
                        getchar();
                        system("cls");
                        iniciarMapa();
                    }
                }
            } else {
                printf("Você precisa derrotar pelo menos um monstro antes de mudar de sala.\n");
                printf("Pressione qualquer tecla para voltar para a sala\n");
                getchar();
                system("cls");
                iniciarMapa();
            }
            break;
        case 'W':
        case 'w':
            if (jogadorX > 1) jogadorX--;
            verificarCombate();
            verificarVendedor();
            system("cls");
            iniciarMapa();
            break;
        case 'A':
        case 'a':
            if (jogadorY > 1) jogadorY--;
            verificarCombate();
            verificarVendedor();
            system("cls");
            iniciarMapa();
            break;
        case 'S':
        case 's':
            if (jogadorX < tamanhoTela - 2) jogadorX++;
            verificarCombate();
            verificarVendedor();
            system("cls");
            iniciarMapa();
            break;
        case 'D':
        case 'd':
            if (jogadorY < tamanhoTela - 2) jogadorY++;
            verificarCombate();
            verificarVendedor();
            system("cls");
            iniciarMapa();
            break;
        case '4':
            verificarInventario();
            break;
        default:
            printf("Opção inválida!\n");
            interagirElementos();
            break;
    }
}

void verificarInventario() {
    printf("\nInventário:\n");
    int temItens = 0;
    for (int i = 0; i < MAX_ITENS; i++) {
        if (jogador.inventario[i].quantidade > 0) {
            printf("%d. %s x%d\n", i + 1, jogador.inventario[i].nome, jogador.inventario[i].quantidade);
            temItens = 1;
        }
    }
    if (!temItens) {
        printf("Seu inventário está vazio.\n");
    } else {
        printf("Escolha um item para usar ou 0 para voltar: ");
        int escolhaItem;
        scanf("%d", &escolhaItem);
        if (escolhaItem == 0) {
            iniciarMapa();
            // Se escolha for 0, apenas retornar sem fazer nada
            return;
        } else if (escolhaItem > 0 && escolhaItem <= MAX_ITENS && jogador.inventario[escolhaItem - 1].quantidade > 0) {
            // Se escolha for um número válido, chamar a função usarItem()
            usarItem(escolhaItem - 1);
        } else {
            // Se escolha for inválida, exibir mensagem de erro
            printf("Escolha inválida!\n");
        }
    }
    printf("Pressione qualquer tecla para sair do inventário\n");
    getchar();
    getchar();
    system("cls");
    iniciarMapa();
}

void usarItem(int index) {
    if (strcmp(jogador.inventario[index].nome, "Poção de vida") == 0) { // Alterado para jogador.inventario
        if (jogador.vidaAtual < jogador.vidaMaxima) { // Alterado para jogador.vidaAtual e jogador.vidaMaxima
            jogador.vidaAtual += 30; // Alterado para jogador.vidaAtual
            if (jogador.vidaAtual > jogador.vidaMaxima) jogador.vidaAtual = jogador.vidaMaxima; // Alterado para jogador.vidaAtual e jogador.vidaMaxima
            jogador.inventario[index].quantidade--; // Alterado para jogador.inventario
            printf("Você usou uma Poção de vida.\nVida atual: %d\n", jogador.vidaAtual); // Alterado para jogador.vidaAtual
        } else {
            printf("Sua vida já está cheia.\n");
        }
    }
}

void verificarCombate() {
    for (int i = 0; i < 6; i++) {
        if (jogadorX == monstros[i]->posX && jogadorY == monstros[i]->posY && monstros[i]->vida > 0) {
            printf("Você encontrou um %s! Iniciando combate...\n", monstros[i]->nome);
            combate(monstros[i]);
            
            if (monstros[i] == &dragao && monstros[i]->vida <= 0) {
                printf("---------------------------------------------------------------------------\n");
                printf("Parabéns! Você derrotou o Dragão e fugiu da Dungeon!\n");
                printf("---------------------------------------------------------------------------\n");
                printf("Número de salas passadas: %d\n", totalSalasPassadas);
                printf("Quantidade de ouro coletado de báus: %d\n", totalOuroColetado);
                printf("Quantidade total de ouro ganho com os monstros: %d\n", totalOuroMonstros);
                printf("Número de báus abertos: %d\n", totalBausAbertos);
                printf("Número de monstros derrotados: %d\n", totalMonstrosDerrotados);
                printf("---------------------------------------------------------------------------\n");
                printf("Pressione qualquer tecla para sair do jogo...\n");
                getchar();
                exit(0);
            }
            
        }
    }
}

void combate(Monstro* monstro) {
    while (monstro->vida > 0 && jogador.vidaAtual > 0) {
        printf("---------------------------------------------\n");
        printf("Turno do combate contra %s\n", monstro->nome);
        printf("---------------------------------------------\n");
        
        printf("Seu turno! Escolha uma ação:\n");
        printf("1 - Atacar\n");
        printf("2 - Fugir\n");
        printf("3 - Usar poção de vida\n");
        int acao;
        scanf("%d", &acao);
        if (acao == 1) {
            int chanceCritico = rand() % 100 + 1;
            if (espadaEquipadaDano == 40) { //espada de aço negro
                chanceCritico = rand() % 2 + 1; // crítico 50%
            }
            if (chanceCritico <= 25) {
                int danoCritico = espadaEquipadaDano * 2;
                monstro->vida -= danoCritico;
                printf("Você causou um acerto crítico e deu %d de dano ao %s.\nVida restante do monstro: %d\n", danoCritico, monstro->nome, monstro->vida);
            } else {
                monstro->vida -= espadaEquipadaDano;
                printf("Você causou %d de dano ao %s.\nVida restante do monstro: %d\n", espadaEquipadaDano, monstro->nome, monstro->vida);
            }
        } else if (acao == 2) {
            printf("Você fugiu!\n");
            jogador.vidaAtual -= 10;
            printf("Você perdeu 10 de vida ao fugir!\n");
            Sleep(2500);
            return;
        } else if (acao == 3) {
            usarPocao(&jogador);
        } else {
            printf("Ação inválida! Perdeu seu turno.\n");
        }
        verificarMortePersonagem();
        if (monstro->vida > 0) {
            int chanceEsquiva = rand() % 100;
            if (chanceEsquiva < 25) {
                printf("Você se esquivou do ataque do %s!\n", monstro->nome);
            } else {
                jogador.vidaAtual -= monstro->dano;
                printf("O %s atacou e causou %d de dano.\nSua vida restante: %d\n", monstro->nome, monstro->dano, jogador.vidaAtual);
            }    
        }
    }
    if (jogador.vidaAtual <= 0) {
        printf("---------------------------------------------\n");
        getchar();
        sairJogo();
        
        getchar();
        
    } else if (monstro->vida <= 0) {
        printf("---------------------------------------------\n");
        printf("Você derrotou o %s!\n", monstro->nome);
        monstro->ativo = false;
        monstroDerrotado = 1;
        totalMonstrosDerrotados++;
        monstrosDerrotados++;
        int recompensaOuro = MIN_RECOMPENSA_OURO + rand() % (MAX_RECOMPENSA_OURO - MIN_RECOMPENSA_OURO + 1);
        printf("Você encontrou %d de ouro no %s!\n", recompensaOuro, monstro->nome);
        moedas += recompensaOuro;
        totalOuroMonstros += recompensaOuro;
        
        printf("Pressione qualquer tecla para sair do combate\n");
        getchar();
        getchar();
        return;
    }
    system("cls");
    iniciarMapa();
}

void inicializarInventario(Jogador *jogador) {
    for(int i = 0; i < MAX_ITENS; i++) {
        strcpy(jogador->inventario[i].nome, "");
        jogador->inventario[i].quantidade = 0;
    }
}

void adicionarItem(char nome[], int quantidade) {
    // Verifica se o inventário já está cheio
    int inventarioCheio = 1;
    for(int i = 0; i < MAX_ITENS; i++) {
        if(strcmp(jogador.inventario[i].nome, "") == 0) {
            inventarioCheio = 0;
            break;
        }
    }

    // Se o inventário estiver cheio, exibe uma mensagem e sai da função
    if (inventarioCheio) {
        printf("O inventário está cheio. Não é possível adicionar mais itens.\n");
        return;
    }

    // Procura por um item com o mesmo nome no inventário
    for(int i = 0; i < MAX_ITENS; i++) {
        if(strcmp(jogador.inventario[i].nome, nome) == 0) {
            jogador.inventario[i].quantidade += quantidade;
            return;
        }
    }
    
    // Se não encontrar um item com o mesmo nome, procura por uma posição vazia para adicionar o novo item
    for (int i = 0; i < MAX_ITENS; i++) {
        if(strcmp(jogador.inventario[i].nome, "") == 0) {
            strcpy(jogador.inventario[i].nome, nome);
            jogador.inventario[i].quantidade = quantidade;
            return;
        }
    }
}

void usarPocao(Jogador *jogador) {
    for(int i = 0; i < MAX_ITENS; i++) {
        if(strcmp(jogador->inventario[i].nome, "Poção de vida") == 0 && jogador->inventario[i].quantidade > 0) {
            int vidaCura = 30;

            // Verificar se a cura ultrapassa a vida máxima
            if(jogador->vidaAtual + vidaCura <= jogador->vidaMaxima) {
                jogador->vidaAtual += vidaCura;
            } else {
                jogador->vidaAtual = jogador->vidaMaxima;
            }
            
            // Diminuir a quantidade de poções
            jogador->inventario[i].quantidade--;

            // Imprimir a vida atual
            printf("Você usou uma Poção de vida!\nVida atual: %d\n", jogador->vidaAtual);
            return;
        }
    }
    printf("Você não tem Poções de vida!\n");
}

void movimentarSala() {
    totalSalasPassadas++;
    salaAtual++;
    printf("Entrando na próxima sala...\n");
    entrarSala();
}

void verificarMortePersonagem() {
    if (vidaPersonagem <= 0) {
        sairJogo();
    }
}

void abrirBau() {
    int moedasEncontradas = 5 + rand() % 31; // 5 até 35 moedas
    printf("Você encontrou %d moedas no baú!\n", moedasEncontradas);
    
    if (moedas + moedasEncontradas > MAX_MOEDAS) {
        printf("Você atingiu o limite máximo de %d.\n", MAX_MOEDAS);
        moedas = MAX_MOEDAS;
    } else{
        moedas += moedasEncontradas;
        totalOuroColetado += moedasEncontradas;
    }
    
    int chanceDropEspadaNegra = rand() % 100; // Gera um número entre 1 e 100
    if (chanceDropEspadaNegra < 5) {
        printf("Você encontrou uma Espada de aço negro no baú!\n");
        printf("Adicionando Espada de aço negro ao inventário...\n");
        espadaEquipadaDano = espadaAcoNegroDano; // Equipa a espada de aço negro
    }
    
    printf("Agora você possui %d moedas.\n", moedas);
    
    // Chance de 50% de encontrar uma poção de vida
    int chanceEncontrarPocao = rand() % 2; // 0 ou 1
    if (chanceEncontrarPocao == 1) {
        printf("Você encontrou uma Poção de vida no baú!\n");
        adicionarItem("Poção de vida", 1);
    }
    
    totalBausAbertos++;
    
    if (totalBausAbertos == 5) {
        printf("---------------------------------------------------------------------------------------\n");
        printf("Parabéns você completou uma missão oculta! Você abriu 5 báus e ganhou 30 moedas!\n");
        printf("---------------------------------------------------------------------------------------\n");
        moedas += 30;
        totalOuroColetado += 30;
    }
    
    
    bausAbertosNaSala++;
    
    interagirElementos();
}

void verificarVendedor() {
    if (vendedorPresente && jogadorX == vendedorX && jogadorY == vendedorY) {
        printf("Você encontrou o vendedor!\n");
        interagirVendedor();
    }
}

void interagirVendedor() {
    printf("---------------------------------------------\n");
    printf("\nItens disponíveis para venda:\n");
    printf("Melhorar Espadas:\n");
    printf("1 - Espada de ferro (20 moedas)\n");
    printf("2 - Espada de ouro (40 moedas)\n");
    printf("3 - Espada de rubi (80 moedas)\n");
    printf("4 - Espada de aço negro (200 moedas)\n");
    printf("Poções:\n");
    printf("5 - Poção de vida (15 moedas)\n");
    printf("6 - Sair do vendedor\n");
    printf("Escolha o item que deseja comprar: ");

    int escolhaVendedor;
    scanf("%d", &escolhaVendedor);

    printf("---------------------------------------------\n");
    switch (escolhaVendedor) {
        case 1:
            if (moedas >= 20) {
                printf("Você melhorou sua espada para Espada de ferro!\n");
                espadaEquipadaDano = espadaFerroDano;
                moedas -= 20;
            } else {
                printf("Você não tem moedas suficientes!\n");
            }
            break;
        case 2:
            if (moedas >= 40) {
                printf("Você melhorou sua espada para Espada de ouro!\n");
                espadaEquipadaDano = espadaOuroDano;
                moedas -= 40;
            } else {
                printf("Você não tem moedas suficientes!\n");
            }
            break;
        case 3:
            if (moedas >= 80) {
                printf("Você melhorou sua espada para Espada de rubi!\n");
                espadaEquipadaDano = espadaRubiDano;
                moedas -= 80;
            } else {
                printf("Você não tem moedas suficientes!\n");
            }
            break;
        case 4:
            if (moedas >= 200) {
                printf("Você melhorou sua espada para Espada de aço negro!\n");
                espadaEquipadaDano = espadaAcoNegroDano;
                moedas -= 200;
            } else {
                printf("Você não tem moedas suficientes!\n");
            }
            break;
        case 5:
            if (moedas >= 15) {
                printf("Você comprou uma Poção de vida!\n");
                moedas -= 15;
                adicionarItem("Poção de vida", 1);
            } else {
                printf("Você não tem moedas suficientes!\n");
            }
            break;
        case 6:
            printf("Você saiu do vendedor.\n");
            system("cls");
            iniciarMapa();
            return;
        default:
            printf("Opção inválida!\n");
            break;
    }

    printf("---------------------------------------------\n");
    printf("Moedas restantes: %d\n", moedas);
    interagirVendedor();  // Volta para o vendedor após a compra
}

void iniciarJogo() {
    inicializarInventario(&jogador);
    entrarSala();
}

void telaInicial() {
    printf("Bem-vindo ao Brave Knight em texto!\n");
    printf("Escolha uma opção:\n");
    printf("1 - Iniciar jogo\n");
    printf("2 - Sair do jogo\n");

    int escolhaInicial;
    printf("Opção: ");
    scanf("%d", &escolhaInicial);

    switch (escolhaInicial) {
        case 1:
            iniciarJogo();
            break;
        case 2:
            sairJogo();
            break;
        default:
            printf("Opção inválida!\n");
            telaInicial();
            break;
    }
}

void sairJogo() {
    printf("Você morreu durante o combate!\n");
    printf("---------------------------------------------\n");
    printf("Número de salas passadas: %d\n", totalSalasPassadas);
    printf("Quantidade de ouro coletado de báus: %d\n", totalOuroColetado);
    printf("Quantidade total de ouro ganho com os monstros: %d\n", totalOuroMonstros);
    printf("Número de báus abertos: %d\n", totalBausAbertos);
    printf("Número de monstros derrotados: %d\n", totalMonstrosDerrotados);
    printf("---------------------------------------------\n");
    printf("Pressione qualquer tecla para sair do jogo...\n");
    getchar();
    exit(0);
}

int main() {
    setlocale(LC_ALL, "Portuguese_Brazil.1252");
    telaInicial();
    system("PAUSE");
    return 0;
}
