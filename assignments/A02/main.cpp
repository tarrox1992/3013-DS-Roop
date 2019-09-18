/**
*  Course: CMPS 3013 - Adv. Structures and Algorithms
*
*  Purpose: Program to take in a file of integers and insert them into a BST
*
*  @author Tyler Roop
*  @version 1.1 09/16/19
*  @github repo: https://github.com/tarrox1992/3013-DS-Roop/tree/master/assignments/A02
*/

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void Sort(int* Data, int Size);
void ArrayBSTSort(int*Data, int Size);

/**
 * TreeNode
 *
 * Description:
 *      Used to hold data and child pointers
 *      in our Binary Search Tree
 */
struct TreeNode {
	int Data;
	TreeNode* left;
	TreeNode* right;

	TreeNode() {
		Data = INT_LEAST32_MIN;
		left = NULL;
		right = NULL;
	}

	TreeNode(int x) {
		Data = x;
		left = NULL;
		right = NULL;
	}
};

class BST {
	//////////////////////////////////////////////////////////////////////////
private:
	//////////////////////////////////////////////////////////////////////////
	TreeNode* Root;
	int Counter;

	/**
	 * private _Insert
	 *
	 * Description:
	 *      Recursive method that traverses left and right depending
	 *      on inserted value eventually finding a NULL pointer and
	 *      inserting the new node there.
	 *
	 * Params:
	 *      TreeNode* &nptr     : A reference pointer so we can update actuall tree
	 *      TreeNode* temp      : The new node to be linked in
	 *
	 * Returns:
	 *      void
	 */
	void _Insert(TreeNode* &nptr, TreeNode* temp) {
		if (!nptr) {
			nptr = temp;
			return;
		}

		if (temp->Data < nptr->Data) {
			_Insert(nptr->left, temp);
		}
		else {
			_Insert(nptr->right, temp);
		}
	}

	//////////////////////////////////////////////////////////////////////////
public:
	//////////////////////////////////////////////////////////////////////////
		/**
		 * private Bst
		 *
		 * Description:
		 *      Class constructor inits tree to empty
		 *
		 * Params:
		 *      void
		 *
		 * Returns:
		 *      void
		 */
	BST() {
		Root = NULL;
	}

	/**
	 * public Insert
	 *
	 * Description:
	 *      Calls private insert with the new node and root of the tree.
	 *      We need to private method because caller does not now about
	 *      the "root" of our tree.
	 *
	 * Params:
	 *      string s : a word to insert
	 *
	 * Returns:
	 *      void
	 */
	void Insert(int s) {
		TreeNode* temp = new TreeNode(s);

		if (!Root) {
			Root = temp;
		}
		else {
			_Insert(Root, temp);
		}
	}

	/**
	 * private TreeFromAry
	 *
	 * Description:
	 *      Recursive method that takes a sorted array and inserts the numbers
	 *      in a way to create a balanced binary search tree
	 *
	 * Params:
	 *      int First			: Index of smallest number in the tree/subtree
	 *      int Last	        : Index of largest number in the tree/subtree
	 *		int* Array			: Array of integers to be inserted into BST
	 *
	 * Returns:
	 *      void
	 */

	void TreeFromAry(int First, int Last, int* Array)
	{
		// Exits function if index for the last number in the tree/subtree is larger
		// than the index for the first number in the tree/sub tree
		// This should only happen after a leaf node
		if (Last < First)
			return;
		
		// Sets middle value to be exactly between largest and smallest indices of
		// current array/sub array
		int Mid = (First + Last) / 2;

		// Inserts current middle value into BST, creating a new node
		Insert(Array[Mid]);

		// Moves to the left
		TreeFromAry(First, Mid - 1, Array);

		// Moves to the right
		TreeFromAry(Mid + 1 , Last, Array);
		
		return;
	}

