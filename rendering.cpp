#ifndef RENDERING
#define RENDERING

#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/Graphics.hpp>
#include "Board.h"

const double PI = 3.14159265358979;


sf::Color colorOfTeam(int team)
{
	if (team==0)
		return sf::Color::Red;
	else if (team==1)
		return sf::Color::Green;
	else if (team==2)
		return sf::Color::Blue;
	else if (team==3)
		return sf::Color::Yellow;
	else if (team==4)
		return sf::Color::Magenta;
	else if (team==5)
		return sf::Color::Cyan;
	
	// default case
	return sf::Color::White;
}


void renderBoardVertices(sf::RenderWindow &window, Board board)
{
	#ifdef DEBUG_RENDERING
	cout << "--- Rendering board vertices ---" << endl;
	#endif
	
	// vertex shape and colour
	double vertexSize = 0.05;
	sf::CircleShape vertexShape(vertexSize);
	vertexShape.setFillColor(sf::Color::Black);
	
	for (Vertex vertex : board.getVertices())
	{
		// vertex position in window
		double x = vertex.getX() - vertexSize;
		double y = vertex.getY() - vertexSize;
		
		vertexShape.setPosition(sf::Vector2f(x,y));
		
		window.draw(vertexShape);
	}
}





void renderTextVertices(sf::RenderWindow &window, Board board)
{
	#ifdef DEBUG_RENDERING
	cout << "--- Rendering text on vertices ---" << endl;
	#endif
	
	sf::Font font;
	if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"));
	
	// scale difference as text is set in pixels not in board coordinates
	double pixelWidth = window.mapPixelToCoords(sf::Vector2i(0,0)).x
	                  - window.mapPixelToCoords(sf::Vector2i(1,0)).x;
	pixelWidth = abs(pixelWidth);
	
	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color::Red);
	text.setCharacterSize(20);
	text.setScale(pixelWidth,pixelWidth);
	
	vector<Vertex> vertices = board.getVertices();
	
	for (int i=0; i<vertices.size(); i++)
	{
		text.setString(to_string(i));
		text.setPosition(vertices[i].getX(),vertices[i].getY());
		window.draw(text);
	}
}





void renderBoardEdges(sf::RenderWindow &window, Board board)
{
	#ifdef DEBUG_RENDERING
	cout << "--- Rendering board edges ---" << endl;
	#endif
	
	vector<Vertex> vertices = board.getVertices();
	
	for (Vertex vertex : vertices)
	{
		// vertex position in window
		double x = vertex.getX();
		double y = vertex.getY();
		
		vector<int> neighbours = vertex.getNeighbours();
		
		// draw edge for each vertex (will be overlapping but ok)
		for (int neighbour : neighbours)
		{
			Vertex vertex2 = vertices[neighbour];
			
			// neighbour position in window
			double x2 = vertex2.getX();
			double y2 = vertex2.getY();
			
			double d = sqrt((x2-x)*(x2-x)+(y2-y)*(y2-y));
			double angle;
			if ((x2-x)==0 && (y2-y)>0) angle = 90;
			else if ((x2-x)==0 && (y2-y)<0) angle = -90;
			else if ((x2-x)>0) angle = atan((y2-y)/(x2-x)) * 180/PI;
			else if ((x2-x)<0) angle = atan((y2-y)/(x2-x)) * 180/PI + 180;
			
			// difficult to have the same width for all edges when rendered
			//double edgesWidth = 0.02;
			double pixelWidth = window.mapPixelToCoords(sf::Vector2i(0,0)).x
			                  - window.mapPixelToCoords(sf::Vector2i(1,0)).x;
			double edgesWidth = abs(pixelWidth)*2;
			sf::RectangleShape edgeShape(sf::Vector2f(d,edgesWidth));
			edgeShape.setFillColor(sf::Color::Black);
			edgeShape.setRotation(angle);
			
			// correct position to take into account the rectangle width
			double xc = x - edgesWidth/2*cos(PI/180*(angle+90));
			double yc = y - edgesWidth/2*sin(PI/180*(angle+90));
			edgeShape.setPosition(sf::Vector2f(xc,yc));
			
			window.draw(edgeShape);
		}
	}
}





