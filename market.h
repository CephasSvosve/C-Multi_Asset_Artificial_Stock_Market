//
// Created by Cephas Svosve on 8/6/2021.
//

#ifndef MARKET_ECOLOGY_MARKET_H
#define MARKET_ECOLOGY_MARKET_H

#include "marketWatch.h"
#include "assets.h"
#include <Eigen/Dense>
#include <vector>

using namespace Eigen;


class Market{
private:
    Watch marketClock;
    vector<RiskyAsset> assets;


public:




    Watch addClock(Watch clock)
    {
        marketClock = clock;
    }


    void addAsset(RiskyAsset &Stk)
    {
        assets.push_back(Stk);
    }


    vector<RiskyAsset> qoutes()
    {
        return assets;
    }
    //MatrixXd getCrossCorr(){return crossCorr;}
    //void addAsset(RiskyAsset a){assets = a;}

    //double* getAssets(){return assets;}



};

#endif //MARKET_ECOLOGY_MARKET_H
