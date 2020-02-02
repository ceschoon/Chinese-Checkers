#include <iostream>
#include <math.h>
#include "Board.h"

using namespace std;

const double PI = 3.14159265358979;





// Generation of the vertices in the hexagram-shaped board.
// The method is explained in the algorithm.

void Hexagram::generateVertices()
{
	// Hexagram is two equilateral triangle in each other (one upside down)
	// Start with first triangle
	
	// Begin the first triangle with one edge
	// From the first edge, draw diagonals starting from each vertex to 
	// the second edge
	
	#ifdef DEBUG
	cout << "--- Generation of vertices ---" << endl;
	#endif
	
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
	#ifdef DEBUG
	cout << "--- First neighbours computation ---" << endl;
	#endif
	
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
	#ifdef DEBUG
	cout << "--- Second neighbours computation ---" << endl;
	#endif
	
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






// we identify the branch with a line that seperates the branch from
// the rest of the hexagram

vector<int> Hexagram::verticesOnBranch(int branch)
{
	#ifdef DEBUG
	cout << "--- Computing vertices on branch ---" << endl;
	#endif
	
	vector<int> verticesSelected;
	
	for (int i=0; i<vertices_.size(); i++)
	{
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
		
		// if we arrive at this stage, this means that the vertex is on the
		// right branch
		
		verticesSelected.push_back(i);
	}
	
	return verticesSelected;
}





void Hexagram::attributeHomeToTeams()
{
	#ifdef DEBUG
	cout << "--- Attributiong homes to teams ---" << endl;
	#endif
	
	// clear
	homes_.empty();
	
	// attribute homes according to number of teams
	if (nTeams_==1) 
	{
		homes_.push_back(verticesOnBranch(0));
	}
	else if (nTeams_==2) 
	{
		homes_.push_back(verticesOnBranch(0));
		homes_.push_back(verticesOnBranch(3));
	}
	else if (nTeams_==3) 
	{
		homes_.push_back(verticesOnBranch(0));
		homes_.push_back(verticesOnBranch(2));
		homes_.push_back(verticesOnBranch(4));
	}
	else if (nTeams_==4) 
	{
		homes_.push_back(verticesOnBranch(0));
		homes_.push_back(verticesOnBranch(1));
		homes_.push_back(verticesOnBranch(3));
		homes_.push_back(verticesOnBranch(4));
	}
	else if (nTeams_==6) 
	{
		homes_.push_back(verticesOnBranch(0));
		homes_.push_back(verticesOnBranch(1));
		homes_.push_back(verticesOnBranch(2));
		homes_.push_back(verticesOnBranch(3));
		homes_.push_back(verticesOnBranch(4));
		homes_.push_back(verticesOnBranch(5));
	}
}






void Hexagram::attributeTargetToTeams()
{
	#ifdef DEBUG
	cout << "--- Attributing targets to teams ---" << endl;
	#endif
	
	// clear
	targets_.empty();
	
	// attribute homes according to number of teams
	if (nTeams_==1) 
	{
		targets_.push_back(verticesOnBranch(3));
	}
	else if (nTeams_==2) 
	{
		targets_.push_back(verticesOnBranch(3));
		targets_.push_back(verticesOnBranch(0));
	}
	else if (nTeams_==3) 
	{
		targets_.push_back(verticesOnBranch(3));
		targets_.push_back(verticesOnBranch(5));
		targets_.push_back(verticesOnBranch(1));
	}
	else if (nTeams_==4) 
	{
		targets_.push_back(verticesOnBranch(3));
		targets_.push_back(verticesOnBranch(4));
		targets_.push_back(verticesOnBranch(0));
		targets_.push_back(verticesOnBranch(1));
	}
	else if (nTeams_==6) 
	{
		targets_.push_back(verticesOnBranch(3));
		targets_.push_back(verticesOnBranch(4));
		targets_.push_back(verticesOnBranch(5));
		targets_.push_back(verticesOnBranch(0));
		targets_.push_back(verticesOnBranch(1));
		targets_.push_back(verticesOnBranch(2));
	}
}






void Hexagram::placePawnsOnVertices()
{
	#ifdef DEBUG
	cout << "--- Placing pawns on vertices ---" << endl;
	#endif
	
	// clear vertex to pawn association
	
	vertexToPawn_.empty();
	for (int i=0; i<vertices_.size(); i++)
		vertexToPawn_.push_back(-1);
	
	pawnToVertex_.empty();
	for (int i=0; i<pawns_.size(); i++)
		pawnToVertex_.push_back(-1);
	
	// place pawns on their home vertices
	
	for (int i=0; i<pawns_.size(); i++)
	{
		int team = pawns_[i].getTeam();
		vector<int> homeVertices = homes_[team];
		
		for (int j=0; j<homeVertices.size(); j++)
		{
			// check if vertex free
			if (vertexToPawn_[homeVertices[j]]>=0) continue;
			
			// if it is, place pawn
			vertexToPawn_[homeVertices[j]] = i;
			pawnToVertex_[i] = homeVertices[j];
			break;
		}
	}
}






void Board::checkPawnPlacement()
{
	#ifdef DEBUG
	cout << "--- Checking pawn placement ---" << endl;
	#endif
	
	// check home and target attribution
	cout << "-----------------" << endl;
	for (int team=0; team<nTeams_; team++)
	{
		cout << "team=" << team << endl;
		for (int vertex: homes_[team]) cout << vertex << " ";
		cout << endl;
		for (int vertex: targets_[team]) cout << vertex << " ";
		cout << endl;
	}
	
	// check vertices to pawn association
	cout << "-----------------" << endl;
	cout << "from vertices=" << " ";
	for (int vertex=0; vertex<vertices_.size(); vertex++) 
		cout << vertex << " ";
	cout << endl;
	cout << "to pawns=" << " ";
	for (int vertex=0; vertex<vertices_.size(); vertex++)
		cout << vertexToPawn_[vertex] << " ";
	cout << endl;
	cout << "from pawns=" << " ";
	for (int pawn=0; pawn<pawns_.size(); pawn++) 
		cout << pawn << " ";
	cout << endl;
	cout << "to vertices=" << " ";
	for (int pawn=0; pawn<pawns_.size(); pawn++)
		cout << pawnToVertex_[pawn] << " ";
	cout << endl;
}





double angle(double x1, double y1, double x2, double y2)
{
	double angle12;
	if ((x2-x1)==0 && (y2-y1)>0) angle12 = 90;
	else if ((x2-x1)==0 && (y2-y1)<0) angle12 = -90;
	else if ((x2-x1)>0) angle12 = atan((y2-y1)/(x2-x1)) * 180/PI;
	else if ((x2-x1)<0) angle12 = atan((y2-y1)/(x2-x1)) * 180/PI + 180;
	while (angle12<0) angle12+=360;
	while (angle12>=360) angle12-=360;
	
	return angle12;
}

// On the triangular lattice of the hexagram, the distance is the minimum
// number of steps vertex to vertex. These steps can be taken along three
// axes (lines with angles 0,60,120). We can easily see that the shortest
// path is taken by moving along the lines which angle is closest to the
// angle between the two points.

int Hexagram::distance(Vertex vertex1, Vertex vertex2)
{
	#ifdef DEBUG
	cout << "--- Computation of distance in Hexagram ---" << endl;
	#endif
	
	double small_number = 1e-8;
	
	// positions of vertices
	double x1 = vertex1.getX();
	double y1 = vertex1.getY();
	double x2 = vertex2.getX();
	double y2 = vertex2.getY();
	
	// compute angle and find the axis with closest angle
	double angle12 = angle(x1,y1,x2,y2);
	double angleAxis1 = int(angle12/60)*60.0;
	double angleAxis2 = int(angle12/60)*60.0+60.0;
	
	// basis vectors for steps along the two axis
	double x3 = cos(angleAxis1*PI/180)*1.0;
	double y3 = sin(angleAxis1*PI/180)*1.0;
	double x4 = cos(angleAxis2*PI/180)*1.0;
	double y4 = sin(angleAxis2*PI/180)*1.0;
	
	// find number of steps in each direction
	double det = x3*y4-x4*y3;
	double coord3 = 1/det*( y4*(x2-x1) - x4*(y2-y1));
	double coord4 = 1/det*(-y3*(x2-x1) + x3*(y2-y1));
	int numSteps3 = int(abs(coord3)+small_number);
	int numSteps4 = int(abs(coord4)+small_number);
	
	return numSteps3+numSteps4;
}




// Sum the distances between home and target vertices. Also compute the 
// current distance travelled by the pawns and return the fraction of the 
// two distances.

double Board::progressFromDistance(int team)
{
	#ifdef DEBUG
	cout << "--- Computing progress from distance ---" << endl;
	#endif
	
	// vertices
	vector<int> homeVertices = homes_[team];
	vector<int> targetVertices = targets_[team];
	vector<int> pawnVertices;
	
	// compute vertices of the right team's pawns
	for (int i=0; i<pawns_.size(); i++)
		if (pawns_[i].getTeam()==team)
			pawnVertices.push_back(pawnToVertex_[i]);
	
	// compute the total distance between home and target
	int distHomeToTarget = 0;
	for (int i=0; i<homeVertices.size(); i++)
		distHomeToTarget += distance(vertices_[homeVertices[i]], 
		                             vertices_[targetVertices[i]]);
	
	// compute the total distance between home and pawns
	int distHomeToPawns = 0;
	for (int i=0; i<homeVertices.size(); i++)
		distHomeToTarget += distance(vertices_[homeVertices[i]], 
		                             vertices_[pawnVertices[i]]);
	
	return double(distHomeToPawns)/distHomeToTarget;
}




// Move a Pawn to a detination Vertex.
// Returns 0 and performs the move if it is valid
// Returns 1 if the move is incorrect
// Returns 2 if the pawn or vertex doesn't exist
// Returns 3 if the pawn's team has already finished the game

int Hexagram::move(int ipawn, int ivertex, ofstream &recordFile)
{
	#ifdef DEBUG
	cout << "--- Move in Hexagram ---" << endl;
	#endif
	
	// Check if pawn and vertex exist
	if (ipawn >= pawns_.size()) return 2;
	if (ivertex >= vertices_.size()) return 2;
	
	// Check if team has not already finished the game
	int team = pawns_[ipawn].getTeam();
	vector<int> teamsDone = teamsOnTarget();
	for (int team2 : teamsDone)
		if (team == team2) return 3;
	
	// Check if move is a valid direct move
	int ivertexCurrent = pawnToVertex_[ipawn];
	bool inList1 = false;
	for (int ivertex2 : availableMovesDirect(ivertexCurrent))
	{
		if (ivertex2==ivertex)
		{
			inList1 = true;
			break;
		}
	}
	
	// Check if move is a valid hopping move
	bool inList2 = false;
	for (int ivertex2 : availableMovesHopping(ivertexCurrent))
	{
		if (ivertex2==ivertex)
		{
			inList2 = true;
			break;
		}
	}
	if (!inList1 && !inList2) return 1;
	
	// If we arrive to this point, then the move is valid
	// We thus perform the move
	vertexToPawn_[ivertexCurrent] = -1;
	vertexToPawn_[ivertex] = ipawn;
	pawnToVertex_[ipawn] = ivertex;
	
	// Recompute neighbours, etc.
	// TODO: nothing else ?
	// TODO: do not recompute all neighbours
	computeNeighbours();
	computeNeighbours2();
	
	// Record move in file
	recordFile << "Move from vertex " << ivertexCurrent << " to " 
	           << ivertex << endl;
	
	return 0;
}




// List of all possible direct moves from given vertex (no hopping)

vector<int> Board::availableMovesDirect(int ivertex)
{
	#ifdef DEBUG
	cout << "--- Computaing available direct moves ---" << endl;
	#endif
	
	vector<int> destinations;
	
	// Add all free neighbours
	for (int ivertex2 : vertices_[ivertex].getNeighbours())
		if (vertexToPawn_[ivertex2]<0) destinations.push_back(ivertex2);
	
	return destinations;
}





// List of all possible moves by hopping, from a given vertex.
// Note that this is a recursive search. This is why we need to forbid the
// selection of the vertex that we are coming from.

vector<int> Board::availableMovesHopping(int ivertex, 
            vector<int> &ivertexForbidden)
{
	#ifdef DEBUG
	if (ivertexForbidden.size()==0)
		cout << "--- Computing available hopping moves ---" << endl;
	else
		cout << "recursion from " 
		     << ivertexForbidden[ivertexForbidden.size()-1] << endl;
	#endif
	
	// list no available moves if the vertex is forbidden
	// otherwise, add the vertex to the forbidden ones
	bool isForbidden = false;
	for (int ivertex2 : ivertexForbidden)
	{
		if (ivertex == ivertex2) 
		{
			isForbidden = true;
			break;
		}
	}
	vector<int> empty;
	if (isForbidden) return empty;
	else ivertexForbidden.push_back(ivertex);
	
	vector<int> destinations;
	vector<int> neighbours = vertices_[ivertex].getNeighbours();
	vector<int> neighbours2 = vertices_[ivertex].getNeighbours2();
	
	// Add all second neighbours and their own available hopping moves
	for (int i=0; i<neighbours.size(); i++)
	{
		int ivertex1 = neighbours[i];
		int ivertex2 = neighbours2[i];
		
		if (vertexToPawn_[ivertex1]>=0 && vertexToPawn_[ivertex2]<0)
		{
			destinations.push_back(ivertex2);
			
			// recursion
			for (int ivertex3 : availableMovesHopping(ivertex2, ivertexForbidden))
				destinations.push_back(ivertex3);
		}
	}
	
	// Clear multiplicities
	vector<int> destinations2;
	for (int ivertex : destinations)
	{
		bool alreadyIn = false;
		for (int ivertex2 : destinations2)
			if (ivertex2 == ivertex) alreadyIn = true;
		if (!alreadyIn) destinations2.push_back(ivertex);
	}
	
	return destinations;
}

vector<int> Board::availableMovesHopping(int ivertex)
{
	vector<int> empty;
	
	return availableMovesHopping(ivertex, empty);
}




vector<int> Board::teamsOnTarget()
{
	#ifdef DEBUG
	cout << "--- Computing teams on target ---" << endl;
	#endif
	
	vector<int> teamsOnTarget_;
	
	for (int team=0; team<nTeams_; team++)
	{
		bool allTargetVerticesFilled = true;
		vector<int> iverticesTarget = getTargetOfTeam(team);
		
		for (int ivertex : iverticesTarget)
		{
			int ipawn = vertexToPawn_[ivertex];
			Pawn pawn = pawns_[ipawn];
			if (pawn.getTeam() != team) 
			{
				allTargetVerticesFilled = false;
				break;
			}
		}
		
		if (allTargetVerticesFilled) teamsOnTarget_.push_back(team);
	}
	
	return teamsOnTarget_;
}