void renderPawns(sf::RenderWindow &window, Board board, int pawnSelected=-1)
{
	#ifdef DEBUG_RENDERING
	cout << "--- Rendering pawns ---" << endl;
	#endif
	
	double pawnSize = 0.2;
	sf::CircleShape pawnShape(pawnSize);
	
	vector<Pawn> pawns = board.getPawns();
	for (int i=0; i<pawns.size(); i++)
	{
		// do not draw selected pawn
		if (i==pawnSelected) continue;
		
		// associated vertex
		int j = board.getVertexFromPawn(i);
		Vertex vertex = board.getVertices()[j];
		
		// pawn position in window
		double x = vertex.getX() - pawnSize;
		double y = vertex.getY() - pawnSize;
		pawnShape.setPosition(sf::Vector2f(x,y));
		
		// color according to team
		pawnShape.setFillColor(colorOfTeam(pawns[i].getTeam()));
		
		window.draw(pawnShape);
	}
}




void renderSelectedPawn(sf::RenderWindow &window, Board board, 
                        int pawnSelected=-1)
{
	#ifdef DEBUG_RENDERING
	cout << "--- Rendering selected pawn ---" << endl;
	#endif
	
	vector<Pawn> pawns = board.getPawns();
	if (pawnSelected<0 && pawnSelected>=pawns.size()) return ;
	
	// pawn shape
	double pawnSize = 0.2;
	sf::CircleShape pawnShape(pawnSize);
	
	// coordinates of mouse in graph
	double graphX = window.mapPixelToCoords(sf::Mouse::getPosition(window)).x;
	double graphY = window.mapPixelToCoords(sf::Mouse::getPosition(window)).y;
	
	// pawn position in window
	double x = graphX - pawnSize;
	double y = graphY- pawnSize;
	pawnShape.setPosition(sf::Vector2f(x,y));
	
	// color according to team
	pawnShape.setFillColor(colorOfTeam(pawns[pawnSelected].getTeam()));
	
	window.draw(pawnShape);
}




void renderWinners(sf::RenderWindow &window, Hexagram board)
{
	#ifdef DEBUG_RENDERING
	cout << "--- Rendering winning order ---" << endl;
	#endif
	
	sf::Font font;
	if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"));

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	
	vector<int> winningOrder = board.getWinningOrder();
	int nTeams = winningOrder.size();
	
	for (int team=0; team<nTeams; team++)
	{
		// print if already won
		if (winningOrder[team]>0)
			text.setString(to_string(winningOrder[team]));
		else
			continue;
		
		// text position in window
		double d = board.getTotalSizeX()/2;
		double angle = PI/180*(360.0/nTeams*team+100);
		double x = d * cos(angle);
		double y = d * sin(angle);
		text.setPosition(x,y);
		
		// color according to team
		text.setFillColor(colorOfTeam(team));
		
		window.draw(text);
	}
}




void renderAvailableMoves(sf::RenderWindow &window, Board board, 
                          int pawnSelected=-1)
{
	#ifdef DEBUG_RENDERING
	cout << "--- Rendering available moves ---" << endl;
	#endif
	
	double vertexSize = 0.05;
	sf::CircleShape vertexShape(vertexSize);
	
	// get pawn's team for color
	vector<Pawn> pawns = board.getPawns();
	int team = pawns[pawnSelected].getTeam();
	vertexShape.setFillColor(colorOfTeam(team));
	
	vector<Vertex> vertices = board.getVertices();
	int ivertex = board.getVertexFromPawn(pawnSelected);
	
	// list available moves
	vector<int> availMoves;
	for (int i : board.availableMovesDirect(ivertex)) 
		availMoves.push_back(i);
	for (int i : board.availableMovesHopping(ivertex)) 
		availMoves.push_back(i);
	
	for (int i : availMoves)
	{
		// vertex position in window
		double x = vertices[i].getX() - vertexSize;
		double y = vertices[i].getY() - vertexSize;
		
		vertexShape.setPosition(sf::Vector2f(x,y));
		window.draw(vertexShape);
	}
}



#endif