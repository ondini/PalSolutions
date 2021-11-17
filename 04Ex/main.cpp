#include <iostream>
#include <cmath>
#include <stack>

#include <bits/stdc++.h>

#define UNVISITED -2
#define NUMCOLS 4

using namespace std;

class CompareNodes
{
public:
    bool operator()(pair<int, int> a, pair<int, int> b)
    {
        return (a.first == b.first) ? (a.second > b.second) : (a.first > b.first);
    }
};

typedef priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes>
    p_queue_pair;

void readEdges(vector<vector<int>> *adjList, int numV, int numA);

void printQueue(p_queue_pair gq);

void printGraph(vector<vector<int>> adjList);

void printVec(vector<int> vec);

void printVecPairs(vector<pair<int, int>> vec);

void printMatrix(vector<int> *matrix, int matRows, int matCols);

int main(int argc, char const *argv[])
{
    // read metadata - num. of automaton states, Alphabet size, num. final states, number of negative and positive examples and their length
    int numStates, sizeAlpha, numFins, numPosits, numNegats, explLength;
    cin >> numStates >> sizeAlpha >> numFins >> numPosits >> numNegats >> explLength;

    // variable for graph and its filling
    vector<vector<int>> adjList(numStates);

    readEdges(&adjList, numStates, sizeAlpha);

    printGraph(adjList);

    return 0;
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, GET METADATA FUNCTIONS ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void readEdges(vector<vector<int>> *adjList, int numV, int numA)
{
    // read values from input, add directly create adjacecny list
    int endState, startState;

    for (int i = 0; i < numV; ++i)
    {
        cin >> startState;
        for (int j = 0; j < numA; ++j)
        {
            cin >> endState;
            adjList->at(startState).push_back(endState);
        }
    }
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, ASSIGNMENT SPECIFIC FUNCTIONS ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, PRINT FUNCTIONS ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void printGraph(vector<vector<int>> adjList)
{
    int V = adjList.size();
    for (int u = 0; u < V; u++)
    {
        cout << u;
        for (int el : adjList[u])
        {
            cout << " -> (" << el << ")";
        }
        cout << "\n";
    }
}

void printVec(vector<int> vec)
{
    cout << "( ";
    for (int el : vec)
    {
        cout << el << ", ";
    }
    cout << ")\n";
}

void printVecPairs(vector<pair<int, int>> vec)
{
    cout << "[ ";
    for (pair<int, int> el : vec)
    {
        cout << "(" << el.first << "," << el.second << "); ";
    }
    cout << "]\n";
}

void printMatrix(vector<int> *matrix, int matRows, int matCols)
{
    cout << "_";
    for (int i = 0; i < matCols; i++)
    {
        cout << "____";
    }

    cout << "__" << endl;
    for (int i = 0; i < matRows; ++i)
    {
        cout << "|";
        for (int j = 0; j < matCols; ++j)
        {
            printf("%4d", matrix->at(i * matCols + j));
        }
        cout << " | " << endl;
    }

    cout << "_";
    for (int i = 0; i < matCols; i++)
    {
        cout << "____";
    }
    cout << "__" << endl;
}

void printQueue(priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes> gq)
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes> g = gq;
    while (!g.empty())
    {
        cout << '\t' << g.top().first << " : " << g.top().second;
        g.pop();
    }
    cout << '\n';
}