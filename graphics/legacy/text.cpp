#include "core.h"

#include "text.h"

#pragma region Word

Word::Word(double fontSize) : fontSize(fontSize), width(0.0) {

}

void Word::addCharacter(const Character& character) {
	characters.push_back(character);
	width += character.width + (double) character.advance;
}

#pragma endregion

#pragma region Line

Line::Line(double maxWidth, double spaceWidth) : maxWidth(maxWidth), spaceWidth(spaceWidth), width(0.0) {

}	

bool Line::addWord(const Word& word) {
	// Get word width
	double extraWidth = word.width;
	// Add space in front
	extraWidth += (!words.empty() ? spaceWidth : 0.0);
	if (width + extraWidth <= maxWidth) {
		words.push_back(word);
		width += extraWidth;
		return true;
	}

	return false;
}

#pragma endregion

#pragma region Text

Text::Text(std::string textString, double x, double y, Font* font, double fontSize, double maxWidth, double spaceWidth, TextFlags textFlags) : textString(textString), x(x), y(y), font(font), fontSize(fontSize), maxWidth(maxWidth), spaceWidth(spaceWidth), textFlags(textFlags) {

}


#pragma endregion

#pragma region TextLoader

std::vector<Line> TextLoader::loadText(const Text& text) {
	const int ASCII_SPACE = 32;

	std::vector<Line> lines;
	Line currentLine(text.maxWidth, text.spaceWidth);
	Word currentWord(text.fontSize);
	
	for (char character : text.textString) {
		int ascii = (int) character;

		// Add word if character is a space
		if (ascii == ASCII_SPACE) {
			// Check if the word can be added
			bool added = currentLine.addWord(currentWord);
			if (!added) {
				// Add word to new line
				lines.push_back(currentLine);
				currentLine = Line(text.maxWidth, text.spaceWidth);
				currentLine.addWord(currentWord);
			}
			currentWord = Word(text.fontSize);
		} else {
			// Add character to the word
			currentWord.addCharacter(text.font->getCharacter(ascii));
		}
	}

	// Add last word
	bool added = currentLine.addWord(currentWord);
	if (!added) {
		// Add word to new line
		lines.push_back(currentLine);
		currentLine = Line(text.maxWidth, text.spaceWidth);
		currentLine.addWord(currentWord);
	}
	// Add last line
	lines.push_back(currentLine);

	return lines;
}

#pragma endregion