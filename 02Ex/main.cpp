#include <iostream>
#include <cmath>
#include <stack>

#include "matrix.hpp"
#include <bits/stdc++.h>

#define UNVISITED -2

using namespace std;

// Utilized approach: DFS from each hub

void readConvs(vector<int> roadAdjList[], vector<int> roadAdjListRev[], int numRoads);

void printGraph(vector<int> adj[], int V);

void printVec(vector<int> vec);

void tarjanStep(int point, stack<int> *dfs_stack, vector<int> *points, int *openTime, vector<int> convAdjList[], vector<int> convAdjListRev[], vector<int> convAdjListCond[], int *lastSCC);

void printMatrix(vector<int> *matrix, int matRows, int matCols);

int main(int argc, char const *argv[])
{

    // read metadata
    int numPoints, numConvs, iCentral;
    cin >> numPoints >> numConvs >> iCentral;

    cout << numPoints << " " << numConvs << " " << iCentral << endl;

    //variables for graph storing
    vector<int> convAdjList[numPoints];
    vector<int> convAdjListRev[numPoints];
    vector<int> convAdjListCond[numPoints];

    readConvs(convAdjList, convAdjListRev, numConvs);
    printGraph(convAdjList, numPoints);
    printGraph(convAdjListRev, numPoints);

    // Find SSUs using Tarjan

    stack<int> dfs_stack;
    vector<int> points(numPoints * 3, UNVISITED);
    int openTime = 0;
    int lastSCC = 0;

    for (int point = 0; point < numPoints; ++point)
    {
        if (points[point * 3] == UNVISITED)
        {
            //cout << "Visiting point no. " << point << endl;
            tarjanStep(point, &dfs_stack, &points, &openTime, convAdjList, convAdjListRev, convAdjListCond, &lastSCC);
        }
    }

    printGraph(convAdjListCond, numPoints);
    return 0;
}

void tarjanStep(int point, stack<int> *dfs_stack, vector<int> *points, int *openTime, vector<int> convAdjList[], vector<int> convAdjListRev[], vector<int> convAdjListCond[], int *lastSCC)
{

    points->at(point * 3) = *openTime;
    points->at(point * 3 + 1) = *openTime;
    *openTime += 1;
    dfs_stack->push(point);
    points->at(point * 3 + 2) = 1;

    vector<int> successors = convAdjList[point]; // point successors

    //cout << "Successors: ";
    //printVec(successors);
    //printMatrix(points, 17, 3);

    // consider each point successor
    for (int successor : successors)
    {
        if (points->at(successor * 3) == UNVISITED)
        {
            // Successor has not yet been visited; recurse on it
            //cout << "Not visited: " << successor << endl;
            tarjanStep(successor, dfs_stack, points, openTime, convAdjList, convAdjListRev, convAdjListCond, lastSCC);
            points->at(point * 3 + 1) = min(points->at(point * 3 + 1), points->at(successor * 3 + 1));
        }
        else if (points->at(successor * 3 + 2) == 1)
        {
            //cout << "Visited and onstack: " << successor << endl;
            // Successor w is in stack S and hence in the current SCC
            // If w is not on stack, then (v, w) is an edge pointing to an SCC already found and must be ignored
            // Note: The next line may look odd - but is correct.
            // It says w.index not w.lowlink; that is deliberate and from the original paper
            points->at(point * 3 + 1) = min(points->at(point * 3 + 1), points->at(successor * 3));
        }
    }

    // If v is a root node, pop the stack and generate an SCC
    if (points->at(point * 3 + 1) == points->at(point * 3))
    {
        // start new SCC
        vector<int> SCC;
        int u = -1;
        bool inConvs = false;

        while (u != point)
        {
            u = dfs_stack->top();
            //cout << "Popping: " << u << endl;
            dfs_stack->pop();
            points->at(u * 3 + 2) = UNVISITED;

            SCC.push_back(u);
            for (int succ : convAdjList[u])
            {
                convAdjListCond[*lastSCC].push_back(succ);
            }
            inConvs = !convAdjListRev[u].empty();
        }
        if (!inConvs)
        {
            cout << *lastSCC << " is onewayer!\n";
        }

        cout << "SCC " << *lastSCC << ": ";
        *lastSCC += 1;
        printVec(SCC);
    }
}

void readConvs(vector<int> convAdjList[], vector<int> convAdjListRev[], int numConvs)
{
    // read values from input, add directly create adjacecny list
    int pointA, pointB;

    for (int i = 0; i < numConvs; ++i)
    {
        cin >> pointA >> pointB;
        convAdjList[pointA].push_back(pointB);
        convAdjListRev[pointB].push_back(pointA);
    }
}

void printGraph(vector<int> adj[], int V)
{
    for (int u = 0; u < V; u++)
    {
        cout << u;
        for (int el : adj[u])
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