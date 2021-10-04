#include <iostream>
#include <string>
#include <cmath>

#include <chrono>

using namespace std;

// Two possible approaches?:
//      > version of Kruskal's algorithm
//      > dfs from each hub


 
void heapify(int* arr, int iBound, int iParent)
{
    
    int iLargest = iParent;
    int iChildL = 2*iParent + 1; 
    int iChildR = 2*iParent + 2;

    
    if (iChildL < iBound && arr[iChildL*3+2] > arr[iLargest*3+2])
        iLargest = iChildL;

    if (iChildR < iBound && arr[iChildR*3+2] > arr[iLargest*3+2])
        iLargest = iChildR;
 
    if (iLargest != iParent) {
        swap(arr[iLargest*3], arr[iParent*3]);
        swap(arr[iLargest*3+1], arr[iParent*3+1]);
        swap(arr[iLargest*3+2], arr[iParent*3+2]);

        heapify(arr, iBound, iLargest);
    }
}

int main(int argc, char const *argv[])
{

    // read metadata
    int numFarms, numRoads;
    cin >> numFarms >> numRoads;
    
    cout << numFarms << " " << numRoads << endl;

    //variables for graph storing
    int* roads = new int [numRoads*3];
    int* farms = new int [numFarms*2];

    // insert first road to array
    int farmA, farmB, price;
    cin >> farmA >> farmB >> price;

    roads[0*3 + 0] = farmA;
    roads[0*3 + 1] = farmB;
    roads[0*3 + 2] = price;
    
    // read values from input, add to array and directly heapify that array
    for(int i = 1; i < numRoads; ++i)
    {
        cin >> farmA >> farmB >> price;

        roads[i*3 + 0] = farmA;
        roads[i*3 + 1] = farmB;
        roads[i*3 + 2] = price;

        int iChild = i;
        int iParent = (int)((double) i/2 + 0.5) - 1;

        //heapify = while parent has greater price, swap

        while (iChild != 0 && roads[iParent*3 + 2] < roads[iChild*3 + 2]) {
            swap(roads[iChild*3], roads[iParent*3]);
            swap(roads[iChild*3+1], roads[iParent*3+1]);
            swap(roads[iChild*3+2], roads[iParent*3+2]);

            iChild = iParent;
            iParent =  (int)((double) iParent/2 + 0.5) - 1;

        }
        
    }

    // heapsort the heapified array
    for (int i = numRoads - 1; i > 0; i--) {

        // Move current max to the end
        swap(roads[0], roads[i*3]);
        swap(roads[1], roads[i*3+1]);
        swap(roads[2], roads[i*3+2]);
 
        // heapify the rest
        heapify(roads, i, 0);
    }

    // now are the roads sorted by price

    // read metadata
    int numHubs;
    cin >> numHubs;

    // read and mark hubs
    int iHub;
    for(int i = 0; i < numHubs; i++)
    {
        cin >> iHub;
        farms[iHub*2] = -1;
        farms[iHub*2 + 1] = -1;
    }

    // Perform dfs from each hub
    for(int i = 0; i < numHubs; i++)
    {   
        farms = 
    }
    
    
    
    //      Matrix printer
    int matRows = numRoads;
    int matCols = 3;
    
    cout << "____________" << endl;
    for (int i = 0; i < matRows; ++i)
    {
        cout << "| ";
        for (int j = 0; j < matCols; ++j)
        {
            cout << roads[i*matCols + j] << ' ';
        }
        cout << "| " << endl;
    }
    cout << "____________" << endl;





    //      Matrix printer
    matRows = numFarms;
    matCols = 2;
    
    cout << "____________" << endl;
    for (int i = 0; i < matRows; ++i)
    {
        cout << "| ";
        for (int j = 0; j < matCols; ++j)
        {
            cout << farms[i*matCols + j] << ' ';
        }
        cout << "| " << endl;
    }
    cout << "____________" << endl;




    return 0;
}
