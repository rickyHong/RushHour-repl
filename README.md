RushHour
========

RushHour puzzle solver written in C++

This code resolves any kind of RushHour puzzle with m rows and n columns and cars with different lengths, not only the 
standard game. It is based on an AStar search, using a hashtable for the CLOSED set and a priority queue for the OPEN 
set. As heuristic function, it is using the number of blocking cars for the red car plus one.
One or more single steps for a car in any direction represent only one move. 
The number of minimal moves and every change of the board are printed in the file called "out.txt".
Input data is read from the file called "in.txt". The puzzle is given in an ASCII mode. 


