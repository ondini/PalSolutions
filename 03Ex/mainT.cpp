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

class CompareRows
{
public:
    bool operator()(vector<int> a, vector<int> b)
    {
        int N = a.size();
        for (int i = 0; i < N; i++)
        {
            if (a[i] > b[i])
            {
                return true;
            }
            else if (a[i] < b[i])
            {
                return false;
            }
        }

        return false;
    }
};

typedef priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes> p_queue_pair;
typedef priority_queue<vector<int>, vector<vector<int>>, CompareRows> p_queue_vec;

vector<vector<int>> packBuffer;
p_queue_vec packFinal;

// Utilized approach: DFS from each hub

void readEdges(vector<vector<int>> *adjList, int numV);

void printQueue(p_queue_pair gq);

void printGraph(vector<vector<int>> adjList);

void printVec(vector<int> vec);

void printMatrix(vector<int> *matrix, int matRows, int matCols);

int findParent(vector<int> *boss, int i)
{
    if (boss->at(i) == -1 || boss->at(i) == i)
    {
        boss->at(i) = i;
        return i;
    }
    return findParent(boss, boss->at(i));
}

int getPackConns(vector<int> pack, vector<vector<int>> adjList)
{
    int V = pack.size();
    vector<int> mask(adjList.size(), -1);

    for (int i = 0; i < V; ++i)
    {
        mask[pack[i]] = i;
    }

    //printVec(pack);
    // utilities for components conectivity
    vector<int> compBoss(V, -1);
    vector<int> compRank(V, 0);

    int parentA, parentB, maxParent;

    int numConns = 0;
    for (int i = 0; i < V; ++i)
    {
        //cout << "Pack_member " << pack[i] << " adjList size " << adjList[pack[i]].size();
        for (int el : adjList[pack[i]])
        {
            //cout << pack[i] << " " << el << " " << mask[el] << endl;
            if (mask[el] != -1)
            {
                numConns += 1;
                // Parents used for union-find
                parentA = findParent(&compBoss, i);
                parentB = findParent(&compBoss, mask[el]);

                //cout << parentA << " " << parentB << endl;

                if (parentA != parentB) //condition to process the farm
                {
                    //cout << compRank[parentA] << compRank[parentB];
                    if (compRank[parentA] == compRank[parentB])
                    {

                        compRank[parentA] += 1;
                        //cout << "bigging " << compRank[parentA] << endl;
                    }
                    maxParent = (compRank[parentA] > compRank[parentB]) ? parentA : parentB;
                    compBoss[parentA] = maxParent;
                    compBoss[parentB] = maxParent;
                }
                //printVec(compBoss);
                //printVec(compRank);
            }
        }
    }
    //cout << "FINISHED" << endl;

    for (int i = 0; i < V; ++i)
    {
        if (compBoss[i] == -1)
            return -1;
    }

    return numConns / 2;
}

void kSubset(vector<int> set, int iEnd, vector<int> result, int remainingDepth, int packConns, vector<vector<int>> adjList)
{
    //cout << remainingDepth;
    if (remainingDepth < 0)
    {
        if (getPackConns(result, adjList) == packConns)
        {

            packBuffer.push_back(result);
            //printVec(result);
        }
        return;
    }
    for (int i = iEnd; i > (remainingDepth - 1); --i)
    {
        result[remainingDepth] = set[i];
        kSubset(set, i - 1, result, remainingDepth - 1, packConns, adjList);
    }
}

bool compareEdges(p_queue_pair edgesA, p_queue_pair edgesB)
{
    while (!edgesA.empty() || !edgesB.empty())
    {
        if (edgesA.top().first != edgesB.top().first || edgesA.top().second != edgesB.top().second)
        {
            return false;
        }
        edgesA.pop();
        edgesB.pop();
    }
    if (edgesA.empty() && edgesB.empty())
    {
        return true;
    }
    return false;
}

