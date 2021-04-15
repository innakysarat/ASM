#include <iostream>
#include <thread>
#include <vector>
#include <ctime>
#include <random>

void threadFunction(std::vector<int> team, int &winner) {
    int energy1 = 0;
    int energy2 = 0;
    int sizeTeam = team.size();
    if (sizeTeam > 2) {
        size_t size = sizeTeam / 2 + sizeTeam % 2;
        std::vector<int> team_first(size);
        std::vector<int> team_second(size);
        for (size_t i = 0; i < size; ++i) {
            team_first[i] = team[i];
            team_second[i] = team[sizeTeam - 1 - i];
        }
        if (sizeTeam % 2 == 1) team_second[size - 1] = 0;
        std::thread thr1(threadFunction, team_first, std::ref(energy1));
        std::thread thr2(threadFunction, team_second, std::ref(energy2));
        thr1.join();
        thr2.join();
        (energy1 > energy2) ? winner = energy1 : winner = energy2;
    } else {
        (team[0] > team[1]) ? winner = team[0] : winner = team[1];
    }
}

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

int main() {
    int n;
    int max_energy;
    std::vector<int> winners;
    do {
        std::cout << "Input a number of monks <= 100:" << std::endl;
        n = input();
    } while (n <= 0 || n > 100);
    do {
        std::cout << "Input max energy value: " << std::endl;
        max_energy = input();
    } while (max_energy <= 0);
    std::vector<int> team(n);
    std::mt19937 gen;
    gen.seed(time(0));
    for (size_t i = 0; i < n; ++i) {
        team[i] = gen() % max_energy + 1;
    }
    int result = 0;
    threadFunction(team, result);
    for (size_t i = 0; i < n; ++i) {
        if (i < (n / 2 + n % 2)) {
            std::cout << "Guan-yin. Monk " << i + 1 << ".Qi energy " << team[i] << "\n";
        } else {
            std::cout << "Guan-yang. Monk " << i + 1 << ".Qi energy " << team[i] << "\n";
        }
        if (team[i] == result)
            winners.push_back(i + 1);
    }
    if (winners.size() > 1) {
        std::cout << "\nThe battle ended in a draw" << "\n";
        for (size_t i = 0; i < winners.size(); ++i) {
            std::cout << "Monk " << winners[i] << " won with qi energy " << result << "\n";
        }
    } else {
        std::cout << "\n" << "Monk " << winners[0] << " won with qi energy " << result << "\n";
        (winners[0] > (n / 2 + n % 2)) ? std::cout << "Guan-yang got bodhisattva statue." : std::cout
                << "Guan-yin got bodhisattva statue.";
    }
    return 0;
}
