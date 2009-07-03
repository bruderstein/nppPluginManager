#include <string>
#include <iostream>
#include "DirectLinkSearch.h"
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;


DirectLinkSearch::DirectLinkSearch(char *filename)
{
	_file.open(filename);
}

DirectLinkSearch::~DirectLinkSearch()
{
}

shared_ptr<char> DirectLinkSearch::search(char *filename)
{
	size_t patternLength = strlen(filename);
    int shiftTable[256];

    for (int position = 0; position < 256; position++)
    {
       shiftTable[position] = patternLength;
    }

	for (size_t position = 0; position <= patternLength; position++)
    {
       shiftTable[filename[position]] = patternLength - position - 1;
    }

	int checkOffset = 0;
	int shiftPosition;
	size_t currentPosition = patternLength - 1;
	int currentChar;
	do 
	{
		currentChar = _file.getCharAt(currentPosition - checkOffset);
		shiftPosition = shiftTable[currentChar];
		if (currentChar == filename[patternLength - checkOffset - 1])
		{
			checkOffset++;
			if (checkOffset == patternLength)
			{
				// Found filename
				size_t realPosition = validateDirectLink(currentPosition-checkOffset + 1);
				if (realPosition != LINK_NOT_VALID)
				{
					shared_ptr<char> buffer(new char[currentPosition - realPosition + 2]);
					int bufferPosition = 0;
					while(realPosition <= currentPosition)
					{
						buffer.get()[bufferPosition++] = _file.getCharAt(realPosition++);
					}
					buffer.get()[bufferPosition] = '\0';
					
					return buffer;
				}
				else
				{
					// Reset the check offset
					checkOffset = 0;
					// Advance to next posible position
					currentPosition += patternLength;
				}
			}
		}
		else
		{
			currentPosition += shiftPosition - checkOffset;
			checkOffset = 0;
		}

		
	} while (_file.getCharAt(currentPosition) != FILEBUFFER_EOF);
	
	shared_ptr<char> empty;
	return empty;

}


long DirectLinkSearch::validateDirectLink(long currentPosition)
{
	char searchString[] = "href=\"http://";
	char searchString2[] = "href=\"https://";
	char validUriChars[] = ":@&=+$,;/?:";

	size_t charListLen = strlen(validUriChars);
	const int LINKOFFSET = 6;

	long lastNonDomain = currentPosition;
	long minPosition = currentPosition - MAX_RESULT_SIZE;

	size_t startSearchPos = strlen(searchString) - 1;
	size_t startSearchPos2 = strlen(searchString2) - 1;
	size_t searchPos = startSearchPos;
	size_t searchPos2 = startSearchPos2;

	bool found = false;

	while(currentPosition >= minPosition)
	{
		int currentChar = _file.getCharAt(currentPosition);
		if (currentChar != '.'
			&& currentChar != '-'
			&& !(currentChar >= 'a' && currentChar <= 'z') 
			&& !(currentChar >= 'A' && currentChar <= 'Z')
			&& !(currentChar >= '0' && currentChar <= '9'))
		{
			lastNonDomain = currentPosition;
		

			if (!findChar(currentChar, validUriChars, charListLen)
				&& !(currentChar == '\"' && (searchString[searchPos] == '\"' || searchString2[searchPos2] == '\"')))
				break;
		}
		
		
		if (searchString[searchPos] == currentChar)
		{
			--searchPos;
			if (searchPos < 0)
			{
				return currentPosition + LINKOFFSET;
			}
		}
		else
			searchPos = startSearchPos;

		if (searchString2[searchPos2] == currentChar)
		{
			--searchPos2;
			if (searchPos < 0)
			{
				return currentPosition + LINKOFFSET;
			}
		}
		else
			searchPos2 = startSearchPos2;
		
		--currentPosition;
	} /* while */

	
	return LINK_NOT_VALID;

}


bool DirectLinkSearch::findChar(int ch, const char *charList, int charListLen)
{
	charListLen--;
	while (charListLen >= 0)
	{
		if (ch == charList[charListLen])
			return true;
		charListLen--;

	}
	return false;
}
