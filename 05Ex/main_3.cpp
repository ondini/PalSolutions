#include <iostream>
#include <math.h>

#include <bits/stdc++.h>

#define UNVISITED -2
#define NUMCOLS 4

using namespace std;

void readEdges(vector<map<char, int>> *adjList, int numV, int numA);

void printGraph(vector<map<char, int>> adjList);

void printVec(vector<int> vec);
void printVec(vector<long> vec);
void printVecPairs(vector<pair<int, int>> vec);
void printMatrix(vector<int> *matrix, int matRows, int matCols);

long eratos_sieve(long Pmax, vector<long> *primes);
void generate_valid_EFGs(vector<long> *primes, int P, long Mmax, long Mmin, long Nprimes);
vector<int> shit(vector<long> *primes, long start, long MSoFar, long Mmax, long Mmin, long Nprimes, long backSize, vector<long> factors);

long Pa, Ama, Ami, Cma, Cmi;
vector<long> preview;
vector<long> preview2;
int main(int argc, char const *argv[])
{
    // read metadata - num. of automaton states, Alphabet size, num. final states, number of negative and positive examples and their length
    long Amin, Amax, Cmin, Cmax, Mmin, Mmax, P;
    cin >> Amin >> Amax >> Cmin >> Cmax >> Mmin >> Mmax >> P;
    Pa = P;
    Ama = Amax;
    Ami = Amin;
    Cma = Cmax;
    Cmi = Cmin;

    long factors[] = {2, 6, 30, 210, 2310, 30030, 510510, 9699690}; // 223092870
    long Pmax = Mmax / factors[P - 2];                              // max reasonable value of prime factor of Mmax

    cout << "Pmax " << Pmax << endl;
    vector<long> primes(1.25506 * Pmax / log(Pmax) + 1); // num of primes smaller than Pmax is upperbounded by this relation
    long Nprimes = eratos_sieve(Pmax, &primes);
    primes.resize(Nprimes);
    cout << Nprimes << endl;
    printVec(primes);

    vector<long> factorsM(P); // varible for storing prime factors of M
    vector<int> ret = shit(&primes, 0, 1, Mmax, Mmin, Nprimes, P - 1, factorsM);
    printf("Final shit: %d, %d\n", ret[0], ret[1]);
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

long getC(vector<long> factors, long Cmin, long Cmax)
{ //a dumb function for getting C factors
    bool c = true;
    long count = 0;
    for (long C = Cmin; C <= Cmax; C++)
    {
        c = true;
        for (long i = 0; i < (long)factors.size(); i++)
        {
            //printf("     C== C:%d, factor: %d, C//f = %d\n", C, factors[i], C % factors[i]);
            if (C % factors[i] == 0)
            {
                //cout << "            konec\n";
                c = false;
                break;
            }
        }
        if (c)
        {
            //cout << "            +1\n";
            count += 1;
        }
    }
    //cout << "   count" << count << endl;
    return count;
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

long getC2(vector<long> factors, long Cmin, long Cmax, int numFacs, int sign)
{

    vector<vector<long>> subsetBuffer;
    vector<long> result(numFacs, 0);
    kSubset(factors, (int)factors.size() - 1, result, numFacs - 1, &subsetBuffer);
    printf("Geting C, susBufSize%d\n", (int)subsetBuffer.size());
    long numC = 0;
    long res;
    for (int i = 0; i < (int)subsetBuffer.size(); ++i)
    {
        long factor = 1;
        printVec(subsetBuffer[i]);
        for (int j = 0; j < (int)subsetBuffer[i].size(); j++)
        {
            factor *= subsetBuffer[i][j];
        }
        res = sign * (floor((Cmax) / (double)factor) - ceil((Cmin) / (double)factor) + 1);
        numC += res;
        printf("Factor: %ld, res: %ld, %lf, %lf, numC: %ld \n", factor, res, floor((Cmax) / (double)factor), ceil((Cmin) / (double)factor), numC);

        preview.push_back(sign * factor);
        preview2.push_back(res);
    }
    if (numFacs < (int)factors.size())
        numC += getC2(factors, Cmin, Cmax, numFacs + 1, sign * -1);

    printf("returning K %ld\n", numC);
    return numC;
}

vector<int> shit(vector<long> *primes, long start, long MSoFar, long Mmax, long Mmin, long Nprimes, long backSize, vector<long> factors)
{
    printf("M== D: %ld, MSoFar: %ld, start:%ld \n", Pa - backSize, MSoFar, start);
    long LCGCount = 0; // num of valid LCGs in this loop
    long i = start;
    vector<int> temp(2);
    long prime, newM, numA, numC, numC2;
    for (; i < Nprimes - backSize; ++i) // iterator in the range of primes
    {
        for (int j = 0; j < Pa - backSize; j++)
        {
            cout << " ";
        }

        printf("M== d:%ld, i:%ld, p_i: %ld", Pa - backSize, i, primes->at(i));
        prime = primes->at(i);
        newM = MSoFar * prime;
        if (backSize == 0)
        {
            if (newM > Mmax)
            { // over the max bound
                cout << endl;
                break;
            }
            else if (newM > Mmin)
            { // bootm bound is ok
                numA = floor((Ama - 1) / (double)newM) - ceil((Ami - 1) / (double)newM) + 1;
                cout << " -> " << newM << " :A: " << numA;
                factors[Pa - backSize - 1] = prime;
                if (numA)
                {
                    printVec(factors);
                    numC2 = 0;
                    numC2 = Cma - Cmi - getC2(factors, Cmi, Cma, 1, 1) + 1;
                    printVec(preview);
                    printVec(preview2);
                    numC = getC(factors, Cmi, Cma);
                    printf("Got C2 %ld, %ld!\n", numC, numC2);
                }
                cout << endl;
                LCGCount += numA * numC;
            }
            cout << endl;
        }
        else
        {
            cout << endl;
            factors[Pa - backSize - 1] = prime;
            temp = shit(primes, i + 1, MSoFar * prime, Mmax, Mmin, Nprimes, backSize - 1, factors);
            LCGCount += temp[1];
            if (temp[0] == -1)
                break;
        }
    }

    // return vals
    temp[0] = 1;
    if (i == start)
        temp[0] = -1;
    temp[1] = LCGCount;
    return temp;
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
