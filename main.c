#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "print.h"
#include "mover.h"
#include "bitops.h"
#include "positions.h"
#include "commitmove.h"
#include "jumper.h"
#include "commitjump.h"
#include "ai.h"



int main (int argc, const char * argv[]) {
	GAME theGame;
	int select;
	int theTime;
	int move;
	int moveCounter = 0;

	
	srand(time(0));
	
	
	printf("press 1 for new game, press 2 for game to test crowning:");
	scanf("%d",&select);

	if (select == 2)
		theGame = game(0x22, 0x41, 0, 'w');
	else
		theGame = newGame();

	theTime = time(0);
	cleanUp(&theGame);

	printGame(&theGame);
	
	
	findJumpersForGame(&theGame);
	findMoversForGame(&theGame);
	printf("found %d moves", theGame.moveCount);
	printf("found %d jumps", theGame.jumpCount);
	

	
	while (theGame.white && theGame.black && (theGame.moveCount || theGame.canJump)) {
		
		
		if (theGame.canJump) {
			move = (rand() % (theGame.jumpCount+1))-1;
			makeJump(move, &theGame);
		}else {
			move = rand() % theGame.moveCount;
			makeMove(move, &theGame);	
		}
		cleanUp(&theGame);
		printf("\n%c turn\n", theGame.turn);
		printGame(&theGame);
		changeTurn(&theGame);
		findJumpersForGame(&theGame);	
		
		if (theGame.canJump) {
			printf("\n%d jumps found\n", theGame.jumpCount+1);
		}else{
			findMoversForGame(&theGame);
			printf("\n%d moves found\n", theGame.moveCount);
		}
		
		
		moveCounter++;
	}
	
	printGame(&theGame);
	
	theTime = time(0) - theTime;
	
	printf("%d, moves in %d seconds\n",moveCounter, theTime );
	
	return 0;
}



//Setsup a new game.
GAME newGame(){
	BITBOARD white = 0x041C71C3;
	BITBOARD black = 0xE3820C38;
	BITBOARD kings = 0x00000000;
	char turn = 'b';
	return game(black, white, kings, turn);
}

GAME game(BITBOARD black, BITBOARD white, BITBOARD kings, char turn){
	
	GAME game;
	
	if (turn != 'w' && turn !='b')
		turn = 'b';
	
	game.black=black;
	game.white=white;
	game.kings=kings;
	game.notOccupied =~(game.white|game.black);
	game.turn=turn;
	game.moveCount=0;
	game.jumpCount=0;
	game.canJump = 0;
	game.blackPieces.piecesCount = 12;
	game.whitePieces.piecesCount = 12;
	piecesInGameForPlayer(&game, 'b');
	piecesInGameForPlayer(&game, 'w');

	
	game.jumpCount = 0;

	
	return game;
}

void changeTurn(PGAME game){

	if ((*game).turn == 'w')
		(*game).turn = 'b';
	else
		(*game).turn = 'w';
	
	
	if ((*game).black & ~(LEGAL_SOUTH_MOVE)){
		(*game).kings = (*game).kings | ((*game).black & ~(LEGAL_SOUTH_MOVE));
	}
	if ((*game).white & ~(LEGAL_NORTH_MOVE)){
		(*game).kings = (*game).kings | ((*game).white & ~(LEGAL_NORTH_MOVE));
	}
	return;
}

void cleanUp (PGAME game){

	(*game).notOccupied =~((*game).white|(*game).black);
	(*game).moveCount = 0;
	(*game).jumpCount = 0;
	(*game).canJump = 0;

	return;
}

int isPieceFriendly (GAME game, BITBOARD position){

	if (game.turn == 'w'){
		if (pieceAtPosition(&game, position) == 'w' || pieceAtPosition(&game, position) == 'W')
			return 1;
	}
	
	if (game.turn == 'b'){
		if (pieceAtPosition(&game, position) == 'b' || pieceAtPosition(&game, position) == 'B')
			return 1;
		}
	return 0;
}

