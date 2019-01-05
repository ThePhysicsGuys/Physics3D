#include "testsMain.h"

#include <vector>
#include <iostream>
#include "../util/log.h"

#include <string>
#include <fstream>
#include "stdarg.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
const char sepChar = '\\';
#else
const char sepChar = '/';
#endif


std::stringstream logStream;

void resetLog() {
	logStream.str("");
	logStream.clear();
}

char logBuffer[256 * 256];

void logf(const char* format, ...) {
	va_list args;
	va_start(args, format);
	int length = sprintf_s(logBuffer, 256 * 256 - 2, format, args);
	va_end(args);

	/*logBuffer[length-1] = '\n';
	logBuffer[length] = 'e';
	logBuffer[length + 1] = '\0';*/

	logStream << logBuffer << '\n';
}

class Test {
public:
	const char* file;
	const char* func;
	void(*f)();

	Test() : file(nullptr), func(nullptr), f(nullptr) {};
	Test(const char* file, const char* func, void(*f)()) : file(file), func(func), f(f) {}
	void run() {
		resetLog();
		try {
			const char* fileName = strrchr(this->file, sepChar) ? strrchr(this->file, sepChar) + 1 : this->file;

			Log::info("Running %s:%s", fileName, func);
			f();
		} catch (AssertionError& e) {

			// on my machine this crashes when inFile.good() is tested right after inFile is created,
			// but when there's a little delay (the error print) it works. I've got no good explanation. 
			// I've got no idea, but it works, so I'm leaving it at that
			
			std::ifstream inFile(file);
			
			Log::setColor(10);
			std::cout << logStream.str().c_str();
			
			Log::error("An assertion was incorrect at line %d:", e.line);

			if (inFile.good()) {
				for(int i = 0; i < e.line - 3; i++) {
					std::string l;
					std::getline(inFile, l);
				}

				std::string s;
				for(int i = 0; i < 5; i++) {
					std::getline(inFile, s);
					printf("%d: %s", e.line - 2 + i, s.c_str());
					if(i == 2) {
						Log::setColor(14);
						std::cout << "  <<<<";
						Log::setColor(15);
					}
					std::cout << std::endl;
				}
			} else {
				Log::error("Could not open File %s for debugging :(", file);
			}

			Log::setColor(14);
			std::cout << " >>>> " << e.what() << std::endl;
			Log::setColor(15);

		} catch (std::exception& e) {
			Log::error("An general error was thrown: %s", e.what());
		} catch (std::string& e) {
			Log::error("An string exception was thrown: %s", e.c_str());
		} catch (...) {
			Log::error("An unknown exception was thrown! ");
		}
	}
};

AssertionError::AssertionError(int line, const char* info) : line(line), info(info) {}
const char* AssertionError::what() const { return info; }

// For some reason having this in static memory breaks it, a pointer seems to work
std::vector<Test>* tests = nullptr;

int main(int argc, char * argv[]) {
	Log::setColor(15);
	std::cout << "Starting tests: " << std::endl;
	std::cout << "Testcount: " << tests->size() << std::endl;
	for (Test& t : *tests) {
		t.run();
	}
	std::cout << "Tests finished" << std::endl;
	std::cin.get();
}

void logAssertError(std::string text) {
	Log::error(text.c_str());
}

TestAdder::TestAdder(const char* file, const char* name, void(*f)()) {
	if (tests == nullptr) tests = new std::vector<Test>();

	tests->push_back(Test(file, name, f));
}
