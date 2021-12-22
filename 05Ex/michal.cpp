#include <vector>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <stdint.h>
//// Typedefs ////
typedef unsigned long int uli;

//// Global variables ////
// Problem definition
uli A_min, A_max, C_min, C_max, M_min, M_max;
int P;
// Decrease number of operations by precomputing A - 1
uli A_min_1, A_max_1, C_min_1;
uli C_len;
std::vector<uli> all_primes{};
uli prime_count, tmp_prime;
uli min_possible_prime, max_possible_prime;

// Since P is known, the largest number to consider can be determined by
// multiplying the first P-1 primes and then dividing the M_max by this number
// Since P <a= 9, these can be specified manually and do not have to be calculated
std::array<uli, 9> first_9_primes = {2, 3, 5, 7, 11, 13, 17, 19, 23};
uli max_considered_number_id;

// Output
uli R = 0;


// Helper functions
void load_data();
void eratosthenes_sieve();
void print_primes(const std::vector<bool> &is_prime);
void print_primes(const std::vector<uli> &primes);
void count_possible_As(const uli &M, const uli &C_count);
const uli update_possible_Cs(const std::vector<uli> &primes, const uli &new_prime, const uli &C_count, const size_t &primes_size);
void select_next_prime(std::vector<uli> &primes, uli M, const int remaining, const uli start, const uli &C_count);
void try_next_powers(uli A_1, const int start, const uli &K, const uli &C_count, const std::vector<uli> &primes);
inline const uli id_to_number(const uli &id)
{
	return 2 * id + 3;
}

inline const uli number_to_id(const uli &number)
{
	return (number - 3) / 2;
}

// Main function

int main()
{
	load_data();

	  // The maximal prime will be M_max / 2 (for P = 2 and p1 = 2, 2*p_max <= M_max)
	  // By discarding all even numbers, I can use sieve of Eratosthenes with (M_max/2 - 1)/2 elements
	uli first_prime_product = 1;
	for (int i = 0; i < P - 1; ++i) {
		first_prime_product *= first_9_primes[i];
	}
	max_considered_number_id = (M_max / first_prime_product - 1) / 2;

	eratosthenes_sieve();

	std::vector<uli> primes{};
	uli C_count;
	for (uli i = 0; i < prime_count; ++i) {
		C_count = update_possible_Cs(primes, all_primes[i], C_len, 0);
		primes.push_back( all_primes[i] );
		select_next_prime(primes, all_primes[i], P - 1, i + 1, C_count);
		primes.pop_back();
	}

	  // R is incremented inside 'select_next_prime' function
	std::cout << R << "\r\n";

	return 0;
}

void load_data()
{
	std::string line;
	int line_no = 1;

	while ( std::getline(std::cin, line) ) {
		std::stringstream line_ss{line};
		switch (line_no++) {
		case 1:
			line_ss >> A_min >> A_max;
			A_min_1 = A_min - 1;
			A_max_1 = A_max - 1;
			break;
		case 2:
			line_ss >> C_min >> C_max;
			C_len = C_max - C_min + 1;
			C_min_1 = C_min - 1;
			break;
		case 3:
			line_ss >> M_min >> M_max;
			break;
		case 4:
			line_ss >> P;
			break;
		}

	}
}

void eratosthenes_sieve()
{
	  // When computing sieve where max is M, only sqrt(M) checks is needed
	  // In this implementation, M is id_to_number(n - 1) (last id is 'n-1')
	uli max_number = id_to_number(max_considered_number_id - 1);
	uli n_sqrt = std::sqrt(max_number);

	  // Number of primes less then n is approximately given by x/log(x)
	all_primes.reserve( max_number / std::log10(max_number) );
	all_primes.push_back(2);

	  // Ensure that n_sqrt is at least 3, so that number_to_id returns 0 or more
	  // (and does not underflow)
	n_sqrt = n_sqrt >= 3 ? n_sqrt : 3;
	uli i_lim = number_to_id(n_sqrt);
	std::vector<bool> is_prime(max_considered_number_id, true);
	for (uli i = 0; i <= i_lim; ++i) {
		  // v[i] is true, therefore id_to_number(i) is a prime
		if (is_prime[i]) {
			tmp_prime = id_to_number(i);
			all_primes.push_back(tmp_prime);
			for (uli j = number_to_id(tmp_prime * tmp_prime); j < max_considered_number_id; j += tmp_prime) {
				is_prime[j] = false;
			}
		}
	}
	  // All primes will be marked, save them to all_primes
	for (uli i = i_lim + 1; i < max_considered_number_id; ++i) {
		if (is_prime[i]) {
			all_primes.push_back( id_to_number(i) );
		}
	}
	prime_count = all_primes.size();
}

