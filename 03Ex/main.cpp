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

struct CompareDegrees
{
    inline bool operator()(pair<int, int> a, pair<int, int> b)
    {
        return a.first > b.first;
    }
};

typedef priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes>
    p_queue_pair;
typedef priority_queue<vector<int>, vector<vector<int>>, CompareRows> p_queue_vec;

vector<vector<int>> packBuffer;
p_queue_vec packFinal;

// Utilized approach: DFS from each hub

void readEdges(vector<vector<int>> *adjList, int numV);

void printQueue(p_queue_pair gq);

void printGraph(vector<vector<int>> adjList);

void printVec(vector<int> vec);

void printVecPairs(vector<pair<int, int>> vec);

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

int validatePack(vector<int> pack, vector<vector<int>> adjList)
{
    int packSize = pack.size();

    vector<int> mask(adjList.size(), -1);

    for (int i = 0; i < packSize; ++i)
    {
        mask[pack[i]] = i;
    }

    // FIND if is the pack connected and contains right number of connss
    // utilities for components conectivity - uninon find variables
    vector<int> compBoss(packSize, -1);
    vector<int> compRank(packSize, 0);

    int parentA, parentB, maxParent;

    int numConns = 0;
    for (int i = 0; i < packSize; ++i) // check all pack members
    {
        for (int el : adjList[pack[i]]) //look at all their successors
        {
            if (mask[el] != -1)
            {
                numConns += 1;

                // Parents used for union-find alogrithm
                parentA = findParent(&compBoss, i);
                parentB = findParent(&compBoss, mask[el]);

                if (parentA != parentB) //not in the same component
                {
                    if (compRank[parentA] == compRank[parentB]) //select new boss
                        compRank[parentA] += 1;
                    maxParent = (compRank[parentA] > compRank[parentB]) ? parentA : parentB;
                    compBoss[parentA] = maxParent;
                    compBoss[parentB] = maxParent;
                }
            }
        }
    }

    // find wheter are all pack members connected
    for (int i = 0; i < packSize; ++i)
    {
        if (compBoss[i] == -1)
            return -1;
    }

    return numConns / 2;
}

void kSubset(vector<int> set, int iEnd, vector<int> result, int remainingDepth, int packConns, vector<vector<int>> adjList)
{
    //function generating all k-subsets of given set

    if (remainingDepth < 0) // all digits covered, return
    {
        if (validatePack(result, adjList) == packConns)
        {
            packBuffer.push_back(result);
            //printVec(result);
        }
        return;
    }

    // recursively solve for all remaining digits
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

bool checkIsomorphism(vector<int> packA, vector<int> packB, vector<vector<int>> adjList)
{
    int numSmA = packA.size();
    int numSmB = packB.size();
    vector<int> mask(adjList.size(), -1);

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

    vector<pair<int, int>> degsA(numSmA); //stores degrees of nodes and corresponding node names
    vector<pair<int, int>> degsB(numSmB);

    p_queue_pair edgesA;

    for (int i = 0; i < numSmA; ++i)
    {
        for (int el : adjList[packA[i]])
        {
            if (mask[el] == mask[packA[i]])
            {
                degsA[i].first++;
                degsA[i].second = packA[i];
                edgesA.push((el <= packA[i]) ? make_pair(el, packA[i]) : make_pair(packA[i], el));
            }
        }
    }

    p_queue_pair edgesB;
    for (int i = 0; i < numSmB; ++i)
    {
        for (int el : adjList[packB[i]])
        {
            // cout << packA[i] << " " << el << " "
            //      << " " << mask[packA[i]] << " " << mask[el] << endl;
            if (mask[el] == mask[packB[i]])
            {
                degsB[i].first++;
                degsB[i].second = packB[i];
                edgesB.push((el <= packB[i]) ? make_pair(el, packB[i]) : make_pair(packB[i], el));
            }
        }
    }

    // cout << "A: ";
    // printVec(packA);
    // cout << "B: ";
    // printVec(packB);
    sort(degsA.begin(), degsA.end(), CompareDegrees());
    sort(degsB.begin(), degsB.end(), CompareDegrees());
    cout << "A: ";
    printVecPairs(degsA);
    cout << "B: ";
    printVecPairs(degsB);
    //sort(packB.begin(), packB.end());
    //printVec(packB);
    bool nextPerm = true;
    int n1, n2, vA, vB;

    while (nextPerm)
    {
        p_queue_pair edgesB_t = edgesB;
        p_queue_pair edgesB_r;

        //printVec(packB);
        while (!edgesB_t.empty())
        {

            vA = edgesB_t.top().first;
            vB = edgesB_t.top().second;
            edgesB_t.pop();

            n1 = packA[find(packB.begin(), packB.end(), vA) - packB.begin()];
            n2 = packA[find(packB.begin(), packB.end(), vB) - packB.begin()];
            edgesB_r.push((n1 <= n2) ? make_pair(n1, n2) : make_pair(n2, n1));
        }

        if (compareEdges(edgesA, edgesB_r))
            return false;

        //return false;
        nextPerm = next_permutation(packB.begin(), packB.end());
    }

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
            if (!checkIsomorphism(packA, packB, adjList))
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