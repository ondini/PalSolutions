#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdint.h>

//// Type definitions /////
typedef std::vector<std::vector<bool>> BoolMat;
typedef std::vector<uint32_t> UInt32Vec;
typedef std::map<uint32_t, std::set<int>> PackMap;
typedef std::map<std::set<int>, uint32_t> PackMapReversed;
///////////////////////////

//// Utility functions ////
void print(const BoolMat &mat);
void print(const std::vector<uint32_t> &v);
void print(const std::vector<int> &v);
void print(const std::set<uint32_t> &v);
void print(const PackMap &map);
void print(const PackMapReversed &map);
///////////////////////////

//// Main functions ///////
void load_data(BoolMat &connectivity_matrix, UInt32Vec &connectivity_vector, int &num_smugglers, int &pack_size, int &pack_connections)
{
	int line_no = 0;
	int M;
	int a, b;
	std::string line;

	while (std::getline(std::cin, line))
	{
		std::stringstream stream{line};
		if (line_no == 0)
		{
			stream >> num_smugglers >> M >> pack_size >> pack_connections;
			connectivity_matrix.reserve(num_smugglers);
			connectivity_vector.reserve(num_smugglers);
			for (int id = 0; id < num_smugglers; ++id)
			{
				connectivity_matrix.push_back(std::vector<bool>(num_smugglers, false));
				connectivity_vector.push_back(0);

				// Do not set if a == b to decrease operation number later on
				// connectivity_matrix[id][id] = true;
			}
		}
		else if (line_no <= M)
		{
			stream >> a >> b;
			if (a == b)
			{
				// Shouldn't happen, but who knows?
				++line_no;
				continue;
			}
			connectivity_matrix[a][b] = true;
			connectivity_matrix[b][a] = true;

			// This will condensate each row of the connectivity_matrix to
			// one uint32_t - this can be done since num_smugglers <= 30
			connectivity_vector[a] |= (1 << b);
			connectivity_vector[b] |= (1 << a);
		}
		++line_no;
	}
}

void find_pack_map(PackMap &pack_map, const BoolMat &connectivity_matrix, const std::vector<bool> &v, std::vector<int> current_pack, const int start, const int stop, int remaining)
{
	for (int i = start; i < stop; ++i)
	{
		// Already in pack
		if (std::find(current_pack.begin(), current_pack.end(), i) != current_pack.end())
		{
			continue;
		}
		// Check if 'i' is a neighbor of anyone in the pack
		for (const int member : current_pack)
		{
			if (connectivity_matrix[member][i])
			{
				current_pack.push_back(i);
				if (remaining)
				{
					find_pack_map(pack_map, connectivity_matrix, connectivity_matrix[i], current_pack, start, stop, remaining - 1);
				}
				else
				{
					uint32_t pack_rank = 0;
					// std::cout << "Pack: ";
					for (const auto &s : current_pack)
					{
						pack_rank |= (1 << s);
						// std::cout << s << " ";
					}
					// std::cout << "\tRank: " << pack_rank << "\n";
					pack_map[pack_rank] = std::set<int>(current_pack.begin(), current_pack.end());
				}
				current_pack.pop_back();
				break;
			}
		}
	}
}

bool are_isomorphic(const BoolMat &connectivity_matrix, const std::set<int> &pack_a, const std::set<int> &pack_b)
{
	// Brute force isomorphism check
	const int n = pack_a.size();
	std::vector<int> bijection{};
	for (int i = 0; i < n; ++i)
	{
		bijection.push_back(i);
	}

	std::vector<int> pack_a_vec(pack_a.begin(), pack_a.end());
	std::vector<int> pack_b_vec(pack_b.begin(), pack_b.end());

	int pack_a_1, pack_a_2, pack_b_1, pack_b_2;
	bool is_bijection;
	// For each possible permutation, test whether for each edge between
	// nodes exists an edge in none or both packs
	do
	{
		is_bijection = true;
		for (int i = 0; i < n && is_bijection; ++i)
		{
			for (int j = 0; j < n && is_bijection; ++j)
			{

				pack_a_1 = pack_a_vec[i];
				pack_a_2 = pack_a_vec[j];
				pack_b_1 = pack_b_vec[bijection[i]];
				pack_b_2 = pack_b_vec[bijection[j]];

				// When there exists a connection in the first graph, which does not
				// exist in the second one, they can't be isomorphic under
				// given bijection
				if (connectivity_matrix[pack_a_1][pack_a_2] != connectivity_matrix[pack_b_1][pack_b_2])
				{
					is_bijection = false;
				}
			}
		}
		if (is_bijection)
		{
			return true;
		}
	} while (std::next_permutation(bijection.begin(), bijection.end()));

	// If no permutation was correct, the graphs are not isomorphic
	return false;
}

