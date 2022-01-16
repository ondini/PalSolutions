#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <vector>
#include <set>
#include <limits>
#include <queue>
#include <algorithm>
#include <tuple>

int DEBUG = 0;

typedef std::vector<std::vector<int>> matrix;

void print_vector(const std::vector<int> &v)
{
  for (long i = 0; i < v.size(); i++)
  {
    std::cout << v[i] << " ";
  }

  std::cout << std::endl;
}

void print_ld(const matrix &ld, const std::string &T, const std::string &P)
{
  std::cout << "  - ";
  for (int i = 0; i < T.size(); i++)
  {
    std::cout << "" << T[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "- ";
  print_vector(ld[0]);

  for (int i = 1; i < P.size() + 1; ++i)
  {
    std::cout << P[i - 1] << " ";
    print_vector(ld[i]);
  }
}

matrix compute_ld(const std::string &T, const std::string &P, bool allow_shift)
{

  // Init matrix with Levenshtein
  matrix ld(P.size() + 1, std::vector<int>(T.size() + 1));

  for (int i = 0; i < P.size() + 1; i++)
  {
    ld[i][0] = i;
  }

  if (!allow_shift)
  {
    for (int i = 0; i < T.size() + 1; i++)
    {
      ld[0][i] = i;
    }
  }

  for (int row = 1; row < (P.size() + 1); row++)
  {
    for (int col = 1; col < (T.size() + 1); col++)
    {
      // Compute minimum
      ld[row][col] = std::min(
          {ld[row - 1][col] + 1,
           ld[row][col - 1] + 1,
           ld[row - 1][col - 1] + ((P[row - 1] == T[col - 1]) ? 0 : 1)});
    }
  }

  return ld;
}

void stupid_approach(std::vector<std::pair<int, int>> *subsets, int M, int H, int last, std::vector<int> &path, std::vector<std::pair<int, int>> *results)
{
  bool full = true;
  int lb;

  for (int i = 0; i < (*subsets).size(); i++)
  {
    if (last == -1)
    {
      lb = -1;
    }
    else
    {
      lb = (*subsets)[last].second;
    }

    if ((*subsets)[i].first > lb)
    {
      full = false;
      std::vector<int> c_path(path);
      c_path.push_back(i);
      stupid_approach(subsets, M + 1, (H - ((*subsets)[i].second - (*subsets)[i].first + 1)), i, c_path, results);
    }
  }

  if (full)
  {
    (*results).push_back(std::make_pair(M, H));

    if ((H == 81) && (M == 6))
    {
      print_vector(path);
    }
  }
}

int main()
{

  // Load text
  std::string T;
  std::cin >> T;

  std::string Tr(T);
  std::reverse(Tr.begin(), Tr.end());

  // Load pattern
  std::string P;
  std::cin >> P;

  std::string Pr(P);
  std::reverse(Pr.begin(), Pr.end());

  // Load similarity threshold
  int D;
  std::cin >> D;

  // Compute LD using dynamic programming
  matrix ld = compute_ld(T, P, true);

  std::vector<std::pair<int, int>> subsets;

  // Go through the ld table left to right
  for (int col = 0; col < T.size(); col++)
  {
    if (ld[P.size()][col + 1] <= D)
    {
      // std::cout << "p: " << col << " l: " << T[col] << " d: " << ld[P.size()][col+1] << std::endl;

      matrix ld_substring = compute_ld(Tr.substr(T.size() - col - 1, P.size() + D), Pr, false);
      // print_ld(ld_substring, Tr.substr(T.size()-col-1, P.size() + D), Pr);

      // Check which substrings to add to higlights
      // for (int i = 0; i < std::min(int (P.size()) + D, col+1); i++) {
      for (int i = std::min(int(P.size()) + D - 1, col); i >= P.size() - D - 1; i--)
      {
        if (ld_substring[P.size()][i + 1] <= D)
        {
          subsets.push_back(std::make_pair(col - i, col));
          // std::cout << "Added pair [" << col-i << ", " << col << "]" << std::endl;
        }
      }
    }
  }

  // std::cout << "Subsets generated." << std::endl;
  std::cout << subsets.size() << std::endl;

  // Find longest path in DAG - subsets is vector of topologicaly sorted vertices.
  std::vector<int> dist(subsets.size(), 0);
  std::vector<int> h_dist(subsets.size(), 0); // Highlighted up to i-th node (including it)
  std::vector<int> parent(subsets.size(), -1);
  int max_dist = 0;
  int max_h_dist = 0;
  int max_i = -1;
  int first, second, d, hd;

  int min_following_start, min_following_end;

  for (int i = 0; i < subsets.size(); i++)
  {

    d = dist[i];
    first = subsets[i].first;
    second = subsets[i].second;
    if (h_dist[i] == 0)
    {
      h_dist[i] = second - first + 1;
    }
    hd = h_dist[i];

    if ((d > max_dist) || ((d == max_dist) && (hd > max_h_dist)))
    {
      max_dist = d;
      max_i = i;
      max_h_dist = hd;
    }

    min_following_start = T.size() + 1;
    min_following_end = T.size() + 1;

    for (int j = i + 1; j < subsets.size(); j++)
    {

      if (subsets[j].first > min_following_end)
      {
        break;
      }

      if (subsets[j].first > second)
      {

        if (subsets[j].first < min_following_start)
        {
          min_following_start = subsets[j].first;
          min_following_end = subsets[j].second;
        }

        if (d + 1 > dist[j])
        {
          dist[j] = d + 1;
          parent[j] = i;
          h_dist[j] = hd + subsets[j].second - subsets[j].first + 1;
        }
        else if ((parent[j] != -1) && (dist[j] == d + 1) && (hd > h_dist[parent[j]]))
        {
          dist[j] = d + 1;
          parent[j] = i;
          h_dist[j] = hd + subsets[j].second - subsets[j].first + 1;
        }
      }
    }
  }

  // // Print subsets
  // for (int i = 0; i < subsets.size(); i++) {
  //   std::cout << "i: " << i << " subset: [" << subsets[i].first << ", " << subsets[i].second
  //     << "]" << " d: " << dist[i] << " p: " << parent[i] << " hd: " << h_dist[i];
  //   std::cout << std::endl;
  // }

  // std::cout << max_dist << " " << max_i << std::endl;

  // Compute the output
  int M = 0;
  int H = T.size();
  int pos = max_i;
  while (true)
  {
    std::cout << subsets[pos].first << ":" << subsets[pos].second << std::endl;
    M++;
    H -= subsets[pos].second - subsets[pos].first + 1;
    pos = parent[pos];
    if (pos == -1)
    {
      break;
    }
  }

  // std::cout << "==========" << std::endl;
  std::cout << M << " " << H << std::endl;
  // std::cout << T.size() << std::endl;

  // std::vector<std::pair<int, int>> results;
  // std::vector<int> path;
  // stupid_approach(&subsets, 0, T.size(), -1, path, &results);
  // for (auto p : results) {
  //   // if (p.first == 6) {
  //   //   std::cout << p.first << " " << p.second << std::endl;
  //   // }

  //   // if (p.second == 81) {
  //   //   std::cout << p.first << " " << p.second << std::endl;
  //   // }
  // }

  return 0;
}
