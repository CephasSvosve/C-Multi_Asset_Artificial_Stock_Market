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
//#include "market_data.h"
#include "VTtest.h"
//#include "Newtrader.h"
#include "TrendTest.h"
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
simulation::model model_(environment_, simulation::parameter::parametrization(0, 0, 10));


int main(int c, char *a[]) {

//type definition for the stocks key variable
    typedef std::tuple<identity<company>, share_class> key_t;



//we create a method of adding tradeable assets into the market
void
append_Property(
        vector<std::tuple<std::shared_ptr<company>, share_class>> *share_classes
            ,map<key_t, identity<law::property>> *stocks
                ,law::property_map<quote> *traded_assets
                    ,std::vector<std::shared_ptr<property>> *properties
                        ,int shares_outstanding
                            ,double starting_price);



//we create asset parameters
    vector<std::tuple<std::shared_ptr<company>, share_class>> share_classes_;
        map<key_t, identity<law::property>> stocks_;
            law::property_map<quote> traded_assets_;
                std::vector<std::shared_ptr<property>> properties_;



//create the assets, by creating the companies that issued the shares first
append_Property(&share_classes_, &stocks_, &traded_assets_, &properties_, 2000, 200);
append_Property(&share_classes_, &stocks_, &traded_assets_, &properties_, 1000, 300);
append_Property(&share_classes_, &stocks_, &traded_assets_, &properties_, 1000, 400);


auto market_ = model_.template create<price_setter>();
market_->traded_properties = traded_assets_;

//create a collection with trading agents who are also shareholders
    vector<std::shared_ptr<TrendAgent>> participants_ =
            {
            model_.template create<TrendAgent>(),
            model_.template create<TrendAgent>()
            };


    auto cash_ = std::make_shared<cash>(USD);
    for (auto &p : participants_) {
        std::cout << "TrendAgent" << p->identifier << std::endl;
            // add the trading agents to the market participant list
                market_->participants.insert(*p);
                    // notify agents about the stock and key
                    for (const auto &[k, v]: stocks_) {
                        p->stocks.insert(std::make_pair(k, v));
                            // give initial stock valuation to agent
                            p->lookup_.mark_to_market.emplace(
                                    v, price::approximate(200, currencies::USD));
                    }

                    // give agents an endowment of stocks
                    for (const auto &[k, v]: traded_assets_) {
                        (void) v;
                            (*p).shareholder::owner<stock>::properties.insert(
                                    std::dynamic_pointer_cast<stock>(k), quantity(500));
                    }


                    (*p).shareholder::owner<cash>::take(
                            cash_, cash_->amount(1'000'000));


                    std::cout << "created " << share_classes_.size()
                        << " share classes" << std::endl;


                    // notify company of new shareholders
                    for (auto &share: share_classes_) {
                         auto c = std::get<0>(share);
                            map<share_class, std::uint64_t> holdings_;
                                holdings_.emplace(std::get<1>(share), 500);
                                    c->shareholders.emplace(*p, holdings_);
                    }
    }



    // during the first timestep, the market sends out quotes, and nothing else happens
    model_.step({0, 1});
    std::cout << std::get<price>(market_->traded_properties.find(properties_[0])->second.type) << std::endl;
    std::cout << std::get<price>(market_->traded_properties.find(properties_[1])->second.type) << std::endl;
    std::cout << std::get<price>(market_->traded_properties.find(properties_[2])->second.type) << std::endl;

    // we have the first market interaction: prices are formed by the market agent
    model_.step({1, 2});
    std::cout << std::get<price>(market_->traded_properties.find(properties_[0])->second.type) << std::endl;
    std::cout << std::get<price>(market_->traded_properties.find(properties_[1])->second.type) << std::endl;
    std::cout << std::get<price>(market_->traded_properties.find(properties_[2])->second.type) << std::endl;

        // the market runs again
    model_.step({2, 3});
    std::cout << std::get<price>(market_->traded_properties.find(properties_[0])->second.type) << std::endl;
    std::cout << std::get<price>(market_->traded_properties.find(properties_[1])->second.type) << std::endl;
    std::cout << std::get<price>(market_->traded_properties.find(properties_[2])->second.type) << std::endl;
    model_.step({3, 4});

}



#endif //UNTITLED21_MAIN_H
