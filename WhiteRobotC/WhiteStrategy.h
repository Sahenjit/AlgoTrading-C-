//
// Created by Shankar on 25/09/2021.
//

#pragma once
#include <vector>

class WhiteStrategy
{
public:

    WhiteStrategy();
    int stateAnalyser(int &m_state);
    int whiteStateMachine(double &last_trade_investment,int &m_state, std::vector<double> &m_slope, int &m_point,
                          double &m_slopeMin_long, int &m_mode_long, std::vector<double> &m_ma_small_long, std::vector<double> &m_ma_medium_long,
                          std::vector<double> &m_ma_large_long, double &m_slopeMin_short, std::vector<double> &m_ma_small_short,
                          std::vector<double> &m_ma_medium_short,std::vector<double> &m_ma_large_short, int &m_mode_short,
                          std::vector<int> &m_state_signal,std::vector<double> &m_portfolio_value, std::vector<int> &m_stop_loss,
                          int &m_long_stop_loss, double &m_stopLoss, int &m_short_stop_loss, std::vector<double> &m_prices);
    double orderAnalyser(double& current_cash, double& last_trade_investment, double& cfd_units,std::vector<int> &m_order_signal,
                                        int &m_point, std::vector<double> &m_prices, int &m_long_trades, int &m_short_trades, double &m_long_trades_profit,
                                        std::vector<double> &m_trade_profit, int &m_good_long_trades, double &m_short_trades_profit, int &m_good_short_trades,
                                        std::vector<double> &m_current_cash, std::vector<double> &m_cfd_units, std::vector<double> &m_last_trade_investment);
<<<<<<< Updated upstream
    bool checkStopLoss(double last_trade_investment,std::vector<double> m_portfolio_value, int m_point, int m_state,
                                      std::vector<int> m_stop_loss, int m_long_stop_loss, double m_stopLoss, int m_short_stop_loss,
                                      std::vector<double> m_prices);
=======
    bool checkStopLoss(double &last_trade_investment,std::vector<double> &m_portfolio_value, int &m_point, int &m_state,
                                      std::vector<int> &m_stop_loss, int &m_long_stop_loss, double &m_stopLoss, int &m_short_stop_loss,
                                      std::vector<double> &m_prices);
>>>>>>> Stashed changes
    bool trailingStopLoss();
};




