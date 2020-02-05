#ifndef BOARD
#define BOARD

#include <iostream>
#include <fstream>
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
			
			// other
			playingTeam_ = 0;
			winningOrder_ = vector<int>(nTeams,-1);
		}
		
		int getNTeams() {return nTeams_;}
		int getPlayingTeam() {return playingTeam_;}
		
		// vertices and pawns
		vector<Vertex> getVertices() {return vertices_;}
		vector<Pawn> getPawns() {return pawns_;}
		
		// vertex to pawn relation
		int getVertexFromPawn(int ipawn) {return pawnToVertex_[ipawn];}
		int getPawnFromVertex(int ivertex) {return vertexToPawn_[ivertex];}
		
		// homes and targets
		vector<int> getHomeOfTeam(int team) {return homes_[team];}
		vector<int> getTargetOfTeam(int team) {return targets_[team];}
		vector<int> getWinningOrder() {return winningOrder_;}
		vector<int> teamsOnTarget();
		
		// geometry
		virtual int distance(Vertex vertex1, Vertex vertex2) {return -1;}
		virtual bool aligned(Vertex vertex1, Vertex vertex2, 
		                     Vertex vertex3) {return false;}
		double progressFromDistance(int team);
		
		// moves
		int move(int ipawn, int ivertex, ofstream &recordFile);
		vector<int> availableMovesDirect(int ivertex);
		vector<int> availableMovesHopping(int ivertex);
		vector<int> availableMovesHopping(int ivertex, 
		                                  vector<int> &ivertexForbidden);
		
		void print();
	
	protected:
		// construction of graph (to override)
		virtual void generateVertices() {;}
		virtual void attributeHomeToTeams() {;}
		virtual void attributeTargetToTeams() {;}
		
		// place pawns on graph
		void placePawnsOnVertices(); // should not be used before making graph
		void checkPawnPlacement();
		
		// computation of neighbours
		void computeNeighbours();
		void computeNeighbours2();
		
		// playing order subroutines
		void nextPlayingTeam();
		void prevPlayingTeam();
		
		// member variables
		int nTeams_;
		int playingTeam_;
		vector<Vertex> vertices_;
		vector<Pawn> pawns_;
		vector<int> pawnToVertex_;    // index of pawn at given vertex
		vector<int> vertexToPawn_;
		vector<vector<int>> homes_;   // list of home vertices for each team
		vector<vector<int>> targets_;
		vector<int> winningOrder_;
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
		
		int getNumPawnsPerTeam() {return size_*(size_+1)/2;}
		double getTotalSizeX() {return 2*sqrt(3)*size_;}
		double getTotalSizeY() {return 2*sqrt(3)*size_;}
		
		// geometry (override of virtual board functions)
		int distance(Vertex vertex1, Vertex vertex2);
		bool aligned(Vertex vertex1, Vertex vertex2, Vertex vertex3);
		
	protected:
		// construction of graph (override of virtual board functions)
		void generateVertices();
		vector<int> verticesOnBranch(int branch);
		void attributeHomeToTeams();
		void attributeTargetToTeams();
		
		// member variables
		int size_;
};





#endif