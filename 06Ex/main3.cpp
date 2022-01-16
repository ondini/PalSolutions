#include <iostream>
#include <math.h>

#include <bits/stdc++.h>

using namespace std;

class CompareNodes
{
public:
    bool operator()(pair<int, int> a, pair<int, int> b)
    {
        return (a.first == b.first) ? (a.second > b.second) : (a.first > b.first);
    }
};

typedef priority_queue<pair<int, int>, vector<pair<int, int>>, CompareNodes>
    p_queue_pair;

void printGraph(vector<map<char, int>> adjList);
void printVec(vector<int> vec);
void printVec(vector<long> vec);
void printVecPairs(vector<pair<int, int>> vec);
void printMatrix(vector<int> *matrix, int matRows, int matCols);
void printQueue(p_queue_pair gq);

void distLevenhtein(vector<int> *d, string T, string P, int Tl, int Pl, bool shift = true)
{

    int Pl1 = Pl + 1; // length of string P + 1, because of empty string in the table
    int Tl1 = Tl + 1;
    // fill first row where values depend on shft parameter
    if (!shift)
    {
        for (int k = 0; k < Tl1; k++)
        {
            d->at(k) = k;
        }
    }

    // fill first column
    for (int i = 0; i < Pl1; i++)
    {
        d->at(i * Tl1) = i;
    }

    // fill the rest of table
    for (int i = 1; i < Pl1; i++)
    {
        for (int k = 1; k < Tl1; k++)
        {
            d->at(Tl1 * i + k) = min({d->at(Tl1 * i + k - 1) + 1, d->at(Tl1 * (i - 1) + k) + 1, d->at(Tl1 * (i - 1) + k - 1) + ((P[i - 1] == T[k - 1]) ? 0 : 1)}); //min({, d->at(Tl1 * (i - 1) + k - 1) + ((P[i] == T[k]) ? 1 : 0)});
        }
    }
}

