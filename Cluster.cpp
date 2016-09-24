/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Cluster.cpp
 * Author: jcoelho
 * 
 * Created on September 12, 2016, 10:16 AM
 */

#include "Cluster.h"



Cluster::Cluster( Pixel lab, float x, float y )
{
    _lab = lab;
    _x = x;
    _y = y;
    npx = 0;
    maxColorD = maxSpaceD = 0.0;
}



Pixel Cluster::getPixel( ) const
{
    return _lab;
}



float Cluster::getX( ) const
{
    return _x;
}



float Cluster::getY( ) const
{
    return _y;
}



void Cluster::set( Pixel lab, float x, float y )
{
    _lab = lab;
    _x = x;
    _y = y;
}



void Cluster::setPixel( Pixel lab )
{
    _lab[0] = lab[0];
    _lab[1] = lab[1];
    _lab[2] = lab[2];
}



void Cluster::setPosition( float x, float y )
{
    _x = x;
    _y = y;
}


