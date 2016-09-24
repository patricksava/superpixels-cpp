/* 
 * File:   main.cpp
 * Author: psava
 *
 * Created on September 24, 2016, 1:29 AM
 */

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include "Image.h"
#include "Cluster.h"
#include "Pixel.h"
#include "Superpixel.h"

#define ITERATIONS 5

using namespace std;


/**
 * Inicializa os clusters.
 * @param clusters - clusters que DEVEM ser alocados e inicializados.
 * @param Lab - imagem em Lab.
 * @param k - numero desejado de superpixels.
 * @param npx - numero de pixels dentro de um superpixel
 * @return - numero de superpixels.
 */
int initializeClusters( Cluster*& clusters, vector<Cluster*>& labels, Image& Lab, int k, int npx )
{
    int width = Lab.getW();
    int height = Lab.getH();

    //Aloca o espaço de memória necessario
    clusters = (Cluster*) malloc(sizeof(Cluster) * k);

    //Inicializa clusters vazios
    for (int i = 0; i < k; i++){
        clusters[i] = Cluster();
    }

    //Preenche os labels e quantidade de pixels por cluster
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int clusterIndex = (y/npx) * ceil((float)width/npx) + (x/npx);
            Cluster& c = clusters[clusterIndex];

            c.npx ++;

            labels[y * width + x] = &c;
        }
    }

    return k;
}

/**
 * Atualiza os clusters para próxima iteração.
 * @param Lab - Imagem em lab.
 * @param clusters - clusters inicializados.
 * @param labels - labels inicializadas.
 * @param k - numero de superpixels.
 * @param npx - numero de pixels por superpixel.
 */
void updateClusters( Image& Lab, Cluster* clusters, vector<Cluster*>& labels, int k, int npx ) {
    
    //Zerar todos os parâmetros do cluster
    for (int i = 0; i < k; i++) {
        Cluster& c = clusters[i];
        c.set(Pixel(), 0.0, 0.0);
        c.maxSpaceD = 0.0;
        c.maxColorD = 0.0;
        c.npx = 0;
    }

    //Para cada pixel, verificar o seu cluster atual e acumular os valores
    //do pixel no cluster
    for (int y = 0; y < Lab.getH(); y++) {
        for (int x = 0; x < Lab.getW(); x++) {
            Cluster& c = *(labels[y * Lab.getW() + x]);
            Pixel px = Lab.getPixel(x, y);

            Pixel pxClust = c.getPixel();
            Pixel p = px + pxClust;

            c.npx ++;
            c.setPixel(p);
            c.setPosition(c.getX() + x, c.getY() + y);
        }
    }

    //Tirar a média dos valores do cluster para representar os pixels dentro dele
    for (int i = 0; i < k; i++){
        Cluster& c = clusters[i];
        float newX = c.getX() / c.npx;
        float newY = c.getY() / c.npx;

        c.setPosition(newX, newY);
        c.setPixel(c.getPixel() / c.npx);
    }
}


/**
 * Atualiza o vetor de distancias de pixel para superpixel 
 * para próxima iteração.
 * @param Lab - Imagem em lab.
 * @param labels - labels inicializados.
 * @param distances - distancias inicializadas.
 * @param npx - numero de pixels por superpixels.
 */
void updateDistances( Image& Lab, vector<Cluster*> labels, vector<float>& distances, int npx ){
    
    //Para cada pixel, atualiza a distancia dele ao cluster
    for (int y = 0; y < Lab.getH(); y++) {
        for (int x = 0; x < Lab.getW(); x++) {
            Cluster& c = *(labels[y * Lab.getW() + x]);
            Pixel p = Lab.getPixel(x, y);

            float spaceD = sqrt(pow(c.getX()-x,2) + pow(c.getY()-y,2));
            float colorD = sqrt(pow(c.getPixel()[0] - p[0],2) + 
                                pow(c.getPixel()[1] - p[1],2) + 
                                pow(c.getPixel()[2] - p[2],2));
            float distance = sqrt(pow(colorD/c.maxColorD,2) + pow(spaceD/c.maxSpaceD,2));
            c.maxSpaceD = max(c.maxSpaceD, spaceD);
            c.maxColorD = max(c.maxColorD, colorD);

            distances[y * Lab.getW() + x] = distance;
        }
    }
}