bool checkOverlap(vector<int> packA, vector<int> packB, vector<vector<int>> adjList)
{
    int numSmA = packA.size();
    int numSmB = packB.size();
    vector<int> mask(adjList.size(), -1);
    // cout << "A: ";
    // printVec(packA);
    // cout << "B: ";
    // printVec(packB);

    for (int i = 0; i < numSmA; ++i)
    {
        mask[packA[i]] = 1;
    }

    for (int i = 0; i < numSmB; ++i)
    {

        if (mask[packB[i]] != -1)
            return true;

        mask[packB[i]] = 2;
    }

    //printVec(mask);

    vector<int> degsA(numSmA);
    vector<int> degsB(numSmB);

    p_queue_pair edgesA;

    for (int i = 0; i < numSmA; ++i)
    {
        for (int el : adjList[packA[i]])
        {
            // cout << packA[i] << " " << el << " "
            //      << " " << mask[packA[i]] << " " << mask[el] << endl;
            if (mask[el] == mask[packA[i]])
            {
                degsA[i]++;
                edgesA.push((el <= packA[i]) ? make_pair(el, packA[i]) : make_pair(packA[i], el));
            }
        }
    }

    for (int i = 0; i < numSmB; ++i)
    {
        for (int el : adjList[packB[i]])
        {
            // cout << packA[i] << " " << el << " "
            //      << " " << mask[packA[i]] << " " << mask[el] << endl;
            if (mask[el] == mask[packB[i]])
            {
                degsB[i]++;
            }
        }
    }

    // cout << "A: ";
    // printVec(packA);
    // cout << "B: ";
    // printVec(packB);

    sort(packB.begin(), packB.end());
    //printVec(packB);
    bool nextPerm = true;
    int n1, n2;

    while (nextPerm)
    {
        priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes> edgesB;

        //printVec(packB);
        for (int i = 0; i < numSmB; ++i)
        {
            for (int el : adjList[packB[i]])
            {
                // cout << packA[i] << " " << el << " "
                //      << " " << mask[packA[i]] << " " << mask[el] << endl;
                if (mask[el] == mask[packB[i]])
                {

                    n1 = packA[find(packB.begin(), packB.end(), el) - packB.begin()];
                    //cout << n1 << "=" << el << " ";
                    n2 = packA[find(packB.begin(), packB.end(), packB[i]) - packB.begin()];
                    //cout << n2 << "=" << packB[i] << "\n";
                    edgesB.push((n1 <= n2) ? make_pair(n1, n2) : make_pair(n2, n1));
                }
            }
        }
        if (compareEdges(edgesA, edgesB))
            return false;

        //return false;
        nextPerm = next_permutation(packB.begin(), packB.end());
    }

    return true;
}

bool checkIsomorphism(vector<int> packA, vector<int> packB)
{
    return true;
}

void getPackPairs(vector<vector<int>> adjList)
{
    int numGenPacks = packBuffer.size();
    vector<int> packA, packB;

    for (int i = 0; i < numGenPacks - 1; i++)
    {
        for (int j = i + 1; j < numGenPacks; j++)
        {
            //cout << " >>>>> TRYIN' " << i << "-" << j << endl;
            packA = packBuffer[i];
            packB = packBuffer[j];
            if (!checkOverlap(packA, packB, adjList))
            {
                if (checkIsomorphism(packA, packB))
                {
                    // cout << "ISO!\n";
                    // cout << "A: ";
                    // printVec(packA);
                    // cout << "B: ";
                    // printVec(packB);
                    if (packA[0] < packB[0])
                    {
                        packA.insert(packA.end(), packB.begin(), packB.end());
                        packFinal.push(packA);
                    }
                    else
                    {
                        packB.insert(packB.end(), packA.begin(), packA.end());
                        packFinal.push(packB);
                    }
                }
            }
        }
    }

    while (!packFinal.empty())
    {
        vector<int> vec = packFinal.top();
        int N = vec.size();
        for (int i = 0; i < N; ++i)
        {
            cout << vec[i];
            if (i != N - 1)
                cout << " ";
        }
        cout << "\n";
        packFinal.pop();
    }
}

int main(int argc, char const *argv[])
{
    // read metadata - num. of smugglers, connections, pack size and num. of connections per pack
    int numSumgs, numConns, packSize, packConns;
    cin >> numSumgs >> numConns >> packSize >> packConns;

    //cout << numSumgs << numConns << packSize << packConns;

    // variables for original graph and reversed directions graph storing
    vector<vector<int>> adjList(numSumgs);
    //vector<vector<int>> convAdjListRev(numPoints);

    // read graph
    readEdges(&adjList, numConns);
    //printGraph(adjList);

    // create set
    vector<int> set(numSumgs);
    for (int i = 0; i < numSumgs; i++)
    {
        set[i] = i;
    }

    vector<int> result(packSize, 0);

    kSubset(set, numSumgs - 1, result, packSize - 1, packConns, adjList);

    getPackPairs(adjList);
    return 0;
}

void readEdges(vector<vector<int>> *adjList, int numV)
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