//
// Created by Cephas Svosve on 30/10/2021.
//

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
using namespace Eigen;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct NT
        : public differentiable_order_message
{
public:

    //params
    std::map<identity<law::property>, double> earnings;
        price net_asset_value;
            double agression;
                double leverage;




    //constructor
    NT(const identity<agent> &sender
            , const identity<agent> &recipient
                , simulation::time_point sent     = simulation::time_point()
                    , simulation::time_point received = simulation::time_point()
                        , price nav = price(0, USD)
                            , std::map<identity<law::property>, double> earnings = {})
                                :differentiable_order_message(sender, recipient, sent, received)
                                    ,earnings(earnings)
                                        ,net_asset_value(nav){}



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



class NTAgent
        : public fund
{
public:
    explicit NTAgent( const esl::identity<fund> &i
            ,const jurisdiction &j = esl::law::jurisdictions::US);


    time_point invest(std::shared_ptr<quote_message> message,
                      time_interval interval, std::seed_seq &seed) override;

    
    MatrixXd generateWhiteNoise(int rows, int columns);
        double lateralcorrcoef(VectorXd a);

        
    [[nodiscard]] std::string describe() const override{
        return "Noise trader";
    }

};

#endif //UNTITLED21_NOISETRADER_H
