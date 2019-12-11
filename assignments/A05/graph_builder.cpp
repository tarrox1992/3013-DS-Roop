///////////////////////////////////////////////////////////////////////////////
//
// Date:             10 December 2019
// Title:            Program 3 - Graphs
// Semester:         Fall 2019
// Course:           CMPS 3013 
// Assignment:       A05
// Author:           Tyler Roop
// Email:            tarrox16@yahoo.com
// Files:            graph_builder.cpp
//					 cities.json
// Description:
//       This program reads in a file containing information on 1000 cities.
//		 It then uses this information to calculate the D closest cities that
//		 do not already have D number of roads; D being a commmand line perameter 
//		 integer between 1 and 9 to decide how many roads each city should have.
//		 The program then prints the first 10 and last 10 cities on the list and
//		 Their respective roads.
//
/////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#define _USE_MATH_DEFINES

#include <math.h>

using namespace std;

// Edges between cities containing the city travelling to, the distance
// to said city, and the rank on the list of said city
struct Road
{
	string NameTo;
	double Distance;
	int RankTo;
};

// City data type that holds all information about each city
struct City
{
	string Name, State;
	double Growth, Lat, Lon;
	long Pop;
	int Rank, NumRoads;
	Road * Edges;
};

/**
 * ReadIn
 *
 * Description:
 *      Reads data in from a file and places it into the World array
 *
 * Params:
 *      string input : name of input file
 *      City * World : array that holds all 1000 cities
 *		int D : number of roads each city is expected to have
 *
 * Returns:
 *      void
 */
void ReadIn(string Input, City * World, int D)
{
	ifstream Infile;
	Infile.open(Input);
	int i = 1;
	while (Infile >> World[i].Name)
	{
		Infile >> World[i].Growth;
		Infile >> World[i].Lat;
		Infile >> World[i].Lon;
		Infile >> World[i].Pop;
		Infile >> World[i].Rank;
		Infile >> World[i].State;
		World[i].NumRoads = 0;
		World[i].Edges = new Road[D];
		i++;
	}
}

struct Heap
{

	Road * Roads;
	int Max;
	int Size;

	Heap(int Max);

	void Heapify(int i);

	//Method that returns the parent of entered index
	int parent(int i)
	{
		return (i / 2);
	}

	// Method that returns the left child of entered index
	int left(int i)
	{
		return 2 * i;
	}

	// Method that returns the right child of entered index
	int right(int i)
	{
		return (2 * i + 1);
	}

	Road popHeap();

	void Insert(Road a);
};

class Coordinate {
public:
	Coordinate(double latitude, double longitude) : myLatitude(latitude), myLongitude(longitude) {}

	double Latitude() const {
		return myLatitude;
	}

	double Longitude() const {
		return myLongitude;
	}

private:
	double myLatitude;
	double myLongitude;
};

const static double EarthRadiusKm = 6372.8;

/**
 * Swap
 *
 * Description:
 *      Swaps to values in thier original memory locations
 *
 * Params:
 *      Road *t : location 1 (the '*' accesses the location to get the value)
 *      Road *r : locatoin 2
 *
 * Returns:
 *      void
 */
void swap(Road *t, Road *r)
{
	Road temp = *t;
	*t = *r;
	*r = temp;
};

// method that initializes a heap
Heap::Heap(int newMax)
{
	Roads = new Road[newMax];
	Size = 1;
	Max = newMax;
}

// method that is used to insert data into a heap. It first 
// inserts the data at the end and bubbles up to the correct
// index
void Heap::Insert(Road a)
{
	int i = Size;
	Size++;
	Roads[i] = a;

	while (i != 1 && Roads[parent(i)].Distance > Roads[i].Distance)
	{
		swap(&Roads[i], &Roads[parent(i)]);
		i = parent(i);
	}
}

// method that retrieves and deletes the root of the heap
// by switching it with the last value and decrementing the
// size variable. It then calls heapify to rearrange the heap
Road Heap::popHeap()
{
	if (Size <= 1)
	{
		Road Empty{ "", 0, 0 };
		return Empty;
	}

	if (Size == 2)
	{
		Size--;
		return Roads[1];
	}

	Road root = Roads[1];
	Size--;
	Roads[1] = Roads[Size];
	Heapify(1);

	return root;
}

