/****************************************************************************************
* Project		:	AlgoTrading Jorge, David, Camilo, Shanka
* File			:	WhiteRobot.cpp
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
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/

#include "WhiteRobot.h"

/****************************************************************************************
*									MEMBER FUNCTIONS									*
****************************************************************************************/

//constructor
WhiteRobot::WhiteRobot()
{
	m_point = 0;
	m_state = 1;
}

//Getters and setters

vector<double> WhiteRobot::getPrices() {
	return this->m_prices;
}

void WhiteRobot::printPrices() {
	cout << "Printing Data" << endl;
	for (auto& element : m_prices) {
		cout << element << endl;
	}
}

string WhiteRobot::getTimeStr() {
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::string s(30, '\0');
	std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	return s;
}

//Tokenize a string into a vector of strings by a given character
vector<string> WhiteRobot::tokenize(string& str, char delim) {
	size_t start;
	size_t end = 0;
	vector<string> result;

	while ((start = str.find_first_not_of(delim, end)) != string::npos)
	{
		end = str.find(delim, start);
		result.push_back(str.substr(start, end - start));
	}

	return result;
}


// Load the index data from CSV file
void WhiteRobot::loadData(string fileName) {

	string line;
	ifstream myStream(fileName);
	if (myStream.is_open()) {

		getline(myStream, line);

		cout << "The first line is: " << line << endl;


		while (getline(myStream, line)) {

			//Parse the line 
			vector<string> fields = tokenize(line, ',');
			double price = stod(fields[1]);

			// Store the readed values
			if (price > 0) {
				m_dates.push_back(fields[0]);
				m_prices.push_back(price);
			}
		}
		myStream.close();
	}
	else
	{
		cout << "There was a problem opening the file: " << fileName << endl;
	}
}

// Simple moving average function
double WhiteRobot::movingAverage(vector<double> prices, int windowSize) {
	vector<double> window;
	window = vector < double > (prices.end() - windowSize, prices.end());
	auto n = window.size();
	double average = 0.0;
	if (n != 0) {
		average = accumulate(window.begin(), window.end(), 0.0) / n;
	}
	return average;
}

// Moving slope function
double WhiteRobot::movingSlope(const vector<double> prices, int windowSize) {
	//from https://stackoverflow.com/questions/18939869/how-to-get-the-slope-of-a-linear-regression-line-using-c
	
	vector<double> y;
	y = vector < double >(prices.end() - windowSize, prices.end());
	auto n = y.size();
	
	vector<double> x(n);
	iota(x.begin(), x.end(), 0); // x will become: [0..n]

	const auto s_x = std::accumulate(x.begin(), x.end(), 0.0);
	const auto s_y = std::accumulate(y.begin(), y.end(), 0.0);
	const auto s_xx = std::inner_product(x.begin(), x.end(), x.begin(), 0.0);
	const auto s_xy = std::inner_product(x.begin(), x.end(), y.begin(), 0.0);
	const auto slope = (n * s_xy - s_x * s_y) / (n * s_xx - s_x * s_x);
	return slope;
}

// Signal generator function
vector<double> WhiteRobot::generateSignals(vector<double> prices, int maPointsS, int maPointsM, int maPointsL, int slopePoints) {
	vector<double> signals;
	double maS, maM, maL, slope;
	maS = movingAverage(prices, maPointsS);
	signals.push_back(maS);
	maM = movingAverage(prices, maPointsM);
	signals.push_back(maM);
	maL = movingAverage(prices, maPointsL);
	signals.push_back(maL);
	slope = movingSlope(prices, slopePoints);
	signals.push_back(slope);
	return signals;
}

// Generates the order signal from the state of the State Machine
int WhiteRobot::stateAnalyser() {
	if (m_state == 3 || m_state == 4) {
		//Long position
		return 1;
	}
	else if (m_state == 6 || m_state == 7) {
		//Short position
		return -1;
	}
	else {
		//Not invested
		return 0;
	}
	
}

// Analyses if the stop loss condition has been reached
bool WhiteRobot::checkStopLoss(double stopLoss, double last_trade_investment) {

	double curren_trade_profit = (m_portfolio_value[m_point-1] - last_trade_investment) / last_trade_investment;

	if ( (m_state == 3 || m_state == 4 || m_state == 6 || m_state == 7) &&  curren_trade_profit < - stopLoss) {
		m_stop_loss.push_back(1);
		return true;
	}
	else {
		m_stop_loss.push_back(0);
		return false;
	}
		
}

