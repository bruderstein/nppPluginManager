
#include <tchar.h>
#include <string>
#include <iostream>
#include "DirectLinkSearch.h"
#include <boost/shared_ptr.hpp>

// Copied over from winnt.h, in order to avoid dependency to windows just for this.
#ifndef TEXT
	#ifdef  UNICODE
		#define TEXT(quote) L##quote
	#else
		#define TEXT(quote) quote
	#endif
#endif

using namespace std;
using namespace boost;


DirectLinkSearch::DirectLinkSearch(const TCHAR *filename)
{
	_file.open(filename);
}

DirectLinkSearch::~DirectLinkSearch()
{
}

shared_ptr<TCHAR> DirectLinkSearch::search(const TCHAR *filename)
{
	size_t patternLength = _tcslen(filename);
    size_t shiftTable[256];

    for (int position = 0; position < 256; position++)
    {
       shiftTable[position] = patternLength;
    }

	for (size_t position = 0; position <= patternLength; position++)
    {
       shiftTable[filename[position]] = patternLength - position - 1;
    }

	size_t checkOffset = 0;
	size_t shiftPosition;
	size_t currentPosition = patternLength - 1;
	TCHAR currentChar;
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
					shared_ptr<TCHAR> buffer(new TCHAR[currentPosition - realPosition + 2]);
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
	
	shared_ptr<TCHAR> empty;
	return empty;

}


size_t DirectLinkSearch::validateDirectLink(size_t currentPosition)
{
	TCHAR searchString[] = TEXT("href=\"http://");
	TCHAR searchString2[] = TEXT("href=\"https://");
	TCHAR validUriChars[] = TEXT(":@&=+$,;/?:");

	size_t charListLen = _tcslen(validUriChars);
	const int LINKOFFSET = 6;

	size_t lastNonDomain = currentPosition;
	size_t minPosition = currentPosition - MAX_RESULT_SIZE;

	size_t startSearchPos = _tcslen(searchString) - 1;
	size_t startSearchPos2 = _tcslen(searchString2) - 1;
	size_t searchPos = startSearchPos;
	size_t searchPos2 = startSearchPos2;

	while(currentPosition >= minPosition)
	{
		TCHAR currentChar = _file.getCharAt(currentPosition);
		if (currentChar != TEXT('.')
			&& currentChar != TEXT('-')
			&& !(currentChar >= TEXT('a') && currentChar <= TEXT('z')) 
			&& !(currentChar >= TEXT('A') && currentChar <= TEXT('Z'))
			&& !(currentChar >= TEXT('0') && currentChar <= TEXT('9')))
		{
			lastNonDomain = currentPosition;
		

			if (!findChar(currentChar, validUriChars, charListLen)
				&& !(currentChar == TEXT('\"') && (searchString[searchPos] == TEXT('\"') || searchString2[searchPos2] == TEXT('\"'))))
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


bool DirectLinkSearch::findChar(TCHAR ch, const TCHAR *charList, size_t charListLen)
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
