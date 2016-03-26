#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef double mt;
typedef double pt;
# define PI          3.141592653589793238462643383279502884

FILE *outputFile;
#define MyOut outputFile

typedef struct point3d{
    mt x;
    mt y;
    mt z;
}point3d;

typedef struct coordinate3d{
    point3d *vectorX;
    point3d *vectorY;
    point3d *vectorZ;

    mt inX;
    mt inY;
    mt inZ;

    mt angleA;
    mt angleB;

    point3d move;

}coordinate3d;

point3d rotate(point3d d3, coordinate3d *uklad){
    double a = uklad->angleA, b = uklad->angleB;

    point3d mP;
    mP.x = d3.x*cos(b*PI/180.0) - d3.z*sin(b*PI/180.0);
    mP.y = d3.y*cos(a*PI/180.0) - d3.z*cos(b*PI/180.0)*sin(a*PI/180.0) - d3.x*sin(a*PI/180.0)*sin(b*PI/180.0);
    mP.z = d3.z*cos(a*PI/180.0)*cos(b*PI/180.0) + d3.y*sin(a*PI/180.0) + d3.x*cos(a*PI/180.0)*sin(b*PI/180.0);

    return mP;
}

point3d intoSurface(point3d d3_1, pt obserwator, coordinate3d uklad){
    point3d d3 = rotate(d3_1, &uklad);

    d3.x += uklad.move.x;
    d3.y += uklad.move.y;
    d3.z += uklad.move.z;

    if(d3.z + obserwator == 0){
        fprintf(stderr, "dzielenie przez 0 :(\n");
        return d3;
    }

    if(d3.z + obserwator < 0){
        fprintf(stderr, "punkt za obserwatorem :(\n");
        return d3;
    }

    point3d surP;
    surP.x = d3.x*obserwator/(d3.z + obserwator);
    surP.y = d3.y*obserwator/(d3.z + obserwator);
    surP.z = 0;

    return surP;
}

coordinate3d setCoordinate(mt angleX, mt angleY, point3d start){
    coordinate3d uklad;
    uklad.vectorX = (point3d*)malloc(sizeof(point3d));
    uklad.vectorY = (point3d*)malloc(sizeof(point3d));
    uklad.vectorZ = (point3d*)malloc(sizeof(point3d));

    uklad.vectorX->y = uklad.vectorX->z = 0;
    uklad.vectorY->x = uklad.vectorY->z = 0;
    uklad.vectorZ->x = uklad.vectorZ->y = 0;

    uklad.vectorX->x = uklad.vectorY->y = uklad.vectorZ->z = 1.0;

    uklad.inX = 1.0;
    uklad.inY = 1.0;
    uklad.inZ = 1.0;

    uklad.angleA = angleX;
    uklad.angleB = angleY;

    uklad.move = start;

    return uklad;
}

void moveTo(point3d p){
    fprintf(MyOut, "%.2lf %.2lf moveto\n", p.x, p.y);
}

void lineTo(point3d p){
    fprintf(MyOut, "%.2lf %.2lf lineto\n", p.x, p.y);
}

void setConnection(point3d *p, point3d vector, pt s){
    p->x += s*vector.x;
    p->y += s*vector.y;
    p->z += s*vector.z;
}

point3d* connections(point3d *p, int a, coordinate3d uklad, pt s){
    switch (a) {
    case 0:
        setConnection(p, *uklad.vectorY, -s*uklad.inY);
        break;
    case 1:
        setConnection(p, *uklad.vectorX, -s*uklad.inX);
        break;
    case 2:
        setConnection(p, *uklad.vectorY, s*uklad.inY);
        break;
    case 3:
        setConnection(p, *uklad.vectorZ, s*uklad.inZ);
        break;
    case 4:
        setConnection(p, *uklad.vectorY, -s*uklad.inY);
        break;
    case 5:
        setConnection(p, *uklad.vectorX, s*uklad.inX);
        break;
    case 6:
        setConnection(p, *uklad.vectorY, s*uklad.inY);
        break;
    default:
        break;
    }
    return p;
}