// State machine containing the brain (logic) of the robot
int WhiteRobot::whiteStateMachine(double slopeMin, double stopLoss, double last_trade_investment) {

	if (checkStopLoss(stopLoss, last_trade_investment)) {
		// Slop loss limit reached in the previous point
		m_state = 1;
	}
	else if (m_state == 1) {
		if (m_slope[m_point] > slopeMin) {
			//positive trend
			if ((m_ma_small[m_point] > m_ma_medium[m_point]) && (m_ma_small[m_point - 1] < m_ma_medium[m_point - 1])) {
				//14>21
				m_state = 2;
			}
		}
		else if (m_slope[m_point] < -slopeMin) {
			//negative trend
			if ((m_ma_small[m_point] < m_ma_medium[m_point]) && (m_ma_small[m_point - 1] > m_ma_medium[m_point - 1])) {
				//14<21
				m_state = 5;
			}
		}

	}
	else if (m_state == 2) {
		if ((m_ma_small[m_point] > m_ma_large[m_point]) && (m_ma_small[m_point - 1] < m_ma_large[m_point - 1])) {
			//14>40
			m_state = 3;
		}
	}
	else if (m_state == 3) {
		if ((m_ma_small[m_point] < m_ma_medium[m_point]) && (m_ma_small[m_point - 1] > m_ma_medium[m_point - 1])) {
			//14<21
			m_state = 4;
		}
	}
	else if (m_state == 4) {
		if ((m_ma_small[m_point] < m_ma_large[m_point]) && (m_ma_small[m_point - 1] > m_ma_large[m_point - 1])) {
			//14<40
			m_state = 1;
		}
	}
	else if (m_state == 5) {
		if ((m_ma_small[m_point] < m_ma_large[m_point]) && (m_ma_small[m_point - 1] > m_ma_large[m_point - 1])) {
			//14<40
			m_state = 6;
		}
	}
	else if (m_state == 6) {
		if ((m_ma_small[m_point] > m_ma_medium[m_point]) && (m_ma_small[m_point - 1] < m_ma_medium[m_point - 1])) {
			//14>21
			m_state = 7;
		}
	}
	else if (m_state == 7) {
		if ((m_ma_small[m_point] > m_ma_large[m_point]) && (m_ma_small[m_point - 1] < m_ma_large[m_point - 1])) {
			//14>40
			m_state = 1;
		}
	}
	// Return the order signal according to the state
	return stateAnalyser();
}


// Analyses the current and previous order signal to determine the current portfolio value
double  WhiteRobot::orderAnalyser(double& current_cash, double& last_trade_investment, double& cfd_units) {
	
	double portfolio_value;
	
	// Evaluate order signals and update invesment position variables
	if (m_order_signal[m_point] == 1 && m_order_signal[m_point - 1] == 0) {
		//Start Long trade
		last_trade_investment = current_cash;
		current_cash = 0;
		cfd_units = last_trade_investment / m_prices[m_point];
	}
	else if (m_order_signal[m_point] == 0 && m_order_signal[m_point - 1] == 1) {
		//Stop Long trade
		current_cash = cfd_units* m_prices[m_point];
		cfd_units = 0;
	}
	else if (m_order_signal[m_point] == -1 && m_order_signal[m_point - 1] == 0) {
		//Start short trade
		last_trade_investment = current_cash;
		current_cash = 0;
		cfd_units = last_trade_investment / m_prices[m_point];
	}
	else if (m_order_signal[m_point] == 0 && m_order_signal[m_point - 1] == -1) {
		//Stop short trade
		current_cash = 2* last_trade_investment - cfd_units * m_prices[m_point];
		cfd_units = 0;
	}

	//Anlayse the porfolio value
	if (m_order_signal[m_point] == 1) {
		portfolio_value = current_cash + cfd_units * m_prices[m_point];
	}
	else if (m_order_signal[m_point] == -1) {
		portfolio_value = current_cash + 2 * last_trade_investment - cfd_units * m_prices[m_point];
	}
	else {
		portfolio_value = current_cash;
	}

	//Return the current porfolo value
	return portfolio_value;
}


// White strategy backtest implementation
void WhiteRobot::whiteStrategy(int maPointsS, int maPointsM, int maPointsL, int slopePoints, double slopeMin, double stopLoss, double intialCash) {
	cout << "Executing White strategy" << endl;

	double current_cash = intialCash;
	double last_trade_investment = 1;
	double cfd_units = 0;

	if (slopePoints > maPointsL && maPointsL > maPointsM && maPointsM > maPointsS) {

		// Loop over the first part of the dataset
		for (auto it = m_prices.begin(); it != m_prices.begin() + slopePoints; ++it) {
			m_ma_small.push_back(0.0);
			m_ma_medium.push_back(0.0);
			m_ma_large.push_back(0.0);
			m_slope.push_back(0.0);
			m_order_signal.push_back(0);
			m_portfolio_value.push_back(intialCash);
			m_stop_loss.push_back(0);
			++m_point;
		}

		// Loop over the tradable part of the dataset
		for (auto it = m_prices.begin() + slopePoints; it != m_prices.end(); ++it) {
			vector<double> window;
			window = vector < double >(it - slopePoints+1, it+1);
			vector<double> signals = generateSignals(window, maPointsS, maPointsM, maPointsL, slopePoints);
			m_ma_small.push_back(signals[0]);
			m_ma_medium.push_back(signals[1]);
			m_ma_large.push_back(signals[2]);
			m_slope.push_back(signals[3]);
			m_order_signal.push_back(whiteStateMachine(slopeMin,stopLoss, last_trade_investment));
			m_portfolio_value.push_back(orderAnalyser(current_cash, last_trade_investment, cfd_units));
			++m_point;
		}
	}
	else
	{
		cout << " Window MA Small: " << maPointsS << endl;
		cout << " Window MA Medium: " << maPointsM << endl;
		cout << " Window MA Large: " << maPointsL << endl;
		cout << " Window Slope: " << slopePoints << endl;
		cout << " Strategy imposible to execute" << endl;
	}	
}

