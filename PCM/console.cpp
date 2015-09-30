#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <iomanip>
#include "Console.h"


ConsoleLogger* ConsoleLogger::m_instance = NULL;
ConsoleLogger::ConsoleLogger()
{
	//creat a new console to the process.
	AllocConsole();
	int hCrun;       
	hCrun = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);   
	file  = _fdopen(hCrun, "w");   
	// use default stream buffer   
	setvbuf(file, NULL, _IONBF, 0);   
	*stdout = *file;   

	std::cout << "Debug Info Console"<<std::endl;   
}

ConsoleLogger::~ConsoleLogger()
{
	FreeConsole();
	fclose(file);
}

void ConsoleLogger::Destory()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
	return ;
}

ConsoleLogger* ConsoleLogger::Instance()
{
	if (m_instance==NULL)
	{
		m_instance = new ConsoleLogger;
	}
	return m_instance;
}
