#include <iostream>
#include <math.h>

#include <bits/stdc++.h>

#define UNVISITED -2
#define NUMCOLS 4

using namespace std;

void readEdges(vector<map<char, int>> *adjList, int numV, int numA);

void printGraph(vector<map<char, int>> adjList);

void printVec(vector<int> vec);

void printVecPairs(vector<pair<int, int>> vec);

void printMatrix(vector<int> *matrix, int matRows, int matCols);

void getPrimeFactors(int num, vector<int> *primeVec)
{
    while (num % 2 == 0)
    {
        num = num / 2;
        primeVec->push_back(2);
    }

    for (int i = 3; i <= sqrt(num); i = i + 2)
    {
        while (num % i == 0)
        {
            num = num / i;
            primeVec->push_back(i);
        }
    }

    if (num > 2)
        primeVec->push_back(num);
}

bool fullPerLCG(int A, int C, int M, int P)
{
    if (M % 4 == 0 && (A - 1) % 4 != 0)
        return false;

    if (C > M && __gcd(M, C) != 1)
        return false;

    if (M % C == 0)
        return false;
    int num = M;
    int distPrimes = 0;

    bool used = false; // was this prime already used

    while (num % 2 == 0)
    {
        num = num / 2;
        if ((A - 1) % 2 != 0 || C % 2 == 0)
            return false;
        if (!used)
        {
            used = true;
            distPrimes += 1;
            // if (distPrimes > P)
            //     return false;
        }
    }
    used = false;

    for (int i = 3; i <= sqrt(num); i = i + 2)
    {
        while (num % i == 0)
        {
            num = num / i;
            if ((A - 1) % i != 0 || C % i == 0)
                return false;
            if (!used)
            {
                used = true;
                distPrimes += 1;
                // if (distPrimes > P)
                //     return false;
            }
        }
        used = false;
    }

    if (num > 1)
    {
        if ((A - 1) % num != 0 || C % num == 0)
            return false;
        distPrimes += 1;
    }

    if (distPrimes != P)
        return false;
    //printf("%d, %d \n", distPrimes, P);

    return true;
}

int main(int argc, char const *argv[])
{
    // read metadata - num. of automaton states, Alphabet size, num. final states, number of negative and positive examples and their length
    int Amin, Amax, Cmin, Cmax, Mmin, Mmax, P;
    cin >> Amin >> Amax >> Cmin >> Cmax >> Mmin >> Mmax >> P;

    int c = 0;
    for (int M = Mmin; M < Mmax; M++)
    {
        for (int C = Cmin; C < Cmax; C++)
        {
            for (int A = Amin; A < Amax; A++)
            {
                if (fullPerLCG(A, C, M, P))
                    c += 1;
                //printf("%d %d %d\n", A, C, M);
            }
        }
    }
    cout << c << endl;
    //printGraph(adjList);

    return 0;
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, ASSIGNMENT SPECIFIC FUNCTIONS ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

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