int main()
{
	// Load data
	BoolMat connectivity_matrix;
	UInt32Vec connectivity_vector;
	int num_smugglers, pack_size, pack_connections;
	load_data(connectivity_matrix, connectivity_vector, num_smugglers, pack_size, pack_connections);

	// std::cout << "Problem definition:\n";
	// std::cout << "num_smugglers:\t\t" << num_smugglers << "\n" << "pack_size:\t\t" << pack_size << "\npack_connections:\t" << pack_connections << "\n\n";
	// std::cout << "connectivity_matrix:\n";
	// print(connectivity_matrix);

	// Create all possible packs and calculate their rank
	// std::cout << "\nSearching for pack map:\n";
	PackMap pack_map;
	for (int i = 0; i < num_smugglers; ++i)
	{
		find_pack_map(pack_map, connectivity_matrix, connectivity_matrix[i], {i}, i, num_smugglers, pack_size - 2);
	}

	// std::cout << "\nPack map:\n";
	// print(pack_map);

	// Filter out packs with wrong number of edges - use rank as a mask to cound edges from the BoolMat
	// Also reverse the order of key - value in the map. Using pack (set of smuggler numbers) will ensure lexographic ordering of results
	// (since iterator over map keys is required to iterate in ascending order in gcc)
	PackMapReversed pack_map_filtered;
	// Since the graph is not oriented, summing the node degrees (in a pack)
	// will give twice the connections
	int pack_connections_2 = 2 * pack_connections;
	for (const auto &item : pack_map)
	{
		int num_edges = 0;
		for (const auto &i : item.second)
		{
			// __builtin_popcount counts number of 1s in the binary representations of the argument
			int deg = __builtin_popcount(connectivity_vector[i] & item.first);
			num_edges += deg;
			if (num_edges > pack_connections_2)
			{
				break;
			}
		}
		if (num_edges == pack_connections_2)
		{
			pack_map_filtered.insert({item.second, item.first});
		}
	}
	// std::cout << "\nFiltered packs (with corrent connection count):\n";
	// print(pack_map_filtered)

	// Report packs that are not connected and are isomorphic to each other
	// std::map is sorted in ascending key order according to gcc specification,
	// therefore no manual sorting is needed
	for (auto pack_a = pack_map_filtered.begin(); pack_a != pack_map_filtered.end(); ++pack_a)
	{
		for (auto pack_b = std::next(pack_a); pack_b != pack_map_filtered.end(); ++pack_b)
		{
			if (((pack_a->second & pack_b->second) == 0) && are_isomorphic(connectivity_matrix, pack_a->first, pack_b->first))
			{
				std::string s = "";
				for (const int i : pack_a->first)
				{
					s += std::to_string(i) + " ";
				}
				for (const int i : pack_b->first)
				{
					s += std::to_string(i) + " ";
				}
				s.pop_back();
				std::cout << s << "\n";
			}
		}
	}
	return 0;
}

void print(const BoolMat &mat)
{
	int r = 0;
	for (int i = 0; i < int(1.2 * mat[0].size() + 3); ++i)
	{
		std::cout << "--";
	}
	std::cout << "\n";
	std::cout << "  ";
	for (int i = 0; i < int(mat[0].size()); ++i)
	{
		if (i % 5 == 0)
		{
			std::cout << "| ";
		}
		std::cout << i << " ";
	}
	std::cout << "|\n";

	for (int i = 0; i < int(1.2 * mat[0].size() + 3); ++i)
	{
		std::cout << "--";
	}
	std::cout << "\n";
	for (const auto &row : mat)
	{
		int c = 0;
		std::cout << r << " ";
		for (const auto &col : row)
		{
			if (c++ % 5 == 0)
			{
				std::cout << "| ";
			}
			std::cout << col << " ";
		}
		std::cout << "|\n";
		if (++r % 5 == 0)
		{
			for (int i = 0; i < int(1.2 * mat[0].size() + 3); ++i)
			{
				std::cout << "--";
			}
			std::cout << "\n";
		}
	}
	for (int i = 0; i < int(1.2 * mat[0].size() + 3); ++i)
	{
		std::cout << "--";
	}
	std::cout << "\n";
}

void print(const std::vector<uint32_t> &v)
{
	for (const auto &e : v)
	{
		std::cout << e << "\n";
	}
}

void print(const std::vector<int> &v)
{
	for (const auto &e : v)
	{
		std::cout << e << " ";
	}
}

void print(const std::set<uint32_t> &v)
{
	for (const auto &e : v)
	{
		std::cout << e << "\n";
	}
}

void print(const PackMap &map)
{
	for (const auto &item : map)
	{
		std::cout << item.first << ":\t";
		for (const auto &i : item.second)
		{
			std::cout << i << " ";
		}
		std::cout << "\n";
	}
}

void print(const PackMapReversed &map)
{
	for (const auto &item : map)
	{
		std::cout << item.second << ":\t";
		for (const auto &i : item.first)
		{
			std::cout << i << " ";
		}
		std::cout << "\n";
	}
}
