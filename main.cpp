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
//	o	distances list, or virtual somehow //TODO
//	o	develop algorithm class with trivial diffusion algorithm
//	o	random forward algorithm
//	o	current best move algorithm
//	o	statistics (number of moves, advancement tracking, game record)

//// Manual mode:
//	o	Possibility to save and re-load the game
//	o	indication of the winning order

/// current minor problems:
//	o	window resizing incorrect

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <thread>
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "rendering.cpp"

using namespace std;

void checkMoves(ofstream &recordFile);

int main()
{
	/////////////////////////////// Files //////////////////////////////////
	
	system("mkdir -p data");
	ofstream recordFile("data/record.dat");
	
	///////////////////////////// Game board ///////////////////////////////
	
	Hexagram board(6,3);
	
	/////////////////////////////// Window /////////////////////////////////
	
	sf::RenderWindow window(sf::VideoMode(640,640),"Chinese Checkers");
	
	////////////////////////////// Game loop ///////////////////////////////
	
	bool gameEnded = false;
	int pawnSelected = -1;
	int counterMoves = 0;
	int playingTeam = 0;
	
	Hexagram boardSave = board;
	bool undoAvailable = false;
	
	while (window.isOpen())
	{
		/////////////////////// Event processing ///////////////////////////
		
		double scaleX = window.getSize().x/board.getTotalSizeX()*0.9;
		double scaleY = window.getSize().y/board.getTotalSizeY()*0.9;
		
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			
			// undo when key 'Z' is pressed
			if (event.type == sf::Event::KeyPressed && 
				event.key.code == sf::Keyboard::Z && !gameEnded)
			{
				if (undoAvailable)
				{
					board = boardSave;
					counterMoves --;
					playingTeam = board.prevPlayingTeam(playingTeam);
					undoAvailable = false;
					recordFile << "Undo" << endl;
				}
			}
			
			// select pawn by pressing mouse left click
			if (event.type == sf::Event::MouseButtonPressed && !gameEnded)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					// coordinates in graph
					double graphX = (double(event.mouseButton.x) 
					                - window.getSize().x/2) / scaleX;
					double graphY = (double(event.mouseButton.y) 
					                - window.getSize().y/2)/ scaleY;
					
					// find the closest vertex
					vector<Vertex> vertices = board.getVertices();
					int ivertexMin = -1;
					double distanceMin = -1;
					for (int i=0; i<vertices.size(); i++)
					{
						double x = vertices[i].getX();
						double y = vertices[i].getY();
						
						double d = sqrt((graphX-x)*(graphX-x)
						               +(graphY-y)*(graphY-y));
						
						if (d<distanceMin || ivertexMin<0) 
						{
							ivertexMin = i;
							distanceMin = d;
						}
					}
					
					// find selected pawn
					pawnSelected = board.getPawnFromVertex(ivertexMin);
					
					#ifdef DEBUG
					cout << "---- Mouse left click pressed ---" << endl;
					cout << "mouse graphX = " << graphX << endl;
					cout << "mouse graphY = " << graphY << endl;
					cout << "vertexSelected = " << ivertexMin << endl;
					cout << "pawnSelected = " << pawnSelected << endl;
					cout << "available moves for selected pawn = ";
					for (int ivertexAvail : board.availableMovesDirect(ivertexMin))
						cout << ivertexAvail << " ";
					for (int ivertexAvail : board.availableMovesHopping(ivertexMin))
						cout << ivertexAvail << " ";
					cout << endl;
					#endif
					
					// check if selected pawn is in the right team
					vector<Pawn> pawns = board.getPawns();
					if (pawns[pawnSelected].getTeam() != playingTeam) 
					{
						pawnSelected = -1;
						cout << "Selected pawn not in the right team" << endl;
					}
				}
			}
			
			// place selected pawn by releasing mouse left click
			if (event.type == sf::Event::MouseButtonReleased && !gameEnded)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					// coordinates in graph
					double graphX = (double(event.mouseButton.x) 
					                - window.getSize().x/2) / scaleX;
					double graphY = (double(event.mouseButton.y) 
					                - window.getSize().y/2)/ scaleY;
					
					// find the closest vertex
					vector<Vertex> vertices = board.getVertices();
					int ivertexMin = -1;
					double distanceMin = -1;
					for (int i=0; i<vertices.size(); i++)
					{
						double x = vertices[i].getX();
						double y = vertices[i].getY();
						
						double d = sqrt((graphX-x)*(graphX-x)
						               +(graphY-y)*(graphY-y));
						
						if (d<distanceMin || ivertexMin<0) 
						{
							ivertexMin = i;
							distanceMin = d;
						}
					}
					
					#ifdef DEBUG
					cout << "---- Mouse left click released ---" << endl;
					cout << "mouse graphX = " << graphX << endl;
					cout << "mouse graphY = " << graphY << endl;
					cout << "vertexSelected = " << ivertexMin << endl;
					#endif
					
					// save the board before making changes
					Hexagram boardSave2 = board;
					
					// place selected pawn
					int status = board.move(pawnSelected, ivertexMin, recordFile);
					if (status == 0) 
					{
						counterMoves ++;
						playingTeam = board.nextPlayingTeam(playingTeam);
						boardSave = boardSave2;
						undoAvailable = true;
						
						#ifdef DEBUG
						cout << "*** Board print ***" << endl;
						board.print();
						cout << "*******************" << endl;
						#endif
					}
					else 
					{
						cout << "Move is not valid, error code "
						     << status << endl;
					}
					
					// clear selected pawn
					pawnSelected = -1;
				}
			}
		}
		
		/////////////////////////// Rendering //////////////////////////////
		
		window.clear(sf::Color::White);
		renderBoardEdges(window,board,scaleX,scaleY);
		renderBoardVertices(window,board,scaleX,scaleY);
		#ifdef DEBUG
		renderTextVertices(window,board,scaleX,scaleY);
		#endif
		renderPawns(window,board,scaleX,scaleY,pawnSelected);
		renderSelectedPawn(window,board,scaleX,scaleY,pawnSelected);
		renderWinners(window,board,scaleX,scaleY);
		window.display();
		
		///////////////////////////// Misc. ////////////////////////////////
		
		// pause to not use all cpu usage
		this_thread::sleep_for(std::chrono::milliseconds(100));
		
		// detect end of the game
		if (playingTeam<0) gameEnded = true;
		
		////////////////////////////////////////////////////////////////////
	}
	
	////////////////////////////////////////////////////////////////////////
	
	return 0;
}


