RushHour
========

RushHour puzzle solver written in C++

This code resolves any kind of RushHour puzzle with m rows and n columns and cars with different lengths, not only the 
standard game. It is based on an AStar search, using a hashtable for the CLOSED set and a priority queue for the OPEN 
set. As heuristic function, it is using the number of blocking cars for the red car plus one.
One or more single steps for a car in any direction represent only one move. 
The number of minimal moves and every change of the board are printed in the file called "out.txt".
Input data is read from the file called "in.txt". The puzzle is given in an ASCII mode. 
Execution time is below 2 seconds for hard puzzles. For easy ones, a time of 0.5 seconds is more than enough.


3 
8 
3 2 E 
2 4 S 
3 4 N 
1 2 N 
4 5 S 
1 1 N 
5 3 S 
5 4 W 
5 
3 2 E 
2 2 E 
3 4 N 
2 5 N 
1 2 E 
9 
3 2 E 
3 3 S 
4 4 S 
1 2 E 
1 5 E 
5 1 W 
5 4 E 
4 1 W 
3 5 S



#1 5 
#2 3 
#3 -1


#1 3 
#2 11 
#3 15 
#4 -1 
#5 15 
#6 5 
#7 12 
#8 14 
#9 8 
#10 6
