//
// Created by Cephas Svosve on 8/6/2021.
//




#include "market.h"
#include "assets.h"
#include <iostream>




using namespace Eigen;
using namespace std;


//Initialize Interface Variables -i.e. cross-sectional correlation matrix and its Lower Cholesky decomposition, L









//set cross-correlation matrix






int main(){
    //we set div cross-sectional correlation
    MatrixXd divCrossCorr(2,2);
    divCrossCorr << 1,0.2,
                    0.2,1;




    //we instantiate a market
    Market thisMarket = Market();




    //we create a market watch
    Watch clock;
    clock.setDt(4.0);



    //test if watch is working properly
    clock.tick();
    clock.tick();
    clock.tick();

    thisMarket.addClock(clock);

    for(int i= 0; i<=1; i++){
        RiskyAsset Stk_i = RiskyAsset(divCrossCorr,clock);
        thisMarket.addAsset(Stk_i);
    }






    //we instantiate the risky assets



    //RiskyAsset Stk2 = RiskyAsset(thisMarket,clock);




    //test output
    //cout << Stk8.computeDiv(1);
    for(int i= 0; i<=1; i++){

        cout << thisMarket.qoutes()[i].computeDiv(1);
    }




    return 0;
}
