/*Oncioiu Raluca
**Rush Hour solver
**/

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <stdexcept>

#define MIN 9999	

using namespace std;

class Node {
	
	public:

		Node(Node *p, vector< pair<int, int> > b)
		{
		 	parent = p; 
			board = b;
			
			if(p == NULL)
			{
				depth = 0;
			}
			else
			{
				depth = p->depth + 1; 
			}
		}
		
		~Node()
		{
			delete parent;
			board.clear();
		}

		Node* getParent() { return parent; }
		int getHeuristicValue() { return heuristicValue; }

		Node *parent;
		vector< pair<int, int> > board;
		int heuristicValue;
		int depth;
};

class NodeComp {
	
	public:
		NodeComp() {};

		const bool operator() (const Node *node1, const Node *node2) const
		{	
			return node1->depth > node2->depth;
		}
};

class Car {
	
	public:

		Car(bool dir, int l, int r, int c) 
		{ 
			type = dir; 
			length = l; 
			x = r; 
			y = c; 
		}
		
		char getType() { return type; }
		char getLength() { return length; }
		
		bool type;		//true for up/down and false for left/right
		int length;	
		int x, y;		
};

class RushHour {
	
	public:
	
		RushHour(int r, int c)
		{ 
			n = r; 
			m = c;
			numCars = 0;
			
			ex = ey = -1;
			rx = ry = rlength = 0;
			
			board = (char **)malloc(n * sizeof(char *));
			
			for(int i = 0; i < n; i++)
			{
				board[i] = (char *)malloc(m * sizeof(char));
			} 
		}
		
		~RushHour()
		{
			cars.clear();
			
			for(int i = 0; i < n; i++)
			{
				free(board[i]);
			} 
			
			free(board);
		}
		
		void getBoard(vector< pair<int, int> >);
		void printSolution(FILE *, Node*);
		void expand(Node*, vector<Node*>&);	
		int heuristic(vector< pair<int, int> >);
		void findExit();
		
		int n, m;
		char **board;
		int numCars;
		vector<Car> cars;
		
		//red car
		int rx, ry;				
		int rlength;
		bool rtype; 
		
		//exit
		int ex, ey;
};

