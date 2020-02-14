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
//    Controls: You can select a pawn by left-clicking on it and place    //
//              it by releasing the mouse above the destination vertex.   //
//              Press "a" to play with the current algorithm in use.      //
//              Press "z" to undo a move. Press "m" to toggle the         //
//              display of available moves for the selected pawn.         //
//              Press "r" to replay a move from the input file "data/     //
//              record_in.dat". A record of each game is automatically    //
//              written to the file "data/record.dat".                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

/// next on the todo list
//	o	compilation for windows
//	o	report on how the game can be played and implementation details
//	o	better algorithms (see algorithm.cpp)

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
	
	system("mkdir -p data");
	ofstream recordFile("data/record.dat");
	ifstream recordInFile("data/record_in.dat");
	
	///////////////////////////// Game board ///////////////////////////////
	
	Hexagram board(6,3);
	
	/////////////////////////////// Window /////////////////////////////////
	
	sf::RenderWindow window(sf::VideoMode(640,640), "Chinese Checkers");
	
	double boardLengthX = board.getTotalSizeX();
	double boardLengthY = board.getTotalSizeY();
	
	sf::View view(sf::FloatRect(-boardLengthX*0.55, -boardLengthY*0.55, 
	                             boardLengthX*1.10,  boardLengthY*1.10));
	window.setView(view);
	
	window.setFramerateLimit(10);
	
	////////////////////////////// Game loop ///////////////////////////////
	
	bool gameEnded = false;
	int pawnSelected = -1;
	int counterMoves = 0;
	bool showAvailableMoves = false;
	
	// board save at each move
	vector<Hexagram> boardSaves(1,board);
	
	// seed from algorithm.cpp
	cout << "seed = " << seed << endl;
	
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
			
			// undo when key 'Z' is pressed
			if (event.type == sf::Event::KeyPressed && 
				event.key.code == sf::Keyboard::Z && !gameEnded)
			{
				if (boardSaves.size()>0)
				{
					// reset to last board saved
					board = boardSaves.back();
					boardSaves.pop_back();
					
					counterMoves --;
					recordFile << "Undo" << endl;
				}
			}
			
			// toggle show available moves when key 'M' is pressed
			if (event.type == sf::Event::KeyPressed && 
				event.key.code == sf::Keyboard::M && !gameEnded)
			{
				showAvailableMoves = !showAvailableMoves;
			}
			
			// make a move using an algorithm when key 'A' is pressed
			if (event.type == sf::Event::KeyPressed && 
				event.key.code == sf::Keyboard::A && !gameEnded)
			{
				// save the board before making changes
				Hexagram boardSave = board;
				
				// decide move to perform
				// we copy the board to prevent the algorithm from making
				// changes
				int ipawnToMove = -1;
				int ivertexDestination = -1;
				Hexagram boardCopy = board;
				algorithm(boardCopy, ipawnToMove, ivertexDestination);
				
				// place selected pawn
				int status = board.move(ipawnToMove, ivertexDestination, recordFile);
				if (status == 0) 
				{
					counterMoves ++;
					boardSaves.push_back(boardSave);
					
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
			}
			
			// select pawn by pressing mouse left click
			if (event.type == sf::Event::MouseButtonPressed && !gameEnded)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					// coordinates of mouse in graph
					sf::Vector2i windowCoords(event.mouseButton.x,
					                          event.mouseButton.y);
					double graphX = window.mapPixelToCoords(windowCoords).x;
					double graphY = window.mapPixelToCoords(windowCoords).y;
					
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
					if (pawns[pawnSelected].getTeam() != 
					    board.getPlayingTeam()) 
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
					// coordinates of mouse in graph
					sf::Vector2i windowCoords(event.mouseButton.x,
					                          event.mouseButton.y);
					double graphX = window.mapPixelToCoords(windowCoords).x;
					double graphY = window.mapPixelToCoords(windowCoords).y;
					
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
					Hexagram boardSave = board;
					
					// place selected pawn
					int status = board.move(pawnSelected, ivertexMin, recordFile);
					if (status == 0) 
					{
						counterMoves ++;
						boardSaves.push_back(boardSave);
						
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
			
			
			
			// replay from file when key 'R' is pressed
			if (event.type == sf::Event::KeyPressed && 
				event.key.code == sf::Keyboard::R && !gameEnded)
			{
				if (recordInFile)
				{
					string line = "";
					getline(recordInFile,line);
					
					if (line.substr(0,4) == "Move")
					{
						#ifdef DEBUG
						cout << "--- Replay move ---" << endl;
						cout << "line = \"" << line << "\"" << endl;
						#endif
						
						// string is formatted as "Move from vertex <num> to <num>"
						// find position of the first number in string
						int posFrom = 0;
						for (int i=4; i<line.size(); i++)
							if (line[i-4] == 't' && line[i-3] == 'e' &&
								line[i-2] == 'x' && line[i-1] == ' ') posFrom = i;
						// find position of the second number in string
						int posTo = 0;
						for (int i=posFrom; i<line.size(); i++)
							if (line[i-4] == ' ' && line[i-3] == 't' &&
								line[i-2] == 'o' && line[i-1] == ' ') posTo = i;
						
						// identify move
						int ivertexFrom = stoi(line.substr(posFrom,posTo-4-posFrom));
						int ivertexTo = stoi(line.substr(posTo,line.size()-posTo));
						int ipawn = board.getPawnFromVertex(ivertexFrom);
						
						#ifdef DEBUG
						cout << "identified move from vertex " << ivertexFrom 
							 << " to " << ivertexTo << endl;
						#endif
						
						// save the board before making changes
						Hexagram boardSave = board;
						
						// place selected pawn
						int status = board.move(ipawn, ivertexTo, recordFile);
						if (status == 0) 
						{
							counterMoves ++;
							boardSaves.push_back(boardSave);
							
							#ifdef DEBUG
							cout << "*** Board print ***" << endl;
							board.print();
							cout << "*******************" << endl;
							#endif
						}
						else 
						{
							cout << "Move to replay is not valid, error code "
								 << status << endl;
						}
					}
					else if (line == "Undo")
					{
						#ifdef DEBUG
						cout << "--- Replay undo ---" << endl;
						cout << "line = \"" << line << "\"" << endl;
						#endif
						
						if (boardSaves.size()>0)
						{
							// reset to last board saved
							board = boardSaves.back();
							boardSaves.pop_back();
							
							counterMoves --;
							recordFile << "Undo" << endl;
						}
						else 
						{
							cout << "Impossible to replay undo" << endl;
						}
					}
					else
					{
						#ifdef DEBUG
						cout << "--- Replay line not recognised ---" << endl;
						cout << "line = \"" << line << "\"" << endl;
						#endif
					}
				}
			}
		}
		
		/////////////////////////// Rendering //////////////////////////////
		
		window.clear(sf::Color::White);
		
		renderHomes(window,board);
		renderBoardEdges(window,board);
		renderBoardVertices(window,board);
		
		#ifdef DEBUG
		renderTextVertices(window,board);
		#endif
		
		renderPawns(window,board,pawnSelected);
		
		if (pawnSelected >= 0) 
			renderSelectedPawn(window,board,pawnSelected);
		if (pawnSelected >= 0 && showAvailableMoves) 
			renderAvailableMoves(window,board,pawnSelected);
		
		renderWinners(window,board);
		
		window.display();
		
		///////////////////////////// Misc. ////////////////////////////////
		
		// detect end of the game
		if (board.getPlayingTeam()<0) gameEnded = true;
		
		////////////////////////////////////////////////////////////////////
	}
	
	////////////////////////////////////////////////////////////////////////
	
	return 0;
}


