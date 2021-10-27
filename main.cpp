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
#include "main.h"

void
append_Property(
        vector<std::tuple<std::shared_ptr<company>, share_class>> *share_classes
            ,map<std::tuple<identity<company>, share_class>, identity<law::property>> *stocks
                ,law::property_map<quote> *traded_assets
                    ,std::vector<std::shared_ptr<property>> *properties
                        ,int shares_outstanding
                            ,double starting_price){


    auto company_identifier_ = model_.template create_identifier<company>();
        // create a US company
        auto company_ = std::make_shared<company>(company_identifier_, law::jurisdictions::US);
        
            // create a share issue for the company for several shares
            auto main_issue_ = share_class();
            company_->shares_outstanding[main_issue_] = shares_outstanding;

                // create stocks from the shares
                for (const auto &[share_, quantity] : company_->shares_outstanding) {
                    (void) quantity;
                        auto stock_ = std::make_shared<stock>(*company_, share_);

                    // suggest to the market that $1.00 is the starting price
                    (*traded_assets).insert({stock_, quote(
                            price::approximate(starting_price, currencies::USD))});
                                (*share_classes).emplace_back(make_tuple(company_, share_));


                         // create a key to store these in associative datastructures
                        auto key_ = std::make_tuple<identity<company>, share_class>
                                (*company_, share_class(share_));
                                (*stocks).insert({key_, stock_->identifier});
                                (*properties).push_back(stock_);

    }
}

