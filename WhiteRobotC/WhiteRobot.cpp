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

//constructors



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

void WhiteRobot::setParameters(int maPointsS_long, int maPointsM_long, int maPointsL_long, double slopeMin_long, int mode_long, int maPointsS_short, int maPointsM_short, int maPointsL_short, double slopeMin_short, int mode_short, int slopePoints, double stopLoss) {
	
	m_maPointsS_long = maPointsS_long;
	m_maPointsM_long = maPointsM_long;
	m_maPointsL_long = maPointsL_long;
	m_slopeMin_long = slopeMin_long;
	m_mode_long = mode_long;

	m_maPointsS_short = maPointsS_short;
	m_maPointsM_short = maPointsM_short;
	m_maPointsL_short = maPointsL_short;
	m_slopeMin_short = slopeMin_short;
	m_mode_short = mode_short;

	m_slopePoints = slopePoints;
	m_stopLoss = stopLoss;

	m_point = 0;
	m_state = 1;
	m_long_stop_loss = 0;
	m_short_stop_loss = 0;

	m_long_trades = 0;
	m_short_trades = 0;
	m_good_long_trades = 0;
	m_good_short_trades = 0;
	m_long_trades_profit = 0;
	m_short_trades_profit = 0;

	m_ma_small_long.clear();
	m_ma_medium_long.clear();
	m_ma_large_long.clear();

	m_ma_small_short.clear();
	m_ma_medium_short.clear();
	m_ma_large_short.clear();


	m_slope.clear();
	m_state_signal.clear();
	m_order_signal.clear();

	m_current_cash.clear();
	m_cfd_units.clear();
	m_last_trade_investment.clear();
	m_portfolio_value.clear();
	m_trade_profit.clear();
	m_stop_loss.clear();
}


string WhiteRobot::getTimeStr() {
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::string s(30, '\0');
	std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	return s;
}

