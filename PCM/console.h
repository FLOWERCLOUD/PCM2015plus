/************************************************************************/
/*                    console output the debug info.                    */
/************************************************************************/

#ifndef _CONSOLE_H
#define _CONSOLE_H
#include <iostream>
#include <string>
using namespace std;

// Supply console to std output
class ConsoleLogger
{
public:
	//output information in console 
	static void Destory();
	static ConsoleLogger * Instance();
public:
	virtual ~ConsoleLogger();

protected:
	ConsoleLogger();

private:
	static ConsoleLogger* m_instance;
	FILE *file;
};

#endif
