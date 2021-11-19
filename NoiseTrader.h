/// \file  Noise_Trader.hpp
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
#ifndef UNTITLED21_NOISETRADER_H
#define UNTITLED21_NOISETRADER_H


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <esl/economics/markets/walras/differentiable_order_message.hpp>
#include <esl/economics/interest_rate.hpp>
#include <esl/economics/currencies.hpp>
#include <esl/economics/price.hpp>
#include <boost/random.hpp>
#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include <chrono>
#include "fund1.h"
#include "marketData.h"



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
struct NT
        : public differentiable_order_message
{
public:

    //params
    std::map<esl::identity<property>, std::tuple<time_point, double>> earnings;
        std::tuple<time_point, price,price>  net_asset_value;
            time_point reb_period_;
                double agression;
                    double leverage;


    //constructor
        NT(const identity<agent> &sender
            , const identity<agent> &recipient
                , simulation::time_point sent     = simulation::time_point()
                    , simulation::time_point received = simulation::time_point()
                        , std::tuple<time_point, price,price> nav = {0,price(0, USD),price(0, USD)}
                            , std::map<esl::identity<property>, std::tuple<time_point, double>> earnings = {}
                                ,time_point reb_period_ = {})
                                    :differentiable_order_message(sender, recipient, sent, received)
                                        ,earnings(earnings)
                                            ,net_asset_value(nav)
                                                ,reb_period_(reb_period_){}



    std::map<identity<law::property>, variable>
        excess_demand(
            const std::map<identity<law::property>,
                    std::tuple<economics::markets::quote, variable>>&quotes) const;


    template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version){
            (void)version;
                archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(differentiable_order_message);
    }
};



class NTAgent
        : public fund
{
public:
    explicit NTAgent( const esl::identity<fund> &i
            ,const jurisdiction &j = esl::law::jurisdictions::US);

                time_point invest(std::shared_ptr<quote_message> message,
                      time_interval interval, std::seed_seq &seed) override;

                    std::map<esl::identity<property>, std::tuple<time_point, double>> earnings_;
                        time_point reb_period=1;

                            MatrixXd generateWhiteNoise(int rows, int columns);
                                double lateralcorrcoef(VectorXd a);


                                    auto process_dividends_(std::shared_ptr<esl::economics::finance::dividend_announcement_message> message,
                                        time_interval interval, std::seed_seq &seed);


    [[nodiscard]] std::string describe() const override{
        return "Noise_Trader";
    }

};

#endif //UNTITLED21_NOISETRADER_H
