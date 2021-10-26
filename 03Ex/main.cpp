#include <iostream>
#include <cmath>
#include <stack>

#include <bits/stdc++.h>

#define UNVISITED -2
#define NUMCOLS 4

using namespace std;

// Utilized approach: DFS from each hub

void readConvs(vector<vector<int>> *adjList, int numV);

class CompareNodes
{
public:
    bool operator()(pair<int, int> a, pair<int, int> b)
    {
        return a.second > b.second;
    }
};

void printQueue(priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes> gq);

void printGraph(vector<vector<int>> adjList, int V);

void printVec(vector<int> vec);

void printMatrix(vector<int> *matrix, int matRows, int matCols);

int main(int argc, char const *argv[])
{
    // read metadata - num. of smugglers, connections, pack size and num. of connections per pack
    int numSumgs, numConns, packSize, packConns;
    cin >> numSumgs >> numConns >> packSize >> packConns;

    //cout << numSumgs << numConns << packSize << packConns;

    // variables for original graph and reversed directions graph storing
    vector<vector<int>> convAdjList(numSumgs);
    //vector<vector<int>> convAdjListRev(numPoints);

    // read graph
    readConvs(&convAdjList, numSumgs);

    printGraph(convAdjList, numSumgs);

    return 0;
}

void readConvs(vector<vector<int>> *adjList, int numV)
{
    // read values from input, add directly create adjacecny list
    int pointA, pointB;

    for (int i = 0; i < numV; ++i)
    {
        cin >> pointA >> pointB;
        adjList->at(pointA).push_back(pointB);
        adjList->at(pointB).push_back(pointA);
    }
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void printGraph(vector<vector<int>> adjList, int V)
{
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