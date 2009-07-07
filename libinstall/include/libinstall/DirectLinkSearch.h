#ifndef _DIRECTLINKSEARCH_H
#define _DIRECTLINKSEARCH_H

#include <string>
#include <iostream>
#include <fstream>
#include "libinstall/FileBuffer.h"
#include <boost/shared_ptr.hpp>

#include <limits.h>

#ifdef UNICODE

#endif

class DirectLinkSearch
{
public:
	DirectLinkSearch(const TCHAR *filename);
	~DirectLinkSearch();

	boost::shared_ptr<TCHAR> search(const TCHAR *filename);


private:
	FileBuffer _file;


	static const size_t LINK_NOT_VALID = UINT_MAX;
	static const int MAX_RESULT_SIZE = 384;

    size_t validateDirectLink(size_t currentPosition);
	bool findChar(TCHAR ch, const TCHAR *charList, size_t charListLen);

};

#endif