// Print backtest simulation results on the console
void WhiteRobot::printResults(int maPointsS, int maPointsM, int maPointsL, int slopePoints, double slopeMin, double stopLoss) {

	cout << endl << "****************************************************************************" << endl;
	cout << endl << "Simulation Results." << endl<<endl;
	//date, initial_index, final_index, index_return, initial_porfolio, final_porfolio, portfolio_return, small_ma, medium_ma, large_ma, slope_points, min_slope, stop_loss, sm_mode_up, sm_mode_up

	cout << "Simulation date: " << getTimeStr() << endl;
	cout << "Initial index: " << m_prices[0] << endl;
	cout << "Final index: " << m_prices.back() << endl;
	cout << "Index return: " << 100 * (m_prices.back() - m_prices[0]) / m_prices[0] << "%" << endl;
	cout << "Initial portfolio: " << m_portfolio_value[0] << endl;
	cout << "Final portfolio: " << m_portfolio_value.back() << endl;
	cout << "portfolio return: " << 100 * (m_portfolio_value.back() - m_portfolio_value[0]) / m_portfolio_value[0] << "%" << endl;

	cout << "Small moving average points: " << maPointsS << endl;
	cout << "Medium moving average point: " << maPointsM << endl;
	cout << "Large moving average point: " << maPointsL << endl;
	cout << "Slope points: " << slopePoints << endl;
	cout << "Min slope: " << slopeMin << endl;
	cout << "Stop loss: " << stopLoss << endl;
	cout << "State machine mode Up: " << "1" << endl;
	cout << "State machine mode Down: " << "1" << endl;

	cout << endl << "End of simulation Results." << endl << endl;
	cout << endl << "****************************************************************************" << endl;
}

// Add backtest simulation results on CSV file
void WhiteRobot::saveSimulation(string fileName, int maPointsS, int maPointsM, int maPointsL, int slopePoints, double slopeMin, double stopLoss) {

	ofstream file_out;


	// File format:
	//date, initial_index, final_index, index_return, initial_porfolio, final_porfolio, portfolio_return, small_ma, medium_ma, large_ma, slope_points, min_slope, stop_loss, mode_up, mode_dn

	file_out.open(fileName, std::ios_base::app);

	file_out << getTimeStr() << ",";
	file_out << m_prices[0] << ",";
	file_out << m_prices.back() << ",";
	file_out << 100 * (m_prices.back() - m_prices[0]) / m_prices[0] << "%" << ",";
	file_out << m_portfolio_value[0] << ",";
	file_out << m_portfolio_value.back() << ",";
	file_out << 100 * (m_portfolio_value.back() - m_portfolio_value[0]) / m_portfolio_value[0] << "%" << ",";

	file_out << maPointsS << ",";
	file_out << maPointsM << ",";
	file_out << maPointsL << ",";
	file_out << slopePoints << ",";
	file_out << slopeMin << ",";
	file_out << stopLoss << ",";
	file_out << "1" << ",";
	file_out << "1" << endl;	

	cout << endl << "Simulation results added to: "<< fileName << endl;

}

// Genereate a new backtest simulation CSV data file
void WhiteRobot::saveSimulationData(string fileName) {

	// Create an output filestream object
	ofstream outFile(fileName);

	// write the file headers
	outFile << "date" << "," << "price" << "," << "ma_small" << "," << "ma_medium" << "," << "ma_large" << "," << "ma_slope" << "," << "order_signal" << "," << "portfolio_value" << "," << "stop_loss" << endl;

	// write data to the file
	for (int i = 0; i != m_prices.size(); i++) {
		outFile << m_dates[i] << "," << m_prices[i] << "," << m_ma_small[i] << "," << m_ma_medium[i] << "," << m_ma_large[i] << "," << m_slope[i] << "," << m_order_signal[i] << "," << m_portfolio_value[i] << "," << m_stop_loss[i] <<endl;
	}
	// close the output file
	outFile.close();

	cout << endl << "Simulation data saved into: "<< fileName << endl;
}

//destructor
WhiteRobot::~WhiteRobot()
{
}

