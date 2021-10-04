#include <iostream>
#include <cmath>
#include <stack>

#include <bits/stdc++.h>

using namespace std;

// Utilized approach: DFS from each hub

void pushEdge(vector<pair<int, int>> roadsAdj[], int farmA, int farmB, int price);

void printGraph(vector<pair<int, int>> adj[], int V);

void printVec(vector<pair<int, int>> adj);

void printMatrix(int *matrix, int matRows, int matCols);

class CompareNodes
{
public:
    bool operator()(tuple<int, int, int> a, tuple<int, int, int> b)
    {
        return get<2>(a) > get<2>(b);
    }
};

void printQueue(priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> gq);

void readEdges(vector<pair<int, int>> roadAdjList[], int numRoads,
               priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> *roads);

void readHubs(int *hubs, int *farms, int numHubs);

int findParent(int parent[], int i);

int main(int argc, char const *argv[])
{

    // read metadata
    int numFarms, numRoads;
    cin >> numFarms >> numRoads;

    //cout << numFarms << " " << numRoads << endl;

    //variables for graph storing
    int *farms = new int[numFarms * 3];
    vector<pair<int, int>> roadAdjList[numFarms];
    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> roads;

    readEdges(roadAdjList, numRoads, &roads);
    //printGraph(roadAdjList, numFarms);
    //printQueue(roads);

    // read metadata, and store hubs
    int numHubs;
    cin >> numHubs;
    int *hubs = new int[numHubs];

    readHubs(hubs, farms, numHubs);

    // Perform DFS from each hub

    stack<tuple<int, int, int>> dfs_stack;
    for (int i = 0; i < numHubs; i++)
    {
        int hub = hubs[i];

        vector<pair<int, int>> roads = roadAdjList[hub]; // hub successors

        //cout << "DFS from hub no. " << hub << endl;
        //printVec(roads);

        // push hub successors to the DFS stack
        for (auto road = roads.begin(); road != roads.end(); road++)
        {
            dfs_stack.push(make_tuple(road->first, road->second, road->second));
        }

        // DFS loop
        while (!dfs_stack.empty())
        {

            auto topElement = dfs_stack.top();
            int farm = get<0>(topElement);
            int price = get<1>(topElement); //price so far
            int priceLastRoad = get<2>(topElement);
            dfs_stack.pop();

            //cout << "Popped: " << farm << ' ' << price << endl;

            bool push = false;
            // if the node was not yet visited or was visited with higher cost, add to stack and change best hub
            if (farms[farm * 3 + 1] == 0 || farms[farm * 3 + 1] > price)
            {
                farms[farm * 3] = hub;
                farms[farm * 3 + 1] = price;
                farms[farm * 3 + 2] = priceLastRoad;

                push = true;
            }
            else if (farms[farm * 3 + 1] == price && farms[farm * 3] != hub)
            {
                // if the cost is same as from some other hub, note it
                farms[farm * 3] = -1;
                push = true;
                //cout << "Equality spotted at farm no. " << farm << endl;
            }
            // other possibilities are of no interest to us = farm is other hub, or
            // already visited with better cost or with same cost from same hub

            //something worth-pushing found
            if (push)
            {
                vector<pair<int, int>> roadsS = roadAdjList[farm];
                //cout << "Pushing succesors of farm no. " << farm << endl;
                //printVec(roadsS);

                for (auto roadS = roadsS.begin(); roadS != roadsS.end(); roadS++)
                {
                    int farmS = roadS->first;
                    int priceS = roadS->second;
                    dfs_stack.push(make_tuple(farmS, price + priceS, priceS));
                }
            }
        }
    }

    // compute number of unconnectable farms
    int numUnconnected = 0;

    for (int i = 0; i < numFarms; i++)
    {
        if (farms[i * 3] == -1)
        {
            numUnconnected += 1;
        }
    }

    // find MSTs

    int *finalFarmsBoss = new int[numFarms];
    fill_n(finalFarmsBoss, numFarms, -1);
    int *finalFarmsRank = new int[numFarms];

    int priceAB, farmA, farmB;
    int minCost = 0;

    int parentA, parentB, maxParent;
    //cout << "Final pops: ";
    while (!roads.empty())
    {
        priceAB = get<2>(roads.top());
        farmA = get<0>(roads.top());
        farmB = get<1>(roads.top());
        //cout << "\t " << priceAB << ':' << farmA << '-' << farmB;
        roads.pop();

        // Parents used for union-find
        parentA = findParent(finalFarmsBoss, farmA);
        parentB = findParent(finalFarmsBoss, farmB);
        //cout << " P:" << parentA << "-" << parentB;

        if (farms[farmA * 3] == farms[farmB * 3] && farms[farmA * 3] != -1 && parentA != parentB) //condition to process the farm
        {
            if (finalFarmsRank[parentA] == finalFarmsRank[parentB])
            {
                finalFarmsRank[parentA] += 1;
            }
            maxParent = (finalFarmsRank[parentA] > finalFarmsRank[parentB]) ? parentA : parentB;
            finalFarmsBoss[parentA] = maxParent;
            finalFarmsBoss[parentB] = maxParent;
            minCost += priceAB;
            //cout << " ADDED" << priceAB;
        }
    }

    //cout << endl;

    fprintf(stdout, "%d %d\n", minCost, numUnconnected);

    delete[] finalFarmsRank;
    delete[] finalFarmsBoss;
    delete[] hubs;
    delete[] farms;
    return 0;
}

