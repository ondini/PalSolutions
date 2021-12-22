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

int eratos_sieve(int Pmax, vector<long> *primes);
void generate_valid_EFGs(vector<long> *primes, int P, int Mmax, int Mmin, int Nprimes);
vector<int> shit(vector<long> *primes, int start, int MSoFar, int Mmax, int Mmin, int Nprimes, int backSize);
// class EFGGen
// {
// public:
//     int CBound[2];
//     int MBound[2];
//     int ABound[2];
//     EFGGen(/* args */);
//     ~EFGGen();
// };

// EFGGen::EFGGen(/* args */)
// {
// }

// EFGGen::~EFGGen()
// {
// }
int Pa, Ama, Ami;

int main(int argc, char const *argv[])
{
    // read metadata - num. of automaton states, Alphabet size, num. final states, number of negative and positive examples and their length
    int Amin, Amax, Cmin, Cmax, Mmin, Mmax, P;
    cin >> Amin >> Amax >> Cmin >> Cmax >> Mmin >> Mmax >> P;
    Pa = P;
    Ama = Amax;
    Ami = Amin;

    int factors[] = {2, 6, 30, 210, 2310, 30030, 510510, 9699690}; // 223092870
    int Pmax = Mmax / factors[P - 2];                              // max reasonable value of prime factor of Mmax

    cout << "Pmax " << Pmax << endl;
    vector<long> primes(1.25506 * Pmax / log(Pmax)); //(Pmax > 16) ? (uint)(1.25506 * Pmax / log(Pmax)) : Pmax / 2 + 1);
    cout << primes.size() << endl;
    int Nprimes = eratos_sieve(Pmax, &primes);
    cout << Nprimes << endl;
    printVec(primes);

    //generate_valid_EFGs(&primes, P, Mmax, Mmin, Nprimes);

    vector<int> ret = shit(&primes, 0, 1, Mmax, Mmin, Nprimes, P - 1);
    printf("Final shit: %d, %d\n", ret[0], ret[1]);
    return 0;
}

int eratos_sieve(int Pmax, vector<long> *primes)
{                                // generate all primes smaller than some Pmax
    vector<uint8_t> sieve(Pmax); // sieve itself
    int prime_i = 0;             // prime counter and indexer

    for (int i = 2; i <= Pmax; i++)
    {
        if (sieve[i] == 0) // is prime
        {
            primes->at(prime_i) = i;
            prime_i += 1;
            if (i <= sqrt(Pmax))
            { // it does make sense to mark all multiples
                for (int j = 2 * i; j <= Pmax; j += i)
                {
                    sieve[j] = 1;
                }
            }
        }
    }
    return prime_i;
}

vector<int> shit(vector<long> *primes, int start, int MSoFar, int Mmax, int Mmin, int Nprimes, int backSize)
{
    printf("M== D: %d, MSoFar: %d, start:%d \n", Pa - backSize, MSoFar, start);
    int LCGCount = 0; // num of valid LCGs in this loop
    int i = start;
    vector<int> temp(2);
    long prime, newM, numA;
    for (; i < Nprimes - backSize; ++i) // iterator in the range of primes
    {
        for (int j = 0; j < Pa - backSize; j++)
        {
            cout << " ";
        }

        printf("M== d:%d, i:%d, p_i: %ld", Pa - backSize, i, primes->at(i));
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
                LCGCount += numA;
            }
            cout << endl;
        }
        else
        {
            cout << endl;
            temp = shit(primes, i + 1, MSoFar * prime, Mmax, Mmin, Nprimes, backSize - 1);
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

void generate_valid_EFGs(vector<long> *primes, int P, int Mmax, int Mmin, int Nprimes)
{
    vector<int> indices(P);
    for (int i = 0; i < P; i++)
    {
        indices[i] = i;
    }

    // start brute-force permutation checking loop
    bool cont = true;
    int ii = 0;
    int iP = 0;
    int Mtemp = 1;
    // std::accumulate(begin(vars), end(vars), 1, std::multiplies<double>());
    int numLCGs = 0;
    while (cont)
    {

        cout << "ii  " << ii << endl;
        printVec(indices);
        if (ii == P - 1) // index ii pointing at last of prime factor index, start looping
        {
            iP = indices[ii];    // get the index
            while (iP < Nprimes) // try all possible primes for the last prime factor
            {
                //cout << "iP: " << iP << "- ";
                if (Mtemp * primes->at(iP) > Mmax)
                { // over amx bound
                    break;
                }
                else if (Mtemp * primes->at(iP) > Mmin)
                { // bootm bound is ok
                    cout << Mtemp * primes->at(iP) << ", ";
                    numLCGs += 1;
                }
                iP++;
            }
            if (iP == indices[ii]) // no prime factors to try, finish
                cont = false;
            //cout << " moving up ";
            indices[ii]++;
            ii--; // move to the previous prime factor
            cout << endl;
        }
        else if (indices[ii + 1] - indices[ii] > 1)
        {
            // cout << "Moving up " << Mtemp << "->";
            Mtemp /= primes->at(indices[ii]); // move to the previous prime factor
            cout << Mtemp << endl;
            indices[ii]++;
            if (ii > 0)
                ii--;
        }
        else
        {
            cout << "Moving down " << Mtemp << "->";
            Mtemp *= primes->at(indices[ii]); // mvoe to the following prime factor
            cout << Mtemp << endl;
            ii++;
        }
    }
    cout << endl
         << numLCGs << endl;
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
