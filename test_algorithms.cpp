////////////////////////////////////////////////////////////////////////////
//                                                                        //
//                         * Chinese Checkers *                           //
//                                                                        //
//    Author: Cédric Schoonen <cedric.schoonen1@gmail.com>                //
//    February 2020                                                       //
//                                                                        //
//    Developped under Ubuntu 18.04 with g++ 7.4.0 and sfml 2.4           //
//    Compile with $ g++ -o chinese_checkers main.cpp Board.h Board.cpp \ //
//                   -lsfml-graphics -lsfml-window -lsfml-system          //
//                                                                        //
//    This file is used for testing algorithms by repeated plays.         //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

/// next on the todo list
//	o	better algorithms (see algorithm.cpp)
//	o	proper tests for the algorithms using different scenarios and 
//		a good statistical analysis
//		->	game alone, vs itself (6 copies), vs other algorithms

/// current minor problems:
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <thread>
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "rendering.cpp"
#include "algorithm.cpp"

using namespace std;

int main()
{
	/////////////////////////////// Files //////////////////////////////////
	
	int sysresult = system("mkdir -p data");
	ofstream recordFile("data/record.dat");
	ifstream recordInFile("data/record_in.dat");
	
	///////////////////////////// Parameters ///////////////////////////////
	
	// board
	int numTeams = 6;
	int boardSize = 3;
	
	// analysis
	int numGames = 100;
	int maxNumMoves = 1000;
	
	////////////////////////////// Game loop ///////////////////////////////
	
	cout << endl;
	cout << "=========== Simulation of games ============" << endl;
	cout << endl;
	
	// seed from algorithm.cpp
	cout << "seed = " << seed << endl;
	
	// analysis variables
	vector<int> numMoves(numGames,0);
	
	for (int iGame=0; iGame<numGames; iGame++)
	{
		Hexagram board(numTeams, boardSize);
		
		// variables to control the game
		bool gameEnded = false;
		int pawnSelected = -1;
		
		// variables for the analysis
		int counterMoves = 0;
		
		while (!gameEnded && counterMoves<maxNumMoves)
		{
			//////////////////////// Make one move /////////////////////////
			
			int ipawnToMove = -1;
			int ivertexDestination = -1;
			int pteam = board.getPlayingTeam();
			
			// use a copy to prevent the algorithm from making changes
			Hexagram boardCopy = board;
			
			// decide move to perform using an algorithm specific to the team
			if (pteam==0)
			{
				temperature = 0.1;
				algorithmHamiltonian(boardCopy, ipawnToMove, 
				                     ivertexDestination);
			}
			else
			{
				temperature = 0.1;
				algorithmHamiltonian(boardCopy, ipawnToMove, 
				                     ivertexDestination);
			}
			
			// place selected pawn
			int status = board.move(ipawnToMove, ivertexDestination, recordFile);
			if (status == 0) 
			{
				counterMoves ++;
			}
			else 
			{
				cout << "Move is not valid, error code "
					 << status << endl;
			}
			
			////////////////////////////////////////////////////////////////
			
			// detect end of the game
			if (board.getPlayingTeam()<0) gameEnded = true;
		}
		
		// after game analysis
		numMoves[iGame] = counterMoves;
		
		cout << "completed game number " << iGame << endl;
	}
	
	//////////////////////// Statistical analysis //////////////////////////
	
	cout << endl;
	cout << "=========== Analysis Results ============" << endl;
	
	///// filter valid games /////
	
	cout << endl;
	cout << "Number of played games is " << numGames << endl;
	
	int numGames0 = numGames; numGames = 0;
	vector<int> numMoves0 = numMoves; numMoves.clear();
	
	for (int i=0; i<numGames0; i++)
	{
		// discard if number of moves exceeds a given limit
		if (numMoves0[i]>=maxNumMoves) continue;
		
		numGames++;
		numMoves.push_back(numMoves0[i]);
	}
	
	cout << "Number of valid games is " << numGames << endl;
	
	///// number of moves (distribution) /////
	
	cout << endl;
	cout << "Distribution of the number of moves per game: " << endl;
	
	for (int i=0; i<numMoves.size(); i++)
		cout << numMoves[i] << " ";
	cout << endl;
	
	///// number of moves (avg and err) /////
	
	double numMoves_avg = 0;
	double numMoves_std = 0;
	double numMoves_err = 0;
	
	for (int i=0; i<numMoves.size(); i++)
		numMoves_avg += double(numMoves[i])/numGames;
	
	assert(numGames>1);
	for (int i=0; i<numMoves.size(); i++)
		numMoves_std += (numMoves[i]-numMoves_avg)*(numMoves[i]-numMoves_avg);
	numMoves_std = sqrt(numMoves_std/(numGames-1));
	numMoves_err = numMoves_std / sqrt(numGames);
	
	cout << endl;
	cout << "Average number of moves per game is " << numMoves_avg
	     << " +- " << numMoves_err << endl;
	
	/////   /////
	
	cout << endl;
	
	////////////////////////////////////////////////////////////////////////
	
	return 0;
}


