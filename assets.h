//
// Created by Cephas Svosve on 8/6/2021.
//

#ifndef MARKET_ECOLOGY_ASSETS_H
#define MARKET_ECOLOGY_ASSETS_H


#include "marketWatch.h"
#include "dividend.h"
#include <Eigen/Dense>


using namespace Eigen;

class RiskyAsset{

private:
    int x;
    Watch clock;
    MatrixXd crossCorr;
    Div* myDiv;




public:
    //constructor
    RiskyAsset(MatrixXd &X, Watch &a);
    double computeDiv(int i);
    Div* createDiv();


};
#endif //MARKET_ECOLOGY_ASSETS_H
