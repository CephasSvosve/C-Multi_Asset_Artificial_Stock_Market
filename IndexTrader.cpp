//
// Created by Cephas Svosve on 11/11/2021.
//
//
// Created by Cephas Svosve on 27/10/2021.
//

#include "IndexTracker.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
Index::excess_demand(
        const std::map<identity<law::property>,
                tuple<markets::quote, esl::variable>> &quotes)const{


//Params definition
    map<esl::identity<property>, esl::variable> result_;
        time_point tau;//previous rebalancing date
            price price_tau;//price at time tau
                double shares_outstanding;
                    double market_cap;
                        std::map<identity<property>, double> market_distribution;
                            vector<double> list_marketCap = {};
                                time_point t = get<0>(net_asset_value);
                                    double cashflow_ = double(get<1>(net_asset_value)),
                                        nav_ = double(get<2>(net_asset_value)),
                                            portfolio_alloc = 1,
                                                sum_of_signals,
                                                    phi;//signal

    
    
//combute market capital using prices at tau for non-rebalancing date and current prices for rebalancing dates
    for(auto &[k, v] : quotes){
        const auto &[quote_, variable_] = v;
            const auto quoted_price_ = double(get<price>(quote_.type));
                tau =get<0>(market_cap_data.find(k)->second);
                    price_tau = get<1>(market_cap_data.find(k)->second);
                        shares_outstanding = get<2>(market_cap_data.find(k)->second);
                            if(tau == t){
                                market_cap = double(shares_outstanding * double(quoted_price_));
                                    }else{ market_cap = double(shares_outstanding * price_tau);}
                                        list_marketCap.push_back( market_cap );
                                            market_distribution.emplace( k, market_cap);}

    
    

//select top two market_cap for a top two stock index
    std::sort(list_marketCap.begin(), list_marketCap.end(), std::greater<>());
        for(auto &[k, v] : quotes) {
            const auto &[quote_, variable_] = v;
                auto market_cap = market_distribution.find(k)->second;
                    phi = double(market_cap);
                        if (phi >= list_marketCap[1]) {
                            sum_of_signals = sum_of_signals + phi;
                                }else{sum_of_signals = sum_of_signals;}
    }



//compute the excess demand
    for(auto &[k, v] : quotes) {
        const auto &[quote_, variable_] = v;
            const auto quoted_price_ = double(get<price>(quote_.type));
                auto market_cap = market_distribution.find(k)->second;
                    double  stock_alloc;
                        phi = double(market_cap);
                            if (phi >= list_marketCap[1]) {
                                stock_alloc = phi / sum_of_signals;}
                                    else{stock_alloc = 0;
        }


        auto i = market_distribution.find(k);
            if (market_distribution.end() != i) {
                auto tau_market_cap = i->second;
                    auto j = this->supply.find(k);
                        if (supply.end() == j) {
                            if (tau == t) {result_.emplace(k, nav_ * stock_alloc * portfolio_alloc/ (quoted_price_ * variable_));}
                                    else{result_.emplace(k, cashflow_ * stock_alloc * portfolio_alloc / (quoted_price_ * variable_));}}
                                        else {  auto supply_long_ = double(std::get<0>(j->second));
                                            auto supply_short_ = double(std::get<1>(j->second));
                                                if (tau == t) {
                                                    result_.emplace(k, (nav_ * stock_alloc * portfolio_alloc/ (quoted_price_ * variable_)) -
                                                        (supply_long_ - supply_short_) * (quoted_price_ * variable_));}
                                                            else {result_.emplace(k, cashflow_ * stock_alloc * portfolio_alloc/ (quoted_price_ *variable_));
                }
            }
        }
    }
    return result_;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Index Agent Constructor

IndexAgent::IndexAgent(const identity<fund> &i, const jurisdiction &j)
        :agent(i)
            ,owner<cash>(i)
                ,owner<stock>(i)
                    ,fund(i, j)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Invest Function - Similar to act() function/

time_point
IndexAgent::invest(shared_ptr<quote_message> message, time_interval interval, seed_seq &seed)
{


//Param definition
    auto t = interval.lower;
        auto cashflow = (t/t)*price(0,USD);//TODO describe cashflow as some function of time t
            auto nav_ = make_tuple(t,cashflow,net_asset_value(interval));
                time_point tau = reb_period * floor(t/reb_period); //tau is the previous portfolio rebalancing day




    if(tau<1){tau = 1;} //simulator gives first price at time 1 but tau begins at 0
        if(message->received < interval.lower){return interval.lower;}
            if(get<2>(nav_).value <= 0){return interval.upper;}
                if(this->target_net_asset_value.has_value() && double(target_net_asset_value.value()) <= 1.){
                    return interval.upper;}





    std::map<identity<property>, tuple<time_point, price, double>> market_cap_data;
        for(auto [property_, quote_]: message->proposed) {
            price price_;
                auto i = historic_prices.find(property_->identifier);
                    if(i == historic_prices.end()) {
                        i = historic_prices.emplace(property_->identifier, std::map<time_point, price>()).first;}
        



//manage historic prices array
        if(interval.lower > reb_period + 1){
            auto t_j = i->second.begin();
                while(t_j->first < interval.lower - reb_period - 1){
                    i->second.erase(t_j);
                        t_j = i->second.begin();
            }
        }

            
            
        i->second[interval.lower] = std::get<price>(quote_.type);
            std::map<time_point, price> &prices_ = i->second;
                price_= prices_[tau];
        

        auto shares_outstanding = data_shares_outstanding.find(property_->identifier)->second;
            if(market_cap_data.end() != market_cap_data.find(property_->identifier)){
                market_cap_data.find(property_->identifier)->second = make_tuple(tau,price_,shares_outstanding);
        }
                    else{market_cap_data.emplace(property_->identifier, make_tuple(tau,price_,shares_outstanding));
        }
    }



    std::cout << "sending IndexTest" << std::endl;
        auto message_ = this->template create_message<Index>(
            message->sender, interval.lower, (*this), message->sender,
                interval.lower, interval.lower,nav_, market_cap_data);





    message_->agression = this->aggression;
    message_->leverage = this->maximum_leverage;





    for(auto [p,q]: inventory){
        auto cast_ = std::dynamic_pointer_cast<stock>(p);
            if(cast_){if(0 == q.amount){continue;}
                 message_->supply.emplace(p->identifier, std::make_tuple(q, quantity(0)));
                        }else{auto cast2_ = std::dynamic_pointer_cast<securities_lending_contract>(p);
                            if(cast2_){if(0 == q.amount){continue;
                                }if(message_->supply.end() != message_->supply.find(cast2_->security)){
                                    std::get<1>( message_->supply.find(cast2_->security)->second ) = q;
                                        }else{message_->supply.emplace(cast2_->security, std::make_tuple(quantity(0), q));
                }
            }
        }
    }

    return interval.lower;
}


