#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

#define SCALE 5000


using namespace std;



int main(int argc, char const *argv[])
{
    // if(argc < 2)
    // {
    //     cerr << "ERR:Not enough input arguments!" << endl;
    //     return 1;
    // }

    // string inputFileName = argv[1];
    
    // //macout << "Reading from " << inputFileName << endl;

    // ifstream inputFile(inputFileName);

    int numPoles;
    cin >> numPoles;
    
    //cout << numPoles << endl;

    int poles[numPoles][2];
    double length = 0;
    
    int x, y;

    cin >> x >> y;

    poles[0][0] = x;
    poles[0][1] = y;

    for(int i = 1; i < numPoles; i++)
    {   

        cin >> x >> y;

        poles[i][0] = x;
        poles[i][1] = y;

        double distance = sqrt(pow((poles[i-1][0] - x),2) + pow((poles[i-1][1] - y),2));
        length += distance;
    }

    length += sqrt(pow((poles[0][0] - x),2) + pow((poles[0][1] - y),2));

    length *= SCALE/1000;

    int result = (int)(length+0.999);

    cout << result << endl; //<< " - " << length << endl;

    
    return 0;
}


    // std::cout << "____________" << endl;
    // for (int i = 0; i < numPoles; ++i)
    // {
    //     for (int j = 0; j < 2; ++j)
    //     {
    //         std::cout << poles[i][j] << ' ';
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << "____________" << endl;
    