//public member functions

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
		cout << endl <<" " << fileName << " successfully opened." << line << endl;
		getline(myStream, line);
		cout << "The first line is: " << line << endl;


		while (getline(myStream, line)) {

			//Parses the line
			vector<string> fields = tokenize(line, ',');
			double price = stod(fields[1]);

			// Stores the read values
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

//Load Data based on Time Constraints
void WhiteRobot:: loadSelectedData(string fileName, string from, string to) {
    string line;
    ifstream myStream(fileName);
    if (myStream.is_open()) {
        cout << endl << " " << fileName << " successfully opened." << line << endl;
        getline(myStream, line);
        cout << "The first line is: " << line << endl;


        while (getline(myStream, line)) {

            //Parses the line
            vector<string> fields = tokenize(line, ',');
            string time = (fields[0]);
            double price = stod(fields[1]);
            if ((strcmp(time.c_str(), from.c_str()) >= 0)) {
                // Stores the read values
                if (price > 0) {
                    m_dates.push_back(fields[0]);
                    m_prices.push_back(price);
                }
            }
            else if (time == from)
            {
                break;
            }
        }
        myStream.close();
    } else {
        cout << "There was a problem opening the file: " << fileName << endl;
    }
}



// Signal generator function
void WhiteRobot::generateSignals(vector<double> prices_window) {


	m_ma_small_long.push_back(generate.movingAverage(prices_window, m_maPointsS_long));
	m_ma_medium_long.push_back(generate.movingAverage(prices_window, m_maPointsM_long));
	m_ma_large_long.push_back(generate.movingAverage(prices_window, m_maPointsL_long));

	m_ma_small_short.push_back(generate.movingAverage(prices_window, m_maPointsS_short));
	m_ma_medium_short.push_back(generate.movingAverage(prices_window, m_maPointsM_short));
	m_ma_large_short.push_back(generate.movingAverage(prices_window, m_maPointsL_short));

	m_slope.push_back(generate.movingSlope(prices_window, m_slopePoints));
}


// White strategy backtest implementation
void WhiteRobot::RunStrategy( double intialCash) {
    //cout << "Executing White strategy" << endl;

    double current_cash = intialCash;
    double last_trade_investment = 1;
    double cfd_units = 0;
    vector<int> max_vect{m_maPointsS_long, m_maPointsM_long, m_maPointsL_long, m_maPointsS_short, m_maPointsM_short,
                         m_maPointsL_short, m_slopePoints};
    int max_window_size = *max_element(max_vect.begin(), max_vect.end());

    if (m_maPointsS_long > 1 && m_maPointsM_long > 1 && m_maPointsL_long > 1 && m_maPointsS_short > 1 &&
        m_maPointsM_short > 1 && m_maPointsL_short > 1 && m_slopePoints > 1) {

        //Resizes the vectors for dataset entry
        m_ma_small_long.resize(max_window_size, 0);
        m_ma_medium_long.resize(max_window_size,0);
        m_ma_large_long.resize(max_window_size,0);

        m_ma_small_short.resize(max_window_size,0);
        m_ma_medium_short.resize(max_window_size,0);
        m_ma_large_short.resize(max_window_size,0);

        m_slope.resize(max_window_size,0);

        m_state_signal.resize(max_window_size,0);
        m_order_signal.resize(max_window_size,0);

        m_current_cash.resize(max_window_size,intialCash);
        m_cfd_units.resize(max_window_size,0);
        m_last_trade_investment.resize(max_window_size,0);
        m_portfolio_value.resize(max_window_size,intialCash);
        m_trade_profit.resize(max_window_size,0);

        m_stop_loss.resize(max_window_size,0);
        m_point = max_window_size;

        // Loop over the tradable part of the dataset
        for (auto it = m_prices.begin() + max_window_size; it != m_prices.end(); ++it) {
            vector<double> prices_window;
            prices_window = vector<double>(it - max_window_size + 1, it + 1);
            generateSignals(prices_window);
            if(m_point==403)
            {std::cout<<"caught";}
            m_order_signal.push_back(ws.whiteStateMachine(last_trade_investment, m_state, m_slope, m_point, m_slopeMin_long,
                                                          m_mode_long, m_ma_small_long, m_ma_medium_long, m_ma_large_long,
                                                          m_slopeMin_short, m_ma_small_short, m_ma_medium_short,
                                                          m_ma_large_short, m_mode_short, m_state_signal, m_portfolio_value, m_stop_loss,
                                                          m_long_stop_loss, m_stopLoss, m_short_stop_loss, m_prices));

            m_portfolio_value.push_back(ws.orderAnalyser(current_cash, last_trade_investment, cfd_units, m_order_signal,
                     m_point, m_prices, m_long_trades, m_short_trades, m_long_trades_profit,
                     m_trade_profit, m_good_long_trades, m_short_trades_profit, m_good_short_trades,
                     m_current_cash, m_cfd_units, m_last_trade_investment));
            ++m_point;
        }
    } else {

        cout << " Strategy Impossible to execute" << endl;

        // Fill everythong with 0 to avoid memory acces errors
        for (auto it = m_prices.begin(); it != m_prices.end(); ++it) {

            m_ma_small_long.push_back(0.0);
            m_ma_medium_long.push_back(0.0);
            m_ma_large_long.push_back(0.0);

            m_ma_small_short.push_back(0.0);
            m_ma_medium_short.push_back(0.0);
            m_ma_large_short.push_back(0.0);

            m_slope.push_back(0.0);

            m_state_signal.push_back(0);
            m_order_signal.push_back(0);

            m_current_cash.push_back(intialCash);
            m_cfd_units.push_back(0.0);
            m_last_trade_investment.push_back(0.0);
            m_portfolio_value.push_back(intialCash);
            m_trade_profit.push_back(0.0);

            m_stop_loss.push_back(0);

            ++m_point;
        }
    }
}



// Print backtest simulation results on the console
void WhiteRobot::printResults() {

	cout << endl << "****************************************************************************" << endl;
	cout << endl << "Simulation Results:" << endl<<endl;

	cout << "Simulation date: " << getTimeStr() << endl << endl;

	cout << "Initial date: " << m_dates[0] << endl ;
	cout << "Final date: " << m_dates.back() << endl << endl;

	cout << "Initial index: " << fixed << setprecision(2) << m_prices[0] << endl;
	cout << "Final index: " << fixed << setprecision(2) << m_prices.back() << endl;
	cout << "Index return: " << fixed << setprecision(2) << 100 * (m_prices.back() - m_prices[0]) / m_prices[0] << "%" << endl << endl;

	cout << "Initial portfolio value: " << fixed << setprecision(2) << m_portfolio_value[0] << endl;
	cout << "Final portfolio value: " << fixed << setprecision(2) << m_portfolio_value.back() << endl;
	cout << "portfolio return: " << fixed << setprecision(2) << 100 * (m_portfolio_value.back() - m_portfolio_value[0]) / m_portfolio_value[0] << "%" << endl << endl;

	cout << endl << "Trades statistics:" << endl << endl;

	cout << "Long trades: " << m_long_trades << endl;
	cout << "Good long trades: " << m_good_long_trades << endl;
	cout << "Long trades Profit: " << fixed << setprecision(2) << m_long_trades_profit << endl;
	cout << "Activations of Long stop-loss: " << m_long_stop_loss << endl << endl;

	cout << "Short trades: " << m_short_trades << endl;
	cout << "Good short trades: " << m_good_short_trades << endl;
	cout << "Short trades profit: " << fixed << setprecision(2) << m_short_trades_profit << endl;
	cout << "Activations of short stop-loss: " << m_short_stop_loss << endl << endl;

	cout << endl << "Simulation Parameters:" << endl << endl;

	cout << "Long strategy parameters: " << endl;
	cout << "Small moving average points: " << m_maPointsS_long << endl;
	cout << "Medium moving average point: " << m_maPointsM_long << endl;
	cout << "Large moving average point: " << m_maPointsL_long << endl;
	cout << "Min slope: " << fixed << setprecision(4) << m_slopeMin_long << endl;
	cout << "State machine mode for Long trades: " << m_mode_long << endl << endl;

	cout << "Short strategy parameters: " << endl;
	cout << "Small moving average points: " << m_maPointsS_short << endl;
	cout << "Medium moving average point: " << m_maPointsM_short << endl;
	cout << "Large moving average point: " << m_maPointsL_short << endl;
	cout << "Min slope: " << fixed << setprecision(4) << m_slopeMin_short << endl;
	cout << "State machine mode for short trades: " << m_mode_short << endl << endl;

	cout << "General strategy parameters: " << endl;
	cout << "Slope points: " << m_slopePoints << endl;
	cout << "Stop loss: " << fixed << setprecision(4) << m_stopLoss << endl;


	cout << endl << "End of simulation Results." << endl << endl;
	cout << endl << "****************************************************************************" << endl;
}

// Add backtest simulation results on CSV file
void WhiteRobot::saveSimulation(string fileName) {

	ofstream file_out;

	// File format:
	//simulation_date, intial_date, final_date, initial_index, final_index, index_return, initial_porfolio, final_porfolio, portfolio_return, long_trades, good_long_trades, long_trades_profit,long_stop_loss, short_trades, good_short_trades, short_trades_profit, short_stop_loss, small_ma_long, medium_ma_long, large_ma_long, min_slope_long, sm_mode_long, small_ma_short, medium_ma_short, large_ma_short, min_slope_short, sm_mode_short, slope_points, stop_loss


	file_out.open(fileName, ios_base::app);

	file_out << getTimeStr() << ",";
	file_out << m_dates[0] << ",";
	file_out << m_dates.back() << ",";
	file_out << fixed << setprecision(2) << m_prices[0] << ",";
	file_out << fixed << setprecision(2) << m_prices.back() << ",";
	file_out << fixed << setprecision(2) << 100 * (m_prices.back() - m_prices[0]) / m_prices[0] << "%" << ",";

	file_out << fixed << setprecision(2) << m_portfolio_value[0] << ",";
	file_out << fixed << setprecision(2) << m_portfolio_value.back() << ",";
	file_out << fixed << setprecision(2) << 100 * (m_portfolio_value.back() - m_portfolio_value[0]) / m_portfolio_value[0] << "%" << ",";

	file_out << m_long_trades << ",";
	file_out << m_good_long_trades << ",";
	file_out << fixed << setprecision(2) << m_long_trades_profit << ",";
	file_out << m_long_stop_loss << ",";

	file_out << m_short_trades << ",";
	file_out << m_good_short_trades << ",";
	file_out << fixed << setprecision(2) << m_short_trades_profit << ",";
	file_out << m_short_stop_loss << ",";

	file_out << m_maPointsS_long << ",";
	file_out << m_maPointsM_long << ",";
	file_out << m_maPointsL_long << ",";
	file_out << fixed << setprecision(4) << m_slopeMin_long << ",";
	file_out << m_mode_long << ",";

	file_out << m_maPointsS_short << ",";
	file_out << m_maPointsM_short << ",";
	file_out << m_maPointsL_short << ",";
	file_out << fixed << setprecision(4) << m_slopeMin_short << ",";
	file_out << m_mode_short << ",";

	file_out << m_slopePoints << ",";
	file_out << fixed << setprecision(4) << m_stopLoss << endl;

	//cout << endl << "Simulation results added to: "<< fileName << endl;

}

// Genereate a new backtest simulation CSV data file
void WhiteRobot::saveSimulationData(string fileName) {

	// Create an output filestream object
	ofstream file_out(fileName);

	// write the file headers
	file_out << "date" << ",";
	file_out << "price" << ",";
	file_out << "ma_small_long" << ",";
	file_out << "ma_medium_long" << ",";
	file_out << "ma_large_long" << ",";
	file_out << "ma_small_short" << ",";
	file_out << "ma_medium_short" << ",";
	file_out << "ma_large_short" << ",";
	file_out << "ma_slope" << ",";
	file_out << "state_signal" << ",";
	file_out << "order_signal" << ",";
	file_out << "current_cash" << ",";
	file_out << "cfd_units" << ",";
	file_out << "portfolio_value" << ",";
	file_out << "last_trade_investment" << ",";
	file_out << "m_trade_profit" << " ,";
	file_out << "stop_loss" << endl;


	// write data to the file
	for (int i = 0; i != m_prices.size(); i++) {
		file_out << m_dates[i] << ",";
		file_out << m_prices[i] << ",";
		file_out << m_ma_small_long[i] << ",";
		file_out << m_ma_medium_long[i] << ",";
		file_out << m_ma_large_long[i] << ",";
		file_out << m_ma_small_short[i] << ",";
		file_out << m_ma_medium_short[i] << ",";
		file_out << m_ma_large_short[i] << ",";
		file_out << m_slope[i] << ",";
		file_out << m_state_signal[i] << ",";
		file_out << m_order_signal[i] << ",";
		file_out << m_current_cash[i] << ",";
		file_out << m_cfd_units[i] << ",";
		file_out << m_portfolio_value[i] << ",";
		file_out << m_last_trade_investment[i] << ",";
		file_out << m_trade_profit[i] << ",";
		file_out << m_stop_loss[i] << endl;
	}
	// close the output file
	file_out.close();

	cout << endl << "Simulation data saved into: "<< fileName << endl;
}

//destructor
WhiteRobot::~WhiteRobot()
{
}