//function that expands the current board and saves the next states
void RushHour::expand(Node *node, vector<Node*> &mult)
{
	int distance, movesR, movesL, movesU, movesD;
	bool left, right, up, down;
	
	for(int i = 0; i < numCars; i++)
	{
		vector< pair<int, int> > puzzle(node->board);
	
		//horizontal car
		if(cars[i].getType() == false)
		{
			left = false;
			right = false;
			
			movesR = MIN;
			movesL = MIN;
			
			//check if there are cars that are blocking the current car
			for(int k = 0; k < numCars; k++)
			{	
				//horizontal cars
				if(i != k && cars[k].getType() == false && 
					 node->board[i].first == node->board[k].first) 
				{
					//how many steps to the right are possible 
					distance = node->board[k].second - node->board[i].second - 
						cars[i].length;
					
					if(distance == 0)
					{
						right = true;
					}
					else if(distance > 0 && distance < movesR)
					{	
						movesR = distance;
					}
					
					//how many steps to the left are possible
					distance = node->board[i].second - node->board[k].second - 
						cars[k].length;
					
					if(distance == 0)
					{
						left = true;
					}
					else if(distance > 0 && distance < movesL)
					{
						movesL = distance;
					}
				}
				
				//vertical cars
				if(i != k && cars[k].getType() == true && 
					 node->board[k].first <= node->board[i].first && 
					 node->board[i].first <= node->board[k].first + cars[k].length - 1) 
				{
					//how many steps to the right are possible
					distance = node->board[k].second - node->board[i].second - 
						cars[i].length;
					
					if(distance == 0)
					{
						right = true;
					}
					else if(distance > 0 && distance < movesR)
					{
						movesR = distance;
					}
					
					//how many steps to the left are possible
					distance = node->board[i].second - node->board[k].second - 1;
					
					if(distance == 0)
					{
						left = true;
					}
					else if(distance > 0 && distance < movesL)
					{
						movesL = distance;
					}
				}
			}
			
			//the car can move to the right
			if(right == false)
			{	
				//if there weren't cars to the right, we compute the number of steps
				//until the edge of the board
				if(movesR == MIN)
				{
					movesR = m - node->board[i].second - cars[i].length - 1;
					
					//the red car can move outside the board (it reaches the exit point)
					if(i == numCars - 1 && node->board[i].second < ey)
					{
						movesR++;
					}
				}
				
				//new states having as parent the current board
				for(int k = 1; k <= movesR; k++)
				{
					puzzle[i].first = node->board[i].first; 
					puzzle[i].second = node->board[i].second + k;
		
					Node *next = new Node(node, puzzle);
					mult.push_back(next);
				}
			}
		
			//the car can move to the left
			if(left == false) 
			{
				//if there weren't cars to the left, we compute the number of steps
				//until the edge of the board
				if(movesL == MIN)
				{
					movesL = node->board[i].second - 1;
					
					//the red car can move outside the board (it reaches the exit point)
					if(i == numCars - 1 && node->board[i].second > ey)
					{
						movesL++;
					}
				}
				
				//new states having as parent the current board
				for(int k = 1; k <= movesL; k++)
				{
					puzzle[i].first = node->board[i].first; 
					puzzle[i].second = node->board[i].second - k;			

					Node *next = new Node(node, puzzle);
					mult.push_back(next);
				}
			}
		}
		//vertical car
		else
		{
			up = false;
			down = false;
			
			movesU = MIN;
			movesD = MIN;
			
			//check if there are cars that are blocking the current car
			for(int k = 0; k < numCars; k++)
			{
				//horizontal cars
				if(i != k && cars[k].getType() == false &&
					 node->board[k].second <= node->board[i].second &&
					 node->board[i].second <= node->board[k].second + cars[k].length - 1) 
				{
					//how many steps up are possible
					distance = node->board[i].first - node->board[k].first - 1; 
					
					if(distance == 0)
					{
						up = true;
					}
					else if(distance > 0 && distance < movesU)
					{
						movesU = distance;
					}
					
					//how many steps down are possible
					distance = node->board[k].first - node->board[i].first -
					 cars[i].length;
					
					if(distance == 0)
					{
						down = true;
					}
					else if(distance > 0 && distance < movesD)
					{
						movesD = distance;
					}
				}
				
				//vertical cars
				if(i != k && cars[k].getType() == true &&
					node->board[i].second == node->board[k].second) 
				{
					//how many steps up are possible
					distance = node->board[i].first - node->board[k].first -
					 cars[k].length;
					
					if(distance == 0)
					{
						up = true;
					}
					else if(distance > 0 && distance < movesU)
					{
						movesU = distance;
					}
					
					//how many steps down are possible
					distance = node->board[k].first - node->board[i].first - 
						cars[i].length; 
					
					if(distance == 0)
					{
						down = true;
					}
					else if(distance > 0 && distance < movesD)
					{
						movesD = distance;
					}
				}
			}
			
			//the car can move up
			if(up == false)
			{
				//it there weren't cars, we compute the number of steps
				//until the edge of the board
				if(movesU == MIN)
				{
					movesU = node->board[i].first - 1;
				
					//the red car can move outsite the board (it reaches the exit point)
					if(i == numCars - 1 && node->board[i].first > ex)
					{
						movesU++;
					}
				}
				
				//new states having as parent the current board
				for(int k = 1; k <= movesU; k++)
				{
					puzzle[i].first = node->board[i].first - k; 
					puzzle[i].second = node->board[i].second;
			
					Node *next = new Node(node, puzzle);
					mult.push_back(next);
				}
			}
			
			//the car can move down
			if(down == false)
			{
				//if there weren't cars, we compute the number of steps 
				//until the edge of the board
				if(movesD == MIN)
				{
					movesD = n - node->board[i].first - cars[i].length - 1;
					
					//the red car can move outside the board (it reaches the exit point)
					if(i == numCars - 1 && node->board[i].first < ex)
					{
						movesD++;
					}
				}
					
				//new states having as parent the current board	
				for(int k = 1; k <= movesD; k++)
				{
					puzzle[i].first = node->board[i].first + k; 
					puzzle[i].second = node->board[i].second;
			
					Node *next = new Node(node, puzzle);
					mult.push_back(next);
				}
			}
		}
	}
}

