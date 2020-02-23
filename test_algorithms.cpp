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
	
	int sysresult = system("mkdir -p data analysis");
	ofstream recordFile("data/record.dat");
	ifstream recordInFile("data/record_in.dat");
	ofstream distMovesFile("analysis/distMoves.dat");
	
	///////////////////////////// Parameters ///////////////////////////////
	
	// board
	int numTeams = 6;
	int boardSize = 3;
	
	// analysis
	int numGames = 1000;
	int maxNumMoves = 1000;
	
	// report
	cout << endl;
	cout << "=========== Parameters ============" << endl;
	cout << endl;
	cout << "numTeams = " << numTeams << endl;
	cout << "boardSize = " << boardSize << endl;
	cout << "numGames = " << numGames << endl;
	cout << "maxNumMoves = " << maxNumMoves << endl;
	cout << endl;
	cout << "Algorithm: Hamiltonian \"Target\" with temperature=0.3" << endl;
	
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
				temperature = 0.3;
				algorithmHamiltonian(boardCopy, ipawnToMove, 
				                     ivertexDestination);
			}
			else
			{
				temperature = 0.3;
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
		
		if (iGame%(numGames/10)==0)
			cout << "completed games up to number " << iGame << endl;
	}
	
	//////////////////////// Statistical analysis //////////////////////////
	
	cout << endl;
	cout << "=========== Analysis Results ============" << endl;
	
	///// filter valid games /////
	
	int numGames0 = numGames; numGames = 0;
	vector<int> numMoves0 = numMoves; numMoves.clear();
	
	cout << endl;
	cout << "Number of played games is " << numGames0 << endl;
	
	for (int i=0; i<numGames0; i++)
	{
		// discard if number of moves exceeds a given limit
		if (numMoves0[i]>=maxNumMoves) continue;
		
		numGames++;
		numMoves.push_back(numMoves0[i]);
	}
	
	cout << "Number of valid games is " << numGames << endl;
	cout << "Fraction of invalid games is " << 1-double(numGames)/numGames0 << endl;
	
	///// number of moves (distribution) /////
	
	int maxNMoves = 0;
	for (int i=0; i<numMoves.size(); i++) 
		if (numMoves[i]>maxNMoves) maxNMoves = numMoves[i];
	
	vector<int> frequencies(maxNMoves+1,0);
	for (int i=0; i<numMoves.size(); i++) frequencies[numMoves[i]]++;
	
	distMovesFile << "# numMoves  frequency" << endl;
	for (int i=0; i<maxNMoves; i++) 
		distMovesFile << i << " " << frequencies[i] << endl;
	
	///// gnuplot scipt to plot the distribution /////
	
	ofstream plotFile;
	plotFile.open("analysis/plot_distMoves");
	
	plotFile << "set term svg enhanced mouse #size 600,500" << endl;
	plotFile << "set output 'distMoves.svg'" << endl;
	plotFile << endl;
	plotFile << "set title \"Number of moves to finish the game\" font \",20\"" << endl;
	plotFile << "set xlabel \"number of moves\" font \",20\"" << endl;
	plotFile << "set ylabel \"number of games\" font \",20\"" << endl;
	plotFile << endl;
	plotFile << "set key off" << endl;
	plotFile << endl;
	
	plotFile << "set style histogram" << endl;
	plotFile << "set style fill solid 0.5" << endl;
	plotFile << "plot \"distMoves.dat\" smooth freq with boxes" << endl;
	
	plotFile.close();
	sysresult = system("cd analysis; gnuplot plot_distMoves");
	
	///// number of moves (avg and err) /////
	
	double numMoves_avg = 0;
	double numMoves_std = 0;
	double numMoves_avgerr = 0;
	
	for (int i=0; i<numMoves.size(); i++)
		numMoves_avg += double(numMoves[i])/numGames;
	
	assert(numGames>1);
	for (int i=0; i<numMoves.size(); i++)
		numMoves_std += (numMoves[i]-numMoves_avg)*(numMoves[i]-numMoves_avg);
	numMoves_std = sqrt(numMoves_std/(numGames-1));
	numMoves_avgerr = numMoves_std / sqrt(numGames);
	
	cout << endl;
	cout << "Average number of moves per game is " << numMoves_avg
	     << " +- " << numMoves_avgerr << endl;
	
	///// number of moves (median) ////
	
	vector<int> cumulFrequencies(maxNMoves+1,0);
	for (int i=0; i<frequencies.size(); i++)
		for (int j=i; j<cumulFrequencies.size(); j++)
			cumulFrequencies[j] += frequencies[i];
	
	int numMoves_median = 0;
	for (int i=0; i<cumulFrequencies.size(); i++)
		if (cumulFrequencies[i]<0.5*numGames) numMoves_median = i;
		else break;
	
	cout << "Median number of moves per game is " << numMoves_median << endl;
	
	///// number of moves (05th and 95th percentiles) ////
	
	int numMoves_percentile05 = 0;
	for (int i=0; i<cumulFrequencies.size(); i++)
		if (cumulFrequencies[i]<0.05*numGames) numMoves_percentile05 = i;
		else break;
	
	cout << "05th percentile for the number of moves per game is " 
	     << numMoves_percentile05 << endl;
	
	int numMoves_percentile95 = 0;
	for (int i=0; i<cumulFrequencies.size(); i++)
		if (cumulFrequencies[i]<0.95*numGames) numMoves_percentile95 = i;
		else break;
	
	cout << "95th percentile for the number of moves per game is " 
	     << numMoves_percentile95 << endl;
	
	/////   /////
	
	////////////////////////////////////////////////////////////////////////
	
	return 0;
}


