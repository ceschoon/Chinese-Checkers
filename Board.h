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
		vector<int> neighbours2_;
};





class Pawn
{
	public: 
		Pawn(int team) : team_(team) {;}
		
		int getTeam() {return team_;}
	
	protected:
		int team_;
};





// generic graph class meant to be inherited
class Board
{
	public: 
		Board(int nTeams, int nPawnsPerTeam) : nTeams_(nTeams)
		{
			// create pawns
			for (int i=0; i<nTeams; i++)
				for (int j=0; j<nPawnsPerTeam; j++)
					pawns_.push_back(Pawn(i));
			
			// contruct graph
			generateVertices();
			computeNeighbours();
			computeNeighbours2();
			
			// place pawns on graph
			attributeHomeToTeams();
			attributeTargetToTeams();
			placePawnsOnVertices();
		}
		
		vector<Vertex> getVertices() {return vertices_;}
		vector<Pawn> getPawns() {return pawns_;}
		
		int getVertexFromPawn(int ipawn) {return pawnToVertex_[ipawn];}
		int getPawnFromVertex(int ivertex) {return vertexToPawn_[ivertex];}
		
		vector<int> getHomeOfTeam(int team) {return homes_[team];}
		vector<int> getTargetOfTeam(int team) {return targets_[team];}
		
		int distance(Vertex vertex1, Vertex vertex2) {return -1;} // to override
		double progressFromDistance(int team);
		
		int move(int ipawn, int ivertex);
		vector<int> availableMovesDirect(int ivertex);
		vector<int> availableMovesHopping(int ivertex, int ivertexForbidden=-1);
	
	protected:
		// construction of graph (to override)
		// -> Rewrite the computation of neighbours with distance and 
		//    alignment functions?
		void generateVertices() {;}
		void computeNeighbours() {;}
		void computeNeighbours2() {;}
		
		// place pawns on graph (to override)
		void attributeHomeToTeams() {;}
		void attributeTargetToTeams() {;}
		void placePawnsOnVertices() {;}
		void checkPawnPlacement();
		
		// member variables
		int nTeams_;
		vector<Vertex> vertices_;
		vector<Pawn> pawns_;
		vector<int> pawnToVertex_;    // index of pawn at given vertex
		vector<int> vertexToPawn_;
		vector<vector<int>> homes_;   // list of home vertices for each team
		vector<vector<int>> targets_;
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
			
			// contruct graph
			generateVertices();
			computeNeighbours();
			computeNeighbours2();
			
			// place pawns on graph
			attributeHomeToTeams();
			attributeTargetToTeams();
			placePawnsOnVertices();
			
			#ifdef DEBUG
			cout << "== Checking pawn placement in Hexagram class constructor ==" << endl;
			checkPawnPlacement();
			#endif
		}
		
		int getNumPawnsPerTeam(){return size_*(size_+1)/2;}
		double getTotalSizeX() {return 2*sqrt(3)*size_;}
		double getTotalSizeY() {return 2*sqrt(3)*size_;}
		vector<int> verticesOnBranch(int branch);
		int distance(Vertex vertex1, Vertex vertex2);
		
	protected:
		// construction of graph
		void generateVertices();
		void computeNeighbours();
		void computeNeighbours2();
		
		// place pawns on graph
		void attributeHomeToTeams();
		void attributeTargetToTeams();
		void placePawnsOnVertices();
		
		// member variables
		int size_;
};





#endif