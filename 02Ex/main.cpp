#include <iostream>
#include <cmath>
#include <stack>

#include "matrix.hpp"
#include <bits/stdc++.h>

#define UNVISITED -2
#define NUMCOLS 4

using namespace std;

// Utilized approach: DFS from each hub

void readConvs(vector<int> roadAdjList[], vector<int> convAdjListRev[], int numRoads);

void printGraph(vector<int> adj[], int V);

void printVec(vector<int> vec);

void tarjanStep(int point, stack<int> *dfs_stack, vector<int> *points, int *openTime, int *lastSCC, vector<int> convAdjList[]);

void printMatrix(vector<int> *matrix, int matRows, int matCols);

void condGraph(int numPoints, vector<int> convAdjList[], vector<int> convAdjListRev[], vector<int> convAdjListCond[], vector<int> *points)
{

    for (int i = 0; i < numPoints; i++)
    {
        // if (convAdjList[i].empty())
        // {
        //     startPoints
        // }
        int succCond, compCond;
        for (int succ : convAdjList[i])
        {
            compCond = points->at(i * NUMCOLS + 3);
            succCond = points->at(succ * NUMCOLS + 3);
            if (succCond != compCond)
                convAdjListCond[compCond].push_back(succCond);
            // if (std::find(convAdjListCond[points->at(i * NUMCOLS + 3)].begin(), convAdjListCond[points->at(i * NUMCOLS + 3)].end(), points->at(succ * NUMCOLS + 3)) != convAdjListCond[points->at(i * NUMCOLS + 3)].end())
            // {
            //     convAdjListCond[points->at(i * NUMCOLS + 3)].push_back(points->at(succ * NUMCOLS + 3));
            // }
        }
    }
}

int main(int argc, char const *argv[])
{

    // read metadata
    int numPoints, numConvs, iCentral;
    cin >> numPoints >> numConvs >> iCentral;

    cout << numPoints << " " << numConvs << " " << iCentral << endl;

    //variables for graph storing
    vector<int> convAdjList[numPoints];
    vector<int> convAdjListRev[numPoints];

    readConvs(convAdjList, convAdjListRev, numConvs);
    printGraph(convAdjList, numPoints);

    // Find SSUs using Tarjan

    stack<int> dfs_stack;
    vector<int> points(numPoints * NUMCOLS, UNVISITED);
    int openTime = 0;
    int lastSCC = 0;

    for (int point = 0; point < numPoints; ++point)
    {
        if (points[point * NUMCOLS] == UNVISITED)
        {
            //cout << "Visiting point no. " << point << endl;
            tarjanStep(point, &dfs_stack, &points, &openTime, &lastSCC, convAdjList);
        }
    }

    printMatrix(&points, 17, NUMCOLS);

    vector<int> convAdjListCond[lastSCC];
    condGraph(numPoints, convAdjList, convAdjListRev, convAdjListCond, &points);
    printGraph(convAdjListCond, lastSCC);

    return 0;
}

void tarjanStep(int point, stack<int> *dfs_stack, vector<int> *points, int *openTime, int *lastSCC, vector<int> convAdjList[])
{

    points->at(point * NUMCOLS) = *openTime;
    points->at(point * NUMCOLS + 1) = *openTime;
    *openTime += 1;
    dfs_stack->push(point);
    points->at(point * NUMCOLS + 2) = 1;

    vector<int> successors = convAdjList[point]; // point successors

    // cout << "Successors: ";
    // printVec(successors);
    // printMatrix(points, 17, NUMCOLS);

    // consider each point successor
    for (int successor : successors)
    {
        if (points->at(successor * NUMCOLS) == UNVISITED)
        {
            // Successor has not yet been visited; recurse on it
            //cout << "Not visited: " << successor << endl;
            tarjanStep(successor, dfs_stack, points, openTime, lastSCC, convAdjList);
            points->at(point * NUMCOLS + 1) = min(points->at(point * NUMCOLS + 1), points->at(successor * NUMCOLS + 1));
        }
        else if (points->at(successor * NUMCOLS + 2) == 1)
        {
            //cout << "Visited and onstack: " << successor << endl;
            // Successor w is in stack S and hence in the current SCC
            // If w is not on stack, then (v, w) is an edge pointing to an SCC already found and must be ignored
            // Note: The next line may look odd - but is correct.
            // It says w.index not w.lowlink; that is deliberate and from the original paper
            points->at(point * NUMCOLS + 1) = min(points->at(point * NUMCOLS + 1), points->at(successor * NUMCOLS));
        }
    }

    // If v is a root node, pop the stack and generate an SCC
    if (points->at(point * NUMCOLS + 1) == points->at(point * NUMCOLS))
    {
        // start new SCC
        vector<int> SCC;
        int u = -1;

        while (u != point)
        {
            u = dfs_stack->top();
            //cout << "Popping: " << u << endl;
            dfs_stack->pop();
            points->at(u * NUMCOLS + 2) = UNVISITED;

            SCC.push_back(u);
            points->at(u * NUMCOLS + 3) = *lastSCC;
        }

        *lastSCC += 1;
        // cout << "SCC " << *lastSCC << ": ";
        // printVec(SCC);
    }
}

// void condenseGraph()
// {
// }
// <<<<<<<<<<<<<<, ,>>>>>>>>>>>>>>>>>>>>>

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