//heuristic function for the search algorithm that return the number of 
//blocking cars for the red car plus one
int RushHour::heuristic(vector< pair<int, int> > moves)
{
	int value = 0;
	
	//newest coordinates for the red car
	int newX = moves[numCars - 1].first;
	int newY = moves[numCars - 1].second;
	
	for(int i = 0; i < numCars - 1; i++)
	{
		//the red car is horizontal
		if(rtype == false)
		{
			//horizontal blocking cars depending on the exit's position
			if(cars[i].getType() == false && moves[i].first == newX)
			{
				if((newY < ey && newY + rlength - 1 < moves[i].second) ||
					 (newY > ey && moves[i].second + cars[i].getLength() - 1 < newY))
				{
					value++;
				}
			}
			
			//vertical blocking cars depending on the exit's position
			if(cars[i].getType() == true && 
				 moves[i].first <= newX && newX <= moves[i].first + cars[i].length - 1)
			{
				if((newY < ey && newY + rlength - 1 < moves[i].second) ||
					 (newY > ey && moves[i].second < newY))
				{
					value++;
				}
			}
		}
		//the red car is vertical
		else
		{
			//horizontal blocking cars depending on the exit's position
			if(cars[i].getType() == false && 
				moves[i].second <= newY && newY <= moves[i].second + cars[i].length - 1)
			{
				if((newX > ex && moves[i].first < newX) ||
					 (newX < ex && newX + rlength - 1 < moves[i].first))
				{
					value++;
				}
			}
			
			//vertical blocking cars depending on the exit's position
			if(cars[i].getType() == true && moves[i].second == newY)
			{
				if((newX > ex && moves[i].first + cars[i].length - 1 < newX)||
						(newX < ex && newX + rlength - 1 < moves[i].first))
				{
					value++;
				}
			} 
		}
	}
	
	//the red car is horizontal and it hasn't obstacles
	if(rtype == false && (newY + rlength - 1 == ey || newY == ey))
	{
		return 0;
	}
	
	//the red car is vertical and it hasn't obstacles
	if(rtype == true && (newX + rlength - 1 == ex || newX == ex))
	{
		return 0;
	}
	
	return value + 1;
}

//function that prints the board in ASCII mode
void RushHour::getBoard(vector< pair<int, int> > moves)
{
	for(int i = 0; i < n; i++)
	{
		board[i][0] = '0';
		board[i][m - 1] = '0';
		
		board[0][i] = '0';
		board[n - 1][i] = '0';
	}
	
	board[ex][ey] = '*';
	
	for(int i = 1; i < n - 1; i++)
	{
		for(int j = 1; j < m - 1; j++)
		{
			board[i][j] = '-';
		}
	} 
	
	//we print the cars, but the red one
	for(int i = 0; i < numCars - 1; i++)
	{
		if(cars[i].getType() == true)
		{
			board[moves[i].first][moves[i].second] = '^';
			
			for(int k = 1; k < cars[i].length; k++)
			{
				board[moves[i].first + k][moves[i].second] = '|';
			}
			
			board[moves[i].first + cars[i].length - 1][moves[i].second] = 'v';
		}
		else
		{
			board[moves[i].first][moves[i].second] = '<';
			
			for(int k = 1; k < cars[i].length; k++)
			{
				board[moves[i].first][moves[i].second + k] = '-';
			}
			
			board[moves[i].first][moves[i].second + cars[i].length - 1] = '>';
		}
	}
	
	//we print the red car
	if(rtype == false)
	{
		board[rx][moves[numCars - 1].second] = '?';
			
		for(int k = 1; k < rlength; k++)
		{
			board[rx][moves[numCars - 1].second + k] = '?';
		}
	}
	else
	{
		board[moves[numCars - 1].first][ry] = '?';
			
		for(int k = 1; k < rlength; k++)
		{
			board[moves[numCars - 1].first + k][ry] = '?';
		}
	}
}

//recursive function that prints every change of the board 
void RushHour::printSolution(FILE *g, Node *node)
{	
	if(node == NULL)
	{
		return;
	}
	
	printSolution(g, node->parent);
	getBoard(node->board);
	
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < m; j++)
		{
			fprintf(g, "%c ", board[i][j]);
		}
		
		fprintf(g, "\n");
	}
	
	fprintf(g, "\n");
}

//hash function for unordered_map used in findExit()
size_t name_hash(const vector< pair <int, int> > &board)
{
	string sir;
	int board_size = board.size();
	
	for(int i = 0; i < board_size; i++)
	{
		sir.push_back(board[i].first);
		sir.push_back(board[i].second);
	}
	
	return hash<string>()(sir);
}

