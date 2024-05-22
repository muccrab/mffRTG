#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <random>
#include <chrono>
#include <algorithm>

// Represents a single production rule with a weight and a computed probability
struct Rule {
	char predecessor;
	std::string successor;
	double weight;
	double probability;
};

// Container for the set of rules
class RuleSet {
public:
	// Adds a rule to the set
	void addRule(const Rule& rule) {
		rules[rule.predecessor].push_back(rule);
		normalizeProbabilities(rule.predecessor);
	}

	// Selects a rule nondeterministically based on provided probability
	std::string applyRule(char predecessor, double randomValue) const {
		const auto& possibleRules = rules.at(predecessor);
		double cumulativeProbability = 0.0;

		for (const auto& rule : possibleRules) {
			cumulativeProbability += rule.probability;
			if (randomValue <= cumulativeProbability) {
				return rule.successor;
			}
		}
		return ""; // Should never reach here if probabilities are set correctly
	}

	// Loads the rule set from a text file
	static RuleSet loadFromFile(const std::string& filename) {
		RuleSet ruleSet;
		std::ifstream file(filename);
		std::string line;

		while (std::getline(file, line)) {
			Rule rule = parseRule(line);
			ruleSet.addRule(rule);
		}
		return ruleSet;
	}

private:
	std::unordered_map<char, std::vector<Rule>> rules;

	static Rule parseRule(const std::string& line) {
		Rule rule;
		size_t pos1 = line.find('(');
		size_t pos2 = line.find(')');
		size_t pos3 = line.find("->");

		rule.predecessor = line.at(0);
		rule.weight = std::stod(line.substr(pos1 + 1, pos2 - pos1 - 1));
		rule.successor = line.substr(pos3 + 3); // +3 to account for "->" and the space

		return rule;
	}

	void normalizeProbabilities(char predecessor) {
		auto& possibleRules = rules[predecessor];
		double totalWeight = 0.0;
		for (const auto& rule : possibleRules) {
			totalWeight += rule.weight;
		}
		for (auto& rule : possibleRules) {
			rule.probability = rule.weight / totalWeight;
		}
	}
};

// Generates a string using the L-system rules over a number of iterations
std::string generateLSystemString(const RuleSet& ruleSet, const std::string& axiom, int iterations) {
	std::string current = axiom;
	std::default_random_engine rng(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<double> dist(0.0, 1.0);

	for (int i = 0; i < iterations; ++i) {
		std::string next;
		for (const char& c : current) {
			double randomValue = dist(rng);
			std::string s(1, c);
			if (ruleSet.applyRule(c, randomValue).empty()) {
				next += c; // Keep the character if no rule is found
			} else {
				next += ruleSet.applyRule(c, randomValue);
			}
		}
		current = next;
	}

	return current;
}

// // Example usage
// int main() {
//     // Load rules from file
//     RuleSet ruleSet = RuleSet::loadFromFile("rules.txt");
//
//     // Generate a string using the L-system rules
//     std::string axiom = "A"; // Axiom
//     int iterations = 4;
//
//     std::string result = generateLSystemString(ruleSet, axiom, iterations);
//     std::cout << "Resulting L-system string: " << result << std::endl;
//
//     return 0;
// }
//
