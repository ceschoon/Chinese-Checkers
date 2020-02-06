#ifndef ALGORITHM
#define ALGORITHM

#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include "Board.h"

using namespace std;


///////////////////////////// Declarations /////////////////////////////////


// Random number generator
random_device true_gen;
int seed = true_gen();
default_random_engine gen(seed);
uniform_real_distribution<double> dist01(0,1);

// Algorithms
void randomMove(Board &board, int &ipawnToMove, int &ivertexDestination);
void bestMove0MinSum(Board &board, int &ipawnToMove, int &ivertexDestination);
void bestMove0MinFree(Board &board, int &ipawnToMove, int &ivertexDestination);


// generic algorithm function used to redirect to other ones
void algorithm(Board &board, int &ipawnToMove, int &ivertexDestination)
{
	//randomMove(board, ipawnToMove, ivertexDestination);
	//bestMove0MinSum(board, ipawnToMove, ivertexDestination);
	bestMove0MinFree(board, ipawnToMove, ivertexDestination);
}



//////////////////////////// Implementations ///////////////////////////////



class Move
{
	public: 
		Move(int ivertexFrom, int ivertexTo) 
		: ivertexFrom_(ivertexFrom), ivertexTo_(ivertexTo) {;}
		
		int ivertexFrom_;
		int ivertexTo_;
};



void randomMove(Board &board, int &ipawnToMove, int &ivertexDestination)
{
	#ifdef DEBUG
	cout << "--- randomMove algorithm ---" << endl;
	#endif
	
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



// Fit function using the summed distances to the target vertices
int fitDistanceToTargets(Board &board, int ivertexFrom, int ivertexTo,
                         int team)
{
	vector<Vertex> vertices = board.getVertices();
	vector<int> targets = board.getTargetOfTeam(team);
	
	int distance1 = 0;
	for (int itarget : targets)
		distance1 += board.distance(vertices[ivertexFrom], vertices[itarget]);
		
	int distance2 = 0;
	for (int itarget : targets)
		distance2 += board.distance(vertices[ivertexTo], vertices[itarget]);
	
	return distance1-distance2;
}

// Fit function using the distance to a free target vertex
// Tweaked to limit moves from a target vertex 
int fitDistanceToFreeTarget(Board &board, int ivertexFrom, int ivertexTo,
                            int team)
{
	vector<Vertex> vertices = board.getVertices();
	vector<int> targets = board.getTargetOfTeam(team);
	
	// devaluate moves from a target vertex 
	for (int itarget : targets) if (itarget == ivertexFrom) return 0;
	
	// find the free targets
	vector<int> freeTargets;
	for (int itarget : targets)
		if (board.getPawnFromVertex(itarget)<0)
			freeTargets.push_back(itarget);
	
	// choose a free target randomly
	// chose a occupied one if none are free (e.g. start of the game)
	int itargetChosen;
	if (freeTargets.size()>0)
		itargetChosen = freeTargets[int(dist01(gen)*freeTargets.size())];
	else
		itargetChosen = targets[int(dist01(gen)*targets.size())];
	
	// distances to free target
	int distance1 = 0;
	for (int itarget : targets)
		distance1 += board.distance(vertices[ivertexFrom], vertices[itargetChosen]);
	int distance2 = 0;
	for (int itarget : targets)
		distance2 += board.distance(vertices[ivertexTo], vertices[itargetChosen]);
		
	return distance1-distance2;
}



// Choose best move looking 0 steps ahead (immediate best move)
// this one tries to minimise the summed distances to the target vertices
void bestMove0MinSum(Board &board, int &ipawnToMove, int &ivertexDestination)
{
	#ifdef DEBUG
	cout << "--- bestMove0MinSum algorithm ---" << endl;
	#endif
	
	vector<Pawn> pawns = board.getPawns();
	vector<Move> moves;
	int pteam = board.getPlayingTeam();
	
	// compute available moves
	for (int ipawn=0; ipawn<pawns.size(); ipawn++)
	{
		if (pawns[ipawn].getTeam() != pteam) continue;
		
		int ivertexFrom = board.getVertexFromPawn(ipawn);
		
		for (int ivertexTo: board.availableMovesDirect(ivertexFrom))
			moves.push_back(Move(ivertexFrom, ivertexTo));
		for (int ivertexTo: board.availableMovesHopping(ivertexFrom))
			moves.push_back(Move(ivertexFrom, ivertexTo));
	}
	
	// initialise best move
	Move moveBest = moves[0];
	int bestFit = fitDistanceToTargets(board, moveBest.ivertexFrom_, 
	                                   moveBest.ivertexTo_, pteam);
	
	for (Move move : moves)
	{
		int fit = fitDistanceToTargets(board, move.ivertexFrom_,
		                               move.ivertexTo_, pteam);
		
		#ifdef DEBUG
		cout << "move from " << move.ivertexFrom_
		     << " to " << move.ivertexTo_ << " : "
		     << "fit = " << fit << endl;
		#endif
		
		if (fit > bestFit) 
		{
			moveBest = move;
			bestFit = fit;
		}
		// to introduce a bit of randomness
		else if (fit == bestFit && dist01(gen)<0.5)
		{
			moveBest = move;
			bestFit = fit;
		}
	}
	
	// return best move
	ipawnToMove = board.getPawnFromVertex(moveBest.ivertexFrom_);
	ivertexDestination = moveBest.ivertexTo_;
}


// Choose best move looking 0 steps ahead (immediate best move)
// this one tries to minimise the distance to a free target vertex
void bestMove0MinFree(Board &board, int &ipawnToMove, int &ivertexDestination)
{
	#ifdef DEBUG
	cout << "--- bestMove0MinFree algorithm ---" << endl;
	#endif
	
	vector<Pawn> pawns = board.getPawns();
	vector<Move> moves;
	int pteam = board.getPlayingTeam();
	
	// compute available moves
	for (int ipawn=0; ipawn<pawns.size(); ipawn++)
	{
		if (pawns[ipawn].getTeam() != pteam) continue;
		
		int ivertexFrom = board.getVertexFromPawn(ipawn);
		
		for (int ivertexTo: board.availableMovesDirect(ivertexFrom))
			moves.push_back(Move(ivertexFrom, ivertexTo));
		for (int ivertexTo: board.availableMovesHopping(ivertexFrom))
			moves.push_back(Move(ivertexFrom, ivertexTo));
	}
	
	// initialise best move
	Move moveBest = moves[0];
	int bestFit = fitDistanceToFreeTarget(board, moveBest.ivertexFrom_,
	                                      moveBest.ivertexTo_, pteam);
	
	for (Move move : moves)
	{
		int fit = fitDistanceToFreeTarget(board, move.ivertexFrom_, 
		                                  move.ivertexTo_, pteam);
		
		#ifdef DEBUG
		cout << "move from " << move.ivertexFrom_
		     << " to " << move.ivertexTo_ << " : "
		     << "fit = " << fit << endl;
		#endif
		
		if (fit > bestFit) 
		{
			moveBest = move;
			bestFit = fit;
		}
		// to introduce a bit of randomness
		else if (fit == bestFit && dist01(gen)<0.5)
		{
			moveBest = move;
			bestFit = fit;
		}
	}
	
	// return best move
	ipawnToMove = board.getPawnFromVertex(moveBest.ivertexFrom_);
	ivertexDestination = moveBest.ivertexTo_;
}



#endif