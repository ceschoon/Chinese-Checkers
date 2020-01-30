#ifndef BOARD
#define BOARD

#include <iostream>
#include <vector>
#include <math.h>
#include <assert.h>

using namespace std;





class Vertex
{
	public: 
		Vertex(double x, double y) : x_(x), y_(y) {;}
		
		double getX() {return x_;}
		double getY() {return y_;}
		vector<int> getNeighbours() {return neighbours_;}
		vector<int> getNeighbours2() {return neighbours2_;}
		
		void setNeighbours(vector<int> neighbours) {neighbours_ = neighbours;}
		void setNeighbours2(vector<int> neighbours2) {neighbours2_ = neighbours2;}
	
	protected:
		double x_;
		double y_;
		vector<int> neighbours_;
		vector<int> weights_;
		vector<int> neighbours2_;
		vector<int> weights2_;
};





class Pawn
{
	public: 
		Pawn(int team) : team_(team) {;}
		
		//Vertex getVertex() {return vertex_;}
		int getTeam() {return team_;}
		
		//void setVertex(Vertex vertex) {vertex_ = vertex;}
	
	protected:
		//Vertex vertex_;
		int team_;
		//int home_; // identifier for home location
		//int target_; // identifier for target location
};





// generic graph class meant to be inherited
class Board
{
	public: 
		Board(int nTeams, int nPawnsPerTeam) : nTeams_(nTeams)
		{
			for (int i=0; i<nTeams; i++)
				for (int j=0; j<nPawnsPerTeam; j++)
					pawns_.push_back(Pawn(i));
		}
		
		vector<Vertex> getVertices() {return vertices_;}
		vector<Pawn> getPawns() {return pawns_;}
		
		int getVertexFromPawn(int ipawn) {return pawnToVertex_[ipawn];}
		int getPawnFromVertex(int ivertex) {return vertexToPawn_[ivertex];}
	
	protected:
		int nTeams_;
		vector<Vertex> vertices_;
		vector<Pawn> pawns_;
		vector<int> pawnToVertex_; // index of pawn at vertex
		vector<int> vertexToPawn_;
};




class Hexagram : public Board
{
	public:
		Hexagram(int nTeams, int size)
		: Board(nTeams, size*(size+1)/2), size_(size) 
		{
			// check correct number of teams (1,2,3,4,6)
			assert(nTeams>0);
			assert(nTeams<=6);
			assert(nTeams!=5);
			
			generateVertices();
			computeNeighbours();
			computeNeighbours2();
			placePawnsOnVertices();
		}
		
		void generateVertices();
		void computeNeighbours();
		void computeNeighbours2();
		void placePawnsOnVertices();
		
		int getNumPawnsPerTeam(){return size_*(size_+1)/2;}
		double getTotalSizeX() {return 2*sqrt(3)*size_;}
		double getTotalSizeY() {return 2*sqrt(3)*size_;}
		
	protected:
		void placeTeamOnBranch(int, int);
		
		int size_;
};





#endif