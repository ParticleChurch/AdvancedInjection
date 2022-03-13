#pragma once
#include "injectable.hpp"

#include <vector>

#include <Windows.h>


namespace AdvancedInjection {
	class BaseException : public std::exception {
	public:
		BaseException(std::string what) : std::exception(what.c_str()) {};
	};
	class OpenProcessError : public BaseException { using BaseException::BaseException; };


	typedef bool (*InjectionProgressCallback)( // return true if the injection should be terminated, otherwise false
		const Injectable& root, // the initial injection
		const double& overallProgress, // an estimation of the overall progress
		const Injectable& current, // what we are currently injecting (differs from `root` when injecting a dependency)
		const double& currentProgress // progress on current injection
	);

	class Program {
		DWORD processId = 0;
		HANDLE processHandle = 0;

		bool callback(const Injectable& root, const double& overallProgress, const Injectable& current, const double& currentProgress)
		{
			return this->progressCallback && this->progressCallback(root, overallProgress, current, currentProgress);
		}

		bool injectSingle(const Injectable& root, const Injectable& inj)
		{

		}

	public:
		InjectionProgressCallback progressCallback = nullptr;

		Program(DWORD processId) {
			this->processId = processId;
			this->processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, this->processId);

			if (!this->processHandle)
			{
				throw OpenProcessError("Failed to open handle to process with Windows error: " + std::to_string(GetLastError()));
			}
		}

		bool inject(const Injectable& root) {
			if (this->callback(root, 0, root, 0)) return false;

			// gather dependencies
			std::vector<Injectable> deps = root.getDeepDependencies();

			// inject dependencies
			for (const auto& dep : deps)
				if (!this->injectSingle(root, dep))
					return false;

			// inject root
			if (!this->injectSingle(root, root))
				return false;
		}

		~Program() {
			if (this->processHandle) CloseHandle(this->processHandle);
		}
	};
}