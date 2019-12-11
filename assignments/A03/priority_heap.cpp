///////////////////////////////////////////////////////////////////////////////
//
// Date:             10 December 2019
// Title:            Program 2 - Priority Queues
// Semester:         Fall 2019
// Course:           CMPS 3013 
// Assignment:       A03
// Author:           Tyler Roop
// Email:            tarrox16@yahoo.com
// Files:            priority_heap.cpp
//					 animals.json
// Description:
//       This program takes in various pieces of data from an
//       input file and uses them to creat buckets of
//       maxheaps.The animal name and priority are calculated
//       from inputs. A two digit command line argument is used to
//       determine the number of buckets. For numbers less than 10, a 0
//		 is needed in the tens place. The program then
//       prints off the top five animals from each heap.I
//       couldn't get the json files to work correctly, so I
//       modified the data file slightly to help with input.
//       All fields should be the same
//
///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define _USE_MATH_DEFINES

#include <math.h>

using namespace std;

// A struct that acts as my binary heap. It holds an array
// of pairs that has the animal name and priority
struct Heap
{
private:
	pair<string, double> * Animals;
	int Max;
	int Size;
public:
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

	pair<string, double> popHeap();

	void Insert(pair<string, double> a);
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

// method that swaps two index values in a heap
void swap(pair<string, double> *t, pair<string, double> *r)
{
	pair<string, double> temp = *t;
	*t = *r;
	*r = temp;
};

// method that initializes a heap
Heap::Heap(int newMax)
{
	Animals = new pair<string, double>[newMax];
	Size = 1;
	Max = newMax;
}

// method that is used to insert data into a heap. It first 
// inserts the data at the end and bubbles up to the correct
// index
void Heap::Insert(pair<string, double> a)
{
	int i = Size;
	Size++;
	Animals[i] = a;

	while (i != 1 && Animals[parent(i)].second < Animals[i].second)
	{
		swap(&Animals[i], &Animals[parent(i)]);
		i = parent(i);
	}
}

// method that retrieves and deletes the root of the heap
// by switching it with the last value and decrementing the
// size variable. It then calls heapify to rearrange the heap
pair<string, double> Heap::popHeap()
{
	if (Size <= 1)
		return (pair <string, double>("empty heap", 0));
	if (Size == 2)
	{
		Size--;
		return Animals[1];
	}

	pair<string, double> root = Animals[1];
	Animals[1] = Animals[Size];
	Size--;
	Heapify(1);

	return root;
}

// uses a given index value to bubbledown and sort the heap
void Heap::Heapify(int i)
{
	int l = left(i);
	int r = right(i);
	int biggest = i;
	if (l < Size && Animals[l].second > Animals[i].second)
		biggest = l;
	if (r < Size && Animals[r].second > Animals[biggest].second)
		biggest = r;
	if (biggest != i)
	{
		swap(&Animals[i], &Animals[biggest]);
		Heapify(biggest);
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

// method to read in data from a given file and create heaps 
// to fille the buckets by calculating priority
void ReadIn(string Input, Heap** Buckets, int NumberOfBuckets)
{
	string Animal, Version, Valid;
	double Lat, Lon, D, Adjust;
	float Priority;
	bool V = 0;
	int Bucket, L;
	long Date;
	Coordinate From(33.9137, -98.4934);

	ifstream Infile;
	Infile.open(Input);

	while (Infile >> Animal)
	{
		Infile >> Lat;
		Infile >> Lon;
		Infile >> Date;
		Infile >> Version;
		Infile >> Valid;
		if (Valid == "true")
		{
			V = 1;
		}
		Infile >> Adjust;
		L = Animal.length();
		Coordinate LatLon(Lat, Lon);

		D = HaversineDistance(From, LatLon);

		Priority = (6372.8 - D) * Adjust / L;

		if (V)
		{
			Priority *= -1;
		}

		Bucket = abs(Date) % NumberOfBuckets;

		Buckets[Bucket]->Insert(pair <string, double>(Animal, Priority));

	}

};

// method to print the top five items in each heap
void ReadOut(string Output, Heap** Buckets, int NumberOfBuckets)
{
	pair <string, double> Printed;
	ofstream Outfile;
	Outfile.open(Output);
	Outfile << "Tyler Roop\n\n";
	for (int i = 0; i < NumberOfBuckets; i++)
	{
		Outfile << "Heap " << i << '\n';
		for (int j = 1; j <= 5; j++)
		{
			Printed = Buckets[i]->popHeap();
			Outfile << Printed.first << ' ' << Printed.second << '\n';
		}
	}
	Outfile.close();
};

int main(int argc, char * argv[])
{
	int Tens = argv[1][0] - '0';
	int Ones = argv[1][2] - '0';
	Tens *= 10;
	int NumberOfBuckets = Tens + Ones;

	Heap** Buckets = new Heap*[NumberOfBuckets];

	for (int i = 0; i < NumberOfBuckets; i++) {
		Buckets[i] = new Heap(1000);
	}

	ReadIn("animals.json", Buckets, NumberOfBuckets);
	ReadOut("output.txt", Buckets, NumberOfBuckets);

}
