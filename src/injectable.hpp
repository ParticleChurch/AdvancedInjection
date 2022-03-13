#pragma once
#include <vector>
#include <string>
#include <unordered_set>

namespace AdvancedInjection {
	// represents any object that can be injected into a program
	struct Injectable {
		const std::string identifier;
		Injectable(const std::string& identifier) : identifier(identifier) {};

		// return injectables which must be injected prior to injecting this one
		virtual std::vector<Injectable> getShallowDependencies() const = 0;

		/*
			You shouldn't need to override functions following this comment
		*/
	private:
		static void getDeepDependencies_recursiveHelper(const Injectable& root, std::unordered_set<std::string>& alreadyIncluded, std::vector<Injectable>& output) {
			alreadyIncluded.insert(root.identifier);

			for (const auto& dep : root.getShallowDependencies())
			{
				if (alreadyIncluded.contains(dep.identifier)) continue;

				Injectable::getDeepDependencies_recursiveHelper(dep, alreadyIncluded, output);

				output.push_back(dep);
			}
		}

	public:
		std::vector<Injectable> getDeepDependencies() const {
			std::unordered_set<std::string> alreadyIncluded = {};
			std::vector<Injectable> output = {};

			Injectable::getDeepDependencies_recursiveHelper(*this, alreadyIncluded, output);

			return output;
		}

		bool operator ==(const Injectable& other) {
			return this->identifier == other.identifier;
		}
	};
}