// Project 5

#include <iostream>
#include <limits.h>
#include "d_except.h"
#include <list>
#include <fstream>
#include "d_matrix.h"
#include "graph.h"

using namespace std;

class maze
{
public:
    maze(ifstream& fin);
    void print(int, int, int, int);
    bool isLegal(int i, int j);

    void setMap(int i, int j, int n);
    int getMap(int i, int j) const;
    void mapMazeToGraph(graph& g);

private:
    int rows; // number of rows in the maze
    int cols; // number of columns in the maze

    matrix<bool> value;
    matrix<int> map;      // Mapping from maze (i,j) values to node index values
};

void maze::setMap(int i, int j, int n)
// Set mapping from maze cell (i,j) to graph node n. 
{
    map[i][j] = n;
}

int maze::getMap(int i, int j) const
// Return mapping of maze cell (i,j) in the graph.
{
    return map[i][j];
}

maze::maze(ifstream& fin)
// Initializes a maze by reading values from fin.  Assumes that the
// number of rows and columns indicated in the file are correct.
{
    fin >> rows;
    fin >> cols;

    char x;

    value.resize(rows, cols);
    for (int i = 0; i <= rows - 1; i++)
        for (int j = 0; j <= cols - 1; j++)
        {
            fin >> x;
            if (x == 'O')
                value[i][j] = true;
            else
                value[i][j] = false;
        }

    map.resize(rows, cols);
}

void maze::print(int goalI, int goalJ, int currI, int currJ)
// Print out a maze, with the goal and current cells marked on the
// board.
{
    cout << endl;

    if (goalI < 0 || goalI > rows || goalJ < 0 || goalJ > cols)
        throw rangeError("Bad value in maze::print");

    if (currI < 0 || currI > rows || currJ < 0 || currJ > cols)
        throw rangeError("Bad value in maze::print");

    for (int i = 0; i <= rows - 1; i++)
    {
        for (int j = 0; j <= cols - 1; j++)
        {
            if (i == goalI && j == goalJ)
                cout << "*";
            else
                if (i == currI && j == currJ)
                    cout << "+";
                else
                    if (value[i][j])
                        cout << " ";
                    else
                        cout << "X";
        }
        cout << endl;
    }
    cout << endl;
}

bool maze::isLegal(int i, int j)
// Return the value stored at the (i,j) entry in the maze.
{
    if (i < 0 || i > rows - 1 || j < 0 || j > cols - 1)
        throw rangeError("Bad value in maze::isLegal");

    return value[i][j];
}

void maze::mapMazeToGraph(graph& g)
// Create a graph g that represents the legal moves in the maze m.
{
    int id = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            cout << "ROW: " << i << "COL: " << j << endl;
            if (isLegal(i, j))
            {
                node n;
                n.setId(id);
                g.addNode(n);
                setMap(i, j, id);
                id++;
            }
        }
    }

    for (int x = 0; x < rows; x++)
    {
        for (int y = 0; y < cols; y++) 
        {
            cout << map[x][y] << "\t";
        }
        cout << endl;
    }

    for (int x = 0; x < rows; x++)
    {
        for (int y = 0; y < cols; y++)
        {
            if (isLegal(x, y))
            {
                int source_id = getMap(x, y);
                // Right Edge
                if (y + 1 <= cols - 1)
                {
                    if (isLegal(x, y + 1))
                    {
                        g.addEdge(source_id, getMap(x, y + 1));
                    }
                }
                // Left Edge
                if (y - 1 >= 0)
                {
                    if (isLegal(x, y - 1))
                    {
                        g.addEdge(source_id, getMap(x, y - 1));
                    }
                }
                // Up Edge
                if (x - 1 >= 0)
                {
                    if (isLegal(x - 1, y))
                    {

                        g.addEdge(source_id, getMap(x - 1, y));
                    }
                }
                // Down Edge
                if (x + 1 <= rows - 1)
                {
                    if (isLegal(x + 1, y))
                    {
                        g.addEdge(source_id, getMap(x + 1, y));
                    }
                }
            }
        }
    }

}



int main()
{
    char x;
    ifstream fin;

    // Read the maze from the file.
    string fileName = "./mazes/maze1.txt";

    fin.open(fileName.c_str());
    if (!fin)
    {
        cerr << "Cannot open " << fileName << endl;
        exit(1);
    }

    try
    {

        graph g;
        while (fin && fin.peek() != 'Z')
        {
            maze m(fin);
            m.mapMazeToGraph(g);
            cout << g << endl;
            m.print(6,9,0,0);
        }


    }
    catch (indexRangeError& ex)
    {
        cout << ex.what() << endl; exit(1);
    }
    catch (rangeError& ex)
    {
        cout << ex.what() << endl; exit(1);
    }
}