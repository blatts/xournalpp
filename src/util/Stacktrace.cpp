#include "Stacktrace.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <execinfo.h>
#endif

#include <iostream>
using std::endl;

/**
 * This code uses addr2line
 *
 * Another solution would be backtrace-symbols.c from cairo/util, but its really complicated
 */

Stacktrace::Stacktrace() { }

Stacktrace::~Stacktrace() { }

#ifdef WIN32
std::string Stacktrace::getExePath()
{
	char szFileName[MAX_PATH + 1];
	GetModuleFileNameA(NULL, szFileName, MAX_PATH + 1);

	return szFileName;
}
void Stacktrace::printStracktrace(std::ostream& stream)
{
	// Stracktrace is currently not implemented for Windows
	// Currently this is only needed for developing, so this is no issue
}
#else

std::string Stacktrace::getExePath()
{
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
}

#endif

void Stacktrace::printStracktrace(std::ostream& stream)
{
	void* trace[32];
	char** messages = (char**) NULL;
	char buff[2048];

	int trace_size = backtrace(trace, 32);
	messages = backtrace_symbols(trace, trace_size);

	std::string exeName = getExePath();

	// skip first stack frame (points here)
	for (int i = 1; i < trace_size; ++i)
	{
		stream << "[bt] #" << i << " " << messages[i] << endl;

		char syscom[1024];

		sprintf(syscom, "addr2line %p -e %s", trace[i], exeName.c_str());
		FILE* fProc = popen(syscom, "r");
		while (fgets(buff, sizeof(buff), fProc) != NULL)
		{
			stream << buff;
		}
		pclose(fProc);
	}
}

void Stacktrace::printStracktrace()
{
	printStracktrace(std::cerr);
}
