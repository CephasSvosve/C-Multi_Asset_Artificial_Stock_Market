//
// Created by Cephas Svosve on 16/11/2021.
//

//
// Created by Cephas Svosve on 5/8/2021.
//

//
// Created by Cephas Svosve on 17/6/2021.
//

#include "StochasticMath.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <boost/random.hpp>
#include <numeric>




//get process growth rate i.e average of the percentage change in dividend paid-out, dD(t)/D(t-1).
double Stats::getGrowthRate(int tickerID)
{
    return growthRate[tickerID];
}


//get volatility i.e stdev of the growth process.
double Stats::getVolatility(int tickerID)
{
    return volatility[tickerID];
}


double Stats::getAutoCorr(int tickerID)
{
    return autoCorr[tickerID];
}


void Stats::setGrowthRate(double x)
{
    growthRate.push_back(x);
}



void Stats::setVolatility(double x)
{
    volatility.push_back(x);
}


void Stats::setAutoCorr(double x)
{
    autoCorr.push_back(x);
}


void Stats::setCrossCorr()
{

    // std::cout <<  "Div " << crossCorrMat << std::endl;
}



//because our system depends on accuracy of the randomness of the random matrix,
//we create a function that carefully generates a purely random matrix,
//i.e. auto-correlation is 0 precise to the order of e-03 as well as the cross-sectional correlation

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MatrixXd Stats::generateWhiteNoise(int numOfAssets, int numOfTicks)
{
    MatrixXd randoms(numOfAssets,numOfTicks);
      VectorXd a;


    //algorithm for generating random numbers that are seeded on changing time
      time_t now = 3;
        boost::random::mt19937 gen{static_cast<uint32_t>(now)};
          boost::normal_distribution<> nd(0.0, 1.);
            boost::variate_generator<boost::mt19937&,
              boost::normal_distribution<> > var_nor(gen, nd);



    //we generate the matrix of random numbers
    for(int rw=0; rw < numOfAssets; rw++)
    {
        //we make sure the naturally occurring auto-correlation is sufficiently small by using a do-while loop
          do
            {
              //here we load each row with appropriate random numbers
                a = VectorXd(numOfTicks);
                  for(int i = 0; i < numOfTicks; ++i)
                    {
                      a(i) = var_nor();
            }
        }
        while (abs(lateralcorrcoef(a)) > 0.001);


        randoms.row(rw) = a;
    }




    //We then remove any cross-sectional correlation

    return verticallyWhiten(randoms);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//this function removes cross-sectional correlation by multiplying a matrix
// by the inverse of its lower cholesky decomposition matrix
MatrixXd Stats::verticallyWhiten(MatrixXd mx)
{
    MatrixXd X = crossCorr(mx);
      LLT<MatrixXd> llt(X);
        MatrixXd L = llt.matrixL();
          MatrixXd vWhite = L.inverse() * mx;

            return vWhite;
}


//function that mmodels cross-sectional correlation on non-cross-sectionally correlated processes
MatrixXd Stats::verticallyColor(MatrixXd dZ, MatrixXd corr)
{
    LLT<MatrixXd> llt(corr);
    MatrixXd L = llt.matrixL();
    MatrixXd vColored = L * dZ;

    return vColored;
}


//function that generates a noise process with specific one time lag autocorrelation
VectorXd Stats::laterallyColor(VectorXd dZ, double corr)
{
    int n      = dZ.size();
    VectorXd dU = VectorXd(n);
    dU(0) = dZ(0);

    for(int i = 0; i < n-1; i++)
    {
        dU(i+1) = sqrt(1-pow(corr,2))*dZ(i) + corr*dU(i);
    }
    return dU;
}


//computes stdev
double Stats::stdev(VectorXd X)
{
    double sum = X.sum();
    double m =  sum / X.size();

    double accum = 0.0;

    for(int d = 0; d < X.size(); d++)
    {
        accum += pow((X(d) - m),2);
    };

    double stdev = sqrt(accum / (X.size()-1));

    return stdev;
}


//standardizes a set of random numbers to a standard normal distribution
VectorXd Stats::standardize(VectorXd X)
{
    double mean = X.mean();
    double sigma= stdev(X);

    VectorXd sX = VectorXd(X.size());

    for(int i =0; i< X.size(); i++)
    {
        sX(i) = (X(i)-mean)/sigma;
    }

    return sX;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//generates an nxm matrix of both auto- and cross-sectionally correlated processes
MatrixXd Stats::generateColoredNoise(int numOfTicks, VectorXd &autoCorrMat, MatrixXd &crossCorrMat)
{
    int numOfAssets = crossCorrMat.row(0).size();

    //create a random matrix
    MatrixXd X = generateWhiteNoise(numOfAssets, numOfTicks);
    MatrixXd Y = verticallyWhiten(X);


    MatrixXd W(numOfAssets,numOfTicks);
    MatrixXd Z(numOfAssets,numOfTicks);
    MatrixXd OUTPUT(numOfAssets,numOfTicks);

    //put auto-correlation
    for(int i = 0; i < numOfAssets; i++)
    {
        W.row(i) =  standardize(laterallyColor(Y.row(i), autoCorrMat(i)));

    }

    Z = verticallyWhiten(W);
    OUTPUT = verticallyColor(Z,crossCorrMat);

    return OUTPUT;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//this function calculates the coefficient of determination -correlation- between two vectors
//it requires the two vectors x and y, then computes the variables required for the formula
//i.e. variables xx = x^2  ,xy = x*y  and yy = y^2

double Stats::corrcoef(VectorXd x, VectorXd y)
{
    if(x.size() == y.size()) {

        int n = x.size();

        //variable xx represents x^2
        double xx[n];
        double xy[n];
        double yy[n];


        //sumx is the sum of random variables x
        double sumx = x.sum();
        double sumy = y.sum();


        //sumxx is the sum of random variables x^2
        double sumxx;
        double sumxy;
        double sumyy;



        for (int i=0; i < x.size(); i++)
        {
            xx[i] = pow(x[i],2);
            xy[i] = x[i] * y[i];
            yy[i] = pow(y[i],2);

            sumxx = sumxx + xx[i];
            sumxy = sumxy + xy[i];
            sumyy = sumyy + yy[i];
        }


        //this is the formula used to calculate coefficient of determination, *see
        //https://www.jstor.org/stable/2965177?seq=6#metadata_info_tab_contents
        //Reed, William Gardner. “The Coefficient of Correlation.” Publications of the American Statistical Association, vol. 15, no. 118, 1917, pp. 675–684. JSTOR, www.jstor.org/stable/2965177. Accessed 20 June 2021.

        double r = (n*sumxy - sumx*sumy)/sqrt((n*sumxx-pow(sumx,2))*(n*sumyy-pow(sumy,2)));

        return r;
    }
    else
    {throw "found vectors of different size on computing correlation";}

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double Stats::lateralcorrcoef(VectorXd a){

    int n = a.size()-1;

    double x[n];
    double y[n];
    double xx[n];
    double xy[n];
    double yy[n];

    double sumx;
    double sumy;
    double sumxx;
    double sumxy;
    double sumyy;

    for (int i=0; i < n; i++)
    {
        x[i] = a[i+1];
        y[i] = a[i];
        xx[i] = pow(x[i],2);
        xy[i] = x[i] * y[i];
        yy[i] = pow(y[i],2);

        sumx = sumx + x[i];
        sumy = sumy + y[i];

        sumxx = sumxx + xx[i];
        sumxy = sumxy + xy[i];
        sumyy = sumyy + yy[i];
    }

    double r = (n*sumxy - sumx*sumy)/sqrt((n*sumxx-pow(sumx,2))*(n*sumyy-pow(sumy,2)));

    return r;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//computes cross-sectional correlation
MatrixXd Stats::crossCorr(MatrixXd x){
    int m = x.col(0).size();
    int n = x.row(0).size();

    MatrixXd corrMat(m,m);

    for(int i = 0; i < m; i++){
        for(int j = 0; j <= i; j++){

            corrMat(i,j) = corrcoef( x.row(i),x.row(j));
            corrMat(j,i) = corrMat(i,j);
        }
    }

    return corrMat;
}
