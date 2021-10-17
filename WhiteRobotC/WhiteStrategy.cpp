//
// Created by Shankar on 25/09/2021.
//

#include "WhiteStrategy.h"


WhiteStrategy::WhiteStrategy() {}

// Generates the order signal from the state of the State Machine
int WhiteStrategy::stateAnalyser(int &m_state)
{

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

// State machine containing the brain (logic) of the robot
int WhiteStrategy::whiteStateMachine(double &last_trade_investment,int &m_state, std::vector<double> &m_slope, int &m_point,
                                     double &m_slopeMin_long, int &m_mode_long, std::vector<double> &m_ma_small_long, std::vector<double> &m_ma_medium_long,
                                     std::vector<double> &m_ma_large_long, double &m_slopeMin_short, std::vector<double> &m_ma_small_short,
                                     std::vector<double> &m_ma_medium_short,std::vector<double> &m_ma_large_short, int &m_mode_short,
                                     std::vector<int> &m_state_signal,std::vector<double> &m_portfolio_value, std::vector<int> &m_stop_loss,
                                     int &m_long_stop_loss, double &m_stopLoss, int &m_short_stop_loss, std::vector<double> &m_prices) {

    if (checkStopLoss(last_trade_investment, m_portfolio_value, m_point, m_state,
             m_stop_loss, m_long_stop_loss, m_stopLoss, m_short_stop_loss, m_prices)) {
        // Slop loss limit reached in the previous point
        m_state = 1;
    } else if (m_state == 1) {
        if (m_slope[m_point] >= m_slopeMin_long && m_mode_long != 0) {
            //positive trend
            if ((m_ma_small_long[m_point] > m_ma_medium_long[m_point]) &&
            (m_ma_small_long[m_point - 1] < m_ma_medium_long[m_point - 1]) &&
            (m_mode_long == 1 || m_mode_long == 2 || m_mode_long == 3)) {
                //S>M  big cycle
                m_state = 2;
            }
            else if ((m_ma_small_long[m_point] > m_ma_large_long[m_point]) &&
            (m_ma_small_long[m_point - 1] < m_ma_large_long[m_point - 1]) &&
            (m_mode_long == 4 || m_mode_long == 5 )) {
                //S>L
                m_state = 3;
            }
            else if ((m_ma_small_long[m_point] > m_ma_medium_long[m_point]) &&
            (m_ma_small_long[m_point - 1] < m_ma_medium_long[m_point - 1]) &&
            (m_mode_long == 6 || m_mode_long == 7 )) {
                //S>M small cycle
                m_state = 4;
            }
        }
        else if (m_slope[m_point] < -m_slopeMin_short && m_mode_long != 0) {
            //negative trend
            if ((m_ma_small_short[m_point] < m_ma_medium_short[m_point]) &&
            (m_ma_small_short[m_point - 1] > m_ma_medium_short[m_point - 1]) &&
            (m_mode_short == 1 || m_mode_short == 2 || m_mode_short == 3)) {
                //S<M big cycle
                m_state = 5;
            }
            else if ((m_ma_small_short[m_point] < m_ma_large_short[m_point]) &&
            (m_ma_small_short[m_point - 1] > m_ma_large_short[m_point - 1]) &&
            (m_mode_short == 4 || m_mode_short == 5)) {
                //S<L
                m_state = 6;
            }
            else if ((m_ma_small_short[m_point] < m_ma_medium_short[m_point]) &&
            (m_ma_small_short[m_point - 1] > m_ma_medium_short[m_point - 1]) &&
            (m_mode_short == 6 || m_mode_short == 7)) {
                //S<M small cycle
                m_state = 7;
            }
        }
    }
    else if (m_state == 2) {
        if ((m_ma_small_long[m_point] > m_ma_large_long[m_point]) &&
        (m_ma_small_long[m_point - 1] < m_ma_large_long[m_point - 1]) &&
        (m_mode_long == 1 || m_mode_long == 2)) {
            //S>L big cycle
            m_state = 3;
        }
        else if ((m_ma_small_long[m_point] > m_ma_large_long[m_point]) &&
        (m_ma_small_long[m_point - 1] < m_ma_large_long[m_point - 1]) &&
        (m_mode_long == 3)) {
            //S>L medium cycle
            m_state = 4;
        }
    }
    else if (m_state == 3) {
        if ((m_ma_small_long[m_point] < m_ma_medium_long[m_point]) &&
        (m_ma_small_long[m_point - 1] > m_ma_medium_long[m_point - 1]) &&
        (m_mode_long == 1 || m_mode_long == 5)) {
            //S<M
            m_state = 4;
        }
        else if ((m_ma_small_long[m_point] < m_ma_large_long[m_point]) &&
        (m_ma_small_long[m_point - 1] > m_ma_large_long[m_point - 1]) &&
        (m_mode_long == 2 || m_mode_long == 4)) {
            //S<L
            m_state = 1;
        }
    }
    else if (m_state == 4) {
        if ((m_ma_small_long[m_point] < m_ma_large_long[m_point]) &&
        (m_ma_small_long[m_point - 1] > m_ma_large_long[m_point - 1]) &&
        (m_mode_long == 1 || m_mode_long == 3 || m_mode_long == 5 || m_mode_long == 6 )) {
            //S<L
            m_state = 1;
        }
        if ((m_ma_small_long[m_point] < m_ma_medium_long[m_point]) &&
        (m_ma_small_long[m_point - 1] > m_ma_medium_long[m_point - 1]) &&
        (m_mode_long == 7)) {
            //S<M
            m_state = 1;
        }
    }
    else if (m_state == 5) {
        if ((m_ma_small_short[m_point] < m_ma_large_short[m_point]) &&
        (m_ma_small_short[m_point - 1] > m_ma_large_short[m_point - 1]) &&
        (m_mode_short == 1 || m_mode_short == 2)) {
            //S<L big cycle
            m_state = 6;
        }
        else if ((m_ma_small_short[m_point] < m_ma_large_short[m_point]) &&
        (m_ma_small_short[m_point - 1] > m_ma_large_short[m_point - 1]) &&
        (m_mode_short == 3)) {
            //S<L medium cycle
            m_state = 7;
        }
    }
    else if (m_state == 6) {
        if ((m_ma_small_short[m_point] > m_ma_medium_short[m_point]) &&
        (m_ma_small_short[m_point - 1] < m_ma_medium_short[m_point - 1]) &&
        (m_mode_short == 1 || m_mode_short == 5)) {
            //S>M
            m_state = 7;
        }
        else if ((m_ma_small_short[m_point] > m_ma_large_short[m_point]) &&
        (m_ma_small_short[m_point - 1] < m_ma_large_short[m_point - 1]) &&
        (m_mode_short == 2 || m_mode_short == 4)) {
            //S>L
            m_state = 1;
        }
    }
    else if (m_state == 7) {
        if ((m_ma_small_short[m_point] > m_ma_large_short[m_point]) &&
        (m_ma_small_short[m_point - 1] < m_ma_large_short[m_point - 1]) &&
        (m_mode_short == 1 || m_mode_short == 3 || m_mode_short == 5 || m_mode_short == 6)) {
            //S>L
            m_state = 1;
        }
        else if ((m_ma_small_short[m_point] > m_ma_medium_short[m_point]) &&
        (m_ma_small_short[m_point - 1] < m_ma_medium_short[m_point - 1]) &&
        (m_mode_short == 7)) {
            //S>M
            m_state = 1;
        }
    }
    else {
        m_state = 1;
    }


    m_state_signal.push_back(m_state);
    // Return the order signal according to the state
    return stateAnalyser(m_state);
}

// Analyses the current and previous order signal to determine the current portfolio value
double WhiteStrategy::orderAnalyser(double& current_cash, double& last_trade_investment, double& cfd_units,std::vector<int> &m_order_signal,
                                    int &m_point, std::vector<double> &m_prices, int &m_long_trades, int &m_short_trades, double &m_long_trades_profit,
                                    std::vector<double> &m_trade_profit, int &m_good_long_trades, double &m_short_trades_profit, int &m_good_short_trades,
                                    std::vector<double> &m_current_cash, std::vector<double> &m_cfd_units, std::vector<double> &m_last_trade_investment)
{

        double portfolio_value;

        // Evaluate order signals and update invesment position variables
        if (m_order_signal[m_point] == 1 && m_order_signal[m_point - 1] == 0) {
            //Start Long trade
            last_trade_investment = current_cash;
            current_cash = 0;
            cfd_units = last_trade_investment / m_prices[m_point];
            ++m_long_trades;
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
            ++m_short_trades;
        }
        else if (m_order_signal[m_point] == 0 && m_order_signal[m_point - 1] == -1) {
            //Stop short trade
            current_cash = 2* last_trade_investment - cfd_units * m_prices[m_point];
            cfd_units = 0;
        }

        //Calculate the porfolio value
        if (m_order_signal[m_point] == 1) {
            portfolio_value = current_cash + cfd_units * m_prices[m_point];
        }
        else if (m_order_signal[m_point] == -1) {
            portfolio_value = current_cash + 2 * last_trade_investment - cfd_units * m_prices[m_point];
        }
        else {
            portfolio_value = current_cash;
        }

        // Calculate trade profit and performance
        if (m_order_signal[m_point] == 0 && m_order_signal[m_point - 1] == 1) {
            //Long trade detected
            m_long_trades_profit += portfolio_value - last_trade_investment;
            m_trade_profit.push_back(portfolio_value - last_trade_investment);
            if (portfolio_value > last_trade_investment) {
                //Succesfull long trade
                ++m_good_long_trades;
            }
        }
        else if (m_order_signal[m_point] == 0 && m_order_signal[m_point - 1] == -1) {
            //short trade detected
            m_short_trades_profit += portfolio_value - last_trade_investment;
            m_trade_profit.push_back(portfolio_value - last_trade_investment);
            if (portfolio_value > last_trade_investment) {
                ++m_good_short_trades;
            }
        }
        else {
            m_trade_profit.push_back(0);
        }

        m_current_cash.push_back(current_cash);
        m_cfd_units.push_back(cfd_units);
        m_last_trade_investment.push_back(last_trade_investment);

        //Return the current porfolio value
        return portfolio_value;
    }

// Analyses if the stop loss condition has been reached
bool WhiteStrategy::checkStopLoss(double last_trade_investment,std::vector<double> m_portfolio_value, int m_point, int m_state,
                                  std::vector<int> m_stop_loss, int m_long_stop_loss, double m_stopLoss, int m_short_stop_loss,
                                  std::vector<double> m_prices) {

    double current_trade_profit = (m_portfolio_value[m_point-1] - last_trade_investment) / last_trade_investment;

    if ( (m_state == 3 || m_state == 4 ) &&  current_trade_profit < - m_stopLoss) {
        m_stop_loss.push_back(1);
        m_long_stop_loss ++;
        return true;
    }
    else if ((m_state == 6 || m_state == 7) && current_trade_profit < - m_stopLoss) {
        m_stop_loss.push_back(1);
        m_short_stop_loss ++;
        return true;
    }
    else if (m_point == m_prices.size()-1) {
        m_stop_loss.push_back(1);
        return true;
    }
    else {
        m_stop_loss.push_back(0);
        return false;
    }

}




