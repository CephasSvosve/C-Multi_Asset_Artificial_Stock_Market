//
// Created by Cephas Svosve on 8/6/2021.
//

#ifndef MARKET_ECOLOGY_MARKETWATCH_H
#define MARKET_ECOLOGY_MARKETWATCH_H


class Watch{
private:
    //Set clock variables
    double t,dt;


public:
    //Constructor
    Watch(){}

    void setDt(double delta_t);




    //getters for change in time, dt.
    double getDt();
    double getTime();




    //Clock functions
    void startAt(float startTime);
    double tick();
    void reset();


};


#endif //MARKET_ECOLOGY_MARKETWATCH_H
