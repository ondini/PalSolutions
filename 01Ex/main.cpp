#include <iostream>
#include <cmath>

#include <stack>
#include <queue>
#include <bits/stdc++.h>

using namespace std;

// Utilized approach: DFS from each hub

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

int getSum(int *farms, int numFarms)
{
    int length = 0;
    for (int i = 0; i < numFarms; i++)
    {
        if (farms[i * 3 + 1] != -1 && farms[i * 3] != -1)
        {
            length += farms[i * 3 + 2];
        }
    }
    return length;
}

class CompareNodes
{
public:
    bool operator()(tuple<int, int, int> a, tuple<int, int, int> b)
    {
        return get<2>(a) > get<2>(b);
    }
};

void showpq(priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> gq)
{
    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> g = gq;
    while (!g.empty())
    {
        cout << '\t' << get<0>(g.top()) << '-' << get<2>(g.top());
        g.pop();
    }
    cout << '\n';
}

int main(int argc, char const *argv[])
{

    // read metadata
    int numFarms, numRoads;
    cin >> numFarms >> numRoads;

    cout << numFarms << " " << numRoads << endl;

    //variables for graph storing
    int *farms = new int[numFarms * 3];
    vector<pair<int, int>> roadAdjList[numFarms];

    // read values from input, add directly create adjacecny list
    int farmA, farmB, price;

    for (int i = 0; i < numRoads; ++i)
    {
        cin >> farmA >> farmB >> price;
        pushEdge(roadAdjList, farmA, farmB, price);
    }

    printGraph(roadAdjList, numFarms);

    // read metadata
    int numHubs;
    cin >> numHubs;

    // read and mark hubs
    int *hubs = new int[numHubs];
    int iHub;

    for (int i = 0; i < numHubs; i++)
    {
        cin >> iHub;
        hubs[i] = iHub;
        farms[iHub * 3] = iHub;
        farms[iHub * 3 + 1] = -1;
        farms[iHub * 3 + 2] = -1;
    }

    // Creating priority queue for search

    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> PrQueue;

    // Perform search from each hub
    for (int i = 0; i < numHubs; i++)
    {
        int hub = hubs[i];

        vector<pair<int, int>> roads = roadAdjList[hub]; // hub successors

        cout << "SEARCHING from hub no. " << hub << endl;
        printVec(roads);

        // push hub successors to the Priority queue
        for (auto road = roads.begin(); road != roads.end(); road++)
        {
            PrQueue.push(make_tuple(road->first, road->second, road->second));
        }

        // Main Search loop
        while (!PrQueue.empty())
        {

            auto topElement = PrQueue.top();
            int farm = get<0>(topElement);
            int price = get<1>(topElement); //price so far
            int priceLastRoad = get<2>(topElement);
            PrQueue.pop();

            cout << "Popped: " << farm << ' ' << price << ' ' << priceLastRoad << endl;

            bool push = false;

            // if the node was not yet visited or was visited with higher cost, add to stack and change the best hub
            if (farms[farm * 3 + 1] == 0 || (farms[farm * 3] != hub && farms[farm * 3 + 1] > price))
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

                cout << "Equality spotted at farm no. " << farm << endl;
            }
            else if (farms[farm * 3] == hub && farms[farm * 3 + 1] > price)
            {
                farms[farm * 3 + 1] = price;
                push = true;
            }

            // other possibilities are of no interest to us = farm is other hub, or
            // already visited with better cost or with same cost from same hub

            //something worth-pushing found
            if (push)
            {
                vector<pair<int, int>> roadsS = roadAdjList[farm];
                cout << "Pushing succesors of farm no. " << farm << endl;
                printVec(roadsS);

                for (auto roadS = roadsS.begin(); roadS != roadsS.end(); roadS++)
                {
                    int farmS = roadS->first;
                    int priceS = roadS->second;
                    PrQueue.push(make_tuple(farmS, price + priceS, priceS));
                }
            }

            //      Matrix printer
            int matRows = numFarms;
            int matCols = 3;

            cout << "____________" << endl;
            for (int i = 0; i < matRows; ++i)
            {
                cout << "| ";
                for (int j = 0; j < matCols; ++j)
                {
                    cout << farms[i * matCols + j] << ' ';
                }
                cout << "| " << endl;
            }
            cout << "____________" << endl;
        }
    }

    // compute total road length and number of unconnectable farms
    int length = 0;
    int numUnconnected = 0;

    vector<pair<int, int>> roadAdjList2[numFarms];
    for (int i = 0; i < numFarms; i++)
    {
        if (farms[i * 3 + 1] != -1 && farms[i * 3] != -1)
        {
            length += farms[i * 3 + 2];
        }
        if (farms[i * 3] == -1)
        {
            numUnconnected += 1;
        }
        else
        {
            pushEdge(roadAdjList, farmA, farmB, price);
        }
    }

    //      Matrix printer
    int matRows = numFarms;
    int matCols = 3;

    cout << "____________" << endl;
    for (int i = 0; i < matRows; ++i)
    {
        cout << "| ";
        for (int j = 0; j < matCols; ++j)
        {
            cout << farms[i * matCols + j] << ' ';
        }
        cout << "| " << endl;
    }
    cout << "____________" << endl;

    printf("%d %d\n", length, numUnconnected);
    delete[] hubs;
    delete[] farms;
    return 0;
}
