//
// Created by Cephas Svosve on 21/8/2021.
//

#ifndef MARKET_ECOLOGY_VTTEST_H
#define MARKET_ECOLOGY_VTTEST_H



////////////////////////////////////////////////////////////////////////////////////////////////////
#include <esl/economics/markets/walras/differentiable_order_message.hpp>
#include <esl/economics/markets/walras/quote_message.hpp>
#include <esl/economics/markets/walras/price_setter.hpp>
#include <esl/economics/interest_rate.hpp>
#include <esl/economics/company.hpp>
#include <esl/law/jurisdictions.hpp>
#include <esl/simulation/model.hpp>
#include <esl/economics/price.hpp>
#include <boost/python.hpp>
#include <boost/random.hpp>
#include "market_data.h"
#include "fund.hpp"
#include "adept.h"
#include <math.h>

using namespace esl::economics::markets::walras;
using namespace esl;
using namespace esl::simulation;
using namespace esl::economics;
using namespace esl::economics::finance;


using esl::economics::nominal_interest_rate;
using esl::economics::price;
using esl::economics::markets::walras::quote_message;
using esl::simulation::time_point;
using esl::simulation::time_interval;
using esl::economics::nominal_interest_rate;
using std::map;
using std::vector;
////////////////////////////////////////////////////////////////////////////////////////////////////



struct VT
        : public differentiable_order_message
{
    price net_asset_value;
    map<identity<law::property>, double> allocation;
    map<identity<law::property>, economics::price> valuations;
    double agression;
    double leverage;


    VT(     price nav
            ,map<identity<law::property>, double> allocation
    , const quote_message &q = quote_message())
    : differentiable_order_message(q.sender, q.recipient)
    , net_asset_value(nav)
    , allocation(move(allocation))
    {

    }




    map<identity<law::property>, variable> excess_demand(
            const std::map<identity<law::property>,
            std::tuple<markets::quote, variable>> &quotes) const
    {
        std::map<identity<property>, variable> excess_demand_;

        for (auto &[k, v] : quotes)
        {
            //wealth allocated to asset k is given by NAV * Allocation
            auto scale_ = double(net_asset_value)  * allocation.find(k)->second;
            const auto &[quote_, variable_] = v;
            const auto quoted_price_ = double(std::get<price>(quote_.type));
            auto i = allocation.find(k);


            if (allocation.end() != i)
            {
                auto value_ = double(i->second);
                auto j = supply.find(k);

                // Scale by two,
                auto lambda_ = leverage * 2;
                // if we have a neutral position
                if (supply.end() == j)
                {
                    excess_demand_.emplace(k, scale_ * 0.6 * lambda_/(quoted_price_ * variable_) * (quoted_price_ * variable_));
                } else
                {
                    auto supply_long_ = double(std::get<0>(j->second));
                    auto supply_short_ = double(std::get<1>(j->second));
                    excess_demand_.emplace(k, scale_ * 0.6 * lambda_/(quoted_price_ * variable_)
                                              - (supply_long_ - supply_short_) * (quoted_price_ * variable_));
                }
            }
        }

        return excess_demand_;

    }
};





