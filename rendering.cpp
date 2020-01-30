#ifndef RENDERING
#define RENDERING

#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/Graphics.hpp>
#include "Board.h"

const double PI = 3.14159265358979;





void renderBoardVertices(sf::RenderWindow &window, Board board, 
                         double scaleX, double scaleY)
{
	double windowSizeX = window.getSize().x;
	double windowSizeY = window.getSize().y;
	
	double vertexSize = 0.05;
	sf::CircleShape vertexShape(vertexSize);
	vertexShape.setScale(scaleX,scaleY);
	vertexShape.setFillColor(sf::Color::Black);
	
	for (Vertex vertex : board.getVertices())
	{
		// vertex position in window
		double x = windowSizeX/2 + vertex.getX() * scaleX 
		           - vertexSize * scaleX;
		double y = windowSizeY/2 + vertex.getY() * scaleY 
		           - vertexSize * scaleY;
		
		vertexShape.setPosition(sf::Vector2f(x,y));
		window.draw(vertexShape);
	}
}





void renderTextVertices(sf::RenderWindow &window, Board board, 
                        double scaleX, double scaleY)
{
	double windowSizeX = window.getSize().x;
	double windowSizeY = window.getSize().y;
	
	sf::Font font;
	if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"));

	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color::Red);
	text.setCharacterSize(20);
	
	vector<Vertex> vertices = board.getVertices();
	
	for (int i=0; i<vertices.size(); i++)
	{
		// vertex position in window
		double x = windowSizeX/2 + vertices[i].getX() * scaleX;
		double y = windowSizeY/2 + vertices[i].getY() * scaleY;
		
		text.setString(to_string(i));
		text.setPosition(x,y);
		window.draw(text);
	}
}




void renderBoardEdges(sf::RenderWindow &window, Board board, 
                      double scaleX, double scaleY)
{
	double windowSizeX = window.getSize().x;
	double windowSizeY = window.getSize().y;
	
	vector<Vertex> vertices = board.getVertices();
	
	for (Vertex vertex : vertices)
	{
		// vertex position in window
		double x = windowSizeX/2 + vertex.getX() * scaleX;
		double y = windowSizeY/2 + vertex.getY() * scaleY;
		
		vector<int> neighbours = vertex.getNeighbours();
		
		// draw edge for each vertex (will be overlapping but ok)
		for (int neighbour : neighbours)
		{
			Vertex vertex2 = vertices[neighbour];
			
			// neighbour position in window
			double x2 = windowSizeX/2 + vertex2.getX() * scaleX;
			double y2 = windowSizeY/2 + vertex2.getY() * scaleY;
			
			double d = sqrt((x2-x)*(x2-x)+(y2-y)*(y2-y));
			double angle;
			if ((x2-x)==0 && (y2-y)>0) angle = 90;
			else if ((x2-x)==0 && (y2-y)<0) angle = -90;
			else if ((x2-x)>0) angle = atan((y2-y)/(x2-x)) * 180/PI;
			else if ((x2-x)<0) angle = atan((y2-y)/(x2-x)) * 180/PI + 180;
			
			double edgesWidth = 0.5;
			sf::RectangleShape edgeShape(sf::Vector2f(d,edgesWidth));
			edgeShape.setFillColor(sf::Color::Black);
			edgeShape.setPosition(sf::Vector2f(x,y));
			edgeShape.setRotation(angle);
			
			window.draw(edgeShape);
		}
	}
}






void renderPawns(sf::RenderWindow &window, Board board, 
                 double scaleX, double scaleY)
{
	double windowSizeX = window.getSize().x;
	double windowSizeY = window.getSize().y;
	
	double pawnSize = 0.2;
	sf::CircleShape pawnShape(pawnSize);
	pawnShape.setScale(scaleX,scaleY);
	
	vector<Pawn> pawns = board.getPawns();
	for (int i=0; i<pawns.size(); i++)
	{
		// associated vertex
		int j = board.getVertexFromPawn(i);
		Vertex vertex = board.getVertices()[j];
		
		// pawn position in window
		double x = windowSizeX/2 + vertex.getX() * scaleX 
		           - pawnSize * scaleX;
		double y = windowSizeY/2 + vertex.getY() * scaleY 
		           - pawnSize * scaleY;
		
		pawnShape.setPosition(sf::Vector2f(x,y));
		
		// color according to team
		if (pawns[i].getTeam()==0)
			pawnShape.setFillColor(sf::Color::Red);
		else if (pawns[i].getTeam()==1)
			pawnShape.setFillColor(sf::Color::Green);
		else if (pawns[i].getTeam()==2)
			pawnShape.setFillColor(sf::Color::Blue);
		else if (pawns[i].getTeam()==3)
			pawnShape.setFillColor(sf::Color::Yellow);
		else if (pawns[i].getTeam()==4)
			pawnShape.setFillColor(sf::Color::Magenta);
		else if (pawns[i].getTeam()==5)
			pawnShape.setFillColor(sf::Color::Cyan);
		
		window.draw(pawnShape);
	}
}




#endif