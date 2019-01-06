#include "testsMain.h"

#include <vector>
#include <iostream>

#include <string>
#include <fstream>
#include "stdarg.h"

#include <chrono>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
const char sepChar = '\\';
#else
const char sepChar = '/';
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
HANDLE console;

using namespace std;
using namespace chrono;

enum Color {
	DARK_BLUE = 1,
	DARK_GREEN = 2,
	AQUA = 3,
	DARK_RED = 4,
	PURPLE = 5,
	DARK_YELLOW = 6,
	LIGHT_GRAY = 7,
	GRAY = 8,
	BLUE = 9,
	GREEN = 10,
	CYAN = 11,
	RED = 12,
	MAGENTA = 13,
	YELLOW = 14,
	WHITE = 15,
};

Color SUCCESS_COLOR = GREEN;
Color FAILURE_COLOR = MAGENTA;
Color ERROR_COLOR = RED;

void color(Color c) {
	SetConsoleTextAttribute(console, c);
}

stringstream logStream;

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

	logStream << logBuffer << '\n';
}

void printDeltaTime(time_point<system_clock> startTime, Color c) {
	duration<double> delta = system_clock::now() - startTime;
	color(c);
	cout << " (" << fixed << delta.count() << "s)\n";
}

ifstream getFileStream(const char* fileName) {
	ifstream s(fileName);
	if(s.good()) return s;

	s = ifstream(string("..\\tests\\") + fileName);
	if(s.good()) return s;

	return ifstream(string("..\\..\\tests\\") + fileName);
}

void printFileSlice(const char* fileName, int line) {
	ifstream inFile = getFileStream(fileName);

	if(inFile.good()) {
		for(int i = 0; i < line - 3; i++) {
			string l;
			getline(inFile, l);
		}

		color(WHITE);

		string s;
		for(int i = 0; i < 5; i++) {
			getline(inFile, s);
			printf("%d: %s", line - 2 + i, s.c_str());
			if(i == 2) {
				color(YELLOW);
				cout << "  <<<<";
				color(WHITE);
			}
			cout << endl;
		}
	} else {
		color(WHITE);
		printf("Could not open File %s for debugging :(\n", fileName);
	}
}

class Test {
public:
	const char* filePath;
	const char* funcName;
	void(*testFunc)();

	Test() : filePath(nullptr), funcName(nullptr), testFunc(nullptr) {};
	Test(const char* filePath, const char* funcName, void(*testFunc)()) : filePath(filePath), funcName(funcName), testFunc(testFunc) {}
	void run() {
		resetLog();
		time_point<system_clock> startTime;
		try {
			const char* fileName = strrchr(this->filePath, sepChar) ? strrchr(this->filePath, sepChar) + 1 : this->filePath;

			color(CYAN);
			cout << "Running " << fileName << ":" << funcName;

			startTime = system_clock::now();

			testFunc();

			printDeltaTime(startTime, SUCCESS_COLOR);

		} catch (AssertionError& e) {

			printDeltaTime(startTime, FAILURE_COLOR);

			// on my machine this crashes when inFile.good() is tested right after inFile is created,
			// but when there's a little delay (the error print) it works. I've got no good explanation. 
			// I've got no idea, but it works, so I'm leaving it at that
			
			
			
			color(GREEN);
			cout << logStream.str().c_str();
			
			color(RED);
			printf("An assertion was incorrect at line %d:\n", e.line);

			printFileSlice(filePath, e.line);

			color(YELLOW);
			cout << " >>>> " << e.what() << endl;
			color(WHITE);

		} catch (exception& e) {
			printDeltaTime(startTime, ERROR_COLOR);
			color(RED); printf("An general error was thrown: %s\n", e.what());
		} catch (string& e) {
			printDeltaTime(startTime, ERROR_COLOR);
			color(RED); printf("An string exception was thrown: %s\n", e.c_str());
		} catch (...) {
			printDeltaTime(startTime, ERROR_COLOR);
			color(RED); printf("An unknown exception was thrown!\n");
		}
	}
};

AssertionError::AssertionError(int line, const char* info) : line(line), info(info) {}
const char* AssertionError::what() const { return info; }

// For some reason having this in static memory breaks it, a pointer seems to work
vector<Test>* tests = nullptr;

void initConsole() {
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND consoleWindow = GetConsoleWindow();

	RECT r;
	GetWindowRect(consoleWindow, &r);

	MoveWindow(consoleWindow, r.left, r.top, 800, 900, TRUE);
}

int main(int argc, char * argv[]) {
	initConsole();

	color(WHITE); cout << "Starting tests: ";
	color(GREEN); cout << "[SUCCESS] ";
	color(MAGENTA); cout << "[FAILURE] ";
	color(RED); cout << "[ERROR]" << endl;
	color(WHITE); cout << "Number of tests: " << tests->size() << endl;
	for (Test& t : *tests) {
		t.run();
	}
	color(WHITE);  cout << "Tests finished" << endl;
	cin.get();
}

void logAssertError(string text) {
	color(RED);
	cout << text.c_str();
}

TestAdder::TestAdder(const char* file, const char* name, void(*f)()) {
	if (tests == nullptr) tests = new vector<Test>();

	tests->push_back(Test(file, name, f));
}
