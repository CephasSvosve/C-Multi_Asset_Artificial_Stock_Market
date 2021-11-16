/// \file   test_walrasian_market.cpp
///
/// \brief
///
/// \authors    Cephas Svosve and Maarten P. Scholl
/// \date       2021-11-06
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void fundamental(identity<esl::law::property> property_, double v,
                 map<identity<esl::law::property>, double> *k){
                        (*k).insert(make_pair(property_,v));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
        append_Property(
                vector<std::tuple<std::shared_ptr<company>, share_class>> *share_classes
                    ,map<std::tuple<identity<company>, share_class>, identity<law::property>> *stocks
                        ,law::property_map<quote> *traded_assets
                            ,std::vector<std::shared_ptr<property>> *properties
                                    ,double shares_outstanding_
                                        ,double starting_price){




    auto company_identifier_ = model_.template create_identifier<company>();
        // create a US company
                auto company_ = std::make_shared<company>(company_identifier_, law::jurisdictions::US);




            // create a share issue for the company for several shares
                 auto main_issue_ = share_class();
                        company_->shares_outstanding[main_issue_] = shares_outstanding_;




                // create stocks from the shares
                        for (const auto &[share_, quantity] : company_->shares_outstanding) {
                                (void) quantity;
                                        auto stock_ = std::make_shared<stock>(*company_, share_);




                    // suggest to the market that $1.00 is the starting price
                        (*traded_assets).insert({stock_, quote(
                            price::approximate(starting_price, currencies::USD))});
                                (*share_classes).emplace_back(make_tuple(company_, share_));




                        auto key_ = std::make_tuple<identity<company>, share_class>
                                (*company_, share_class(share_));
                                        (*stocks).insert({key_, stock_->identifier});
                                                (*properties).push_back(stock_);
                                                        fundamental(stock_->identifier,quantity, &make_shares_outstanding);

                }
}



void
  wealth_allocation(std::shared_ptr<fund> &p
              ,law::property_map<quote> traded_assets_
                  ,double _cash_
                      ,double stocks_){



    auto cash_ = std::make_shared<cash>(USD);
            for(const auto &[k, v]: traded_assets_){
                (void) v;
                        (*p).shareholder::owner<stock>::take(
                            std::dynamic_pointer_cast<stock>(k), quantity(stocks_));
                                (*p).shareholder::owner<cash>::take(
                                    cash_, cash_->amount(_cash_));
        }
    }


