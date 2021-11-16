/// \file   momentum_investor.cpp
///
/// \brief
///
/// \authors    ceohas and maarten
/// \date       2021-11-16
/// \copyright  Copyright 2017-2020 The Institute for New Economic Thinking,
/// Oxford Martin School, University of Oxford
///
///             Licensed under the Apache License, Version 2.0 (the "License");
///             you may not use this file except in compliance with the License.
///             You may obtain a copy of the License at
///
///                 http://www.apache.org/licenses/LICENSE-2.0
///
///             Unless required by applicable law or agreed to in writing,
///             software distributed under the License is distributed on an "AS
///             IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
///             express or implied. See the License for the specific language
///             governing permissions and limitations under the License.
///
///             You may obtain instructions to fulfill the attribution
///             requirements in CITATION.cff
///

#include "TrendTest.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////
using std::make_tuple;
using std::shared_ptr;
using std::seed_seq;
using std::array;
using std::vector;
using std::tuple;
using std::endl;
using std::get;
using std::map;


using esl::economics::markets::walras::quote_message;
using namespace esl::economics::finance;
using esl::simulation::time_interval;
using namespace esl::simulation;
using namespace esl::economics;
using esl::law::property;
using esl::identity;
using namespace esl;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//sing sxtraction function
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Excess Demand Function