struct VTAgent
        : public fund
{
    VTAgent(const identity<shareholder> &i, const jurisdiction &j)
            : fund(i, j)
    {

    }


    time_point act(std::shared_ptr<quote_message> message
            , time_interval interval
            , std::seed_seq &seed)
    {
        (void) seed;

        //Cephas* replaced single message with inbox
        for(auto [k, message_]: inbox)
        {
            if (message_->received + 1 < interval.lower)
            {
                return interval.lower;
            }


            auto nav_ = net_asset_value(interval);
            if(nav_.value <= 0){
                return interval.upper;
            }
            LOG(trace) << describe() << " " << identifier << " inventory " <<  inventory << std::endl;

            if(target_net_asset_value.has_value() && double(target_net_asset_value.value()) <= 1.){
                return interval.upper;
            }



            switch (message_->type)
            {
                case quote_message::code:
                    auto quote_ = std::dynamic_pointer_cast<quote_message>(message_);
                    std::map<identity<property>, price> valuations_;

                    //Cephas* signal will hold exponents of log(value/Price)- Value to Price ratio
                    std::map<identity<property>, price> signal_;
                    double sum_Signal;
                    map<identity<property>, double> allocation;

                    //Cephas* changed property_ to k, quote_ to v and message->proposed to quote->proposed
                    for (auto[k, v]: quote_->proposed)
                    {
                        auto price_ = std::get<price>(v.type);

                        auto i = market_data.dividend_per_share.find(*k);
                        // it is assumed all assets are quoted with a price
                        auto fundamental_value_ = std::get<price>(v.type);
                        if (i != market_data.dividend_per_share.end())
                        {
                            // in this simplified model, we assume there is one dividend payment
                            // per period, and therefore the dividend yield is in USD/day
                            assert(i->second.valuation == price_.valuation);
                            auto payment_ = double(i->second);
                            auto shares_outstanding_ = market_data.shares_outstanding.find(k->identifier)->second;
                            auto dividend_rate_ = (payment_ / shares_outstanding_);

                            auto growth_ = 0.000'1;
                            auto lower_limit_ = 0.000'000'1;
                            growth_ = std::max(growth_, lower_limit_);
                            auto compounded_rate_return_ = 0.000'2; // FVI expects company to appreciate at this rate
                            auto finite_minimum_ = 0.000'1;
                            auto gordon_ =
                                    dividend_rate_ / (std::max(finite_minimum_, compounded_rate_return_ - growth_));
                            gordon_ = std::max(std::min(gordon_, 10'000.00), lower_limit_);
                            fundamental_value_.value = int64_t(gordon_ * 100);
                            output_signal->put(interval.lower, gordon_);
                        }

                        if (valuations_.end() != valuations_.find(k->identifier))
                        {
                            valuations_.find(k->identifier)->second.value = fundamental_value_.value;
                        } else {
                            valuations_.emplace(k->identifier, fundamental_value_);
                        }
                    }

                    //Cephas* load signal map with the respective exponent values of signals
                    for (auto[k, v]: quote_->proposed)
                    {
                        double price_ = double(std::get<price>(v.type));
                        double value_ = valuations_.find(*k)->second.value;

                        double phi;
                        phi = log10(value_) - log10(price_);
                        signal_.emplace(k->identifier, exp(aggression*phi));
                    }

                    //Cephas* we create a sum of signals which will be the denominator on determining allocations
                    for (auto[k, v]: quote_->proposed)
                    {
                        sum_Signal += signal_.find(*k)->second.value;
                    }

                    //Cephas* we load the allocations here
                    for (auto[k, v]: quote_->proposed)
                    {

                        auto proportion = double(signal_.find(*k)->second.value)/sum_Signal;
                        allocation.emplace(k->identifier, proportion);
                    }


                    LOG(trace) << "create_message<VTAgentf> with "  << valuations_.size() << " valuations: " << valuations_ << std::endl;

                    auto message_ = this->template create_message<VT>( quote_->sender
                            , interval.lower
                            , nav_
                            , allocation
                            , *quote_
                    );


                    message_->agression = this->aggression;
                    message_->leverage = this->maximum_leverage;


                    for(auto [p,q]: inventory)
                    {
                        auto cast_ = std::dynamic_pointer_cast<stock>(p);
                        if(cast_)
                        {
                            if(0 == q.amount)
                            {
                                continue;
                            }
                            message_->supply.emplace(p->identifier, std::make_tuple(q, quantity(0)));
                        }else
                        {
                            auto cast2_ = std::dynamic_pointer_cast<securities_lending_contract>(p);
                            if(cast2_)
                            {
                                if(0 == q.amount)
                                {
                                    continue;
                                }
                                if(message_->supply.end() != message_->supply.find(cast2_->security))
                                {
                                    std::get<1>( message_->supply.find(cast2_->security)->second ) = q;
                                }else
                                {
                                    message_->supply.emplace(cast2_->security, std::make_tuple(quantity(0), q));
                                }
                            }
                        }
                    }
            }
        }
        return interval.upper;
    }
    template<class archive_t>
    void serialize(archive_t &archive, const unsigned int version)
    {
        (void)version;
        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(differentiable_order_message);
    }
};

#endif //MARKET_ECOLOGY_VTTEST_H
