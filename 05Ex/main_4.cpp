#include <iostream>
#include <math.h>

#include <bits/stdc++.h>

#define UNVISITED -2
#define NUMCOLS 4

using namespace std;

typedef struct
{
    long Amin;
    long Amax;
    long Cmin;
    long Cmax;
    long Mmin;
    long Mmax;
    long P;
} LCGspecs;

void readEdges(vector<map<char, int>> *adjList, int numV, int numA);

void printGraph(vector<map<char, int>> adjList);
void printVec(vector<int> vec);
void printVec(vector<long> vec);
void printVecPairs(vector<pair<int, int>> vec);
void printMatrix(vector<int> *matrix, int matRows, int matCols);

long eratos_sieve(long Pmax, vector<long> *primes);
vector<long> getLCGs(vector<long> *primes, long start, long MSoFar, LCGspecs LCG, long Nprimes, long backSize, vector<long> factors);

int main(int argc, char const *argv[])
{
    // read metadata - num. of automaton states, Alphabet size, num. final states, number of negative and positive examples and their length
    long Amin, Amax, Cmin, Cmax, Mmin, Mmax, P;
    cin >> Amin >> Amax >> Cmin >> Cmax >> Mmin >> Mmax >> P;
    LCGspecs LCG = {Amin, Amax, Cmin, Cmax, Mmin, Mmax, P};

    long factors[] = {2, 6, 30, 210, 2310, 30030, 510510, 9699690}; // 223092870
    long Pmax = Mmax / factors[P - 2];                              // max reasonable value of prime factor of Mmax

    vector<long> primes(1.25506 * Pmax / log10(Pmax) + 1); // num of primes smaller than Pmax is upperbounded by this relation
    long Nprimes = eratos_sieve(Pmax, &primes);
    primes.resize(Nprimes);

    vector<long> factorsM(P); // varible for storing prime factors of M
    vector<long> ret = getLCGs(&primes, 0, 1, LCG, Nprimes, P - 1, factorsM);
    cout << ret[1] << endl;
    return 0;
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, ASSIGNMENT SPECIFIC FUNCTIONS ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

long eratos_sieve(long Pmax, vector<long> *primes)
{                                // generate all primes smaller than some Pmax
    vector<uint8_t> sieve(Pmax); // sieve itself
    long prime_i = 0;            // prime counter and indexer

    for (long i = 2; i <= Pmax; i++)
    {
        if (sieve[i] == 0) // is prime
        {
            primes->at(prime_i) = i;
            prime_i += 1;
            if (i <= sqrt(Pmax))
            { // there are multiples to be marked
                for (long j = 2 * i; j <= Pmax; j += i)
                {
                    sieve[j] = 1;
                }
            }
        }
    }
    return prime_i;
}

void kSubset(vector<long> set, int iEnd, vector<long> result, int remainingDepth, vector<vector<long>> *subsetBuffer)
{
    //function generating all k-subsets of given set

    if (remainingDepth < 0) // all digits covered, return
    {
        subsetBuffer->push_back(result);
        //printVec(result);
        return;
    }

    // recursively solve for all remaining digits
    for (int i = iEnd; i > (remainingDepth - 1); --i)
    {
        result[remainingDepth] = set[i];
        kSubset(set, i - 1, result, remainingDepth - 1, subsetBuffer);
    }
}

long getC(vector<long> factors, LCGspecs LCG, int numFacs, long sign)
{

    vector<vector<long>> subsetBuffer;
    vector<long> result(numFacs, 0);
    kSubset(factors, (int)factors.size() - 1, result, numFacs - 1, &subsetBuffer);
    long numC = 0;
    for (int i = 0; i < (int)subsetBuffer.size(); ++i)
    {
        long factor = 1;
        for (int j = 0; j < (int)subsetBuffer[i].size(); j++)
        {
            factor *= subsetBuffer[i][j];
        }
        numC += sign * (floor((LCG.Cmax) / (double)factor) - ceil((LCG.Cmin) / (double)factor) + 1);
    }
    if (numFacs < (int)factors.size())
        numC += getC(factors, LCG, numFacs + 1, sign * -1);

    return numC;
}

vector<long> getLCGs(vector<long> *primes, long start, long MSoFar, LCGspecs LCG, long Nprimes, long backSize, vector<long> factors)
{
    //printf("M== D: %ld, MSoFar: %ld, start:%ld \n", Pa - backSize, MSoFar, start);
    long LCGCount = 0; // num of valid LCGs in this loop
    long i = start;
    vector<long> ret(2);
    long prime, newM, numA, numC;
    for (; i < Nprimes - backSize; ++i) // iterator in the range of primes
    {
        prime = primes->at(i);
        newM = MSoFar * prime;
        if (backSize == 0)
        {                        //last factor of the P factors
            if (newM > LCG.Mmax) // over the max bound
                break;

            else if (newM >= LCG.Mmin)
            { // bootm bound is ok
                numC = 0;
                // get num of As
                numA = floor((LCG.Amax - 1) / (double)newM) - ceil((LCG.Amin - 1) / (double)newM) + 1;
                // get num of Cs
                factors[LCG.P - backSize - 1] = prime;
                if (numA > 0)
                    numC = LCG.Cmax - LCG.Cmin - getC(factors, LCG, 1, 1) + 1;
                LCGCount += numA * numC;
            }
        }
        else
        { // not last factor, move deeper
            factors[LCG.P - backSize - 1] = prime;
            ret = getLCGs(primes, i + 1, MSoFar * prime, LCG, Nprimes, backSize - 1, factors);
            LCGCount += ret[1];
            if (ret[0] == -1) //there were no iterations, boud was initially exceeded
                break;
        }
    }

    // return vals
    ret[0] = 1;
    if (i == start) //there were no iterations, boud was initially exceeded
        ret[0] = -1;
    ret[1] = LCGCount;
    return ret;
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
    for (long el : vec)
    {
        cout << el << ", ";
    }
    cout << ")\n";
}

void printVec(vector<long> vec)
{
    cout << "( ";
    for (long el : vec)
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
