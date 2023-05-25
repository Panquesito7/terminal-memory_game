/**      
 * A simple memory game with 3 different sizes and multiple letters.
 * Copyright (C) 2023 David Leal (halfpacho@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>    /// for IO operations
#include <ctime>       /// for std::time()
#include <cstdlib>     /// for std::srand()
#include <vector>      /// for std::vector
#include <algorithm>   /// for std::shuffle()

#ifdef _WIN32
#include <windows.h>  /// for sleep
#else
#include <unistd.h>  /// for sleep
#endif

// `std::random_shuffle` was deprecated in C++14. To keep support with most compilers, we need to check the C++ version.
#if __cplusplus >= 201402L
    #define SHUFFLE(a, b) std::shuffle(a, b, std::mt19937(std::random_device()()))
#else
    #define SHUFFLE(a, b) std::random_shuffle(a, b)
#endif

/**
 * @namespace
 * @brief Functions for the Memory Game implementation
*/
namespace memory_game {
/**
 * @brief Utility function to verify if the given input is a number or not.
 * This is very useful to prevent the program being stuck in a loop.
 * @tparam T The type of the input
 * @param input The input to check.
 * @returns false if the input IS empty or if it contains a non-digit character
 * @returns true if the input is NOT empty and if it contains only digit characters
*/
template <typename T>
bool is_number(const T& input) {
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(256, '\n');
        return false;
    }

    if (!std::isdigit(input)) {
        return false;
    }

    return true;
}

/**
 * @brief Initializes the table with the letters.
 * @tparam T The type of the table.
 * @param table The table to initialize.
 * @returns void
*/
template <typename T>
void init(std::vector<T> &table) {
    std::vector<char> letters(7);

    // Decrease / increase the number of letters depending on the size.
    if (table.size() == 10) { // 5x2
        letters = { 'A', 'E', 'Z', 'P', 'D' };
    }
    else if (table.size() == 8) { // 4x2
        letters = { 'A', 'E', 'Z', 'D' };
    }
    else if (table.size() == 14) { // 7x2
        letters = { 'A', 'E', 'Z', 'P', 'D', 'B', 'M' };
    }

    std::vector<char> pairs;
    for (char letter : letters) {
        pairs.push_back(letter);
        pairs.push_back(letter);
    }

    SHUFFLE(pairs.begin(), pairs.end());

    for (int i = 0; i < table.size(); i++) {
        table[i] = pairs[i];
    }

    std::cout << "All available types are: ";

    for (int i = 0; i < letters.size(); i++) {
        if (i == letters.size() - 1) {
            std::cout << "and " << letters[i] << ".\n\n";
        }
        else {
            std::cout << letters[i] << ", ";
        }
    }
}

/**
 * @brief Utility function to print the table.
 * @tparam T The type of the table.
 * @param table The table to print.
 * @returns void
*/
template <typename T>
void print_table(const std::vector<T> &table) {
    std::cout << " | ";
    for (int i = 0; i < table.size(); i++) {
        if (i % 5 == 0 && i != 0) {
			std::cout << "\n | ";
		}

		std::cout << table[i] << " | ";
	}
}

// Prototype function. This is needed as `ask_data` calls `reset_data`, and `reset_data` calls `ask_data`.
template <typename T>
void reset_data(const std::vector<T> &table, int& answer, int& old_answer, int& memory_count);

/**
 * @brief Function that asks the user for their
 * input in the table they previously chose.
 * @tparam T The type of the table.
 * @param table The table that's used to get the user's input and data.
 * @param answer The user's answer.
 * @param old_answer The user's previous answer.
 * @param memory_count A counter to check if the user has already answered two values.
 * @returns void
*/
template <typename T>
void ask_data(const std::vector<T> &table, int& answer, int& old_answer, int& memory_count) {
    old_answer = answer;
    print_table(table);

    std::cout << "\nType your response here (number index):\n";
    std::cin >> answer;

    if (is_number(answer)) {
		std::cout << "\nYou must enter a valid number.\n\n";
		reset_data(table, answer, old_answer, memory_count);
	}

    // Increase the memory count, which will be later on used for checking if the user has already answered two values.
    memory_count++;

    if ((answer > table.size()) || (answer < 1)) {
		std::cout << "\nYou can't check a value that doesn't exist (or an invalid number).\n\n";
        reset_data(table, answer, old_answer, memory_count);
	}

    if (old_answer == answer) {
        std::cout << "\nYou can't check the same value twice.\n\n";
        reset_data(table, answer, old_answer, memory_count);
    }

    // If two matches are answered already, but the user checkes a non-answered and an answered value, the program will mark it as no match, however, we must not allow the user to check the same value twice.
    if ((table[answer - 1] != NULL) && ((table[old_answer] == NULL) || (table[old_answer] != NULL))) {
       std::cout << "\nYou can't check the same value twice.\n\n";
       reset_data(table, answer, old_answer, memory_count);
    }
}