void print_primes(const std::vector<bool> &is_prime)
{
	for (uli i = 0; i < is_prime.size(); ++i) {
		if (is_prime[i]) {
			std::cout << id_to_number(i) << "\n";
		}
	}
	std::cout << "\n";
}

void print_primes(const std::vector<uli> &primes)
{
	for (const uli &i : primes) {
		std::cout << i << ", ";
	}
	std::cout << "\n";
}

void count_possible_As(const uli &M, const uli &C_count)
{
	  // A-1 needs to be divisible by all primes in 'primes',
		// it will therefore be in the form
	  // A-1 = K * p1^n1 * p2^n2 * ... pP^nP,

		// Thus it is only needed to determine how many K's exists such that the
		// inequality A_min_1 <= K*M <= A_max_1 holds (i.e. how many numbers
		// divisible by M exist between A_min_1 and A_max_1)
		R += C_count * (A_max_1 / M - (A_min_1 - 1) / M);
		return;
}

const uli update_possible_Cs(const std::vector<uli> &primes, const uli &new_prime, const uli &C_count, const size_t &primes_size)
{
	  // Start with the interval length, determine the count by inclusion-exclusion
	uli divisor;
	uli updated_C_count = C_count;
	uint32_t count = 1 << primes_size;
	  // By including i = 0, the set {new_prime} is also considered
	for (uint32_t i = 0; i < count; ++i) {
		  // Generate the subset
		divisor = new_prime;
		for (size_t j = 0; j < primes_size; ++j) {
			if ( i & (1 << j) ) {
				divisor *= primes[j];
			}
		}
		  // Thanks to integer division, C_max/divisor - (C_min-1)/divisor
		  // will yield number of numbers divisible by divisor in the range [C_min, C_max]
		  // Can't be reduced to (C_max - C_min + 1)/divisor! This would lead to
		  // errors
		  // EX:
		  // divisor = 3, C_min = 1, C_max = 5 => {3}
		  // divisor = 3, C_min = 3, C_max = 8 => {3, 6}
		  // ... even though the interval length is the same, the number of divisible
		  // numbers is different
		if (__builtin_popcount(i) % 2) {
			updated_C_count += C_max / divisor - C_min_1 / divisor;
		} else {
			updated_C_count -= C_max / divisor - C_min_1 / divisor;
		}
	}
	return updated_C_count;
}


void select_next_prime(std::vector<uli> &primes, uli M, const int remaining, const uli start, const uli &C_count)
{
	  // We need precisely P primes, therefore the search can be stopped when
	  // not enough primes remain
	const uli stop = prime_count - remaining + 1;
	const size_t primes_size = P - remaining;
	for (uli i = start; i < stop; ++i) {
		const uli &prime = all_primes[i];

		  // Check whether the product is over M_max, if yes, stop
		if ( (tmp_prime = M * prime) > M_max ) {
			break;
		}

		  // Since the primes have to be distinct, we can calculate the next smallest M
		  // and stop the search earlier
		  // Similar thing can be done for the largest number and M_max
		min_possible_prime = tmp_prime;
		max_possible_prime = tmp_prime;
		for (int j = 1; j < remaining; ++j) {
			if ( (min_possible_prime *= all_primes[i + j]) > M_max ) {
				return;
			}
			max_possible_prime *= all_primes[prime_count - j];
		}

		if (max_possible_prime < M_min) {
			continue;
		}

		  // Update possible C count by generating all subsets including the last
		  // added prime and computing possible Cs
		uli updated_C_count = update_possible_Cs(primes, prime, C_count, primes_size);

		  // There is no reason to continue when no C exists
		if (updated_C_count == 0) {
			continue;
		}
		  // If primes.size() == P, report ids, otherwise select another
		primes.push_back(prime);
		if (remaining == 1) {
			if (tmp_prime >= M_min) {
				count_possible_As(tmp_prime, updated_C_count);
			}
		} else {
			select_next_prime(primes, tmp_prime, remaining - 1, i + 1, updated_C_count);
		}
		primes.pop_back();

	}
}
