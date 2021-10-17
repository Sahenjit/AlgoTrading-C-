/****************************************************************************************
* Project		:	AlgoTrading Jorge, David, Camilo, Shanka
* File			:	RobotMenu.h
* Lenguaje		:	C++
* License		:	Apache License Ver 2.0, www.apache.org/licenses/LICENSE-2.0
* Description	:	main CPP file for the program, entry point.
*
* References	:	- B. Stroustrup: The C++ Programming Language (Fourth Edition).
*					  Addison Wesley. Reading Mass. USA. May 2013. ISBN 0-321-56384-0.
*					- M.Capinski and T.Zastawniak, Numerical Methods in Finance with C++,
*					  Cambridge, 2012, code: http://www.cambridge.org/9780521177160
* Other files	:
* Git Control	:	https://github.com/camiloblanco/WhiteRobotC
* Author - Year	:	Sahenjit Paul - Camilo Blanco Vargas - Year: 2021
* Mail - Web	:	shanks.p.95@gmail.com -:mail@camiloblanco.com
****************************************************************************************/

/****************************************************************************************
*							#GUARDS #INCLUDES AND #CONSTANTS							*
****************************************************************************************/
#pragma once

#include<iostream>
#include<string>
#include<stdlib.h>
#include <random>
#include "WhiteRobot.h"
using namespace std;

/****************************************************************************************
*									CLASS DECLARATION									*
****************************************************************************************/
class RobotMenu
{
public:
	//constructors
	RobotMenu();

	// Console manipulation member functions
	void clearConsole();
	void menuPause();
	void readLine(string& description);

	//public member functions
	void testData();
	void executeWhite();
	void randomWhite();
	void closedRandomWhite();
	void FixedBrainRandomWhite();
	void mainMenu();
	
};