//A* search 
void RushHour::findExit()
{
	FILE *g = fopen("out.txt", "w");
	
	//exit is missing
	if(ex == ey && ex == -1)
	{
		fprintf(g, "0\n");
		fclose(g);
		return;
	}
	
	vector< pair<int, int> > initial_board;
	
	//we create the initial node
	for(int i = 0; i < numCars - 1; i++)
	{
		initial_board.push_back(pair<int, int>(cars[i].x, cars[i].y));
	}
	
	initial_board.push_back(pair<int, int>(rx, ry));
	
	//we use a priority_queue to expand each time the node 
	//with minimum heuristic value
	priority_queue<Node*, vector<Node*>, NodeComp> open;
	unordered_map< vector <pair <int, int> >, Node*, decltype(&name_hash)> 
		closed(1, name_hash);
	
	Node *initial = new Node(NULL, initial_board);
	initial->heuristicValue = heuristic(initial_board);
	
	open.push(initial);
	
	do
	{
		//no more nodes to expand, we finish the search 
		if(open.empty())
		{
			fprintf(g, "0\n");
			fclose(g);
			break;
		}
		
		Node *current = open.top();
		open.pop();
		
		//check if solution node
		if(current->heuristicValue == 0)
		{	
			//se afiseaza solutia
			int moves = current->depth; 
			fprintf(g, "%d\n\n", moves);
			
			printSolution(g, current);	
			fclose(g);
			break;
		}	
		
		vector<Node*> mult;
		
		//the node wasn't explored, we expand it
		try 
		{
			closed.at(current->board);
		}
		catch(out_of_range &e)
    {
			closed.insert(pair< vector<pair <int, int> >, Node* >(current->board,
			 current));
    	expand(current, mult);
    	
    	int mult_size = mult.size();
    	
    	//we don't expand nodes already in closed set
    	for(int i = 0; i < mult_size; i++)
    	{
    		try
    		{
    			closed.at(mult[i]->board);
    		}
				catch(out_of_range &e)
				{
					mult[i]->heuristicValue = heuristic(mult[i]->board);
					open.push(mult[i]);
				}
    	}
    }
    
	}	while(1); 
	
	//we free the memory
	while(!open.empty())
	{
		open.pop();
	}
	
	closed.clear();
}

int main()
{
	int n, m, size;
	
	//we read the dates from the input file
	FILE *f = fopen("in.txt", "r");
	
	if(f == NULL)
	{
		FILE *g = fopen("out.txt", "w");
	
		fprintf(g, "No input file\n");
		fclose(g);
		
		return 0;
	}
	
	int read = fscanf(f, "%d %d\n\n", &n, &m);
	
	RushHour rh(n, m);
	
	//we save the coordinates for the horizontal cars
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < m; j++)
		{
			int read = fscanf(f, "%c ", &rh.board[i][j]);
			
			if(rh.board[i][j] == '*')
			{
				rh.ex = i;
				rh.ey = j;
			}
			
			if(rh.board[i][j] == '?')
			{
				size = 1;
				rh.rx = i;
				rh.ry = j;
				
				while(rh.board[i][j] == '?')
				{
					j++;
					int read = fscanf(f, "%c ", &rh.board[i][j]);
					
					size++;
				}
				
				rh.rlength = size - 1;
			}
			
			if(rh.board[i][j] == '<')
			{
				rh.numCars++;
				size = 1;
			}
			
			if(rh.board[i][j] == '-')
			{
				size++;
			}
			
			if(rh.board[i][j] == '>')
			{
				rh.cars.push_back(Car(false, size + 1, i, j - size));
			}
		}
	}
	
	//we determine the type of the red car depending on the exit position
	if(rh.ey == 0 || rh.ey == m - 1)
	{
		rh.rtype = false;
	}
	else
	{
		rh.rtype = true;
	}
	
	rh.numCars++;

	//we save the coordinates for the vertical cars
	for(int j = 0; j < m; j++)
	{
		for(int i = 0; i < n; i++)
		{	
			//if the red car is vertical, we change the coordinate
			if(rh.rtype == true && rh.board[i][j] == '?')
			{	
				size = 1;
				rh.rx = i;
				rh.ry = j;
				
				while(rh.board[i][j] == '?')
				{
					size++;
					i++;
				}
				
				rh.rlength = size - 1;
			}
		
			if(rh.board[i][j] == '^')
			{
				rh.numCars++;
				size = 1;
			}
			
			if(rh.board[i][j] == '|')
			{
				size++;
			}
			
			if(rh.board[i][j] == 'v')
			{
				rh.cars.push_back(Car(true, size + 1, i - size, j));
			}
		}
	}
	
	//we save the coordinates for the red car
	rh.cars.push_back(Car(rh.rtype, rh.rlength, rh.rx, rh.ry));
	
	fclose(f);
	
	//we search the path to the exit in minimum steps and we print the solution
	rh.findExit();
	
	fflush(stdout);
	
	return 0;
}
