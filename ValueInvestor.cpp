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
//sign extraction function

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Excess Demand Function

map<identity<law::property>, esl::variable>
VT::excess_demand(
        const std::map<identity<law::property>,
                tuple<markets::quote, esl::variable>> &quotes)const{


    //print statement to test if clearer is reaching this point
    std::cout<< "trading 0" <<std::endl;
    map<esl::identity<property>, esl::variable> result_;
             double nav_ = double(net_asset_value),
                Beta = log10(9.0),//dummy threshold TODO: link this variable with actual earnings
                    sum_of_signals,
                        phi;//signal


    for(auto &[k, v] : quotes){
            const auto &[quote_, variable_] = v;
                const auto quoted_price_ = double(get<price>(quote_.type));
                    phi = -(log10(double(quoted_price_)/earnings.find(k)->second) - Beta);
                        sum_of_signals = sum_of_signals + exp(pow(phi,2));}




    for(auto &[k, v] : quotes){
        const auto &[quote_, variable_] = v;
            const auto quoted_price_ = double(get<price>(quote_.type));
                phi = -(log10(double(quoted_price_)/earnings.find(k)->second) - Beta);
                    auto scale_ = sgn(phi) * exp(pow(phi,2)) / sum_of_signals;
                            auto i = earnings.find(k);




        if(earnings.end() != i){
            auto value_ = double(i->second);
                auto j = this->supply.find(k);
                    if(supply.end() == j){
                        result_.emplace(k,  nav_*scale_/(quoted_price_ * variable_));
                            }else{//print statement to test if clearer is reaching this point
                                std::cout<< nav_<<"this is fund NAV" <<net_asset_value<<std::endl;
                                    std::cout<< "trading 3" <<std::endl;
                                        auto supply_long_ = double(std::get<0>(j->second));
                                            auto supply_short_ = double(std::get<1>(j->second));
                                                result_.emplace(k,  (double(net_asset_value)*scale_/(quoted_price_ * variable_))-
                                                    - (supply_long_ - supply_short_) * (quoted_price_ * variable_));
            }
        }
    }
    std::cout<< "result"<<result_ <<std::endl;

    return result_;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Trend Agent Constructor

VTAgent::VTAgent(const identity<fund> &i, const jurisdiction &j, size_t window)
            :agent(i)
                ,owner<cash>(i)
                    ,owner<stock>(i)
                        ,fund(i, j)
                            ,window(window)
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Invest Function - Similar to act() function/

time_point
VTAgent::invest(shared_ptr<quote_message> message, time_interval interval, seed_seq &seed)
{
    vector<tuple<identity<property>, double, size_t, double>> trends_;
        auto nav_ = net_asset_value(interval);
            std::cout<<"NAV interval"<<nav_<<std::endl;
                time_duration window_ = window;
                    size_t index_ = 0;


    if(message->received < interval.lower){return interval.lower;}
    if(nav_.value <= 0){return interval.upper;}
    if(this->target_net_asset_value.has_value() && double(target_net_asset_value.value()) <= 1.){
        return interval.upper;}


    //LOG(trace) << describe() << " " << identifier << " inventory " <<  inventory << std::endl;
    std::map<identity<property>, double> earnings_;
    for(auto [property_, quote_]: message->proposed) {

        auto price_ = std::get<price>(quote_.type);


        if(earnings_.end() != earnings_.find(property_->identifier)){
            earnings_.find(property_->identifier)->second = 12.0;//dummy earnings TODO link with actual earnings
        }else{
            earnings_.emplace(property_->identifier, 12.0);//dummy earnings TODO link with actual earnings

        }

    }
    //std::cout<<"sharedetails"<< valuations_ <<std::endl;
    std::cout << "sending VTTest" << std::endl;
    auto message_ = this->template create_message<VT>(
            message->sender, interval.lower, (*this), message->sender,
            interval.lower, interval.lower,nav_, earnings_);



    message_->agression = this->aggression;
    message_->leverage = this->maximum_leverage;


    for(auto [p,q]: inventory){
        auto cast_ = std::dynamic_pointer_cast<stock>(p);
        if(cast_){
            if(0 == q.amount){
                continue;
            }
            message_->supply.emplace(p->identifier, std::make_tuple(q, quantity(0)));
        }else{
            auto cast2_ = std::dynamic_pointer_cast<securities_lending_contract>(p);
            if(cast2_){
                if(0 == q.amount){
                    continue;
                }
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


