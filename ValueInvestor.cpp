/// \file   Value.cpp
///
/// \brief
///
/// \authors    cephas and maarten
/// \date       2021-10-30
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

#include "ValueInvestor.h"

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
VT::excess_demand(
        const std::map<identity<law::property>,
                tuple<markets::quote, esl::variable>> &quotes)const{

    
    
//Params definition
        map<esl::identity<property>, esl::variable> result_;
            time_point t = get<0>(net_asset_value);
                double cashflow_ = double(get<1>(net_asset_value)),
                    nav_ = double(get<2>(net_asset_value)),
                        Beta = log10(9.0),//dummy threshold TODO: link this variable with actual earnings
                             portfolio_alloc = 0.6,// wealth allocated to stock potfolio
                                sum_of_signals,
                                    phi;//trading signal for stock i

    
  

//compute a sum of exponents of signals, see McFadden choice function...........................(1)
    for(auto &[k, v] : quotes){
        const auto &[quote_, variable_] = v;
            const auto quoted_price_ = double(get<price>(quote_.type));
                phi = -(log10(double(quoted_price_)/get<1>(earnings.find(k)->second)) - Beta);//stock signal
                    sum_of_signals = sum_of_signals + exp(pow(phi,2));}



    
//compute the ratio of each stock's exponent signal relative to sum described in (1)
    for(auto &[k, v] : quotes){
        const auto &[quote_, variable_] = v;
            const auto quoted_price_ = double(get<price>(quote_.type));
                phi = -(log10(double(quoted_price_)/get<1>(earnings.find(k)->second)) - Beta);
                    auto stock_alloc = sgn(phi) * exp(pow(phi,2)) /sum_of_signals;//stock wealth allocation

       
        
        
//investment in the subject stock
        auto i = earnings.find(k);
        if (earnings.end() != i) {
            auto tau_earnings = i->second;
            auto j = this->supply.find(k);
            if (supply.end() == j) {
                if (get<0>(tau_earnings) == t) {
                    result_.emplace(k, nav_ * stock_alloc * portfolio_alloc / (quoted_price_ * variable_));
                }
                else {
                    result_.emplace(k, cashflow_ * stock_alloc * portfolio_alloc / quoted_price_ * variable_);
                }
            }
            else {  auto supply_long_ = double(std::get<0>(j->second));
                    auto supply_short_ = double(std::get<1>(j->second));
                    if (get<0>(tau_earnings) == t) {result_.emplace(k, (nav_ * stock_alloc * portfolio_alloc /
                                  (quoted_price_ * variable_)) - (supply_long_ - supply_short_) * (quoted_price_ * variable_));
                }
                else {result_.emplace(k, cashflow_ * stock_alloc * portfolio_alloc / (quoted_price_ *
                                       variable_) +(supply_long_ - supply_short_) * (quoted_price_ * variable_));
                }
            }
        }
    }


    return result_;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Trend Agent Constructor

VTAgent::VTAgent(const identity<fund> &i, const jurisdiction &j)
        :agent(i)
            ,owner<cash>(i)
                ,owner<stock>(i)
                    ,fund(i, j)
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Invest Function - Similar to act() function/

time_point
VTAgent::invest(shared_ptr<quote_message> message, time_interval interval, seed_seq &seed)
{
    auto t = interval.lower;
        auto cashflow = (t/t)*price(0,USD);//TODO describe cashflow as some function of time t
            auto nav = make_tuple(t,cashflow,net_asset_value(interval));
                time_point tau = reb_period * floor(t/reb_period); //tau is the previous portfolio rebalancing day

    
    
    
    if(tau<1){tau = 1;} //manually match beginning tau to simulator's beginning time point [where tau would be 0 we set both values to 1]
    if(message->received < interval.lower){return interval.lower;}
    if(get<2>(nav).value <= 0){return interval.upper;}//nav is a tuple with net_asset value at index 2
    if(this->target_net_asset_value.has_value() && double(target_net_asset_value.value()) <= 1.){
        return interval.upper;}


    
    
//load earnings variable with current time point, tau and earnings at time tau
    for(auto [property_, quote_]: message->proposed) {
        auto price_ = std::get<price>(quote_.type);
              auto e = data_earnings.find(property_->identifier)->second[tau-1];
                    if(earnings_.end() != earnings_.find(property_->identifier)){
                        get<0>(earnings_.find(property_->identifier)->second) = tau;
                            get<1>(earnings_.find(property_->identifier)->second) = e;
                        }else{
                            earnings_.emplace(property_->identifier,make_tuple(tau,e));

                        }

    }


    
    
    std::cout << "sending GITest" << std::endl;
        auto message_ = this->template create_message<VT>(
            message->sender, interval.lower, (*this), message->sender,
                interval.lower, interval.lower,nav,earnings_);

    

    message_->agression = this->aggression;
    message_->leverage = this->maximum_leverage;




    for(auto [p,q]: inventory){
        auto cast_ = std::dynamic_pointer_cast<stock>(p);
            if(cast_){
                if(0 == q.amount){
                    continue;}
                        message_->supply.emplace(p->identifier, std::make_tuple(q, quantity(0)));
                            }else{
                                auto cast2_ = std::dynamic_pointer_cast<securities_lending_contract>(p);
                                    if(cast2_){
                                        if(0 == q.amount){
                                            continue;}
                                                if(message_->supply.end() != message_->supply.find(cast2_->security)){
                                                    std::get<1>( message_->supply.find(cast2_->security)->second ) = q;
                                                        }else{
                                                            message_->supply.emplace(cast2_->security, std::make_tuple(quantity(0), q));
                }
            }
        }
    }


    return interval.lower;
}

