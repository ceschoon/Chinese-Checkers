////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    Implementation file for algorithms that play chinese checkers.      //
//                                                                        //
//    Author: Cédric Schoonen <cedric.schoonen1@gmail.com>                //
//    February 2020                                                       //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

//	Ideas for better algorithms
//	o	fitness term to keep pawns close to each others
//	o	for convex graphs/hexagram, fitness term to go away from the center


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

// class for typical move
class Move
{
	public: 
		Move(int ivertexFrom, int ivertexTo) 
		: ivertexFrom_(ivertexFrom), ivertexTo_(ivertexTo) {;}
		
		int ivertexFrom_;
		int ivertexTo_;
		double weight_;
};

// Algorithms (basic)
void randomMove(Board&, int&, int&);
void bestMove0MinSum(Board&, int&, int&);
void bestMove0MinFree(Board&, int&, int&);

// Algorithms (hamiltonian family)
void algorithmHamiltonian(Board &board, int &ipawnToMove, int &ivertexDestination);
const double temperature = 0.3;
double hamiltonianTarget(Board&, Move);
double hamiltonian(Board &board, Move move)
{
	return hamiltonianTarget(board, move);
}

// generic algorithm function used to redirect to other ones
void algorithm(Board &board, int &ipawnToMove, int &ivertexDestination)
{
	//randomMove(board, ipawnToMove, ivertexDestination);
	//bestMove0MinSum(board, ipawnToMove, ivertexDestination);
	//bestMove0MinFree(board, ipawnToMove, ivertexDestination);
	algorithmHamiltonian(board, ipawnToMove, ivertexDestination);
}



//////////////////////////// Implementations ///////////////////////////////




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
double fitDistanceToTargets(Board &board, int ivertexFrom, int ivertexTo,
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
double fitDistanceToFreeTarget(Board &board, int ivertexFrom, int ivertexTo,
                               int team)
{
	vector<Vertex> vertices = board.getVertices();
	vector<int> targets = board.getTargetOfTeam(team);
	
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
	
	// devaluate moves from a target vertex 
	for (int itarget : targets) if (itarget == ivertexFrom) 
		return 1.0/3*(distance1-distance2);
	
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
	double bestFit = fitDistanceToTargets(board, moveBest.ivertexFrom_, 
	                                      moveBest.ivertexTo_, pteam);
	
	for (Move move : moves)
	{
		double fit = fitDistanceToTargets(board, move.ivertexFrom_,
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
		// TODO: uniform density because first moves in the list are 
		//       less probable with current scheme, which leads to a
		//       struggle at the end and a bias towards players that
		//       have the favorable vertex order.
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
	double bestFit = fitDistanceToFreeTarget(board, moveBest.ivertexFrom_,
	                                         moveBest.ivertexTo_, pteam);
	
	for (Move move : moves)
	{
		double fit = fitDistanceToFreeTarget(board, move.ivertexFrom_, 
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
		// TODO: uniform density because first moves in the list are 
		//       less probable with current scheme, which leads to a
		//       struggle at the end and a bias towards players that
		//       have the favorable vertex order.
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






//////////////////////////// Hamiltonian Family ////////////////////////////


double hamiltonianTarget(Board& board, Move move)
{
	vector<Vertex> vertices = board.getVertices();
	vector<int> targets = board.getBestTargets();
	int pteam = board.getPlayingTeam();
	
	// this is a method for convex graphs, check if it applies this one
	assert(targets[pteam]>=0);
	
	// distances to best target
	int distance1 = board.distance(vertices[move.ivertexFrom_], 
	                               vertices[targets[pteam]]);
	int distance2 = board.distance(vertices[move.ivertexTo_], 
	                               vertices[targets[pteam]]);
	
	double energy = distance2-distance1;
	
	return energy;
}




void algorithmHamiltonian(Board &board, int &ipawnToMove, int &ivertexDestination)
{
	#ifdef DEBUG
	cout << "--- generic hamiltonian algorithm ---" << endl;
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
	
	// compute weight of each move
	double sumWeights = 0;
	for (int i=0; i<moves.size(); i++)
	{
		double energy = hamiltonian(board, moves[i]);
		moves[i].weight_ = exp(-energy/temperature);
		sumWeights += moves[i].weight_;
		
		#ifdef DEBUG
		cout << "move from " << moves[i].ivertexFrom_
		     << " to " << moves[i].ivertexTo_ << " : "
		     << "energy = " << energy 
		     << " weight = " << moves[i].weight_ << endl;
		#endif
	}
	
	// select move to perform
	double ran = dist01(gen);
	double cumulatedProba = 0;
	for (Move move : moves)
	{
		cumulatedProba += move.weight_/sumWeights;
		if (ran < cumulatedProba) 
		{
			ipawnToMove = board.getPawnFromVertex(move.ivertexFrom_);
			ivertexDestination = move.ivertexTo_;
			
			#ifdef DEBUG
			cout << "selected move from " << move.ivertexFrom_
				 << " to " << move.ivertexTo_ << endl;
			#endif
			
			break;
		}
	}
}





#endif