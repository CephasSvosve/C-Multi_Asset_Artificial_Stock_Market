//
// Created by Cephas Svosve on 8/6/2021.
//




#include "market.h"
#include "assets.h"
#include <iostream>




using namespace Eigen;
using namespace std;



//this is the main method where we run the application from
//we basically use it as a screen for intialisation of all parameters required
//we also retrieve output from this section of the code



int main(){
    
    
    
    //Initialize Interface Variables -i.e. cross-sectional correlation matrix and its Lower Cholesky decomposition, L
    //we set div cross-sectional correlation
    
    MatrixXd divCrossCorr(2,2);
    
    divCrossCorr <<     1,0.2,
                        0.2,1;




    //we instantiate a market
    Market thisMarket = Market();




    //we create a market watch- all objects refer theire time stamp from watch to maintain sychrony
    Watch clock;
    clock.setDt(1.0);


 
    //add clock to the market 
    thisMarket.addClock(clock);

    
    //we generate instances of Risky Assets by this loop
    for(int i= 0; i<=1; i++){
        RiskyAsset Stk_i = RiskyAsset(divCrossCorr,clock);
        thisMarket.addAsset(Stk_i);
    }




    //test output
    for(int i= 0; i<=1; i++){
        cout << thisMarket.qoutes()[i].computeDiv(1);
    }




    return 0;
}