int findParent(int parent[], int i)
{
    if (parent[i] == -1 || parent[i] == i)
    {
        parent[i] = i;
        return i;
    }
    return findParent(parent, parent[i]);
}

void pushEdge(vector<pair<int, int>> roadsAdj[], int farmA, int farmB, int price)
{
    roadsAdj[farmA].push_back(make_pair(farmB, price));
    roadsAdj[farmB].push_back(make_pair(farmA, price));
}

void printGraph(vector<pair<int, int>> adj[], int V)
{
    int v, w;
    for (int u = 0; u < V; u++)
    {
        cout << u;
        for (auto it = adj[u].begin(); it != adj[u].end(); it++)
        {
            v = it->first;
            w = it->second;
            cout << " -> (" << v << ", w:"
                 << w << ")";
        }
        cout << "\n";
    }
}

void printVec(vector<pair<int, int>> adj)
{
    cout << "( ";
    int node, price;
    for (auto it = adj.begin(); it != adj.end(); it++)
    {
        node = it->first;
        price = it->second;
        cout << node << "-" << price << "; ";
    }
    cout << ")\n";
}

void printQueue(priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> gq)
{
    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> g = gq;
    while (!g.empty())
    {
        cout << '\t' << get<2>(g.top()) << ':' << get<0>(g.top()) << '-' << get<1>(g.top());
        g.pop();
    }
    cout << '\n';
}

void readEdges(vector<pair<int, int>> roadAdjList[], int numRoads,
               priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> *roads)
{
    // read values from input, add directly create adjacecny list
    int farmA, farmB, price;

    for (int i = 0; i < numRoads; ++i)
    {
        cin >> farmA >> farmB >> price;
        pushEdge(roadAdjList, farmA, farmB, price);
        roads->push(make_tuple(farmA, farmB, price));
    }
}

void printMatrix(int *matrix, int matRows, int matCols)
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
            printf("%4d", matrix[i * matCols + j]);
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

void readHubs(int *hubs, int *farms, int numHubs)
{
    int iHub;
    for (int i = 0; i < numHubs; i++)
    {
        cin >> iHub;
        hubs[i] = iHub;
        farms[iHub * 3] = iHub;
        farms[iHub * 3 + 1] = -1;
        farms[iHub * 3 + 2] = -1;
    }
}