map<identity<law::property>, esl::variable>
TTest::excess_demand(
        const std::map<identity<law::property>,
                tuple<markets::quote, esl::variable>> &quotes)const{
        
        

//Params definition
        map<esl::identity<property>, esl::variable> result_;
                time_point t = get<0>(net_asset_value);
                        double cashflow_ = double(get<1>(net_asset_value)),
                                nav_ = double(get<2>(net_asset_value)),
                                        portfolio_alloc = 0.6,// wealth allocated to stock potfolio
                                                sum_of_signals,
                                                        phi;//trading signal for stock i




//compute a sum of exponents of signals, see McFadden choice function...........................(1)
    for(auto &[k, v] : quotes){
        const auto &[quote_, variable_] = v;
            const auto quoted_price_ = double(get<price>(quote_.type));
                int window_size_MA1 = int(get<1>(trends.find(k)->second));
                    int window_size_MA2 = int(get<2>(trends.find(k)->second));
                        double sum_MA1 = double(get<3>(trends.find(k)->second));
                             double sum_MA2 = double(get<4>(trends.find(k)->second));
                                double trend1 = (double(sum_MA1+quoted_price_)/window_size_MA1);
                                    double trend2 = (double(sum_MA2+quoted_price_)/window_size_MA2);
                                        phi = double(log10(trend1/trend2));//stock signal
                                            sum_of_signals = sum_of_signals + exp(pow(phi,2));}



        
        

//compute the ratio of each stock's exponent signal relative to sum described in (1)
    for(auto &[k, v] : quotes){
        const auto &[quote_, variable_] = v;
            const auto quoted_price_ = double(get<price>(quote_.type));
                int window_size_MA1 = int(get<1>(trends.find(k)->second));
                    int window_size_MA2 = int(get<2>(trends.find(k)->second));
                        double sum_MA1 = double(get<3>(trends.find(k)->second));
                            double sum_MA2 = double(get<4>(trends.find(k)->second));
                                double trend1 = (double(sum_MA1+quoted_price_)/window_size_MA1);
                                    double trend2 = (double(sum_MA2+quoted_price_)/window_size_MA2);
                                        phi = double(log10(trend1/trend2));//stock signal
                                            auto stock_alloc = sgn(phi) * exp(pow(phi,2)) /sum_of_signals;//stock wealth allocation



            
            

//investment in the subject stock
        auto i = trends.find(k);
                if(trends.end() != i) {
                        auto tau_trends = i->second;
                                auto j = this->supply.find(k);
                                        if(supply.end() == j) {
                                                if(get<0>(tau_trends) == t) {
                                                        result_.emplace(k, nav_ * stock_alloc * portfolio_alloc / 
                                                               (quoted_price_ * variable_));}
                                                                        else {result_.emplace(k, cashflow_ * stock_alloc * portfolio_alloc /
                                                                               quoted_price_ * variable_);
                }
            }
                        
            else {  auto supply_long_ = double(std::get<0>(j->second));
                auto supply_short_ = double(std::get<1>(j->second));
                        if (get<0>(tau_trends) == t) {result_.emplace(k, (nav_ * stock_alloc * portfolio_alloc /
                                (quoted_price_ * variable_)) - (supply_long_ - supply_short_) * (quoted_price_ * variable_));}
                                        else {result_.emplace(k, cashflow_ * stock_alloc * portfolio_alloc / (quoted_price_ *variable_));
                }
            }
        }
    }


    return result_;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Trend Agent Constructor

TrendAgent::TrendAgent(const identity<fund> &i, const jurisdiction &j, size_t window)
        : agent(i)
                , owner<cash>(i)
                        , owner<stock>(i)
                                , fund(i, j)
                                        , window(window)
{

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Invest Function - Similar to act() function/

time_point
TrendAgent::invest(shared_ptr<quote_message> message, time_interval interval, seed_seq &seed)
{
    map<identity<property>, tuple<time_point,int,int,double,double>> trends_;
        auto t = interval.lower;
                auto cashflow = (t/t)*price(0,USD);//TODO describe cashflow as some function of time t
                        auto nav_ = make_tuple(t,cashflow,net_asset_value(interval));
                                time_point tau = reb_period * floor(t/reb_period); //tau is the previous portfolio rebalancing day




    if(tau<1){tau = 1;}
        time_duration window_ = window;
                size_t index_ = 0;

        
        
    if(message->received < interval.lower){return interval.lower;}
        if(get<2>(nav_).value <= 0){return interval.upper;}
                if(this->target_net_asset_value.has_value() && double(target_net_asset_value.value()) <= 1.){
                        return interval.upper;}



    //LOG(trace) << describe() << " " << identifier << " inventory " <<  inventory << std::endl;
    for(auto [stock_, quote_] : message->proposed){
        auto i = historic_prices.find(stock_->identifier);
                if(i == historic_prices.end()) {
                        i = historic_prices.emplace(stock_->identifier, std::map<time_point, price>()).first;
        }


            
        if(interval.lower > window_ + 1){
            auto t_j = i->second.begin();
                while(t_j->first < interval.lower - window_ - 1){
                        i->second.erase(t_j);
                                t_j = i->second.begin();
            }
        }

        i->second[interval.lower] = std::get<price>(quote_.type);
                std::map<time_point, price> &prices_ = i->second;
                        double trend_ = 0.;
                                int window_size_MA1 = 1;
                                        int window_size_MA2 = 3;
                                                double sumMA1=0, sumMA2=0;


        for(auto const &[k,v]:i->second){
            if(int(k) > (int(interval.lower) -(window_size_MA2 +1))){
                if(int(k) > (int(interval.lower) -(window_size_MA1 +1))){
                        sumMA1 = sumMA1 + double(v);}
                                sumMA2 = sumMA2 + double(v);
            }

        }

            
        if(!prices_.empty() && prices_.rbegin()->first > window_){
            if(prices_.size() > this->window){
            }
        }

        trends_.emplace(stock_->identifier, make_tuple(tau, window_size_MA1,window_size_MA2,sumMA1,sumMA2));
    }


time_point a = interval.upper;
    std::cout << "sending TTest" << std::endl;
        auto message_ = this->template create_message<TTest>(
            message->sender, interval.lower, (*this), message->sender,
                interval.lower, a, nav_, trends_);



    message_->agression = this->aggression;
    message_->leverage = this->maximum_leverage;



    for(auto [p,q]: inventory){
        auto cast_ = std::dynamic_pointer_cast<stock>(p);
                if(cast_){if(0 == q.amount) {continue;}
                        message_->supply.emplace(p->identifier, std::make_tuple(q, quantity(0)));
                                }else{auto cast2_ = std::dynamic_pointer_cast<securities_lending_contract>(p);
                                        if(!cast2_ || 0 == q.amount){continue;}
                                                if(message_->supply.end() != message_->supply.find(cast2_->security)){
                                                        std::get<1>( message_->supply.find(cast2_->security)->second ) = q;
                                                                }else{ message_->supply.emplace(cast2_->security, std::make_tuple(quantity(0), q));
            }
        }
    }

    return interval.lower;
}


