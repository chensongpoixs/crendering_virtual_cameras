#include "cutil.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
char* load_file_context(const char* url)
{
	char* fileContext = NULL;
	FILE* fp = ::fopen(url, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		if (len > 0)
		{
			rewind(fp);
			fileContext = new char[len + 1];
			fread(fileContext, 1, len, fp);
			fileContext[len] = 0;
		}
	}
	return fileContext;
}