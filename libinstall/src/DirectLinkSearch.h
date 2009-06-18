#ifndef _DIRECTLINKSEARCH_H
#define _DIRECTLINKSEARCH_H

#include <string>
#include <iostream>
#include <fstream>
#include "FileBuffer.h"
#include <boost/shared_ptr.hpp>

class DirectLinkSearch
{
public:
	DirectLinkSearch(char *filename);
	~DirectLinkSearch();

	boost::shared_ptr<char> search(char *filename);


private:
	FileBuffer _file;


	static const int LINK_NOT_VALID = -1;
	static const int MAX_RESULT_SIZE = 384;

    long validateDirectLink(long currentPosition);
	bool findChar(int ch, const char *charList, int charListLen);

};

#endif