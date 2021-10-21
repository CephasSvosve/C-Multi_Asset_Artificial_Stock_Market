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
#include <tuple>
#include <utility>
#include "market_data.h"
#include "VTtest.h"
#include "Newtrader.h"
#include "TrendTest.h"
#include "NoiseTest.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE walrasian_market


#include <esl/economics/markets/walras/quote_message.hpp>
#include <esl/economics/markets/walras/price_setter.hpp>
#include <boost/test/included/unit_test.hpp>
#include <esl/computation/environment.hpp>
#include <esl/law/jurisdictions.hpp>
#include <esl/economics/company.hpp>
#include <esl/simulation/model.hpp>
#include <esl/data/log.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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



BOOST_AUTO_TEST_SUITE(ESL)

///
/// \brief  Tests that a market quotes the right number of prices for property,
///         and that these are delivered to participants.


    BOOST_AUTO_TEST_CASE(walras_market_quote_multiple_assets)
    {
        computation::environment environment_;
        simulation::model model_(environment_, simulation::parameter::parametrization(0, 0, 10));

        vector<std::tuple<std::shared_ptr<company>, share_class>> share_classes_;

        typedef std::tuple<identity<company>, share_class> key_t;
        map<key_t, identity<law::property>> stocks_;

        // set the number of traded assets and create a container
        law::property_map<quote> traded_assets_;
        size_t assets = 2;

        std::vector<std::shared_ptr<property>> properties_;

        // create the assets, by creating the companies that issued the shares first
        for(size_t a = 0; a < assets; ++a) {
            auto company_identifier_ = model_.template create_identifier<company>();

            // create a US company
            auto company_ = std::make_shared<company>(company_identifier_, law::jurisdictions::US);

            // create a share issue for the company for several shares
            auto main_issue_ = share_class();
            company_->shares_outstanding[main_issue_] = 1'000;

            // create stocks from the shares
            for(const auto &[share_, quantity] : company_->shares_outstanding) {
                (void) quantity;
                auto stock_ = std::make_shared<stock>(*company_, share_);

                // suggest to the market that $1.00 is the starting price
                traded_assets_.insert({stock_, quote(price::approximate(1.00, currencies::USD))});
                share_classes_.emplace_back(make_tuple(company_, share_));


                // create a key to store these in associative datastructures
                auto key_ = std::make_tuple<identity<company>, share_class>(*company_, share_class(share_));
                stocks_.insert({key_, stock_->identifier});

                properties_.push_back(stock_);
            }
        }




        auto market_ = model_.template create<price_setter>();
        market_->traded_properties = traded_assets_;

        // create a collection with trading agents who are also shareholders
        vector<std::shared_ptr<TrendAgent>> participants_ =
                {
                        model_.template create<TrendAgent>(),
                        model_.template create<TrendAgent>()
                };

        for(auto &p : participants_){
            std::cout << "TrendAgent" << p->identifier << std::endl;
            // add the trading agents to the market participant list
            market_->participants.insert(*p);

            // notify agents about the stock and key
            for(const auto &[k, v]: stocks_) {
                p->stocks.insert(std::make_pair(k, v));
            }

            // give agents an endowment of stocks
            for(const auto &[k, v]: traded_assets_) {
                (void)v;
                (*p).shareholder::owner<stock>::properties.insert(std::dynamic_pointer_cast<stock>(k),quantity(500));
            }

            std::cout << "created " << share_classes_.size() << " share classes" << std::endl;

            // notify company of new shareholders
            for(auto &share: share_classes_) {
                auto c = std::get<0>(share);
                map<share_class, std::uint64_t> holdings_;
                holdings_.emplace(std::get<1>(share), 500);
                c->shareholders.emplace(*p, holdings_);
            }
        }

        // during the first timestep, the market sends out quotes, and nothing else happens
        model_.step({0, 1});

        std::cout<<std::get<price>(market_->traded_properties.find(properties_[0])->second.type)<<std::endl;
        std::cout<<std::get<price>(market_->traded_properties.find(properties_[1])->second.type)<<std::endl;

        // we have the first market interaction: prices are formed by the market agent
        model_.step({1, 2});
        std::cout<<std::get<price>(market_->traded_properties.find(properties_[0])->second.type)<<std::endl;
        std::cout<<std::get<price>(market_->traded_properties.find(properties_[1])->second.type)<<std::endl;


        // the market runs again
        model_.step({2, 3});
        std::cout<<std::get<price>(market_->traded_properties.find(properties_[0])->second.type)<<std::endl;
        std::cout<<std::get<price>(market_->traded_properties.find(properties_[1])->second.type)<<std::endl;
        model_.step({3, 4});
        /*
         // expect: prices unchanged from initial price given above
         BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[0])->second.type) == price(100, currencies::USD));
         BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[1])->second.type) == price(100, currencies::USD));

         // we have the first market interaction: prices are formed by the market agent
         model_.step({1, 2});

         // prices are updated
         BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[0])->second.type) == price( 66, currencies::USD));
         BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[1])->second.type) == price(133, currencies::USD));

         // the agent does not know the new prices until after one time step
         BOOST_CHECK_EQUAL(participants_[0]->prices.size(), assets);
         BOOST_TEST(participants_[0]->prices.find(properties_[0])->second == price(100, currencies::USD));
         BOOST_TEST(participants_[0]->prices.find(properties_[1])->second == price(100, currencies::USD));
         //log(notice) << participants_[0]->prices << std::endl;

         // the market runs again
         model_.step({2, 3});
         // the agent now knows the previous prices from step [1, 2)
         //log(notice) << participants_[0]->prices << std::endl;
         BOOST_TEST(participants_[0]->prices.find(properties_[0])->second == price( 66, currencies::USD));
         BOOST_TEST(participants_[0]->prices.find(properties_[1])->second == price(133, currencies::USD));

         // the market prices are computed again
         BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[0])->second.type) == price( 66, currencies::USD));
         BOOST_TEST(std::get<price>(market_->traded_properties.find(properties_[1])->second.type) == price(133, currencies::USD));
     */
        }

BOOST_AUTO_TEST_SUITE_END()  // ESL