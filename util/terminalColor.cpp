#include "terminalColor.h"

void setColor(TerminalColor foreground) {
	setColor(foreground, TerminalColor::BLACK);
}

void setColor(TerminalColorPair color) {
	setColor(color.foreground, color.background);
}

#ifdef _MSC_VER

#include <Windows.h>
static HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
void setColor(TerminalColor foreground, TerminalColor background) {
	SetConsoleTextAttribute(console, int(foreground) | (int(background) << 4));
}

#else

#include <iostream>
static char cvtWinColorToUnixColor(int winColor){
	switch(winColor){
		case 0: return '0';   // BLACK
		case 1: return '4';   // BLUE
		case 2: return '2';   // GREEN
		case 3: return '6';   // AQUA
		case 4: return '1';   // RED
		case 5: return '5';   // MAGENTA
		case 6: return '3';   // YELLOW
		case 7: return '7';   // WHITE
		default: return '7';  // WHITE
	}
}
void setColor(TerminalColor foreground, TerminalColor background) {
	std::cout << "\033[0;";
	if(int(foreground) & 0b00001000){ // strong
		std::cout << "1;";
	}
	if(background != TerminalColor::BLACK) {
		std::cout << '4' << cvtWinColorToUnixColor(int(background) & 0b00000111) << ';';
	}
	std::cout << '3' << cvtWinColorToUnixColor(int(foreground) & 0b00000111);

	std::cout << 'm';
}

#endif


