//
// Created by Cephas Svosve on 27/10/2021.
//

#ifndef UNTITLED21_GROWTHINVESTOR_H
#define UNTITLED21_GROWTHINVESTOR_H

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <esl/economics/markets/walras/differentiable_order_message.hpp>
#include <esl/economics/interest_rate.hpp>
#include <esl/economics/currencies.hpp>
#include <esl/economics/price.hpp>
#include "fund1.h"
#include "marketData.h"
#include <iostream>



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
struct GI
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
    GI(const identity<agent> &sender
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
                    std::tuple<economics::markets::quote, variable>>
                        &quotes) const;


    template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version){
            (void)version;
                archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(differentiable_order_message);
    }
};



class GIAgent
        : public fund
{
public:
    explicit GIAgent( const esl::identity<fund> &i
            ,const jurisdiction &j = esl::law::jurisdictions::US);

    std::map<esl::identity<property>, std::tuple<time_point, double>> earnings_;
    time_point reb_period=1;

    time_point invest(std::shared_ptr<quote_message> message,
                      time_interval interval, std::seed_seq &seed) override;

    auto process_dividends_(std::shared_ptr<esl::economics::finance::dividend_announcement_message> message,
                            time_interval interval, std::seed_seq &seed);


    [[nodiscard]] std::string describe() const override{
        return "Growth_Investor";
    }

};

#endif //UNTITLED21_GROWTHINVESTOR_H
