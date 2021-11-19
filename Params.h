//
// Created by Cephas Svosve on 6/8/2021.
//

#ifndef UNTITLED21_PARAMS_H
#define UNTITLED21_PARAMS_H
#include "StochasticMath.h"
#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static tuple<VectorXd,VectorXd,VectorXd,MatrixXd, double> params(std::string process){
//trade period
double trade_period = 252;


//cross-sectional correlations
    MatrixXd div_cross_corr(3, 3);
    div_cross_corr <<       1, 0.251, 0.034,
                            0.034, 1, 0.3,
                            0.3, 0.034,1;

    MatrixXd earnings_cross_corr(3, 3);
    earnings_cross_corr <<  1, 0.251, 0.034,
                            0.034, 1, 0.3,
                            0.3, 0.034,1;

    MatrixXd fcf_cross_corr(3, 3);
    fcf_cross_corr <<       1, 0.251, 0.034,
                            0.034, 1, 0.3,
                            0.3, 0.034,1;


//auto-correlations
    VectorXd div_aut = VectorXd(3);
    div_aut(0) =        0.02;
    div_aut(1) =        0.13;
    div_aut(2) =        0.36;

    VectorXd earnings_aut = VectorXd(3);
    earnings_aut(0) =   0.02;
    earnings_aut(1) =   0.13;
    earnings_aut(2) =   0.36;

    VectorXd fcf_aut = VectorXd(3);
    fcf_aut(0) =        0.02;
    fcf_aut(1) =        0.13;
    fcf_aut(2) =        0.36;

//mu
    VectorXd div_mu = VectorXd(3);
    div_mu(0) = 0.01;
    div_mu(1) = 0.02;
    div_mu(2) = 0.03;

    VectorXd earnings_mu = VectorXd(3);
    earnings_mu(0) = 13.00;
    earnings_mu(1) = 20.32;
    earnings_mu(2) = 30.00;

    VectorXd fcf_mu = VectorXd(3);
    fcf_mu(0) = 0.01;
    fcf_mu(1) = 0.02;
    fcf_mu(2) = 0.03;

//sigma
    VectorXd div_sig = VectorXd(3);
    div_sig(0) = 0.012;
    div_sig(1) = 0.023;
    div_sig(2) = 0.076;

    VectorXd earnings_sig = VectorXd(3);
    earnings_sig(0) = 0.012;
    earnings_sig(1) = 0.023;
    earnings_sig(2) = 0.076;

    VectorXd fcf_sig = VectorXd(3);
    fcf_sig(0) = 0.012;
    fcf_sig(1) = 0.023;
    fcf_sig(2) = 0.076;


    tuple<VectorXd,VectorXd,VectorXd,MatrixXd, double> parameters;

    if(process == "dividends"){
        get<0>(parameters) =div_mu;
        get<1>(parameters) =div_sig;
        get<2>(parameters) =div_aut;
        get<3>(parameters) =div_cross_corr;
        get<4>(parameters) = trade_period;

}else
    if(process == "earnings"){
        get<0>(parameters) =earnings_mu;
        get<1>(parameters) =earnings_sig;
        get<2>(parameters) =earnings_aut;
        get<3>(parameters) =earnings_cross_corr;
        get<4>(parameters) = trade_period;

    }else
        if(process == "freecashflow"){
            get<0>(parameters) =fcf_mu;
            get<1>(parameters) =fcf_sig;
            get<2>(parameters) =fcf_aut;
            get<3>(parameters) =fcf_cross_corr;
            get<4>(parameters) = trade_period;

    }
return parameters;

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
vector<double> load_process(int trade_period, int tickerID, VectorXd mu, VectorXd sig, double dt, MatrixXd dU){
    vector<double> process1;
    for(int i = 0; i < trade_period; i++) {
        if (process1.empty()) {
            double a = abs(mu(tickerID));
            process1.push_back(a);
        }else {
            double y = (process1[i - 1] )+ mu(tickerID)  * dt + sig(tickerID) * dU(tickerID, i);
            process1.push_back(y);
        }
    }
    return process1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static vector<vector<double>> generate(std::string process){

int trade_period = get<4>(params(process));
//we set div cross-sectional correlation

    MatrixXd divCrossCorr(3, 3);
    divCrossCorr = get<3>(params(process));

//set the dividend autocorrelations for the individual assets
    VectorXd Aut = VectorXd(3);
    Aut = get<2>(params(process));


//volatility
    VectorXd sig = VectorXd(3);
    sig = get<1>(params(process));


//set the params for the dividend processes of the assets
//mean
    VectorXd mu = VectorXd(3);
    mu = get<0>(params(process));


    MatrixXd dU(3,trade_period);
    dU = Stats::generateColoredNoise(trade_period, Aut, divCrossCorr);


    vector<double> div1 = load_process(trade_period,0,mu,sig,1,dU);
    vector<double> div2 = load_process(trade_period,1,mu,sig,1,dU);
    vector<double> div3 = load_process(trade_period,2,mu,sig,1,dU);

    vector<vector<double>> result;
    result.push_back(div1);
    result.push_back(div2);
    result.push_back(div3);

    return result;

}



#endif //UNTITLED21_WIENER_H
