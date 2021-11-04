/****************************************************************************************
* Project		:	AlgoTrading Jorge, David, Camilo, Shanka
* File			:	WhiteRobot.h
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
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <fstream>
#include <vector> 
#include <numeric>
#include <chrono>
#include <algorithm>
#include <chrono>
#include <ctime>
#include "Signal_Generator.h"
#include "WhiteStrategy.h"
#include "Date.h"
using namespace std;

/****************************************************************************************
*									CLASS DECLARATION									*
****************************************************************************************/

class WhiteRobot
{

public:

	//constructors

	WhiteRobot(): m_maPointsS_long(1), m_maPointsM_long(2), m_maPointsL_long(3), m_slopeMin_long(0.1), m_mode_long(1), m_maPointsS_short(1), m_maPointsM_short(2), m_maPointsL_short(3), m_slopeMin_short(0.1), m_mode_short(1),
	m_slopePoints(4), m_stopLoss(0.1), m_point(0), m_state(1), m_long_stop_loss(0), m_short_stop_loss(0), m_long_trades(0), m_short_trades(0), m_good_long_trades(0), m_good_short_trades(0), m_long_trades_profit(0),
	m_short_trades_profit(0) {}

	WhiteRobot(int maPointsS_long,	int maPointsM_long, int maPointsL_long, double slopeMin_long, int mode_long, int maPointsS_short, int maPointsM_short, int maPointsL_short, double slopeMin_short, int mode_short, int slopePoints,	double stopLoss):
	m_maPointsS_long(maPointsS_long), m_maPointsM_long(maPointsM_long), m_maPointsL_long(maPointsL_long), m_slopeMin_long(slopeMin_long), m_mode_long(mode_long), m_maPointsS_short(maPointsS_short), m_maPointsM_short(maPointsM_short),
	m_maPointsL_short(maPointsL_short), m_slopeMin_short(slopeMin_short), m_mode_short(mode_short), m_slopePoints(slopePoints), m_stopLoss(stopLoss), m_point(0), m_state(1), m_long_stop_loss(0), m_short_stop_loss(0), m_long_trades(0),
	m_short_trades(0), m_good_long_trades(0), m_good_short_trades(0), m_long_trades_profit(0), m_short_trades_profit(0) {}


	//Getters and setters

	void setParameters(int maPointsS_long, int maPointsM_long, int maPointsL_long, double slopeMin_long, int mode_long, int maPointsS_short, int maPointsM_short, int maPointsL_short, double slopeMin_short, int mode_short, int slopePoints, double stopLoss);

	vector<double> getPrices();

	void printPrices();

	string getTimeStr();


	//public member functions	

	vector<string>  tokenize(string& str, char delim);

	void loadData(string fileName);

	void loadSelectedData(string fileName, string from, string to);

	void generateSignals(vector<double> prices_window);

	void RunStrategy(double intialCash);

	void printResults();
	
	void saveSimulation(string fileName);

	void saveSimulationData(string fileName);

	//Creates a friend class to WhiteStategy in order to access private member variables
	~WhiteRobot();

private:

	//private variable members

	int m_maPointsS_long; // Long Moving Average Variable (Small)
	int m_maPointsM_long; // Long Moving Average Variable (Medium)
	int m_maPointsL_long; // Long Moving Average Variable (Large)
	double m_slopeMin_long; //long Slope Variable
	int m_mode_long; // Mode Type Long

	int m_maPointsS_short; // Short Moving Average Variable (Small)
	int m_maPointsM_short; // Short Moving Average Variable (Medium)
	int m_maPointsL_short; // Short Moving Average Variable (Large)
	double m_slopeMin_short; // Short Slop Variable
	int m_mode_short; // Mode Type Short

	int m_slopePoints; //No. of Slope Points
	double m_stopLoss; // Stop Loss


	vector<string> m_dates; // Contains all the asset class dates
	vector<double> m_prices; // Contains the closing prices of the asset class
	vector<double> m_volume; // Contains the volume of the asset class


	vector<double> m_ma_small_long; //Contains the Long moving average (Small) Points
	vector<double> m_ma_medium_long; // Contains the Long moving average (Medium) Points
	vector<double> m_ma_large_long; // Contains the Long moving average (Large) Points
	vector<double> m_ma_small_short; //Contains the Short moving average (Small) Points
	vector<double> m_ma_medium_short; //Contains the Short moving average (Medium) Points
	vector<double> m_ma_large_short; // Contains the Short moving average (Large) points

	vector<double> m_slope; // Contains the slope values

	int m_point; // Keeps track of the points
	int m_state; // Keeps track of the current state of the state machine
	int m_long_stop_loss; // Stores the long stop loss
	int m_short_stop_loss; // Stores the short stop loss

	int m_long_trades; //Stores the No. of Long trades
	int m_short_trades; //Stores the No. of short trades
	int m_good_long_trades; // Stores the No. of good long trades
	int m_good_short_trades; // Stores the No. of good short trades
	double m_long_trades_profit; //Stores the profit made from long trades
	double m_short_trades_profit; //Stores the profit made from short trades

	vector<int> m_state_signal; // Keeps a track of the state signal throughout the simulation process
	vector<int> m_order_signal; //Keep a track of the order signal throughout the simulation process

	vector<double> m_current_cash; // Keeps a track of the cash movement throughout the simulation process
	vector<double> m_cfd_units; // Keeps a track of the No. of CFD Units
	vector<double> m_last_trade_investment; // Keeps a track of the Last trade made of the CFD
	vector<double> m_portfolio_value; // Keeps a track of the portfolio movement with respect to time
	vector<double> m_trade_profit; // Keeps a track of the PNL with respect to time
	vector<int> m_stop_loss; // Stores the stop loss values

	//Brain of the Robot
    WhiteStrategy ws;
    //Signal Generator
    Signal_Generator generate;
};

