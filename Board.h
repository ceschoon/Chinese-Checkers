#ifndef BOARD
#define BOARD

#include <vector>
#include <math.h>

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

// generic graph class meant to be inherited
class Board
{
	public: 
		Board() {;}
		vector<Vertex> getVertices() {return vertices_;}
		//virtual double getTotalSizeX() = 0;
		//virtual double getTotalSizeY() = 0;
		//double getTotalSizeX() {return 0;}
		//double getTotalSizeY() {return 0;}
	
	protected:
		vector<Vertex> vertices_;
};

class Hexagram : public Board
{
	public:
		Hexagram(int size) : Board(), size_(size) 
		{
			generateVertices();
			computeNeighbours();
		}
		
		void generateVertices();
		void computeNeighbours();
		
		int getNumPawnsPerTeam(){return size_*(size_-1)/2;}
		double getTotalSizeX() {return 2*sqrt(3)*size_;}
		double getTotalSizeY() {return 2*sqrt(3)*size_;}
		
	protected:
		int size_;
};

#endif