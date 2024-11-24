#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 20
#define COLS 10
#define BLOCK_SIZE 30
#define SCREEN_WIDTH (COLS * BLOCK_SIZE)
#define SCREEN_HEIGHT (ROWS * BLOCK_SIZE)

void showStartScreen();

typedef struct {
    int x, y;
} Block;

typedef struct {
    Block blocks[4];
    int color;
} Piece;
// Variables globales
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
int table[ROWS][COLS] = {0}; // Tablero 
Piece currentPiece;          // Pieza activa
int running = 1;             // Estado del juego
int score = 0;
TTF_Font *font = NULL; // Declaración global

SDL_Color colors[] = {
    {0, 0, 0, 255},       // Negro 
    {255, 0, 0, 255},     // Rojo
    {0, 255, 0, 255},     // Verde
    {0, 0, 255, 255},     // Azul
    {255, 255, 0, 255},   // Amarillo
    {0, 255, 255, 255},   // Cian
    {255, 0, 255, 255}    // Magenta
};
// Piezas 
Piece shapes[] = {
    // Línea 
    {{{0, 0}, {1, 0}, {2, 0}, {3, 0}}, 1},
    // Cuadrado
    {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}, 2},
    // T
    {{{0, 0}, {1, 0}, {2, 0}, {1, 1}}, 3},
    // L
    {{{0, 0}, {0, 1}, {1, 1}, {2, 1}}, 4},
    // L invertida
    {{{2, 0}, {0, 1}, {1, 1}, {2, 1}}, 5},
    // S
    {{{1, 0}, {2, 0}, {0, 1}, {1, 1}}, 6},
    // Z
    {{{0, 0}, {1, 0}, {1, 1}, {2, 1}}, 7}
};
// libreria inicializa SDL2 (copilot.ia)
int initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("No se pudo inicializar SDL: %s\n", SDL_GetError());
        return 0;
    }
    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("No se pudo crear la ventana: %s\n", SDL_GetError());
        return 0;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("No se pudo crear el renderer: %s\n", SDL_GetError());
        return 0;
    }
    return 1;
}

// Cierra SDL2
void closeSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int isValidPosition(Piece piece) {
    for (int i = 0; i < 4; i++) {
        int x = piece.blocks[i].x;
        int y = piece.blocks[i].y;
        // Verificar ocupación del tablero
        if (x < 0 || x >= COLS || y >= ROWS || (y >= 0 && table[y][x] != 0)) {
            return 0; // Posición no válida
        }
    }
    return 1; // posición válida
}

void rotatePiece(Piece *piece) {
    Piece temp = *piece; // resguardo para probar la rotación
    Block pivot = temp.blocks[0]; //primer bloque como pivote

    for (int i = 0; i < 4; i++) {
        int rel_x = temp.blocks[i].x - pivot.x;
        int rel_y = temp.blocks[i].y - pivot.y;
        // Rotar 90grad en sentido horario
        temp.blocks[i].x = pivot.x - rel_y;
        temp.blocks[i].y = pivot.y + rel_x;
    }
    // verificar si la rotación es válida
    if (isValidPosition(temp)) {
        *piece = temp; // si es válida, aplicamos la rotación
    }
}
// Generar nueva pieza aleatoria
void generatePiece() {
    currentPiece = shapes[rand() % 7];
    for (int i = 0; i < 4; i++) {
        currentPiece.blocks[i].y += 0; // Aparece arriba
        currentPiece.blocks[i].x += COLS / 2 - 1;
    }
    // verifica si la nueva pieza puede colocarse
    if (!isValidPosition(currentPiece)) {
        running = 0; // game over 
    }
}

// dibuja tablero y las piezas
void drawGame() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //tablero
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int colorIndex = table[i][j];
            SDL_SetRenderDrawColor(renderer, colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b, colors[colorIndex].a);
            SDL_Rect rect = {j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    //actual
    SDL_SetRenderDrawColor(renderer, colors[currentPiece.color].r, colors[currentPiece.color].g, colors[currentPiece.color].b, 255);
    for (int i = 0; i < 4; i++) {
        Block b = currentPiece.blocks[i];
        SDL_Rect rect = {b.x * BLOCK_SIZE, b.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }

    // mostrar puntaje
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    SDL_Color color = {255, 255, 255}; // Blanco
    SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, scoreText, color);
    SDL_Texture *scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = {10, 10, scoreSurface->w, scoreSurface->h};
    SDL_FreeSurface(scoreSurface);

    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_DestroyTexture(scoreTexture); // Liberar textura después de renderizarla

    SDL_RenderPresent(renderer);
}
// maneja input teclado
void handleInput(SDL_Event *e) {
    if (e->type == SDL_QUIT) {
        running = 0;
    } else if (e->type == SDL_KEYDOWN) {
        Piece temp = currentPiece;
        switch (e->key.keysym.sym) {
            case SDLK_LEFT:
                for (int i = 0; i < 4; i++) temp.blocks[i].x--;
                if (isValidPosition(temp)) currentPiece = temp;
                break;
            case SDLK_RIGHT:
                for (int i = 0; i < 4; i++) temp.blocks[i].x++;
                if (isValidPosition(temp)) currentPiece = temp;
                break;
            case SDLK_DOWN:
                for (int i = 0; i < 4; i++) temp.blocks[i].y++;
                if (isValidPosition(temp)) currentPiece = temp;
                break;
            case SDLK_UP: // Rotar pieza
                rotatePiece(&currentPiece);
                break;
        }
    }
}
//ola coto
void fixPiece(Piece piece) {
    for (int i = 0; i < 4; i++) {
        int x = piece.blocks[i].x;
        int y = piece.blocks[i].y;

        if (y >= 0) {
            table[y][x] = piece.color;
        }
    }
}

