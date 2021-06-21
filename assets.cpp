//
// Created by Cephas Svosve on 8/6/2021.
//
#include "assets.h"
#include <iostream>


//constructor
RiskyAsset::RiskyAsset(MatrixXd &X, Watch &a)
{
    clock = a;
    crossCorr = X;
}


//test method
double RiskyAsset::computeDiv(int i)
{

    std::cout << "RiskyAsset "  << std::endl;
    //createDiv();

}



Div * RiskyAsset::createDiv()
{
    myDiv = new Div(crossCorr, clock);
    myDiv->setCrossCorr();

}
