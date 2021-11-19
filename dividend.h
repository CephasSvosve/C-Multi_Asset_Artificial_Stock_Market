//
// Created by Cephas Svosve on 17/6/2021.
//

#ifndef MARKET_ECOLOGY_DIVIDEND_H
#define MARKET_ECOLOGY_DIVIDEND_H


#include "marketWatch.h"
#include <vector>
#include <Eigen/Dense>


using namespace std;
using namespace Eigen;

class Div{

private:



    //we store asset's parameters in respective variables, NB-parameters are determined from empirical data-

    vector<double> growthRate,volatility, autoCorr;
    Watch clock;
    MatrixXd crossCorrMat;


public:
    //we initialize instances of this class by setting the respective values of market and watch
    //we would require a watch to maintain accurate synchrony


    Div(MatrixXd X, Watch watch){crossCorrMat = X,clock = watch;}



    //we create getters to access private parameters for dividend computation


    //parameters of all different assets are stored in the same vector,
    //int tickerID is an index that we use to locate a particular asset's parameter within that vector


    double getGrowthRate(int tickerID);
    double getVolatility(int tickerID);
    double getAutoCorr(int tickerID);




    //we create setters to set parameters of the dividend process
    //x is the value observed from empirical data for the parameter


    void setGrowthRate(double x);
    void setVolatility(double x);
    void setAutoCorr(double x);





    //this function is for setting cross-sectional correlation matrix

    void setCrossCorr();




 /***************************************************************************************************/
 //we create essential tools for Noise generation
 /***************************************************************************************************/

    
    double corrcoef(VectorXd x, VectorXd y);
    double lateralcorrcoef(VectorXd a);
    MatrixXd crossCorr(MatrixXd x);




    //we create the noise generator - *see .cpp file for further notes on this
    
    MatrixXd generateWhiteNoise(int numOfAssets, int numOfTicks);





    //vertical whitening removes cross-sectional correlation

    MatrixXd verticallyWhiten(MatrixXd mx);





    //vertical Coloring puts cross-sectional correlation to the random matrix
    //lateral coloring puts auto-correlation to a vector of uncorrelated random numbers


    MatrixXd verticallyColor(MatrixXd x);
    VectorXd laterallyColor(VectorXd x);






    //this is a standardizer of the random numbers generated

    VectorXd standardize(VectorXd x);





    //Noise generation point

    MatrixXd generateColoredNoise(int numOfAssets, int numOfTicks);


};


#endif //MARKET_ECOLOGY_DIVIDEND_H
