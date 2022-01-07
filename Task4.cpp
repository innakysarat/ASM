#include <iostream>
#include <omp.h>
#include <chrono>
#include <vector>
#include <algorithm>


using std::string;
using std::vector;

int input() {
	int n;
	std::cin >> n;
	bool test = true;
	do {
		if (!(test = std::cin.good())) {
			std::cout << "Incorrect input. Try again." << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	} while (!test);
	return n;
}

void threadFunction(vector<int>& team, int l, int r) {
	for (int i = l; i < r - 1; i += 2) {
		(team[i] < team[i + 1]) ? team[i] = 0 : team[i + 1] = 0;
	}
}


int main() {
	int n;
	int max_energy;
	do {
		std::cout << "Input a number of monks <= 40:" << std::endl;
		n = input();
	} while (n <= 1 || n > 40);
	do {
		std::cout << "Input max energy value: " << std::endl;
		max_energy = input();
	} while (max_energy <= 0);

	vector<int> team;
	for (size_t i = 0; i < n; ++i) {
		team.push_back(rand() % max_energy + 1);
		std::cout << "Monk " << i << " with energy " << team[i] << std::endl;
	}

	while (n / 2 + n % 2 >= 2) {

#pragma omp parallel
		{
#pragma omp sections
			{
#pragma omp section
				{
					threadFunction(team, 0, n / 2);
				}
#pragma omp section
				{
					threadFunction(team, n / 2, n);
				}

			}
		}

		int a = 0;
		team.erase(std::remove(team.begin(), team.end(), a), team.end());
		n = team.size();

	}
	(team[0] > team[1]) ? std::cout << "\nMonk with energy " << team[0] << " won"
		<< std::endl : std::cout << "\nMonk with energy " << team[1] << " won" << std::endl;

	return 0;
}


