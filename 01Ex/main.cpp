#include <iostream>
#include <cmath>
#include <stack>

#include <bits/stdc++.h>

using namespace std;

// Utilized approach: DFS from each hub

class CompareNodes
{
public:
    bool operator()(tuple<int, int, int> a, tuple<int, int, int> b)
    {
        return get<2>(a) > get<2>(b);
    }
};

class CompareNodes2
{
public:
    bool operator()(tuple<int, int, int, int> a, tuple<int, int, int, int> b)
    {
        return get<2>(a) > get<2>(b);
    }
};

void readEdges(vector<vector<pair<int, int>>> *roadAdjList, int numRoads,
               priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> *roads);

void readHubs(vector<int> *hubs, vector<int> *farms, int numHubs);

int findParent(vector<int> *farms, int i);

void printGraph(vector<pair<int, int>> adj[], int V);

void printVec(vector<pair<int, int>> adj);

void printQueue(priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> gq);

void printMatrix(int *matrix, int matRows, int matCols);

int main(int argc, char const *argv[])
{

    // read metadata
    int numFarms, numRoads;
    cin >> numFarms >> numRoads;

    //cout << numFarms << " " << numRoads << endl;

    //variables for graph storing
    vector<int> farms(numFarms * 2, 0);
    vector<vector<pair<int, int>>> roadAdjList(numFarms);
    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> roads;

    readEdges(&roadAdjList, numRoads, &roads);
    //printGraph(roadAdjList, numFarms);
    //printQueue(roads);

    // read metadata, and store hubs
    int numHubs;
    cin >> numHubs;
    vector<int> hubs(numHubs);

    readHubs(&hubs, &farms, numHubs);

    // Perform DFS from each hub

    //stack<tuple<int, int, int, int>> dfs_stack;

    priority_queue<tuple<int, int, int, int>, vector<tuple<int, int, int, int>>, CompareNodes2> PrQueue;
    for (int i = 0; i < numHubs; i++)
    {
        int hub = hubs[i];

        vector<pair<int, int>> roads = roadAdjList[hub]; // hub successors

        //cout << "DFS from hub no. " << hub << endl;
        //printVec(roads);

        // push hub successors to the DFS stack
        for (auto road = roads.begin(); road != roads.end(); road++)
        {
            PrQueue.push(make_tuple(road->first, road->second, road->second, hub));
        }
    }

    // DFS loop
    int numUnconnected = 0;
    while (!PrQueue.empty())
    {
        auto topElement = PrQueue.top();
        int farm = get<0>(topElement);
        int price = get<1>(topElement); //price so far
        //int priceLastRoad = get<2>(topElement);
        int parentHub = get<3>(topElement);
        PrQueue.pop();

        //cout << "Popped: " << farm << ' ' << price << endl;

        bool push = false;
        // if the node was not yet visited or was visited with higher cost, add to stack and change best hub
        if (farms[farm * 2 + 1] == 0 || farms[farm * 2 + 1] > price)
        {
            if (farms[farm * 2] == -1)
                numUnconnected -= 1;
            farms[farm * 2] = parentHub;
            farms[farm * 2 + 1] = price;
            push = true;
        }
        else if (farms[farm * 2 + 1] == price && farms[farm * 2] != parentHub && farms[farm * 2] != -1)
        {
            // if the cost is same as from some other hub, note it
            if (farms[farm * 2] != -1)
                numUnconnected += 1;
            farms[farm * 2] = -1;
            push = true;
        }
        // other possibilities are of no interest to us = farm is other hub, or
        // already visited with better cost or with same cost from same hub

        //something worth-pushing found
        if (push)
        {
            vector<pair<int, int>> roadsS = roadAdjList[farm];

            for (auto roadS = roadsS.begin(); roadS != roadsS.end(); roadS++)
            {
                int farmS = roadS->first;
                int priceS = roadS->second;
                PrQueue.push(make_tuple(farmS, price + priceS, priceS, parentHub));
            }
        }
    }

    // find MSTs

    vector<int> finalFarmsBoss(numFarms, -1);
    vector<int> finalFarmsRank(numFarms, 0);
    //finalFarmsRank.reserve(numFarms);

    int priceAB,
        farmA, farmB;
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
        parentA = findParent(&finalFarmsBoss, farmA);
        parentB = findParent(&finalFarmsBoss, farmB);
        //cout << " P:" << parentA << "-" << parentB;

        if (farms[farmA * 2] == farms[farmB * 2] && farms[farmA * 2] != -1 && parentA != parentB) //condition to process the farm
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

    return 0;
}

int findParent(vector<int> *farms, int i)
{
    if (farms->at(i) == -1 || farms->at(i) == i)
    {
        farms->at(i) = i;
        return i;
    }
    return findParent(farms, farms->at(i));
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

void readEdges(vector<vector<pair<int, int>>> *roadAdjList, int numRoads,
               priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareNodes> *roads)
{
    // read values from input, add directly create adjacecny list
    int farmA, farmB, price;

    for (int i = 0; i < numRoads; ++i)
    {
        cin >> farmA >> farmB >> price;
        roadAdjList->at(farmA).push_back(make_pair(farmB, price));
        roadAdjList->at(farmB).push_back(make_pair(farmA, price));
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

void readHubs(vector<int> *hubs, vector<int> *farms, int numHubs)
{
    int iHub;
    for (int i = 0; i < numHubs; i++)
    {
        cin >> iHub;
        hubs->at(i) = iHub;
        farms->at(iHub * 2) = iHub;
        farms->at(iHub * 2 + 1) = -1;
    }
}
