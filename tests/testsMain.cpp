#include "testsMain.h"

#include <vector>
#include <iostream>
#include "../util/log.h"

#include <string>
#include <fstream>


class Test {
public:
	const char* file;
	const char* func;
	void(*f)();

	Test() : file(nullptr), func(nullptr), f(nullptr) {};
	Test(const char* file, const char* func, void(*f)()) : file(file), func(func), f(f) {}
	void run() {
		try {
			#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
				const char* fileName = strrchr(this->file, '\\') ? strrchr(__FILE__, '\\') + 1 : this->file;
			#else
				const char* fileName = strrchr(file, '/') ? strrchr(__FILE__, '/') + 1 : file;
			#endif

			Log::info("Running %s:%s()", fileName, func);
			f();
		} catch (AssertionError& e) {
			Log::error("An assertion was incorrect at line %d:", e.line);

			std::ifstream inFile;
			inFile.open(file);
			if (!inFile) {
				Log::error("Could not open File %s for debugging:(");
				return;
			}

			for (int i = 0; i < e.line - 3; i++) {
				std::string l;
				std::getline(inFile, l);
			}

			std::string s;
			for (int i = 0; i < 5; i++) {
				std::getline(inFile, s);
				printf("%d: %s", e.line - 2 + i, s.c_str());
				if (i == 2) {
					Log::setColor(14);
					std::cout << "  >>>>  " << e.what();
					Log::setColor(15);
				}
				std::cout << std::endl;
			}


		} catch (std::exception& e) {
			Log::error("An general error was thrown: %s", e.what());
		} catch (std::string& e) {
			Log::error("An string exception was thrown: %s", e.c_str());
		} catch (...) {
			Log::error("An unknown exception was thrown! ");
		}
	}
};

AssertionError::AssertionError(int line, std::string info) : line(line), info(info) {}
const char* AssertionError::what() const { return info.c_str(); }


Test* tests;
int testCount = 0;

int main(int argc, char * argv[]) {
	Log::setColor(15);
	std::cout << "Starting tests: " << std::endl;
	std::cout << "Testcount: " << testCount << std::endl;
	for (int i = 0; i < testCount; i++) {
		Test& t = tests[i];
		t.run();
	}
	std::cout << "Tests finished" << std::endl;
	std::cin.get();
}

void logAssertError(std::string text) {
	Log::error(text.c_str());
}

TestAdder::TestAdder(const char* file, const char* name, void(*f)()) {
	if (testCount == 0) tests = new Test[500];


	char* newFile = new char[strlen(file) + 1];
	for (int i = 0; i < strlen(file) + 1; i++) newFile[i] = file[i];

	char* newName = new char[strlen(name) + 1];
	for (int i = 0; i < strlen(name) + 1; i++) newName[i] = name[i];

	tests[testCount++] = Test(newFile, newName, f);
}

#include "testUtils.h"

TEST_CASE(helloTest) {
	int a = 3;
	int b = 7;
	ASSERT(b < a);


}
