#include <pgmspace.h>
#include <stdio.h>    
#include <stdlib.h> 
#include <ina90.h>
#include <io8515.h>
#include "avrlib.h"

#define NUMGHOST 4
#define HERE board[dummy.y][dummy.x]
/* Upper case ghosts are deadly, lower case edible. Blinks when less than 10
ticks left of edibility period. */
#define GHOSTPOINTS 20   /* Score for killing a ghost */
#define GHOSTDEAD 60    /* Length of death period for a ghost */
#define GHOSTBLINK 20    /* Length of blinking period for an edible ghost */
#define GHOSTEDIBLE 40   /* Length of period when ghosts can be eaten */
#define GHOSTCHAR ('G'+32*((ghedible>GHOSTBLINK)|(ghedible%2)))
#define BOARDX 19
#define BOARDY 11
#define NUMDOTS 103

flash char clear[]={27,'[','H',27,'[','2','J',0};
flash char home []={27,'[','H',0};
flash char game_over[] = "Game over!";
flash_char score_text[] = "Score: ";
    
flash char start_board[BOARDY][BOARDX] = {"###################",
                                    "#o...............o#",
                                    "#.#####.....#####.#",
                                    "#.#.............#.#",
                                    "#.#..####.####..#.#",
                                    "#....#.......#....#",
                                    "#.#..####.####..#.#",
                                    "#.#.............#.#",
                                    "#.#####.....#####.#",
                                    "#o...............o#",
                                    "###################"};
struct ghosttype {
    int x,y,dead,dir;
} start_ghost[NUMGHOST] = { {1,1,0,-1}, {1,9,0,-1}, {18,1,0,-1}, {18,9,0,-1} };

struct ghosttype player = {9,5,0,0};
struct ghosttype resurrected_ghost = {9,5,0,-1};
struct ghosttype ghost[NUMGHOST] = { {1,1,0,-1}, {18,1,0,-1}, {1,9,0,-1}, {18,9,0,-1} };
char board[BOARDY][BOARDX];
int ghedible = 0;
int score = 0;
int dots;
int ghostnum = 0;
int intelligence = 0;

int hit_ghost(int x, int y) {
    int i;

    for(i=0; i<ghostnum; i++)
        if(!ghost[i].dead & (x==ghost[i].x) & (y==ghost[i].y))
            return(i+1);
    return(0);
}

void draw_board()
{
    int i,j;
    char strPoints[8]; 

    puts_P(home);
            putchar('P');
            putchar('o');
            putchar('i');
            putchar('n');
            putchar('t');
            putchar('s');
            putchar(':');
            putchar(' ');
    itoa(score,strPoints);
    puts(strPoints);
        putchar('\n');
    for(i=0; i<BOARDY; i++){
        for(j=0; j<BOARDX; j++)
            if((j==player.x) & (i==player.y))
                putchar('@');
            else
                if(!hit_ghost(j,i))
                    putchar(board[i][j]);
                else
                    putchar(GHOSTCHAR);
        putchar('\n');
        putchar('\r');
    }
}

int check_hit(struct ghosttype *player) {
    int hig;

    if(hig = hit_ghost((*player).x,(*player).y))
        if(ghedible) {
            ghost[hig-1].dead = GHOSTDEAD;
            score += GHOSTPOINTS;
        }
        else
            (*player).dead = 1;
    return((*player).dead);
}

char controls(struct ghosttype *player, char key) {
    char key;
    struct ghosttype dummy;

    dummy = *player;
    switch(key){
    case 'e': {dummy.y--; break; }
    case 'd': {dummy.y++; break; }
    case 'a': {dummy.x--; break; }
    case 's': {dummy.x++; break; }
    }
    if(HERE!='#') {
    switch(HERE){
    case '.': {HERE = ' '; score++; dots--; break; }
    case 'o': {HERE = ' '; ghedible = GHOSTEDIBLE; break; }
    }
    *player = dummy;
    }
    check_hit(player);
    return(key);
}

int choose_dir(int x, int y) {
    int dir = 0;

    if(player.x>x)
        dir = 3;
    if(player.x<x)
        dir = 1;
    if(player.y>y)
        dir = 2;
    if(ghedible)
        dir = (dir+2)%4;
    return(dir);
}

void move_ghost(struct ghosttype *ghost) {
    struct ghosttype dummy;
    int timeout = 0;
   
    do{
        dummy = *ghost;
        if(timeout || (!rand()%intelligence))
            dummy.dir = rand()%4;
        else
            dummy.dir = choose_dir(dummy.x, dummy.y);
        if(dummy.dir==0) dummy.y--;
        if(dummy.dir==1) dummy.x--;
        if(dummy.dir==2) dummy.y++;
        if(dummy.dir==3) dummy.x++;
    } while(((timeout++<10)&((*ghost).dir == ((dummy.dir+2)%4)))
            | (HERE==GHOSTCHAR) | (HERE=='#'));
    *ghost = dummy;
}

void interrupt[UART_RX_vect] uartRXHandler(void)  
{
    controls(player,UDR);
    return;
}

void pacman()
{
    int i,j;
    char key;

    srand(0); 
    do {
        /* Reset board each time it is cleared */
        for(i=0; i<BOARDY; i++)
            for(j=0; j<BOARDX; j++)
                board[i][j] = start_board[i][j];
        /* One more ghost per board, up to NUMGHOST */
        if(ghostnum<NUMGHOST)
            ghostnum++;
        for(i=0; i<ghostnum; i++)
            ghost[i] = start_ghost[i];
        ghedible = 0;
        dots = NUMDOTS;
        intelligence++;

        /* Repeat until all dots eaten or player dead or quitting */
        do {
            for(i=0; i<4; i++)
                if(ghost[i].dead) {
                    if(!--ghost[i].dead)
                        ghost[i] = resurrected_ghost;
                }
                else
                    move_ghost(&ghost[i]);
            check_hit(&player);
            draw_board();
            if(ghedible) ghedible--;
            key = controls(&player);
        } while(dots && !player.dead && (key!='q'));
    } while(!player.dead & (key!='q'));
    puts_P(game_over);
}