// uses a given index value to bubbledown and sort the heap
void Heap::Heapify(int i)
{
	int l = left(i);
	int r = right(i);
	int shortest = i;
	if (l < Size && Roads[l].Distance < Roads[i].Distance)
		shortest = l;
	if (r < Size && Roads[r].Distance < Roads[i].Distance)
		shortest = r;
	if (shortest != i)
	{
		swap(&Roads[i], &Roads[shortest]);
		Heapify(shortest);
	}
}

inline double DegreeToRadian(double angle) {
	return M_PI * angle / 180.0;
}

double HaversineDistance(const Coordinate &p1, const Coordinate &p2) {
	double latRad1 = DegreeToRadian(p1.Latitude());
	double latRad2 = DegreeToRadian(p2.Latitude());
	double lonRad1 = DegreeToRadian(p1.Longitude());
	double lonRad2 = DegreeToRadian(p2.Longitude());

	double diffLa = latRad2 - latRad1;
	double doffLo = lonRad2 - lonRad1;

	double computation = asin(sqrt(sin(diffLa / 2) * sin(diffLa / 2) + cos(latRad1) * cos(latRad2) * sin(doffLo / 2) * sin(doffLo / 2)));
	return 2 * EarthRadiusKm * computation;
}

/**
 * BuildRoads
 *
 * Description:
 *      Creates roads between all cities
 *
 * Params:
 *      City * World : Array holding cities
 *      int D : number of roads between cities
 *
 * Returns:
 *      void
 */
void BuildRoads(City * World, int D)
{
	// creates heap used to find closest city without full roads
	Heap Paths(1001);

	// parameters to hold road data before being put into City
	Road Walking, NewRoad, RoadFrom;

	for (int i = 1; i < 1001; i++)
	{
		// resets Heap to overwrite data
		Paths.Size = 1;

		// coordinates of current city
		Coordinate This{ World[i].Lat, World[i].Lon };
	
		for (int j = 1; j < 1001; j++)
		{
			// finds distance between current city and all others and
			// uses that data to create roads placed into a min heap
			if (i != j)
			{
				Coordinate That{ World[j].Lat, World[j].Lon };
				Walking.NameTo = World[j].Name;
				Walking.Distance = HaversineDistance(This, That);
				Walking.RankTo = World[j].Rank;
				Paths.Insert(Walking);
			}
		}

		// checks that current city doesn't have max roads
		for (int k = World[i].NumRoads; k < D; k++)
		{
			NewRoad = Paths.popHeap();

			// checks that destination city doesn't have max roads
			if (World[NewRoad.RankTo].NumRoads < D)
			{
				// creates and places roads into current and destination cities
				World[i].Edges[k] = NewRoad;
				World[i].NumRoads++;
				RoadFrom.Distance = NewRoad.Distance;
				RoadFrom.NameTo = World[i].Name;
				RoadFrom.RankTo = World[i].Rank;
				World[NewRoad.RankTo].Edges[World[NewRoad.RankTo].NumRoads] = RoadFrom;
				World[NewRoad.RankTo].NumRoads++;
			}
		}
	}
}

// Prints first 10 and last 10 cities and their respective roads
void Print(City * World)
{
	ofstream outfile;
	outfile.open("output.txt");
	for (int i = 1; i <= 10; i++)
	{
		outfile << setw(4) << World[i].Rank << " : " << World[i].Name << '\n';
		for (int j = 0; j < World[i].NumRoads; j++)
		{
			outfile << "        " << j + 1 << ')' << World[i].Edges[j].NameTo;
			outfile << ' ' << '(' << World[i].Edges[j].Distance << ")\n";
		}
		
	}

	for (int i = 991; i <= 1000; i++)
	{
		outfile << setw(4) << World[i].Rank << " : " << World[i].Name << '\n';
		for (int j = 0; j < World[i].NumRoads; j++)
		{
			outfile << "        " << j + 1 << ')' << World[i].Edges[j].NameTo;
			outfile << ' ' << '(' << World[i].Edges[j].Distance << ")\n";
		}
	}
	
}

int main(int argc, char * argv[])
{
	int D = argv[1][1] - '0';
	City World[1001];
	ReadIn("cities.json", World, D);
	BuildRoads(World, D);
	Print(World);
};
