#include <math.h>
#include <iostream>
#include "Board.h"

using namespace std;





// Generation of the vertices in the hexagram-shaped board.
// The method is explained in the algorithm.

void Hexagram::generateVertices()
{
	// Hexagram is two equilateral triangle in each other (one upside down)
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





// Computation of direct neighbours. The method is to search for vertices
// at distance 1 from the current vertex.

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





// Computation of second neighbours. The strategy used here is to look 
// within the neighbours' neighbours which ones are aligned in the direct 
// neighbour's direction. 

void Hexagram::computeNeighbours2()
{
	for (int i=0; i<vertices_.size(); i++)
	{
		vector<int> neighboursi1 = vertices_[i].getNeighbours();
		
		// prepare neighbours2 vector
		// index -1 is the default, it indicates that there is no second
		// neighbours behind the corresponding first one.
		vector<int> neighboursi2_new(neighboursi1.size(),-1);
		
		for (int m=0; m<neighboursi1.size(); m++)
		{
			int j = neighboursi1[m];
			vector<int> neighboursj1 = vertices_[j].getNeighbours();
			
			for (int n=0; n<neighboursj1.size(); n++)
			{
				int k = neighboursj1[n];
				
				// vertex i position
				double xi = vertices_[i].getX();
				double yi = vertices_[i].getY();
				
				// vertex j position
				double xj = vertices_[j].getX();
				double yj = vertices_[j].getY();
				
				// vertex k position
				double xk = vertices_[k].getX();
				double yk = vertices_[k].getY();
				
				// alignment check
				// test if the vector from i to j is the same as the one
				// from j to k.
				double small_number = 1e-8;
				double xdiff = abs((xj-xi)-(xk-xj));
				double ydiff = abs((yj-yi)-(yk-yj));
				if (xdiff<small_number && ydiff<small_number)
					// store index k as second neighbour of i behind j
					neighboursi2_new[m] = k;
			}
		}
		
		vertices_[i].setNeighbours2(neighboursi2_new);
	}
}





// Placing of pawns on the graph of vertices
// The placement depends on the number of teams. We divide the problem into
// placing one team at the time. We repeat according to the number of teams.

void Hexagram::placePawnsOnVertices()
{
	// clear vertex to pawn association
	
	vertexToPawn_.empty();
	for (int i=0; i<vertices_.size(); i++)
		vertexToPawn_.push_back(-1);
	
	pawnToVertex_.empty();
	for (int i=0; i<pawns_.size(); i++)
		pawnToVertex_.push_back(-1);
	
	// place pawns according to number of teams
	
	if (nTeams_==1) 
	{
		placeTeamOnBranch(0,0);
	}
	else if (nTeams_==2) 
	{
		placeTeamOnBranch(0,0);
		placeTeamOnBranch(1,3);
	}
	else if (nTeams_==3) 
	{
		placeTeamOnBranch(0,0);
		placeTeamOnBranch(1,2);
		placeTeamOnBranch(2,4);
	}
	else if (nTeams_==4) 
	{
		placeTeamOnBranch(0,0);
		placeTeamOnBranch(0,1);
		placeTeamOnBranch(0,3);
		placeTeamOnBranch(0,4);
	}
	else if (nTeams_==6) 
	{
		placeTeamOnBranch(0,0);
		placeTeamOnBranch(1,1);
		placeTeamOnBranch(2,2);
		placeTeamOnBranch(3,3);
		placeTeamOnBranch(4,4);
		placeTeamOnBranch(5,5);
	}
}

void Hexagram::placeTeamOnBranch(int team, int branch)
{
	for (int j=0; j<pawns_.size(); j++)
	{
		Pawn pawn = pawns_[j];
		
		// check if pawn in right team
		if (pawn.getTeam()!=team) continue;
		
		// find a free vertex on the right branch
		// we identify the branch with a line that seperates the branch from
		// the rest of the haxegram
		
		for (int i=0; i<vertices_.size(); i++)
		{
			// check if vertex free
			if (vertexToPawn_[i]>=0) continue;
			
			// check if vertex on the right branch, the condition is branch-
			// dependent
			double x = vertices_[i].getX();
			double y = vertices_[i].getY();
			double small_number = 1e-8;
			
			if (branch==0)
			{
				double a = 0;
				double b = -sqrt(3)/2*size_;
				
				if (y>a*x+b-small_number) continue;
			}
			else if (branch==1)
			{
				double a = sqrt(3);
				double b = -sqrt(3)*size_;
				
				if (y>a*x+b-small_number) continue;
			}
			else if (branch==2)
			{
				double a = -sqrt(3);
				double b = sqrt(3)*size_;
				
				if (y<a*x+b+small_number) continue;
			}
			else if (branch==3)
			{
				double a = 0;
				double b = sqrt(3)/2*size_;
				
				if (y<a*x+b+small_number) continue;
			}
			else if (branch==4)
			{
				double a = sqrt(3);
				double b = sqrt(3)*size_;
				
				if (y<a*x+b+small_number) continue;
			}
			else if (branch==5)
			{
				double a = -sqrt(3);
				double b = -sqrt(3)*size_;
				
				if (y>a*x+b-small_number) continue;
			}
			
			// if we arrive at this stage, this means we can place the pawn
			// at the current vertex
			
			vertexToPawn_[i] = j;
			pawnToVertex_[j] = i;
			
			break;
		}
	}
}





