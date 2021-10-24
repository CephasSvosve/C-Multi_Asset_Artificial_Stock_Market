/// \file   momentum_investor.hpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-02-13
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
#ifndef ME_momentum_HPP
#define ME_momentum_HPP

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <esl/economics/markets/walras/differentiable_order_message.hpp>
#include <esl/economics/interest_rate.hpp>
#include <esl/economics/currencies.hpp>
#include <esl/economics/price.hpp>
#include "fund.hpp"


namespace esl::economics::markets::walras {
    struct quote_message;
}

using esl::economics::markets::walras::differentiable_order_message;
using esl::economics::markets::walras::quote_message;
using esl::economics::nominal_interest_rate;
using esl::simulation::time_interval;
using esl::simulation::time_point;
using esl::economics::price;
using esl::economics::currencies::USD;
using namespace esl;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct TTest
        : public differentiable_order_message
{
public:

    //params
    std::map<identity<law::property>, double> valuations;
        price net_asset_value;
            double agression;
                double leverage;

    //constructor
    TTest(const identity<agent> &sender
            , const identity<agent> &recipient
                , simulation::time_point sent     = simulation::time_point()
                    , simulation::time_point received = simulation::time_point()
                        , price nav = price(0, USD)
                            , std::map<identity<law::property>, double> valuations = {})
                                : differentiable_order_message(sender, recipient, sent, received)
                                    , valuations(valuations)
                                        , net_asset_value(nav){}


    std::map<identity<law::property>, variable>
        excess_demand(
            const std::map<identity<law::property>,
                    std::tuple<economics::markets::quote, variable>>
                        &quotes) const;

    template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version){
            (void)version;
                archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(differentiable_order_message);
            }
};



class TrendAgent
        : public fund
{
public:
    explicit TrendAgent( const esl::identity<fund> &i
                ,const jurisdiction &j = esl::law::jurisdictions::US
                    ,size_t window = 21);
                    
 /// theta - 1
    size_t window;
        esl::law::property_map<std::map<time_point, price>> historic_prices;
            time_point invest(std::shared_ptr<quote_message> message,
                time_interval interval, std::seed_seq &seed) override;
                
                
                    [[nodiscard]] std::string describe() const override{
                        return "trend_follower trader";
                            }

};

#endif  // ME_momentum_HPP
