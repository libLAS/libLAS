#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>

#include <stdlib.h>
#include <process.h>
#include <windows.h>

#include <iostream.h>

enum PIPES { READ_HANDLE, WRITE_HANDLE }; /* Constants 0 and 1 for READ and WRITE */

// open a gzipped file as if it was a regular file

extern "C"
{
FILE* fopenGzipped(const char* filename, const char* mode)
{
	// check mode
	if (mode[0] == 'r')
	{
		// open input file
		FILE* gzipInput = fopen(filename, mode);
		if (!gzipInput) return NULL;

		// create the pipe
		int hPipe[2];
		if (_pipe(hPipe, 2048, ((mode[1] =='b') ? _O_BINARY : _O_TEXT) | _O_NOINHERIT) == -1)
		{
			cout << "could not create pipe" << endl;
			return NULL;
		}

		// duplicate stdin handle
		int hStdIn = _dup(_fileno(stdin));
		// redirect stdin to input file
		if (_dup2(_fileno(gzipInput), _fileno(stdin)) != 0)
		{
			cout << "could not redirect stdin" << endl;
			return NULL;
		}

		// duplicate stdout handle
		int hStdOut = _dup(_fileno(stdout));
		// redirect stdout to write end of pipe
		if (_dup2(hPipe[WRITE_HANDLE], _fileno(stdout)) != 0)
		{
			cout << "could not set pipe output" << endl;
			return NULL;
		}

		// close original write end of pipe
		close(hPipe[WRITE_HANDLE]);

		// redirect read end of pipe to input file
		if (_dup2(hPipe[READ_HANDLE], _fileno(gzipInput)) != 0)
		{
			cout << "could not redirect input file" << endl;
			return NULL;
		}

		// close original read end of pipe
		close(hPipe[READ_HANDLE]);

		// Spawn process
		HANDLE hProcess = (HANDLE) spawnlp(P_NOWAIT, "gzip", "gzip", "-d", NULL);

		// redirect stdin back into stdin
		if (_dup2(hStdIn, _fileno(stdin)) != 0)
		{
			cout << "could not reconstruct stdin" << endl;
			return NULL;
		}

		// redirect stdout back into stdout
		if (_dup2(hStdOut, _fileno(stdout)) != 0)
		{
			cout << "could not reconstruct stdout" << endl;
			return NULL;
		}

		// return input file
		return gzipInput;
	}
	else
	{
		return NULL;
	}
}
}