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

// global variables
vector<vector<int>> packBuffer;
p_queue_vec packFinal;

// Utilized approach: DFS from each hub

void readEdges(vector<vector<int>> *adjList, int numV);

void printQueue(p_queue_pair gq);

void printGraph(vector<vector<int>> adjList);

void printVec(vector<int> vec);

void printVecPairs(vector<pair<int, int>> vec);

void printMatrix(vector<int> *matrix, int matRows, int matCols);

int findParent(vector<int> *boss, int i);

int validatePack(vector<int> pack, vector<vector<int>> adjList);

void kSubset(vector<int> set, int iEnd, vector<int> result, int remainingDepth, int packConns, vector<vector<int>> adjList);

bool compareNodes(p_queue_pair edgesA, p_queue_pair edgesB, vector<int> *packA, vector<int> *packB);

bool checkIsomorphism(vector<int> packA, vector<int> packB, vector<vector<int>> adjList);

void getPackPairs(vector<vector<int>> adjList);

int main(int argc, char const *argv[])
{
    // read metadata - num. of smugglers, connections, pack size and num. of connections per pack
    int numSumgs, numConns, packSize, packConns;
    cin >> numSumgs >> numConns >> packSize >> packConns;

    // variable for graph and its filling
    vector<vector<int>> adjList(numSumgs);

    readEdges(&adjList, numConns);

    // create set to be k-subseted
    vector<int>
        set(numSumgs);
    for (int i = 0; i < numSumgs; i++)
    {
        set[i] = i;
    }

    vector<int> result(packSize, 0);

    kSubset(set, numSumgs - 1, result, packSize - 1, packConns, adjList);

    getPackPairs(adjList);
    return 0;
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, GET METADATA FUNCTIONS ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

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

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, UNION-FIND AND K-SUBSET FUNCTIONS ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

int findParent(vector<int> *boss, int i)
{
    if (boss->at(i) == -1 || boss->at(i) == i)
    {
        boss->at(i) = i;
        return i;
    }
    return findParent(boss, boss->at(i));
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
        //exit(1);
        return;
    }

    // recursively solve for all remaining digits
    for (int i = iEnd; i > (remainingDepth - 1); --i)
    {
        result[remainingDepth] = set[i];
        kSubset(set, i - 1, result, remainingDepth - 1, packConns, adjList);
    }
}
/// SUBFUNCTION containig union find main alogrithm and also  pack connection shit

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
    vector<int>
        compBoss(packSize, -1);
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
        {

            return -1;
        }
    }

    return numConns / 2;
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, ASSIGNMENT SPECIFIC FUNCTIONS ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

bool compareNodes(p_queue_pair edgesA, p_queue_pair edgesB, vector<int> *packA, vector<int> *packB)
{
    // compare permutation of nodes
    p_queue_pair edgesB_r;
    int n1, n2, vA, vB;
    while (!edgesB.empty())
    {
        // creat re-named variations of all edges according to permutation
        vA = edgesB.top().first;
        vB = edgesB.top().second;
        edgesB.pop();

        n1 = packA->at(find(packB->begin(), packB->end(), vA) - packB->begin());
        n2 = packA->at(find(packB->begin(), packB->end(), vB) - packB->begin());
        edgesB_r.push((n1 <= n2) ? make_pair(n1, n2) : make_pair(n2, n1));
    }
    // chcek whether are all renamed edges the same as in the other pack
    while (!edgesA.empty() || !edgesB_r.empty())
    {
        if (edgesA.top().first != edgesB_r.top().first || edgesA.top().second != edgesB_r.top().second)
        {
            return false;
        }
        edgesA.pop();
        edgesB_r.pop();
    }
    //the length must be the same
    if (edgesA.empty() && edgesB_r.empty())
    {
        return true;
    }
    return false;
}

void getPackPairs(vector<vector<int>> adjList)
{
    // generate all pack pairs and check each for isomorphism

    int numGenPacks = packBuffer.size();

    vector<int> packA, packB;

    for (int i = 0; i < numGenPacks - 1; i++)
    {
        for (int j = i + 1; j < numGenPacks; j++)
        {
            packA = packBuffer[i];
            packB = packBuffer[j];
            if (checkIsomorphism(packA, packB, adjList))
            {
                //pack pair is ismorphic, put it inside the priority que in correct order
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

    // print the result
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

bool checkIsomorphism(vector<int> packA, vector<int> packB, vector<vector<int>> adjList)
{
    //LOONG function for isomorphism checking
    // create component mask
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
            return false;

        mask[packB[i]] = 2;
    }

    // store node degrees and edges
    vector<pair<int, int>> degsA(numSmA); //stores degrees of nodes and corresponding node names
    vector<pair<int, int>> degsB(numSmB);
    p_queue_pair edgesA;
    p_queue_pair edgesB;

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

    for (int i = 0; i < numSmB; ++i)
    {
        for (int el : adjList[packB[i]])
        {
            if (mask[el] == mask[packB[i]])
            {
                degsB[i].first++;
                degsB[i].second = packB[i];
                edgesB.push((el <= packB[i]) ? make_pair(el, packB[i]) : make_pair(packB[i], el));
            }
        }
    }

    // Sort nodes by degrees.
    sort(degsA.begin(), degsA.end(), CompareDegrees());
    sort(degsB.begin(), degsB.end(), CompareDegrees());

    // Chceck neccessary condition of the same node degrees and create borders
    // for blocks of same degree. Permutations are performed only in these blocks.
    vector<int> borders;
    borders.push_back(0);
    for (int i = 0; i < numSmA; i++)
    {
        if (degsA[i].first != degsB[i].first)
            return false;
        if (i > 0 && degsA[i - 1].first != degsA[i].first)
            borders.push_back(i);

        packA[i] = degsA[i].second;
        packB[i] = degsB[i].second;
    }
    borders.push_back(numSmA);

    // start brute-force permutation checking loop
    bool nextPerm = true;
    bool cont = true;
    int numBors = borders.size();
    int bor = numBors - 2;

    while (cont)
    {
        if (nextPerm) //next permutation exist
        {
            if (compareNodes(edgesA, edgesB, &packA, &packB)) // the permutation is correct
            {
                return true;
            }
            if (bor != numBors - 2) // the permutation is not correct, proceed to lowerd permutation block, if there is any
                bor++;
        }
        else if (bor == 0) // next permutation does not exist and we have already processed the topomost permutation block
        {
            cont = false; //therefore the pack combination is invalid
        }
        else // next permutation does not exist, go to higher perm. block
        {
            sort(packB.begin() + borders[bor], packB.begin() + borders[bor + 1]);
            bor--;
        }
        nextPerm = next_permutation(packB.begin() + borders[bor], packB.begin() + borders[bor + 1]);
    }

    return false;
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