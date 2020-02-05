#ifndef ALGORITHM
#define ALGORITHM

#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include "Board.h"

using namespace std;


// Random number generator
random_device true_gen;
int seed = true_gen();
default_random_engine gen(seed);
uniform_real_distribution<double> dist01(0,1);

// Algorithms
void randomMove(Board board, int &ipawnToMove, int &ivertexDestination);




// generic algorithm function used to redirect to other ones
void algorithm(Board board, int &ipawnToMove, int &ivertexDestination)
{
	randomMove(board, ipawnToMove, ivertexDestination);
	//bestMove0(board, ipawnToMove, ivertexDestination);
}



class Move
{
	public: 
		Move(int ivertexFrom, int ivertexTo) 
		: ivertexFrom_(ivertexFrom), ivertexTo_(ivertexTo) {;}
		
		int ivertexFrom_;
		int ivertexTo_;
};



void randomMove(Board board, int &ipawnToMove, int &ivertexDestination)
{
	vector<Pawn> pawns = board.getPawns();
	vector<Move> moves;
	
	// compute available moves
	for (int ipawn=0; ipawn<pawns.size(); ipawn++)
	{
		if (pawns[ipawn].getTeam() != board.getPlayingTeam()) continue;
		
		int ivertexFrom = board.getVertexFromPawn(ipawn);
		
		for (int ivertexTo: board.availableMovesDirect(ivertexFrom))
			moves.push_back(Move(ivertexFrom, ivertexTo));
		for (int ivertexTo: board.availableMovesHopping(ivertexFrom))
			moves.push_back(Move(ivertexFrom, ivertexTo));
	}
	
	// choose move
	Move moveChosen = moves[int(moves.size()*dist01(gen))];
	ipawnToMove = board.getPawnFromVertex(moveChosen.ivertexFrom_);
	ivertexDestination = moveChosen.ivertexTo_;
}


// choose best move looking 0 steps ahead (immediate best move)
// TODO: need a way to evaluate how well a pawn advance in the right direction
//       currently, we copy the board object and evaluate the progress gain
//       following the move, but this is very expensive computationally
/*
void bestMove0(Board board, int &ipawnToMove, int &ivertexDestination)
{
	vector<Pawn> pawns = board.getPawns();
	vector<Move> moves;
	
	// compute available moves
	for (int ipawn=0; ipawn<pawns.size(); ipawn++)
	{
		if (pawns[ipawn].getTeam() != board.getPlayingTeam()) continue;
		
		int ivertexFrom = board.getVertexFromPawn(ipawn);
		
		for (int ivertexTo: board.availableMovesDirect(ivertexFrom))
			moves.push_back(Move(ivertexFrom, ivertexTo));
		for (int ivertexTo: board.availableMovesHopping(ivertexFrom))
			moves.push_back(Move(ivertexFrom, ivertexTo));
	}
	
	// initialise best move
	Move moveBest = moves[0];
	double bestProgress;
	Board board2 = board;
	board2.move();
	
	for (Move move : moves)
	{
		
	}
}
*/



#endif