coordinate3d sequence(coordinate3d uklad, int v){
    switch ((v+1)/2) {
    case 0:
        uklad.inY *= -1;
        uklad.inZ *= -1;
        break;
    case 1:
        uklad.inX *= -1;
        uklad.inZ *= -1;
        break;
    case 2:
        uklad.inX *= -1;
        uklad.inY *= -1;
        break;
    case 3:
        break;
    case 4:
        break;
    default:
        break;
    }

    point3d *X = uklad.vectorX;
    point3d *Y = uklad.vectorY;
    point3d *Z = uklad.vectorZ;

    mt inX = uklad.inX;
    mt inY = uklad.inY;
    mt inZ = uklad.inZ;

    int tab[] = {1,2,2,0,0,2,2,1,0};

    for(int i = 0; i < tab[v]; i++){
        uklad.vectorX = Z;
        uklad.vectorY = X;
        uklad.vectorZ = Y;

        X = uklad.vectorX;
        Y = uklad.vectorY;
        Z = uklad.vectorZ;

        uklad.inX = inZ;
        uklad.inY = inX;
        uklad.inZ = inY;

        inX = uklad.inX;
        inY = uklad.inY;
        inZ = uklad.inZ;
    }
    return uklad;
}

void hilbertCurve3d(int rzadKrzywej, point3d *start, pt obserwator, coordinate3d uklad, pt dlLacznika, int tmp){
    if(rzadKrzywej > 0){
        //printf("\n%%\t rzad: %i\n%%\t wywolanie: %i\n", rzadKrzywej, tmp);
        for(int i = 0; i < 7; i++){
            hilbertCurve3d(rzadKrzywej-1,
                           start,
                           obserwator,
                           sequence(uklad, i),
                           dlLacznika, i);

            lineTo(intoSurface( *connections(start, i, uklad, dlLacznika), obserwator, uklad));
        }
        hilbertCurve3d(rzadKrzywej-1,
                       start,
                       obserwator,
                       sequence(uklad, 7),
                       dlLacznika, 7);
    }
}
int main(int argc, char *argv[])
{
    outputFile = fopen("hilbert3d.eps", "w");
    if(outputFile == NULL)
        return 1;

    int rzadKrzywej = 3;

    pt rozmiarObrazka = 1000;
    pt dlKrawedziSciany = 400;

    pt odlObserwatora = 400;

    point3d move;

    mt angleX = 32.5, angleY = -38;

        if(argc == 10){
        sscanf(argv[1], "%i",  &rzadKrzywej);
        sscanf(argv[2], "%lf", &rozmiarObrazka);
        sscanf(argv[3], "%lf", &dlKrawedziSciany);
        sscanf(argv[4], "%lf", &odlObserwatora);
        sscanf(argv[5], "%lf", &move.x);
        sscanf(argv[6], "%lf", &move.y);
        sscanf(argv[7], "%lf", &move.z);
        sscanf(argv[8], "%lf", &angleX);
        sscanf(argv[9], "%lf", &angleY);
    }else{
        fprintf(stderr, "Podaj wszystkie dane.\n");
        return 1;
    }
    point3d start;
    start.x =  dlKrawedziSciany*1.0/2.0;
    start.y =  dlKrawedziSciany*1.0/2.0;
    start.z = -dlKrawedziSciany*1.0/2.0;

    pt dlLacznika = dlKrawedziSciany / (pt)((1 << rzadKrzywej) - 1);
    coordinate3d uklad = setCoordinate(-angleX, -angleY, move);



    fprintf(MyOut,
           "%%!PS-Adobe-2.0 EPSF-2.0\n"
           "%%%%BoundingBox: %.2lf %.2lf %.2lf %.2lf\n"
           "newpath\n", -rozmiarObrazka/2, -rozmiarObrazka/2, rozmiarObrazka/2, rozmiarObrazka/2);

    moveTo(intoSurface(start, odlObserwatora, uklad));
    hilbertCurve3d(rzadKrzywej, &start, odlObserwatora,
                   uklad,
                   dlLacznika, 0);

    fprintf(MyOut,
           ".3 setlinewidth\n"
           "stroke\n"
           "showpage\n"
           "%%Trailer\n"
           "%%EOF");

    fclose(outputFile);
    return 0;
}
