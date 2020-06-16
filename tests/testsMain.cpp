#include "testsMain.h"

#include <vector>
#include <iostream>
#include <cstring>
#include <stdio.h>

#include <string>
#include <fstream>
#include "stdarg.h"

#include <chrono>

#include "../util/log.h"
#include "../util/terminalColor.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
static const char sepChar = '\\';
#else
static const char sepChar = '/';
#endif


using namespace std;
using namespace chrono;

static const TerminalColor SUCCESS_COLOR = TerminalColor::GREEN;
static const TerminalColor FAILURE_COLOR = TerminalColor::RED;
static const TerminalColor ERROR_COLOR = TerminalColor::MAGENTA;
static const TerminalColor SKIP_COLOR = TerminalColor::LIGHT_GRAY;

stringstream logStream;

static void resetLog() {
	logStream.str("");
	logStream.clear();
}

char logBuffer[1<<16];

void logf(const char* format, ...) {
	
	va_list args;
	va_start(args, format);
	int length = std::vsnprintf(logBuffer, 1<<16, format, args);
	va_end(args);

	logStream << logBuffer << '\n';
}

static void printDeltaTime(time_point<system_clock> startTime, TerminalColor c) {
	duration<double> delta = system_clock::now() - startTime;
	setColor(c);
	cout << " (" << fixed << delta.count() << "s)\n";
}

static ifstream getFileStream(const char* fileName) {
	ifstream s(fileName);
	if(s.good()) return s;

	string path = string("tests") + sepChar + fileName;
	s = ifstream(path);
	if(s.good()) return s;

	path = string("..") + sepChar + path;
	s = ifstream(path);
	if(s.good()) return s;

	return ifstream(string("..") + sepChar + path);
}

#define SHOW_LINES_BEFORE 5
#define SHOW_LINES_AFTER 2

static void printFileSlice(const char* fileName, int line) {
	ifstream inFile = getFileStream(fileName);

	int t = 0;

	if(inFile.good()) {
		// skip lines until right before first line to show
		for(int i = 0; i < line - SHOW_LINES_BEFORE - 1; i++) {
			string l;
			getline(inFile, l);
		}

		setColor(TerminalColor::WHITE);

		string s;
		for(int i = -SHOW_LINES_BEFORE; i <= SHOW_LINES_AFTER; i++) {
			if(!getline(inFile, s)) break;
			if(i == 0) {
				setColor(TerminalColor::YELLOW);
			}
			printf("%d: %s", line + i, s.c_str());
			if(i == 0) {
				cout << "  <<<<";
				setColor(TerminalColor::WHITE);
			}
			cout << endl;
		}
	} else {
		setColor(TerminalColor::WHITE);
		printf("Could not open File %s for debugging :(\n", fileName);
	}
}

static void dumpLog() {
	setColor(TerminalColor::GREEN);
	cout << logStream.str().c_str();
}

class Test {
public:
	const char* filePath;
	const char* fileName;
	const char* funcName;
	void(*testFunc)(TestInterface&);

	Test() : filePath(nullptr), funcName(nullptr), fileName(nullptr), testFunc(nullptr) {};
	Test(const char* filePath, const char* funcName, void(*testFunc)(TestInterface&)) : filePath(filePath), funcName(funcName), testFunc(testFunc),
	fileName(std::strrchr(this->filePath, sepChar) ? std::strrchr(this->filePath, sepChar) + 1 : this->filePath) {}
	void run() {
		resetLog();
		time_point<system_clock> startTime;

		TestInterface testInterface;

		try {
			setColor(TerminalColor::CYAN);
			cout << fileName << ":" << funcName;

			startTime = system_clock::now();

			testFunc(testInterface);

			setColor(TerminalColor::GRAY);
			cout << " [" << testInterface.getAssertCount() << "]";

			printDeltaTime(startTime, SUCCESS_COLOR);
		} catch (AssertionError& e) {

			printDeltaTime(startTime, FAILURE_COLOR);
			dumpLog();
			
			setColor(TerminalColor::RED);
			printf("An assertion was incorrect at line %d:\n", e.line);

			printFileSlice(filePath, e.line);

			setColor(TerminalColor::YELLOW);
			cout << e.what() << endl;
			setColor(TerminalColor::WHITE);

		} catch (exception& e) {
			printDeltaTime(startTime, ERROR_COLOR);
			dumpLog();
			setColor(TerminalColor::RED); printf("An general error was thrown: %s\n", e.what());
		} catch(string& e) {
			printDeltaTime(startTime, ERROR_COLOR);
			dumpLog();
			setColor(TerminalColor::RED); printf("An string exception was thrown: %s\n", e.c_str());
		} catch(const char* ex){
			printDeltaTime(startTime, ERROR_COLOR);
			dumpLog();
			setColor(TerminalColor::RED); printf("A char* exception was thrown: %s\n", ex);
		} catch (...) {
			printDeltaTime(startTime, ERROR_COLOR);
			dumpLog();
			setColor(TerminalColor::RED); printf("An unknown exception was thrown!\n");
		}
	}
};

AssertionError::AssertionError(int line, const char* info) : line(line), info(info) {}
const char* AssertionError::what() const noexcept { return info; }

// For some reason having this in static memory breaks it, a pointer seems to work
vector<Test>* tests = nullptr;

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
HANDLE console;

static void initConsole() {
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND consoleWindow = GetConsoleWindow();

	RECT r;
	GetWindowRect(consoleWindow, &r);

	MoveWindow(consoleWindow, r.left, r.top, 800, 900, TRUE);
}
#else
static void initConsole() {}
#endif

int main(int argc, char * argv[]) {
	initConsole();

	setColor(TerminalColor::WHITE); cout << "Starting tests: ";
	setColor(SUCCESS_COLOR); cout << "[SUCCESS] ";
	setColor(FAILURE_COLOR); cout << "[FAILURE] ";
	setColor(ERROR_COLOR); cout << "[ERROR] ";
	setColor(SKIP_COLOR); cout << "[SKIP]" << endl;
	setColor(TerminalColor::WHITE); cout << "Number of tests: " << tests->size() << endl;

	if(argc == 1 || argc == 2 && std::string("--coverage") == argv[1]) {
		for(Test& t : *tests) {
			t.run();
		}
	} else {
		for(Test& t : *tests) {
			for(size_t i = 1; i < argc; i++) {
				std::string strArg(argv[i]);
				if(strArg == t.fileName || strArg == t.funcName) {
					t.run();
					break;
				}
			}
		}
	}
	setColor(TerminalColor::WHITE);  cout << "Tests finished" << endl;

	if(argc == 2 && std::string("--coverage") == argv[1]) return 0;

	while(true) {
		string input;

		setColor(TerminalColor::WHITE);
		cout << "> ";
		cin >> input;

		if(input == "exit") {
			break;
		} else {
			for(Test& t : *tests) {
				if(input == t.fileName || input + ".cpp" == t.fileName || input == t.funcName) {
					t.run();
				} else if(input == string(t.fileName) + ':' + string(t.funcName)) {
					t.run();
					break;
				}
			}
		}
	}
}

static void logAssertError(string text) {
	setColor(TerminalColor::RED);
	cout << text.c_str();
}

TestAdder::TestAdder(const char* file, const char* name, void(*f)(TestInterface&)) {
	if (tests == nullptr) tests = new vector<Test>();

	tests->push_back(Test(file, name, f));
}