	//************************************************************************
	// Method to help create GraphViz code so the expression tree can
	// be visualized. This method prints out all the unique node id's
	// by traversing the tree.
	// Recivies a node pointer to root and performs a simple recursive
	// tree traversal.
	//************************************************************************
	void GraphVizGetIds(TreeNode *nodePtr, ofstream &VizOut)
	{
		static int NullCount = 0;
		if (nodePtr)
		{
			GraphVizGetIds(nodePtr->left, VizOut);
			VizOut << "node" << nodePtr->Data
				<< "[label=\"" << nodePtr->Data << "\\n"
				//<<"Add:"<<nodePtr<<"\\n"
				//<<"Par:"<<nodePtr->parent<<"\\n"
				//<<"Rt:"<<nodePtr->right<<"\\n"
				//<<"Lt:"<<nodePtr->left<<"\\n"
				<< "\"]" << endl;
			if (!nodePtr->left)
			{
				NullCount++;
				VizOut << "nnode" << NullCount << "[label=\"X\",shape=point,width=.15]" << endl;
			}
			GraphVizGetIds(nodePtr->right, VizOut);
			if (!nodePtr->right)
			{
				NullCount++;
				VizOut << "nnode" << NullCount << "[label=\"X\",shape=point,width=.15]" << endl;
			}
		}
	}

	//************************************************************************
	// This method is partnered with the above method, but on this pass it
	// writes out the actual data from each node.
	// Don't worry about what this method and the above method do, just
	// use the output as your told:)
	//************************************************************************
	void GraphVizMakeConnections(TreeNode *nodePtr, ofstream &VizOut)
	{
		static int NullCount = 0;
		if (nodePtr)
		{
			GraphVizMakeConnections(nodePtr->left, VizOut);
			if (nodePtr->left)
				VizOut << "node" << nodePtr->Data << "->"
				<< "node" << nodePtr->left->Data << endl;
			else
			{
				NullCount++;
				VizOut << "node" << nodePtr->Data << "->"
					<< "nnode" << NullCount << endl;
			}

			if (nodePtr->right)
				VizOut << "node" << nodePtr->Data << "->"
				<< "node" << nodePtr->right->Data << endl;
			else
			{
				NullCount++;
				VizOut << "node" << nodePtr->Data << "->"
					<< "nnode" << NullCount << endl;
			}

			GraphVizMakeConnections(nodePtr->right, VizOut);
		}
	}

	//************************************************************************
	// Recieves a filename to place the GraphViz data into.
	// It then calls the above two graphviz methods to create a data file
	// that can be used to visualize your expression tree.
	//************************************************************************
	void GraphVizOut(string filename)
	{
		ofstream VizOut;
		VizOut.open(filename);
		VizOut << "Digraph G {\n";
		GraphVizGetIds(Root, VizOut);
		GraphVizMakeConnections(Root, VizOut);
		VizOut << "}\n";
		VizOut.close();
	}
};

int main(int argc, char** argv)
{
	// Variable to hold infile information
	ifstream infile;
	
	// Opens infile from command line parameter
	infile.open(argv[1]);

	/** Variables to hold binary search tree, data values inserted from
	 *  file, int to hold the amount of integers inserted, int to increment
	 *  insertion point for array
	 */
	BST BinTree;
	int Val;
	int Count = 0;
	int i = 1;

	// Loop to count values in file
	while (infile >> Val)
		Count++;
	// Dynamic declaration of array for data
	int* Data = new int[Count];

	// sets first element in array to lowest possible 32 bit int
	Data[0] = INT_LEAST32_MIN;

	// clears file so data can actually be inserted
	infile.clear();
	infile.seekg(0, ios::beg);

	// Loop to insert data
	while (infile >> Val)
	{
		Data[i] = Val;
		i++;
	}

	// Sorts data from smallest to largest
	Sort(Data, Count);	

	// Uses sorted array to create BST
	BinTree.TreeFromAry(1, Count, Data);
	BinTree.GraphVizOut(argv[2]);

	infile.close();

	return 0;
}

	/**
	 * Sort
	 *
	 * Description:
	 *      Sorts an array from smallest to largest
	 *
	 * Params:
	 *      int* Data			: Pointer to an array of int
	 *      int Size			: The size of the array
	 *
	 * Returns:
	 *      void
	 */
void Sort(int*Data, int Size)
{
	int temp;
	for (int i = 1; i <= Size; i++)
	{
		for (int j = 1; j < Size; j++)
		{
			if (Data[j + 1] < Data[j])
			{
				temp = Data[j + 1];
				Data[j + 1] = Data[j];
				Data[j] = temp;
			}
		}
	}
}
