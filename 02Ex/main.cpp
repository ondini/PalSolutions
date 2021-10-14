#include <iostream>
#include <cmath>
#include <stack>

#include <bits/stdc++.h>

#define UNVISITED -2
#define NUMCOLS 4

using namespace std;

// Utilized approach: DFS from each hub

void readConvs(vector<vector<int>> *convAdjList, int numRoads);

class CompareNodes
{
public:
    bool operator()(pair<int, int> a, pair<int, int> b)
    {
        return a.second > b.second;
    }
};

void printQueue(priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes> gq);

void printGraph(vector<int> adj[], int V);

void printVec(vector<int> vec);

void tarjanStep(int point, stack<int> *dfs_stack, vector<int> *points, int *openTime, int *lastSCC, vector<vector<int>> *convAdjList);

void printMatrix(vector<int> *matrix, int matRows, int matCols);

void condGraph(int numPoints, vector<vector<int>> *convAdjList, vector<vector<int>> *convAdjListCond, vector<vector<int>> *convAdjListCondRev, vector<int> *points);

void SPSearch(int centrPCond, vector<int> *visited, vector<vector<int>> *convAdjListCond, vector<vector<int>> *convAdjListCondRev, priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes> *searchPQueue);

int main(int argc, char const *argv[])
{
    // read metadata
    int numPoints, numConvs, iCentral;
    cin >> numPoints >> numConvs >> iCentral;

    // variables for original graph and reversed directions graph storing
    vector<vector<int>> convAdjList(numPoints);
    vector<vector<int>> convAdjListRev(numPoints);

    // read graph
    readConvs(&convAdjList, numConvs);

    // Find SCCs using Tarjan's algorithm

    stack<int> dfs_stack;
    vector<int> points(numPoints * NUMCOLS, UNVISITED);
    int openTime = 0;
    int lastSCC = 0;

    for (int point = 0; point < numPoints; ++point)
    {
        if (points[point * NUMCOLS] == UNVISITED)
        {
            tarjanStep(point, &dfs_stack, &points, &openTime, &lastSCC, &convAdjList);
        }
    }

    // create condensed graph
    vector<vector<int>> convAdjListCond(lastSCC);
    vector<vector<int>> convAdjListCondRev(lastSCC);
    condGraph(numPoints, &convAdjList, &convAdjListCond, &convAdjListCondRev, &points);

    // search condensed graph for shortest paths from central point
    int centrPCond = points[iCentral * NUMCOLS + 3];
    vector<int> visited(lastSCC, -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes> searchPQueue;

    SPSearch(centrPCond, &visited, &convAdjListCond, &convAdjListCondRev, &searchPQueue);

    // get sum of paths to delivery points
    int finCount = 0;
    for (int point = 0; point < lastSCC; point++)
    {
        if (convAdjListCondRev[point].empty())
            finCount += visited[point];
    }

    //printVec(visited);
    cout << finCount << endl;

    return 0;
}

void tarjanStep(int point, stack<int> *dfs_stack, vector<int> *points, int *openTime, int *lastSCC, vector<vector<int>> *convAdjList)
{
    points->at(point * NUMCOLS) = *openTime;
    points->at(point * NUMCOLS + 1) = *openTime;
    *openTime += 1;
    dfs_stack->push(point);
    points->at(point * NUMCOLS + 2) = 1;

    vector<int> successors = convAdjList->at(point); // point successors

    // consider each point's successor
    for (int successor : successors)
    {
        if (points->at(successor * NUMCOLS) == UNVISITED)
        {
            // successor not yet visited
            tarjanStep(successor, dfs_stack, points, openTime, lastSCC, convAdjList);
            points->at(point * NUMCOLS + 1) = min(points->at(point * NUMCOLS + 1), points->at(successor * NUMCOLS + 1));
        }
        else if (points->at(successor * NUMCOLS + 2) == 1)
        {
            //successor visited and is on stack
            points->at(point * NUMCOLS + 1) = min(points->at(point * NUMCOLS + 1), points->at(successor * NUMCOLS));
        }
    }

    // point is a root node, pop it from stack to SCC
    if (points->at(point * NUMCOLS + 1) == points->at(point * NUMCOLS))
    {
        // start a new SCC
        vector<int> SCC;
        int u = -1;

        //go through whole new SCC
        while (u != point)
        {
            u = dfs_stack->top();
            dfs_stack->pop();
            points->at(u * NUMCOLS + 2) = UNVISITED;

            SCC.push_back(u);
            points->at(u * NUMCOLS + 3) = *lastSCC;
        }

        *lastSCC += 1;
    }
}

void condGraph(int numPoints, vector<vector<int>> *convAdjList, vector<vector<int>> *convAdjListCond, vector<vector<int>> *convAdjListCondRev, vector<int> *points)
{
    int succCond, compCond;
    for (int i = 0; i < numPoints; i++)
    {

        compCond = points->at(i * NUMCOLS + 3);
        for (int succ : convAdjList->at(i))
        {
            succCond = points->at(succ * NUMCOLS + 3);
            if (succCond != compCond)
            {
                convAdjListCond->at(compCond).push_back(succCond);
                convAdjListCondRev->at(succCond).push_back(compCond);
            }
        }
    }
}

void SPSearch(int centrPCond, vector<int> *visited, vector<vector<int>> *convAdjListCond, vector<vector<int>> *convAdjListCondRev, priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes> *searchPQueue)
{
    searchPQueue->push(make_pair(centrPCond, 0));
    visited->at(centrPCond) = 0;

    while (!searchPQueue->empty())
    {
        auto topElement = searchPQueue->top();
        int point = topElement.first;
        int price = topElement.second;
        searchPQueue->pop();

        if (visited->at(point) != -1 && visited->at(point) < price)
            continue;

        visited->at(point) = price;

        for (int succ : convAdjListCond->at(point))
        {
            if (visited->at(succ) == -1 || visited->at(succ) > price)
            {
                searchPQueue->push(make_pair(succ, price));
            }
        }

        for (int succ : convAdjListCondRev->at(point))
        {
            if (visited->at(succ) == -1 || visited->at(succ) > price)
            {
                searchPQueue->push(make_pair(succ, price + 1));
            }
        }
    }
}

void readConvs(vector<vector<int>> *convAdjList, int numConvs)
{
    // read values from input, add directly create adjacecny list
    int pointA, pointB;

    for (int i = 0; i < numConvs; ++i)
    {
        cin >> pointA >> pointB;
        convAdjList->at(pointA).push_back(pointB);
    }
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void printGraph(vector<vector<int>> adj, int V)
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