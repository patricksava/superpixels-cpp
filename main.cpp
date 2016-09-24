/* 
 * File:   main.cpp
 * Author: jcoelho, psava
 *
 * Created on September 11, 2016, 2:18 PM
 */

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include "Image.h"
#include "Cluster.h"
#include "Superpixel.h"

#define ITERATIONS 5

using namespace std;

/*
 * 
 */
int main( int argc, char** argv )
{
    int nSuperPx;
    cout << "Quantos superpixels desejados?" << endl;
    cin >> nSuperPx;

    Image l;
    //if (l.readBMP( "AB_ufv_0675.bmp" ))
    if (l.readBMP( "estrela.bmp" ))
    {
        printf( "Leitura executada com sucesso\n" );
    }
    
    Superpixel::applyFilter( l, nSuperPx, 20.0 );
    
    if (l.writeBMP( "superpixled_image.bmp" ))
    {
        printf( "Escrita executada com sucesso\n" );
    }

    return 0;
}