/**
 * @brief Utility function that resets the data if the user enters an invalid value.
 * @tparam T The type of the table.
 * @param table The table that will be used to call `ask_data()`.
 * @param answer The user's answer.
 * @param old_answer The user's previous answer.
 * @param memory_count A counter to check if the user has already answered two values.
 * @returns void
*/
template <typename T>
void reset_data(const std::vector<T>& table, int& answer, int& old_answer, int& memory_count) {
    answer = old_answer;
    memory_count--;

    ask_data(table, answer, old_answer, memory_count);
}

/**
 * @brief Checks if the two values given by the user match.
 * @tparam T The type of the table.
 * @param table_empty The table with no values, slowly assigned from `table` depending on the user's input.
 * @param table The table with the original values.
 * @param answer The user's answer.
 * @param first_time A boolean to check if the user has already answered a value.
 * @param old_answer The user's previous answer.
 * @param memory_count A counter to check if the user has already answered two values.
 * @returns true IF the values given by the user match
 * @returns false if the values given by the user do NOT match
*/
template <typename T>
bool match(const std::vector<T> &table, std::vector<T> &table_empty, int &answer, bool &first_time, int &old_answer, int &memory_count) {
    if (first_time == true) {
        return true;
    }

    // Search across the whole table and if the two values match, keep results, otherwise, hide 'em up.
    for (int i = 0; i < table.size() + 1; i++) {
        if (i == answer) {
            if (table[i - 1] == table[old_answer - 1]) {
                first_time = true;
                memory_count = 0;

                old_answer = 0;
				return true;
			}
            else {
                std::cout << "\nNo match (value was " << table[i - 1] << ", index is " << i << ").\n\n";

                table_empty[old_answer - 1] = NULL;
                table_empty[answer - 1] = NULL;

                first_time = true;
                memory_count = 0;

                old_answer = 0;
				return false;
			}
		}
	}

    return false;
}

/**
 * @brief Function to assign the results to the table.
 *
 * Also checkes if the user has answered all the values already, as well as
 * verify if the user made a match or not.
 * @tparam T The type of the tables.
 * @param table_empty The table with no values, slowly assigned from `table` depending on the user's input.
 * @param table The table with the original values.
 * @param answer The user's answer.
 * @param first_time A boolean to check if the user has already answered a value.
 * @param old_answer The user's previous answer.
 * @param memory_count A counter to check if the user has already answered two values.
 * @returns void
*/
template <typename T>
void assign_results(std::vector<T> &table_empty, std::vector<T> &table, int& answer, bool& first_time, int &old_answer, int &memory_count) {
    // Search through the entire table and if the answer matches the index, show the value.
    // If it doesn't match, hide both the values. Don't forget to keep older values already answered.
    for (int i = 0; i < table.size() + 1; i++) {
        if (i == answer) {
            if (match(table, table_empty, answer, first_time, old_answer, memory_count) == true) {
                table_empty[i - 1] = table[i - 1];
				first_time = true;
			}
		}
	}

    if (memory_count == 1) {
        first_time = false;
        memory_count = 0;
    }

    char try_again;

    // Has the user finished the game? Use a `for` loop, and if the table is full, ask the user if he wants to play again.
    for (int i = 0; i < table.size() + 1; i++) {
        if (table_empty[i] == NULL) {
			break;
		}
        else if (i == table.size() - 1) {
            print_table(table);

			std::cout << "\nYou won. Congratulations! Do you want to play again? (y/n)\n";
			std::cin >> try_again;
            if (try_again == 'y') {
                // This is needed when checking if the user has two matches already.
                for (int i = 0; i < table.size(); i++) {
                    table_empty[i] = NULL;
                }

				init(table);
			}
            else if (try_again == 'n') {
				std::cout << "\nThanks for playing the game!\n";
				sleep(3);

				exit(0);
			}
            else {
				std::cout << "\nInvalid input (exitting...).\n";
                exit(0);
			}
		}
	}

    // Ask data again.
    ask_data(table_empty, answer, old_answer, memory_count);
    assign_results(table_empty, table, answer, first_time, old_answer, memory_count);
}
}  // namespace memory_game

/**
 * @brief Main function
 * @returns 0 on exit
*/
int main() {
    std::srand(std::time(nullptr));

    int size = 0;
    int selection = 0;

    int response = 0, old_answer = 0, memory_count = 0;
    bool first_time = true;

    std::cout << "\tMEMORY GAME\n";

    do {
        std::cout << "\n1. 4x2 (1)";
        std::cout << "\n2. 5x2 (2)";
        std::cout << "\n3. 7x2 (3)\n";

        std::cout << "\nChoose table size: ";
        std::cin >> selection;
    } while ((selection < 1 || selection > 3) && (!memory_game::is_number(selection)));

    switch(selection) {
        case 1: size = 8; break;
        case 2: size = 10; break;
        case 3: size = 14; break;
        default: size = 10; break;
    }

    std::vector<char> table(size);
    std::vector<char> table_empty(size);

    std::cout << "\n";
    memory_game::init(table);

    memory_game::ask_data(table_empty, response, old_answer, memory_count);
    memory_game::assign_results(table_empty, table, response, first_time, old_answer, memory_count);

    return 0;
}
