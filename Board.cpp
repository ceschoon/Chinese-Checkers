#include <math.h>
#include <iostream>
#include "Board.h"

using namespace std;

void Hexagram::generateVertices()
{
	// Hexagram is txo equilateral triangle in each other (one upside down)
	// Start with first triangle
	
	// Begin the first triangle with one edge
	// From the first edge, draw diagonals starting from each vertex to 
	// the second edge
	
	vertices_.clear();
	double posX = - (1+1.0/2) * size_;
	double posY = - sqrt(3)/2 * size_;
	double dX = 1.0/2;
	double dY = sqrt(3)/2;
	
	for (int i=0; i<=size_*3; i++)
	{
		// vertices on the first edge
		Vertex vertex_new(posX+i*dX, posY+i*dY);
		vertices_.push_back(vertex_new);
		
		double posX2 = posX+i*dX;
		double posY2 = posY+i*dY;
		double dX2 = 1.0/2;
		double dY2 = -sqrt(3)/2;
		
		// vertices on diagonals starting from first edge's vertices
		for (int j=1; j<=i; j++)
		{
			Vertex vertex_new(posX2+j*dX2, posY2+j*dY2);
			vertices_.push_back(vertex_new);
		}
		
	}
	
	// Add the remaining vertices from the second triangle in three steps,
	// one for each sub-tringle out of the first big one
	
	// First sub-triangle
	
	posX = - (1+1.0/2) * size_;
	posY = + sqrt(3)/2 * size_;
	dX = 1.0/2;
	dY = -sqrt(3)/2;
	
	for (int i=0; i<size_; i++)
	{
		// vertices on the first edge
		Vertex vertex_new(posX+i*dX, posY+i*dY);
		vertices_.push_back(vertex_new);
		
		double posX2 = posX+i*dX;
		double posY2 = posY+i*dY;
		double dX2 = 1.0/2;
		double dY2 = sqrt(3)/2;
		
		// vertices on diagonals starting from first edge's vertices
		for (int j=1; j<=i; j++)
		{
			Vertex vertex_new(posX2+j*dX2, posY2+j*dY2);
			vertices_.push_back(vertex_new);
		}
	}
	
	// Second sub-triangle
	
	posX = + (1+1.0/2) * size_;
	posY = + sqrt(3)/2 * size_;
	dX = -1.0/2;
	dY = -sqrt(3)/2;
	
	for (int i=0; i<size_; i++)
	{
		// vertices on the first edge
		Vertex vertex_new(posX+i*dX, posY+i*dY);
		vertices_.push_back(vertex_new);
		
		double posX2 = posX+i*dX;
		double posY2 = posY+i*dY;
		double dX2 = -1.0/2;
		double dY2 = sqrt(3)/2;
		
		// vertices on diagonals starting from first edge's vertices
		for (int j=1; j<=i; j++)
		{
			Vertex vertex_new(posX2+j*dX2, posY2+j*dY2);
			vertices_.push_back(vertex_new);
		}
	}
	
	// Third sub-triangle
	
	posX = - 1.0/2 * size_;
	posY = - sqrt(3)/2 * size_;
	dX = 1.0/2;
	dY = -sqrt(3)/2;
	
	for (int i=1; i<=size_; i++)
	{
		// vertices on the first edge
		Vertex vertex_new(posX+i*dX, posY+i*dY);
		vertices_.push_back(vertex_new);
		
		double posX2 = posX+i*dX;
		double posY2 = posY+i*dY;
		double dX2 = 1.0/2;
		double dY2 = sqrt(3)/2;
		
		// vertices on diagonals starting from first edge's vertices
		for (int j=1; j<i; j++)
		{
			Vertex vertex_new(posX2+j*dX2, posY2+j*dY2);
			vertices_.push_back(vertex_new);
		}
	}
}







void Hexagram::computeNeighbours()
{
	for (int i=0; i<vertices_.size(); i++)
	{
		vector<int> neighbours;
		
		for (int j=0; j<vertices_.size(); j++)
		{
			if (i==j) continue;
			
			// vertex i position
			double xi = vertices_[i].getX();
			double yi = vertices_[i].getY();
			
			// vertex j position
			double xj = vertices_[j].getX();
			double yj = vertices_[j].getY();
			
			// distance check
			double small_number = 1e-8;
			double d = sqrt((xi-xj)*(xi-xj)+(yi-yj)*(yi-yj));
			if (d>1-small_number && d<1+small_number)
				neighbours.push_back(j);
		}
		
		vertices_[i].setNeighbours(neighbours);
	}
}






/*
void Hexagram::computeNeighbours2()
{
	for (int i=0; i<vertices_.size(); i++)
	{
		vector<int> neighbours;
		
		for (int j=0; j<vertices_.size(); j++)
		{
			if (i==j) continue;
			
			// vertex i position
			double xi = vertices_[i].getX();
			double yi = vertices_[i].getY();
			
			// vertex j position
			double xj = vertices_[j].getX();
			double yj = vertices_[j].getY();
			
			// distance check
			double small_number = 1e-8;
			double d = sqrt((xi-xj)*(xi-xj)+(yi-yj)*(yi-yj));
			if (d>1-small_number && d<1+small_number)
				neighbours.push_back(j);
		}
		
		vertices_[i].setNeighbours(neighbours);
	}
}
*/

