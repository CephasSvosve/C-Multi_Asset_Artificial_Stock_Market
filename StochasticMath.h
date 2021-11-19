//
// Created by Cephas Svosve on 16/11/2021.
//

#ifndef UNTITLED21_STOCHASTICMATH_H
#define UNTITLED21_STOCHASTICMATH_H



#include "marketWatch.h"
#include <vector>
#include <Eigen/Dense>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace Eigen;

class Stats{

private:


    //we store asset's parameters in respective variables, NB-parameters are determined from empirical data-
        vector<double> growthRate,volatility, autoCorr;
            MatrixXd crossCorrMat;


public:
    Stats(){}

    //int tickerID is an index that we use to locate a particular asset's parameter within that vector
      double getGrowthRate(int tickerID);
          double getVolatility(int tickerID);
              double getAutoCorr(int tickerID);


    // x is the value observed from empirical data for the parameter
      void setGrowthRate(double x);
          void setVolatility(double x);
              void setAutoCorr(double x);



    //this function is for setting cross-sectional correlation matrix
      void setCrossCorr();


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //we create essential tools for Noise generation
        static double corrcoef(VectorXd x, VectorXd y);
            static double lateralcorrcoef(VectorXd a);
                static MatrixXd crossCorr(MatrixXd x);



    //we create the noise generator - *see .cpp file for further notes
        static MatrixXd generateWhiteNoise(int numOfAssets, int numOfTicks);


    //vertical whitening removes cross-sectional correlation
        static MatrixXd verticallyWhiten(MatrixXd mx);


    //vertical Coloring puts cross-sectional correlation to the random matrix
    //lateral coloring puts auto-correlation to a vector of uncorrelated random numbers
          static MatrixXd verticallyColor(MatrixXd X, MatrixXd corr);
              static VectorXd laterallyColor(VectorXd dZ, double corr);
                  static double stdev(VectorXd X);


    //this is for standardizing the random numbers generated
        static VectorXd standardize(VectorXd X);


    //Noise generation point
        static MatrixXd generateColoredNoise(int numOfTicks, VectorXd &autoCorrMat, MatrixXd &crossCorrMat);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};
#endif //UNTITLED21_STOCHASTICMATH_H
