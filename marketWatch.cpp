//
// Created by Cephas Svosve on 8/6/2021.
//
#include "marketWatch.h"

void Watch::setDt(double delta_t)
{


        dt = delta_t;
}




double Watch::getDt()
{
    return dt;
}

double Watch::getTime()
{
        return t;
}

void Watch::startAt(float startTime)
{
        t = startTime;
}

void Watch::reset()
{
        t = 0;
}

double Watch::tick()
{
        t = t+dt;
        return t;
}
