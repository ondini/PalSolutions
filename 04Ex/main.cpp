#include <iostream>
#include <cmath>
#include <stack>
#include <map>
#include <set>

#include <bits/stdc++.h>

#define UNVISITED -2
#define NUMCOLS 4

using namespace std;

void readEdges(vector<map<char, int>> *adjList, int numV, int numA);

void printGraph(vector<map<char, int>> adjList);

void printVec(vector<int> vec);

void printVecPairs(vector<pair<int, int>> vec);

void printMatrix(vector<int> *matrix, int matRows, int matCols);

void readWords(vector<string> *posList, vector<string> *negList, int numPos, int numNeg);

int main(int argc, char const *argv[])
{
    // read metadata - num. of automaton states, Alphabet size, num. final states, number of negative and positive examples and their length
    int numStates, sizeAlpha, numFins, numPosits, numNegats, explLength;
    cin >> numStates >> sizeAlpha >> numFins >> numPosits >> numNegats >> explLength;

    // variable for graph and its filling
    vector<map<char, int>> adjList(numStates);
    vector<string> posList(numPosits);
    vector<string> negList(numNegats);
    vector<set<int>> finStates(numStates);

    readEdges(&adjList, numStates, sizeAlpha);
    readWords(&posList, &negList, numPosits, numNegats);

    //printGraph(adjList);

    // main loop, trying all positive words from all states and collecting the finals
    int state;
    for (int i = 0; i < numStates; i++) //all possible start states
    {
        for (string word : posList) //all positive words
        {

            state = i;
            for (char ch : word)
            {
                state = adjList[state][ch];
            }
            finStates[i].insert(state); // collect final state

            if ((int)finStates[i].size() > numFins) //num of final states exceeded
                break;
        }
    }

    for (int u = 0; u < numStates; u++) //print the results
    {
        if ((int)finStates[u].size() != numFins)
            continue;

        cout << u << " ";
        int i = 0;
        for (int fin : finStates[u])
        {
            cout << fin;
            if (i != numFins - 1)
                cout << " ";
            i++;
        }
        cout << "\n";
    }

    return 0;
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, GET METADATA FUNCTIONS ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void readEdges(vector<map<char, int>> *adjList, int numV, int numA)
{
    // read values from input, add directly create adjacecny list
    int endState, startState;

    for (int i = 0; i < numV; ++i)
    {
        cin >> startState;
        for (int j = 0; j < numA; ++j)
        {
            cin >> endState;
            adjList->at(startState)[(char)(97 + j)] = endState;
        }
    }
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, ASSIGNMENT SPECIFIC FUNCTIONS ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void readWords(vector<string> *posList, vector<string> *negList, int numPos, int numNeg)
{
    string word;

    for (int i = 0; i < numPos; ++i)
    {

        cin >> word;
        posList->at(i) = word;
    }
    for (int i = 0; i < numNeg; ++i)
    {
        cin >> word;
        negList->at(i) = word;
    }
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, PRINT FUNCTIONS ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void printGraph(vector<map<char, int>> adjList)
{
    int V = adjList.size();
    for (int u = 0; u < V; u++)
    {
        cout << u;
        for (auto el : adjList[u])
        {
            cout << " --" << el.first << "--> " << el.second;
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