void showStartScreen(TTF_Font *smallFont) {
    int waiting = 1;

    SDL_Color color = {255, 255, 255}; // Blanco

    // Crear superficie y textura para el texto principal
    SDL_Surface *titleSurface = TTF_RenderText_Solid(font, "TETRIS GAME", color);
    SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {50, SCREEN_HEIGHT / 2 - 50, titleSurface->w, titleSurface->h};
    SDL_FreeSurface(titleSurface);

    // Crear superficie y textura para el texto de pie de página
    SDL_Surface *footerSurface = TTF_RenderText_Solid(smallFont, "PRESS ENTER", color);
    SDL_Texture *footerTexture = SDL_CreateTextureFromSurface(renderer, footerSurface);
    SDL_Rect footerRect = {50, SCREEN_HEIGHT / 2 + 50, footerSurface->w, footerSurface->h};
    SDL_FreeSurface(footerSurface);

    while (waiting) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fondo negro
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
        SDL_RenderCopy(renderer, footerTexture, NULL, &footerRect);

        SDL_RenderPresent(renderer);

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
                waiting = 0;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                waiting = 0;
            }
        }
    }

    // Liberar texturas dentro de la función
    SDL_DestroyTexture(titleTexture);
    SDL_DestroyTexture(footerTexture);
}

// limpiar columna
void clearFullRows() {
    for (int y = ROWS - 1; y >= 0; y--) {
        int isFull = 1;
        for (int x = 0; x < COLS; x++) {
            if (table[y][x] == 0) {
                isFull = 0;
                break;
            }
        }

        if (isFull) {
            score += 100; // Incrementar puntaje
            for (int row = y; row > 0; row--) {
                for (int x = 0; x < COLS; x++) {
                    table[row][x] = table[row - 1][x];
                }
            }
            for (int x = 0; x < COLS; x++) {
                table[0][x] = 0;
            }
            y++;
        }
    }
}

// actualiza juego (caida de piezas, colisiones)
void updateGame() {
    Piece temp = currentPiece;
    for (int i = 0; i < 4; i++) {
        temp.blocks[i].y++;
    }

    if (isValidPosition(temp)) {
        // Si la posición es valida, actualizar
        for (int i = 0; i < 4; i++) {
            currentPiece.blocks[i].y++;
        }
    } else {
        // si no es valida, fijar pieza genero otra
        fixPiece(currentPiece);
        clearFullRows();
        generatePiece();
        //gameover
        if (!isValidPosition(currentPiece)) {
            running = 0;
        }
    }
}

//proceso
int main() {
    if (!initSDL()) return 1;

    if (TTF_Init() == -1) {
        printf("Error al inicializar SDL_ttf: %s\n", TTF_GetError());
        return 1;
    }
    font = TTF_OpenFont("04B_30__.ttf", 24);
    if (!font) {
        printf("Error al cargar la fuente: %s\n", TTF_GetError());
        return 1;
    }
    TTF_Font *smallFont = TTF_OpenFont("04B_30__.ttf", 16);
    if (!smallFont) {
        printf("Error al cargar la fuente pequeña: %s\n", TTF_GetError());
        return 1;
    }

    // Pantalla de inicio
    showStartScreen(smallFont);
    srand(time(NULL));
    generatePiece();

    // Bucle principal del juego
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            handleInput(&e);
        }

        updateGame(); // actualiza la posición de las piezas
        drawGame();   
        SDL_Delay(500); // controla la velocidad del juego
    }
    TTF_CloseFont(font); // Cierra las fuentes y sdl
    TTF_CloseFont(smallFont);
    TTF_Quit();          
    closeSDL();          

    return 0;
}
