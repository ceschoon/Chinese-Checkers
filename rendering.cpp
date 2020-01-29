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

#endif