#include "grid.h"
#include <iostream>
#include <cassert>
#include <string>
#include <cctype>
using namespace std;

bool isRouteWellFormed(string route)
{
	if (route == "")    //apparently a sequence of zero route segments is still syntactically valid so yeah
		return true;   
	if (route[0] != 'n' && route[0] != 'N' && route[0] != 's' && route[0] != 'S' && route[0] != 'e' && route[0] != 'E' && route[0] != 'w' && route[0] != 'W')
		return false;	//if the first character is not a valid direction, the route is automatically invalidated
										
	for (int k = 1; k < route.size(); k++)
		if (!(isdigit(route[k]) || route[k] == 'n' || route[k] == 'N' || route[k] == 's' || route[k] == 'S' || route[k] == 'e' || route[k] == 'E' || route[k] == 'w' || route[k] == 'W'))
			return false;			//this invalidates the route if any character in it is not a direction or an integer
		else if (isdigit(route[k]))
			if (k < route.size() - 2) //I restricted k so that when I tested if there were two more integers after an integer, there would be no undefined behavior
			{						  
				if (isdigit(route[k + 1]) && isdigit(route[k + 2]))
					return false;
			}
			else
				continue;
	return true;
}

int navigateSegment(int r, int c, char dir, int maxSteps)
{
	int steps = 0;
	if (c > getCols() || c <= 0 || r > getRows() || r <= 0 || isWall(r, c) || maxSteps < 0)
		return -1;  
	
	if (dir == 'n' || dir == 'N') //for each direction, the function will find the max number of steps before it hits a wall or goes off the grid
		for (int n = (r - 1); n > 0; n--)
		{
			if (isWall(n, c))
				break;
			else if (!isWall(n, c))
				steps++;
		}
	else if (dir == 's' || dir == 'S')
		for (int s = (r + 1); s <= getRows(); s++)
		{
			if (isWall(s, c))
				break;
			else if (!isWall(s, c))
				steps++;
		}
	else if (dir == 'e' || dir == 'E')
		for (int e = (c + 1); e <= getCols(); e++)
		{
			if (isWall(r, e))
				break;
			else if (!isWall(r, e))
				steps++;
		}
	else if (dir == 'w' || dir == 'W')
		for (int w = (c - 1); w > 0; w--)
		{
			if (isWall(r, w))
				break;
			else if (!isWall(r, w))
				steps++;
		}
	else
		return -1;
	
	if (steps < maxSteps) //this will return the number of possible steps before hitting a wall or going off the grid if it was less than the inputted number of steps
		return steps;
	else
		return maxSteps; //if there were no obstructions, the function will just return the inputted amount of steps
}

int navigateRoute(int sr, int sc, int er, int ec, string route, int &nsteps)
{
	if (sc > getCols() || sc <= 0 || sr > getRows() || sr <= 0 || isWall(sr, sc) || ec > getCols() || ec <= 0 || er > getRows() || er <= 0 || isWall(er, ec) || !isRouteWellFormed(route))
		return 2;       
	if (route == "")
		nsteps = 0;
	for (int ey = 0; ey < route.size(); ey++)
	{
		if (isalpha(route[ey])) //if the character in the string was a letter the program will check if there are one or two digits after it
		{
			int numSteps = 1;  //if there are no digits after the direction character, this will default the number of steps in that direction to 1 
			char nsew = route[ey];
			if (ey < route.size() - 1 && isdigit(route[ey + 1])) //I restricted ey to prevent undefined behavior
			{
				int numOne = route[ey + 1] - 48; //to convert a character into an integer, I used the ASCII code system in which the character of a number minus 48 is its integer
				if (ey < route.size() - 2 && isdigit(route[ey + 2]))
				{
					int numTwo = route[ey + 2] - 48;
					numSteps = (numOne * 10) + numTwo;
				}
				else
					numSteps = numOne;
			}
			
			if (ey == 0)
				nsteps = navigateSegment(sr, sc, nsew, numSteps);  
			else
				nsteps += navigateSegment(sr, sc, nsew, numSteps);
			
			if (navigateSegment(sr, sc, nsew, numSteps) < numSteps)
				return 3; //if this function ever returned a value less than the number of steps inputted, the robot definitely hit a wall or left the grid

			if (nsew == 'n' || nsew == 'N')
				sr -= navigateSegment(sr, sc, nsew, numSteps);
			else if (nsew == 's' || nsew == 'S')
				sr += navigateSegment(sr, sc, nsew, numSteps);
			else if (nsew == 'e' || nsew == 'E')
				sc += navigateSegment(sr, sc, nsew, numSteps);
			else
				sc -= navigateSegment(sr, sc, nsew, numSteps); //this changes the position of the robot as it moves along the given route
		}
		else //if the character in the string was not a letter, the loop will continue until it finds the next letter
			continue;
	}
	if (sr == er && sc == ec)
		return 0;
	else
		return 1;
}

int main()
{
	setSize(15, 15);
	setWall(6, 1);
	setWall(2, 2);
	setWall(1, 3);
	setWall(4, 3);
	setWall(2, 4);
	setWall(5, 4);
	setWall(3, 5);
	int len;
	len = 55;  
	assert(navigateRoute(6, 5, 1, 1, "N02wnWnsw5", len) == 3 && len == 9);
	len = -999;
	assert(navigateRoute(3, 1, 3, 4, "e1x", len) == 2 && len == -999); 
	len = -999;
	assert(navigateRoute(6, 1, 3, 4, "e", len) == 2 && len == -999); 
	len = 3424;
	assert(navigateRoute(1, 1, 6, 5, "", len) == 1 && len == 0); 
	len = -123;
	assert(navigateRoute(6, 5, 1, 1, "N02wnWnsw2nN", len) == 0 && len == 11); 
	len = 999;
	assert(navigateRoute(6, 5, 1, 1, "N02wnWns", len) == 1 && len == 7); 
	len = 3576;
	assert(navigateRoute(15, 15, 1, 15, "w14e14N10s5N9", len) == 0 && len == 52);
	assert(isRouteWellFormed("n2e1NSEWnseww01"));
	assert(!isRouteWellFormed("e123"));
	assert(navigateSegment(15, 15, 'N', 20) == 14);
	assert(navigateSegment(5, 4, 'N', 12) == -1);
	assert(navigateSegment(10, 10, 'H', 12) == -1);
	assert(navigateSegment(10, 10, 'w', -3) == -1);
	cout << "All tests succeeded" << endl;	
}