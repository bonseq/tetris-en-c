#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 20
#define COLS 10
#define BLOCK_SIZE 30
#define SCREEN_WIDTH (COLS * BLOCK_SIZE)
#define SCREEN_HEIGHT (ROWS * BLOCK_SIZE)

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
int table[ROWS][COLS] = {0}; // Tablero de juego
Piece currentPiece;          // Pieza activa
int running = 1;             // Estado del juego

SDL_Color colors[] = {
    {0, 0, 0, 255},       // Negro (vacío)
    {255, 0, 0, 255},     // Rojo
    {0, 255, 0, 255},     // Verde
    {0, 0, 255, 255},     // Azul
    {255, 255, 0, 255},   // Amarillo
    {0, 255, 255, 255},   // Cian
    {255, 0, 255, 255}    // Magenta
};

// Piezas predefinidas
Piece shapes[] = {
    // Línea horizontal
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

// Inicializa SDL2
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

        // Verificar bordes y ocupación del tablero
        if (x < 0 || x >= COLS || y >= ROWS || (y >= 0 && table[y][x] != 0)) {
            return 0; // Posición no válida
        }
    }
    return 1; // Posición válida
}

// Genera una nueva pieza aleatoria
void generatePiece() {
    currentPiece = shapes[rand() % 7];
    for (int i = 0; i < 4; i++) {
        currentPiece.blocks[i].y += 0; // Aparece arriba
        currentPiece.blocks[i].x += COLS / 2 - 1;
    }

    // Verificar si la nueva pieza puede colocarse
    if (!isValidPosition(currentPiece)) {
        running = 0; // Fin del juego
    }
}

// Dibuja el tablero y las piezas
void drawGame() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Dibuja el tablero
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int colorIndex = table[i][j];
            SDL_SetRenderDrawColor(renderer, colors[colorIndex].r, colors[colorIndex].g, 
                                   colors[colorIndex].b, colors[colorIndex].a);
            SDL_Rect rect = {j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // Dibuja la pieza actual
    SDL_SetRenderDrawColor(renderer, colors[currentPiece.color].r, 
                           colors[currentPiece.color].g, 
                           colors[currentPiece.color].b, 255);
    for (int i = 0; i < 4; i++) {
        Block b = currentPiece.blocks[i];
        SDL_Rect rect = {b.x * BLOCK_SIZE, b.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_RenderPresent(renderer);
}

// Maneja eventos de teclado
void handleInput(SDL_Event *e) {
    if (e->type == SDL_QUIT) {
        running = 0;
    } else if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_LEFT:
                for (int i = 0; i < 4; i++) currentPiece.blocks[i].x--;
                break;
            case SDLK_RIGHT:
                for (int i = 0; i < 4; i++) currentPiece.blocks[i].x++;
                break;
            case SDLK_DOWN:
                for (int i = 0; i < 4; i++) currentPiece.blocks[i].y++;
                break;
        }
    }
}


void fixPiece(Piece piece) {
    for (int i = 0; i < 4; i++) {
        int x = piece.blocks[i].x;
        int y = piece.blocks[i].y;

        if (y >= 0) { // Asegurarse de no escribir fuera del tablero
            table[y][x] = piece.color;
        }
    }
}
//cleo q limpiar columna
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
            // Mover filas superiores hacia abajo
            for (int row = y; row > 0; row--) {
                for (int x = 0; x < COLS; x++) {
                    table[row][x] = table[row - 1][x];
                }
            }
            // Vaciar la fila superior
            for (int x = 0; x < COLS; x++) {
                table[0][x] = 0;
            }
            y++; // Revisar nuevamente esta fila
        }
    }
}


// Actualiza el juego (caída de piezas, colisiones)
void updateGame() {
    Piece temp = currentPiece;
    for (int i = 0; i < 4; i++) {
        temp.blocks[i].y++;
    }

    if (isValidPosition(temp)) {
        // Si la posición es válida, actualizamos la pieza
        for (int i = 0; i < 4; i++) {
            currentPiece.blocks[i].y++;
        }
    } else {
        // Si no es válida, fijamos la pieza y generamos una nueva
        fixPiece(currentPiece);
        clearFullRows();
        generatePiece();
    }
}

//estructura principal :P
int main() {
    if (!initSDL()) {
        return 1;
    }

    srand(time(NULL));
    generatePiece();

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            handleInput(&e);
        }

        updateGame();
        drawGame();
        SDL_Delay(500); // Velocidad de caída
    }

    closeSDL();
    return 0;
}