/**
 * Realiza o algoritmo de superpixels.
 * @param Lab - Imagem em lab.
 * @param clusters - clusters inicializados.
 * @param labels - labels inicializadas.
 * @param distances - distancias inicializadas.
 * @param k - numero de superpixels.
 * @param M - compacidade.
 */
void performSuperPixelsAlgorithm( Image& Lab, Cluster* clusters, vector<Cluster*>& labels, 
                                vector<float>& distances, int k, double M, int npx )
{  
    for(int rep = 0; rep < ITERATIONS; rep++){
        updateClusters( Lab, clusters, labels, k, npx );
        updateDistances( Lab, labels, distances, npx );

        for (int i = 0; i < k; i++){
            Cluster& c = clusters[i];

            //Inicializando os bounds de busca do kmeans
            int xs = max<int>(c.getX() - npx, 0);
            int xf = min<int>(c.getX() + npx, Lab.getW());
            int ys = max<int>(c.getY() - npx, 0);
            int yf = min<int>(c.getY() + npx, Lab.getH());

            //Executa o kmeans para verificar qual cluster comporta o pixel
            for (int y = ys; y < yf; y++) {
                for (int x = xs; x < xf; x++) {
                    int vPos = y * Lab.getW() + x;
                    Pixel p = Lab.getPixel(x, y);

                    float spaceD = sqrt(pow(c.getX()-x,2) + pow(c.getY()-y,2));
                    float colorD = sqrt(pow(c.getPixel()[0] - p[0],2) + 
                                        pow(c.getPixel()[1] - p[1],2) + 
                                        pow(c.getPixel()[2] - p[2],2));
                    float distance = sqrt(pow(colorD/c.maxColorD,2) + pow(spaceD/c.maxSpaceD,2));
                    
                    //Se o cluster atual melhor representa o pixel atual troca o superpixel dele
                    if (distance < distances[vPos]){
                        Cluster& oldCluster = *(labels[vPos]);
                        //oldCluster.npx --;

                        labels[vPos] = &c;
                        //c.npx ++;

                        distances[vPos] = distance;
                    }
                }
            }
        }
    }
}


/**
 * Troca a cor de cada pixel da imagem para a cor do seu superpixel.
 * @param Lab - Imagem em lab.
 * @param labels - labels inicializadas.
 */
void paintPixels( Image& img, vector<Cluster*>& labels )
{
    for (int y = 0; y < img.getH(); y++) {
        for (int x = 0; x < img.getW(); x++) {
            Cluster* c = labels[y * img.getW() + x];
            img.setPixel(x, y, c->getPixel());
        }
    }
}


void Superpixel::applyFilter( Image& rgb, int k, double M )
{
    //Converte a imagem para Lab.
    Image* Lab = new Image(rgb);
    Lab->convertImageFromRGB2Lab();

    //Calcula o numero de pixels cada superpixel.
    int npx = sqrt( Lab->getW() * Lab->getH() / k );

    printf("Numero de superpixels desejados: %d\n", k);
    printf("Numero de pixels por superpixel: %d\n", npx);

    //Inicializa os os clusters.
    Cluster* clusters;
    vector<Cluster*> labels(Lab->getW() * Lab->getH());
    vector<float> distances(Lab->getW() * Lab->getH());

    k = initializeClusters( clusters, labels, *Lab, k, npx );

    //Executa o algoritmo.
    performSuperPixelsAlgorithm( *Lab, clusters, labels, distances, k, 20, npx );
    
    //define as novas cores dos pixels.
    paintPixels( *Lab, labels );

    //Converte a imagem de volta.
    Lab->convertImageFromLab2RGB();
    
    //Copia a imagem com superpixels para a imagem de entrada
    rgb = *Lab;
}