void distLevenhtein2(vector<int> *d, string T, string P, int Tl, int Pl, bool shift = true)
{

    int Pl1 = Pl + 1; // length of string P + 1, because of empty string in the table
    int Tl1 = Tl + 1;
    // fill first row where values depend on shft parameter
    if (!shift)
    {
        for (int k = 0; k < Tl1; k++)
        {
            d->at(k) = k;
        }
    }

    // fill first column
    for (int i = 0; i < Pl1; i++)
    {
        d->at(i * Tl1) = i;
    }

    // fill the rest of table
    for (int i = 1; i < Pl1; i++)
    {
        for (int k = 1; k < Tl1; k++)
        {
            d->at(Tl1 * i + k) = min({d->at(Tl1 * i + k - 1) + 1, d->at(Tl1 * (i - 1) + k) + 1, d->at(Tl1 * (i - 1) + k - 1) + ((P[i - 1] == T[k - 1]) ? 0 : 1)}); //min({, d->at(Tl1 * (i - 1) + k - 1) + ((P[i] == T[k]) ? 1 : 0)});
        }
    }
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, MAIN ,>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

int main(int argc, char const *argv[])
{
    // read metadata - num. of automaton states, Alphabet size, num. final states, number of negative and positive examples and their length
    string T;
    string P;
    int D;

    getline(cin, T);
    getline(cin, P);
    cin >> D;

    int Pl1 = P.length(); // length of string, one larger than number of letters
    int Tl1 = T.length();
    //cout << Pl1 << " " << Tl1 << endl;

    int Pl = Pl1 - 1;
    int Tl = Tl1 - 1;
    vector<int> levensDist(Pl1 * Tl1, 0); // matrix for levnsthein distance one larger in each dimension because of empty strings
    distLevenhtein(&levensDist, T, P, Tl, Pl);
    //printMatrix(&levensDist, Pl1, Tl1);

    vector<int> reversedDis((Pl1 + D) * Pl1); //  distance of reversed patterns used for finding all possible edits of satisfactory from previous table
    string Pr = P.substr(0, Pl);              // reversed P
    reverse(Pr.begin(), Pr.end());
    string Tr;
    int size, size1; // size of T substring and size + 1

    p_queue_pair substrings; // vector far all substring with satisfactory levensthein distance

    for (int i = 0; i < Tl1; i++) // go through last row of the original table
    {
        if (levensDist[(Pl)*Tl1 + i] <= D) // if the result satisfies distance condition, find all its edits and add them to vector
        {
            size = (i >= (D + (Pl))) ? (D + (Pl)) : i;
            size1 = size + 1;
            Tr = T.substr(i - size, size);
            reverse(Tr.begin(), Tr.end());

            //cout << "i: " << i << "; " << size << " : " << Tr << "  " << Pl << ":" << Pr << endl;

            distLevenhtein(&reversedDis, Tr, Pr, size, Pl, false); // compute distance of reversed strings. This secures alignment from the right side and finds all lengthened/shortened versions satisfing D condition
            //printMatrix(&reversedDis, Pl1, size + 1);
            for (int j = 0; j < size1; j++) // go throug the last row of the second table (size + 1, bcs of empty string)
            {
                //cout << reversedDis[(Pl)*size1 + j] << " ";
                if (reversedDis[(Pl)*size1 + j] <= D)
                {
                    //cout << "Found at " << j << "-> " << i - j << " " << i - 1 << endl;
                    substrings.push(make_pair(i - j, i - 1));
                }
            }
            //cout << endl;
        }
    }
    cout << "Intial  queue:";
    printQueue(substrings);

    vector<int> coverage_so_far(Tl * 2, 0); // create array |T| * 2, where each row [|S|, c]_i represents |S| number of sets til ith character and uncvered letters
    vector<int> covered_end(Tl, -1);        // end of any interval occured here
    cout << "Initial matrix:\n";
    printMatrix(&coverage_so_far, Tl, 2);
    printVec(covered_end);
    int start, end, Ssfn, csfn, Ssf, csf, lastEnd;
    int maxS = 0, maxc = 0, prevStart = 0, iMax = 0;

    while (!substrings.empty())
    {
        pair<int, int> interval = substrings.top();
        start = interval.first;
        end = interval.second;
        substrings.pop();

        printf("\n->Prevstart: %d, iMax: %d.\n", prevStart, iMax);
        printf("Popping: (%d,%d)\n", start, end);

        for (int i = prevStart; (i < start); i++)
        {
            printf("  Testing iMax on %d = (%d,%d, %d)\n", i, coverage_so_far[i * 2], coverage_so_far[i * 2 + 1], covered_end[i]);
            Ssfn = coverage_so_far[i * 2];
            csfn = coverage_so_far[i * 2 + 1];
            Ssf = coverage_so_far[iMax * 2];
            csf = coverage_so_far[iMax * 2 + 1];
            if (Ssf < Ssfn || (Ssf == Ssfn && csfn < csf))
            {
                printf("  Found new iMax better than %d : (%d, %d)\n", iMax, Ssf, csf);
                iMax = i;
            }
        }

        printf("Testing  Coverage on iMax %d = (%d,%d, %d)\n", iMax, coverage_so_far[iMax * 2], coverage_so_far[iMax * 2 + 1], covered_end[iMax]);
        // properties of poppend interval
        Ssfn = coverage_so_far[iMax * 2] + 1;
        csfn = coverage_so_far[iMax * 2 + 1] + start - covered_end[iMax] - 1;

        // properties of currently best set of intervals on the ending index
        Ssf = coverage_so_far[end * 2];
        csf = coverage_so_far[end * 2 + 1];
        if (Ssf < Ssfn || (Ssf == Ssfn && csfn < csf))
        {
            cout << "Found better solution!\n";
            coverage_so_far[end * 2] = Ssfn;
            coverage_so_far[end * 2 + 1] = csfn;
            covered_end[end] = end;
            if (maxS < Ssfn || (maxS == Ssfn && csfn <= maxc))
            {
                maxS = Ssfn;
                maxc = csfn;
                lastEnd = end;
            }
            cout << "Coverage after this step:\n";
            printMatrix(&coverage_so_far, Tl, 2);
            printVec(covered_end);
        }

        prevStart = start;
    }
    maxc += Tl - lastEnd - 1;
    printf("%d %d\n", maxS, maxc);

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
    cout << "______";
    for (int i = 0; i < matCols; i++)
    {
        cout << "____";
    }

    cout << "__" << endl;
    for (int i = 0; i < matRows; ++i)
    {
        printf("%3d: |", i);
        for (int j = 0; j < matCols; ++j)
        {
            printf("%4d", matrix->at(i * matCols + j));
        }
        cout << " | " << endl;
    }

    cout << "______";
    for (int i = 0; i < matCols; i++)
    {
        cout << "____";
    }
    cout << "__" << endl;
}

void printQueue(p_queue_pair gq)
{
    p_queue_pair g = gq;
    while (!g.empty())
    {
        cout << '\t' << g.top().first << " : " << g.top().second;
        g.pop();
    }
    cout << '\n';
}