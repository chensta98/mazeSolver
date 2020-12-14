// Project 5

#include <iostream>
#include <limits.h>
#include "d_except.h"
#include <list>
#include <fstream>
#include "d_matrix.h"
#include "graph.h"
#include <queue>

using namespace std;

class maze
{
public:
    maze(ifstream& fin);
    void print(int, int, int, int);
    bool isLegal(int i, int j);

    void setMap(int i, int j, int n);
    int getMap(int i, int j) const;
    void reverseMap(int& i, int& j, const int& id) const;
    void mapMazeToGraph(maze& m, graph& g);
    char moveDirection(const int& ii, const int& ij, const int& fi, const int& fj) const;

    void findPathRecursive(graph& g);
    void findPathNonRecursive(graph& g);
    void printPath(stack<int>&);

    bool findShortestPath1(graph& g);
    bool findShortestPath2(graph& g);

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

void maze::reverseMap(int& i, int& j, const int& id) const
{
    //finds row, col for node id
    for (int k = 0; k < rows; k++)
    {
        for (int l = 0; l < cols; l++)
        {
            if (getMap(k, l) == id)
            {
                i = k;
                j = l;
                return;
            }
        }
    }
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

    if (goalI < 0 || goalI > rows - 1 || goalJ < 0 || goalJ > cols - 1)
        throw rangeError("Bad value in maze::print");

    if (currI < 0 || currI > rows - 1 || currJ < 0 || currJ > cols - 1)
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
        return false;

    return value[i][j];
}

void maze::mapMazeToGraph(maze& m, graph& g)
// Create a graph g that represents the legal moves in the maze m.
{
    //create nodes for graph
    int id = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (isLegal(i, j))
            {
                node n;
                n.setNode(id, 1);
                setMap(i, j, id);
                id++;
                g.addNode(n);
            }
        }
    }

    //add the edges between the nodes
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (isLegal(i, j))
            {
                node n(g.getNode(getMap(i, j)));

                if (isLegal(i + 1, j))
                {
                    g.addEdge(n.getId(), g.getNode(getMap(i + 1, j)).getId(), 1);
                    g.addEdge(g.getNode(getMap(i + 1, j)).getId(), n.getId(), 1);
                }

                if (isLegal(i, j + 1))
                {
                    g.addEdge(n.getId(), g.getNode(getMap(i, j + 1)).getId(), 1);
                    g.addEdge(g.getNode(getMap(i, j + 1)).getId(), n.getId(), 1);
                }

                if (isLegal(i - 1, j))
                {
                    g.addEdge(n.getId(), g.getNode(getMap(i - 1, j)).getId(), 1);
                    g.addEdge(g.getNode(getMap(i - 1, j)).getId(), n.getId(), 1);
                }

                if (isLegal(i, j - 1))
                {
                    g.addEdge(n.getId(), g.getNode(getMap(i, j - 1)).getId(), 1);
                    g.addEdge(g.getNode(getMap(i, j - 1)).getId(), n.getId(), 1);
                }
            }
        }
    }
}

char maze::moveDirection(const int& ii, const int& ij, const int& fi, const int& fj) const
{
    //using location of two nodes, returns direction to move in
    if (ii == fi && (ij + 1) == fj)
        return 'R';
    if ((ii + 1) == fi && ij == fj)
        return 'D';
    if (ii == fi && (ij - 1) == fj)
        return 'L';
    if ((ii - 1) == fi && ij == fj)
        return 'U';

    throw rangeError("Function args OOR");
}

void maze::findPathRecursive(graph& g) 
{
    //uses recursive search to find path thru maze, prints when found
    g.clearVisit();
    stack<int> path = stack<int>();

    g.dfs(g.getNode(getMap(0, 0)), getMap(rows - 1, cols - 1), path);

    printPath(path);
}

void maze::findPathNonRecursive(graph& g) 
{
    //using stack based first search to find path thru maze
    //print solution when found
    g.clearVisit();
    stack<node> hist = stack<node>();
    node start(g.getNode(getMap(0, 0)));
    hist.push(start);
    start.visit();
    int targetNode(getMap(rows - 1, cols - 1));

    while (!hist.empty())
    {
        node n(hist.top());

        if (n.getId() == targetNode)
            break;
       
        for (int i = 0; i < g.numNodes(); i++)
        {
            if (g.isEdge(n.getId(), i) && !g.getNode(i).isVisited())
            {
                g.getNode(i).visit();
                hist.push(g.getNode(i));
                break;
            }
        }

        if (n.getId() == hist.top().getId())
        {
            hist.pop();
        }
    }

    //reverse history stack to get correct path
    stack<int> path = stack<int>();
    while (!hist.empty())
    {
        path.push(hist.top().getId());
        hist.pop();
    }

    printPath(path);
}

bool maze::findShortestPath1(graph& g)
{
    g.clearVisit();
    queue<node> hist = queue<node>();
    node start(g.getNode(getMap(0, 0)));
    hist.push(start);
    start.visit();
    int targetNode(getMap(rows - 1, cols - 1));

    bool foundPath = false;
    vector<int> prev = vector<int>(g.numNodes());

    while (!hist.empty())
    {
        node n(hist.front());

        if (n.getId() == targetNode)
        {
            foundPath = true;
            break;
        }

        for (int i = 0; i < g.numNodes(); i++)
        {
            if (!g.getNode(i).isVisited() && g.isEdge(n.getId(), i))
            {
                g.getNode(i).visit();
                hist.push(g.getNode(i));
                prev.at(g.getNode(i).getId()) = n.getId();
            }
        }

        hist.pop();

    }

    stack<int> path = stack<int>();
    int backtrackPos = g.numNodes() - 1;
    while (backtrackPos != 0)
    {
        path.push(backtrackPos);
        backtrackPos = prev.at(backtrackPos);
    }

    path.push(0);

    if (foundPath)
    {
        printPath(path);
    }

    return foundPath;
}

bool maze::findShortestPath2(graph& g)
{
    
}

void maze::printPath(stack<int>& path)
{
    //print solution of maze, prints next move and result position
    if (!path.empty())
    {
        int i = 0;
        int j = 0;
        print(rows - 1, cols - 1, i, j);
        path.pop();
        while (!path.empty())
        {
            int ti, tj;
            reverseMap(ti, tj, path.top());
            cout << "Move: " << moveDirection(i, j, ti, tj);
            print(rows - 1, cols - 1, ti, tj);
            i = ti;
            j = tj;
            path.pop();
        }
    }
}

int main()
{
    char x;
    ifstream fin;

    // Read the maze from the file.
    string fileName = "Maze 1.txt";

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
            m.mapMazeToGraph(m, g);
            m.print(0, 0, 0, 0);
            g.printNodes();
            g.printEdges();

            m.findPathRecursive(g);
            m.findPathNonRecursive(g);
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
