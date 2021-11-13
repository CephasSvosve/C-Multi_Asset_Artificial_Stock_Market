//
// Created by Cephas Svosve on 11/11/2021.
//

#ifndef UNTITLED21_INDEXTRACKER_H
#define UNTITLED21_INDEXTRACKER_H

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <esl/economics/markets/walras/differentiable_order_message.hpp>
#include <esl/economics/interest_rate.hpp>
#include <esl/economics/currencies.hpp>
#include <esl/economics/price.hpp>
#include "fund1.h"
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
struct Index
        : public differentiable_order_message
{
public:

    //params
    std::map<identity<law::property>, std::tuple<time_point, double>> market_cap;
          std::tuple<price, price> net_asset_value;
                time_interval interval;
                    double agression;
                          double leverage;


    //constructor
    Index(const identity<agent> &sender
            , const identity<agent> &recipient
                    , simulation::time_point sent     = simulation::time_point()
                          , simulation::time_point received = simulation::time_point()
                                , std::tuple<price, price> nav = std::make_tuple(price(0, USD),price(0, USD))
                                      , std::map<identity<law::property>, std::tuple<time_point, double>> market_cap = {}
                                            ,time_interval interval= {})
            
            :differentiable_order_message(sender, recipient, sent, received)
                    ,market_cap(market_cap)
                        ,net_asset_value(nav)
                                ,interval(interval){}



    std::map<identity<law::property>, variable>
            excess_demand(
                    const std::map<identity<law::property>
                    ,std::tuple<economics::markets::quote, variable>> &quotes) const override;

    template<class archive_t>
      void serialize(archive_t &archive, const unsigned int version){
        (void)version;
          archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(differentiable_order_message);
    }
};



class IndexAgent
        : public fund
{
public:
    explicit IndexAgent( const esl::identity<fund> &i
            ,const jurisdiction &j = esl::law::jurisdictions::US);


    esl::law::property_map<std::map<time_point, price>> historic_prices;
        time_point reb_period=2;

              time_point invest(std::shared_ptr<quote_message> message,
                      time_interval interval, std::seed_seq &seed) override;


                    [[nodiscard]] std::string describe() const override{
                            return "Index trader";
    }

};

#endif //UNTITLED21_INDEXTRACKER_H
