#pragma once

double kfloor(double x)
{
    int _x = (int)x;
    return (double)_x;
}

double kceil(double x)
{
    double floorX = kfloor(x);

    if(x == floorX)
        return x;

    return floorX + 1;
}

double kfabs(double x)
{
    return (x < 0) ? -x : x;
}

double kfmod(double x, double y) // Unsure
{
    while(x >= y)
        x -= y;

    return x;
}