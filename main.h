//
// Created by Cephas Svosve on 27/10/2021.
//
/// \file   test_walrasian_market.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2019-10-08
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
///             Oxford Martin School, University of Oxford
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
#ifndef UNTITLED21_MAIN_H
#define UNTITLED21_MAIN_H




#include <tuple>
#include <utility>
#include <string>
#include "marketData.h"
//#include "market_data.h"
#include "VTtest.h"
#include "GrowthInvestor.h"
#include "NoiseTrader.h"
#include "IndexTracker.h"
//#include "Newtrader.h"
#include "TrendTest.h"
#include "Wiener.h"
#include <fstream>
//#include "fund1.h"
//#include "NoiseTest.h"



#include <esl/economics/markets/walras/quote_message.hpp>
#include <esl/economics/markets/walras/price_setter.hpp>
#include <esl/computation/environment.hpp>
#include <esl/law/jurisdictions.hpp>
#include <esl/economics/company.hpp>
#include <esl/simulation/model.hpp>
#include <esl/data/log.hpp>


using namespace esl::economics::markets::walras;
using namespace esl::economics::markets;
using namespace esl::economics::finance;
using namespace esl::economics;
using esl::economics::company;
using namespace esl::law;
using namespace esl;
using std::vector;
using std::map;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//define market clearer


computation::environment environment_;
simulation::model model_(environment_, simulation::parameter::parametrization(0, 0, 1000));


int main(int c, char *a[]) {

//type definition for the stocks key variable
    typedef std::tuple<identity<company>, share_class> key_t;


//we create a method for associating assets with their fundamental processes
void fundamental(identity<esl::law::property> property_, double v,
                     map<identity<esl::law::property>, double> *k);

    
    
    
//we create a method for adding tradeable assets into the market
    void
    append_Property(
            vector<std::tuple<std::shared_ptr<company>, share_class>> *share_classes,
            map<key_t, identity<law::property>> *stocks, law::property_map<quote> *traded_assets,
            std::vector<std::shared_ptr<property>> *properties, double shares_outstanding, double starting_price);

    
   
    
//we create a method for allocating wealth to traders
    void
    wealth_allocation(std::shared_ptr<fund> &p, law::property_map<quote> traded_assets_,
                      double _cash_, double stocks_);


    
//we create asset parameters
    vector<std::tuple<std::shared_ptr<company>, share_class>> share_classes_;
    map<key_t, identity<law::property>> stocks_;
    law::property_map<quote> traded_assets_;
    std::vector<std::shared_ptr<property>> properties_;


//create the assets, by creating the companies that issued the shares first
    append_Property(&share_classes_, &stocks_, &traded_assets_, &properties_, 1'600'000, 100.00);
    append_Property(&share_classes_, &stocks_, &traded_assets_, &properties_, 1'600'000, 100.00);
    append_Property(&share_classes_, &stocks_, &traded_assets_, &properties_, 1'600'000, 100.00);

    
    
    auto market_ = model_.template create<price_setter>();

    market_->traded_properties = traded_assets_;


//create a collection with trading agents who are also shareholders
    std::shared_ptr<fund>
            VT = model_.template create<VTAgent>(),
            GI = model_.template create<GIAgent>(),
            NT = model_.template create<NTAgent>(),
            TA = model_.template create<TrendAgent>(),
            IA = model_.template create<IndexAgent>();



    vector<std::shared_ptr<fund>> participants_ =
            {

                   GI,IA, VT,TA,NT
            };


    
    auto cash_ = std::make_shared<cash>(USD);
        for (auto &p : participants_) {
            // add the trading agents to the market participant list
                 market_->participants.insert(*p);
                    // notify agents about the stock and key
                        for (const auto &[k, v]: stocks_) {
                             p->stocks.insert(std::make_pair(k, v));
                                p->data_shares_outstanding.insert(std::make_pair(v, make_shares_outstanding.find(v)->second));

                            
                            
            auto earnings_process = generate("earnings")[v.digits[0]];
            auto div_process = generate("dividends")[v.digits[0]];
            auto fcf_process = generate("freecashflow")[v.digits[0]];

            //give agents access to current and historic fundamental information
                p->data_earnings.insert(std::make_pair(v, earnings_process));
                    p->data_dividends.insert(std::make_pair(v, div_process));
                        p->data_free_cash_flow.insert(std::make_pair(v, fcf_process));

            
            // give initial stock valuation to agent
                p->lookup_.mark_to_market.emplace(
                    v, price::approximate(100.00, currencies::USD));
        }
        // notify company of new shareholders
        for (auto &share: share_classes_) {
            auto c = std::get<0>(share);
                map<share_class, std::uint64_t> holdings_;
                    holdings_.emplace(std::get<1>(share), 500'00);
                        c->shareholders.emplace((*p), holdings_);
        }
    }


    wealth_allocation(VT, traded_assets_, 1'000'000'000, 500'00);
    wealth_allocation(GI, traded_assets_, 1'000'000'000, 500'00);
    wealth_allocation(NT, traded_assets_, 1'000'000'000, 500'00);
    wealth_allocation(TA, traded_assets_, 1'000'000'000, 500'00);
    wealth_allocation(IA, traded_assets_, 1'000'000'000, 500'00);


        std::cout << "created " << share_classes_.size()
              << " share classes" << std::endl;


    // during the first timestep, the market sends out quotes, and nothing else happens
        std::ofstream myfile;
            myfile.open ("MarketEcology.csv");
                for (time_point x = 0; x < 1000; x++) {
                    model_.step({x, x + 1});

                        double x1 = double(std::get<price>(market_->traded_properties.find(properties_[0])->second.type));
                        double x2 = double(std::get<price>(market_->traded_properties.find(properties_[1])->second.type));
                        double x3 = double(std::get<price>(market_->traded_properties.find(properties_[2])->second.type));

            myfile << x1 <<","<< x2 <<","<<x3<<std::endl;


    }
 // environment_.run(model_);

    myfile.close();

}
#endif //UNTITLED21_MAIN_H
