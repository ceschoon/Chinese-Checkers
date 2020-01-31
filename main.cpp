////////////////////////////////////////////////////////////////////////////
//                                                                        //
//                         * Chinese Checkers *                           //
//                                                                        //
//    Author: Cédric Schoonen <cedric.schoonen1@gmail.com>                //
//    February 2020                                                       //
//                                                                        //
//    Developped under Ubuntu 18.04 with g++ 7.4.0 and sfml 2.4           //
//    Compile with $ g++ -o chinese_checkers main.cpp \                   //
//                   Algorithm.h Algorithm.cpp Board.h Board.cpp \        //
//                   -lsfml-graphics -lsfml-window -lsfml-system          //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

/// next on the todo list
//	o	develop algorithm class with trivial diffusion algorithm
//	o	end game detection
//	o	random forward algorithm
//	o	current best move algorithm
//	o	statistics (number of moves, advancement tracking, game record)

//// Alternative play modes:
//	o	Manual game, pawns moved with mouse

/// current minor problems:
//	o	window resizing incorrect

#include <iostream>
#include <fstream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "rendering.cpp"

using namespace std;

void checkMoves();

int main()
{
	///////////////////////////// Game board ///////////////////////////////
	
	Hexagram board(6,3);
	
	#ifdef DEBUG
	cout << "== Checking moves and moves detection ==" << endl;
	checkMoves();
	#endif
	
	/////////////////////////////// Window /////////////////////////////////
	
	sf::RenderWindow window(sf::VideoMode(640,640),"Chinese Checkers");
	
	////////////////////////////// Game loop ///////////////////////////////
	
	bool gameEnded = false;
	
	while (window.isOpen())
	{
		/////////////////////// Event processing ///////////////////////////
		
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
		
		double scaleX = window.getSize().x/board.getTotalSizeX()*0.9;
		double scaleY = window.getSize().y/board.getTotalSizeY()*0.9;
		
		/////////////////////////// Rendering //////////////////////////////
		
		window.clear(sf::Color::White);
		renderBoardEdges(window,board,scaleX,scaleY);
		renderBoardVertices(window,board,scaleX,scaleY);
		//renderTextVertices(window,board,scaleX,scaleY);
		renderPawns(window,board,scaleX,scaleY);
		window.display();
	}
	
	////////////////////////////////////////////////////////////////////////
	
	return 0;
}





void checkMoves()
{
	Hexagram board(6,3);
	
	int ivertex = 5;
	int ipawn = board.getPawnFromVertex(ivertex);
	
	cout << "available direct moves from vertex " << ivertex << ": " << endl;
	for (int ivertex2 : board.availableMovesDirect(ivertex))
		cout << ivertex2 << " ";
	cout << endl;
	
	cout << "available hopping moves from vertex " << ivertex << ": " << endl;
	for (int ivertex2 : board.availableMovesHopping(ivertex))
		cout << ivertex2 << " ";
	cout << endl;
	
	ipawn = board.getPawnFromVertex(ivertex);
	cout << "move pawn from vertex " << ivertex;
	ivertex = 8;
	cout << " to vertex " << ivertex << ": ";
	cout << board.move(ipawn,ivertex) << endl;
	
	ipawn = board.getPawnFromVertex(ivertex);
	cout << "move pawn from vertex " << ivertex;
	ivertex = 11;
	cout << " to vertex " << ivertex << ": ";
	cout << board.move(ipawn,ivertex) << endl;
	
	ivertex = 3;
	cout << "available direct moves from vertex " << ivertex << ": " << endl;
	for (int ivertex2 : board.availableMovesDirect(ivertex))
		cout << ivertex2 << " ";
	cout << endl;
	
	cout << "available hopping moves from vertex " << ivertex << ": " << endl;
	for (int ivertex2 : board.availableMovesHopping(ivertex))
		cout << ivertex2 << " ";
	cout << endl;
}


