#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>

#define ROWS 20 
#define COLS 15
#define TRUE 1
#define FALSE 0

char tabla[ROWS][COLS] ={0};
int puntos = 0;
char JUGAR =TRUE;
suseconds_t timer = 400000;
int decrecer = 1000; 
 
typedef struct 
{
    char **array;
    int width, row, col;
} FORMA;
FORMA BACTUAL;

const FORMA FORMAArray[7]= {
	{(char *[]){(char []){0,1,1},(char []){1,1,0}, (char []){0,0,0}}, 3},                           //S      
	{(char *[]){(char []){1,1,0},(char []){0,1,1}, (char []){0,0,0}}, 3},                           //Z     
	{(char *[]){(char []){0,1,0},(char []){1,1,1}, (char []){0,0,0}}, 3},                           //T     
	{(char *[]){(char []){0,0,1},(char []){1,1,1}, (char []){0,0,0}}, 3},                           //L   
	{(char *[]){(char []){1,0,0},(char []){1,1,1}, (char []){0,0,0}}, 3},                           //L2     
	{(char *[]){(char []){1,1},(char []){1,1}}, 2},                                                 //cuadrado
	{(char *[]){(char []){0,0,0,0}, (char []){1,1,1,1}, (char []){0,0,0,0}, (char []){0,0,0,0}}, 4} //barrita larga 
	
};

FORMA CopFORMA(FORMA forma){
	FORMA NUEV_FORMA = forma;
    char **CopFORMA = forma.array;
    NUEV_FORMA.array = (char**)malloc(NUEV_FORMA.width*sizeof(char*));
    int i, j;
    for (i = 0; i < NUEV_FORMA.width; i++)
    {
        NUEV_FORMA.array[i] =(char*)malloc(NUEV_FORMA.width*sizeof(char));
        for(j=0; j<NUEV_FORMA.width; j++){
            NUEV_FORMA.array[i][j]=CopFORMA[i][j];
        }
         
    }
    return NUEV_FORMA;
}
void BorrarForma(FORMA forma){
    char **array = forma.array;
    int i;
    for ( i = 0; i < forma.width; i++){
        free(forma.array[i]);
    }
    lib(forma.array);
}

int ChequeoDePos(FORMA forma){ //chequeo de la posicion del bloque
	char **array = forma.array;
	int i, j;
	for(i = 0; i < forma.width;i++) {
		for(j = 0; j < forma.width ;j++){
			if((forma.col+j < 0 || forma.col+j >= COLS || forma.row+i >= ROWS)){ //bordes
				if(array[i][j]) 
					return FALSE;
				
			}
			else if(tabla[forma.row+i][forma.col+j] && array[i][j])
				return FALSE;
		}
    }
    return TRUE;
}

void NUEVAFORMARAMDOM(){ 
	FORMA NUEV_FORMA = CopFORMA(FORMAArray[rand()%7]);

    NUEV_FORMA.col = rand()%(COLS-NUEV_FORMA.width+1);
    NUEV_FORMA.row = 0;
    BorrarForma(BACTUAL);
	current = NUEV_FORMA;
	if(!ChequeoDePos(BACTUAL)){
		JUGAR = FALSE;
	}
}

void ROTAR_FORMA(FORMA forma){ 
	FORMA tem = CopFORMA(forma);
	int i, j, k, width;
	width = forma.width;
	for(i = 0; i < width ; i++){
		for(j = 0, k = width-1; j < width ; j++, k--){
				forma.array[i][j] = tem.array[k][i];
		}
	}
	BorrarForma(tem);
}

void EscribirTabla(){
	int i, j;
	for(i = 0; i < BACTUAL.width ;i++){
		for(j = 0; j < BACTUAL.width ; j++){
			if(BACTUAL.array[i][j])
				tabla[BACTUAL.row+i][BACTUAL.col+j] = BACTUAL.array[i][j];
		}
	}
}

void REMOVER_COLM_RESET_PUNTAJE(){
	int i, j, sum, count=0;
	for(i=0;i<ROWS;i++){
		sum = 0;
		for(j=0;j< COLS;j++) {
			sum+=tabla[i][j];
		}
		if(sum==COLS){
			count++;
			int l, k;
			for(k = i;k >=1;k--)
				for(l=0;l<COLS;l++)
					tabla[k][l]=tabla[k-1][l];
			for(l=0;l<COLS;l++)
				tabla[k][l]=0;
			timer-=decrecer--;
		}
	}
	puntos += 100*count;
}
void Imprimir_Tabla(){
	char Buffer[ROWS][COLS] = {0};
	int i, j;
	for(i = 0; i < BACTUAL.width ;i++){
		for(j = 0; j < BACTUAL.width ; j++){
			if(BACTUAL.array[i][j])
				Buffer[BACTUAL.row+i][BACTUAL.col+j] = BACTUAL.array[i][j];
		}
	}
	clear();
	for(i=0; i<COLS-9; i++)
		printw(" ");
	printw("ejemplo tetris\n");
	for(i = 0; i < ROWS ;i++){
		for(j = 0; j < COLS ; j++){
			printw("%c ", (tabla[i][j] + Buffer[i][j])? '#': '.');
		}
		printw("\n");
	}
	printw("\nSCR: %d\n", puntos);
}

void ManipulAR_BACTUAL(int action){
	FORMA tem = CopFORMA(BACTUAL);
	switch(action){
		case 's':
			tem.row++;  //mov ABAJO
			if(ChequeoDePos(tem))
				current.row++;
			else {
				EscribirTabla();
				REMOVER_COLM_RESET_PUNTAJE();
                NUEVAFORMARAMDOM();
			}
			break;
		case 'd':
			tem.col++;  //mov der
			if(ChequeoDePos(tem)
				BACTUAL.col++;
			break;
		case 'a':
			tem.col--;  //mov izq
			if(CheckPosition(temp))
				BACTUAL.col--;
			break;
		case 'w':
			ROTAR_FORMA(tem); // rotar bloque
			if(ChequeoDePos(tem))
				ROTAR_FORMA(BACTUAL);
			break;
	}
	BorrarForma(tem);
	Imprimir_Tabla();
}

struct timeval DESPUES, AHORA;
int para_ACTUALIZAR(){
	return ((suseconds_t)(AHORA.tv_sec*1000000 + AHORA.tv_usec) -((suseconds_t)DESPUES.tv_sec*1000000 + DESPUES.tv_usec)) > timer;
}

int main() {
    srand(time(0));
    puntos = 0;
    int c;
    initscr();
	gettimeofday(&DESPUES, NULL);
	timeout(1);
	NUEVAFORMARAMDOM();
    Imprimir_Tabla();
	while(JUGAR){+
		if ((c = getch()) != ERR) {
		  ManipulAR_BACTUAL(c);
		}
		gettimeofday(&AHORA, NULL);
		if (para_ACTUALIZAR()) { 
			ManipulAR_BACTUAL('s');
			gettimeofday(&DESPUES, NULL);
		}
	}
	BorrarForma(BACTUAL);
	endwin();
	int i, j;
	for(i = 0; i < ROWS ;i++){
		for(j = 0; j < COLS ; j++){
			printf("%c ", tabla[i][j] ? '#': '.');
		}
		printf("\n");
	}
	printf("\nte acabaron gay!\n");
	printf("\nScr: %d\n", puntos);
    return 0;
}