#include <bits/stdc++.h>
#include "Points.h"

using namespace std;

class Matrix{
    public:
        double mat[4][4];
        Matrix();
        Matrix(int n);
        Matrix operator*(const Matrix &m);
        Points operator*(const Points &p);
        void print();
        Matrix Identity();
};

Matrix::Matrix(){
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            mat[i][j] = 0;
        }
    }
}

Matrix::Matrix(int n){
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            mat[i][j] = n;
        }
    }
}

Matrix Matrix::operator*(const Matrix &m){
    Matrix ret;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            for(int k=0; k<4; k++){
                ret.mat[i][j] += mat[i][k] * m.mat[k][j];
            }
        }
    }
    return ret;
 }

 Points Matrix::operator*(const Points &p){
    Points ret;
    for(int i=0; i<4; i++){
        for(int k=0; k<4; k++){
            ret.mat[i][0] += mat[i][k] * p.mat[k][0];
        }
    }
    return ret;
 }

 void Matrix::print(){
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            cout << mat[i][j] << " ";
        }
        cout << endl;
    }
}

Matrix Matrix::Identity(){
    Matrix ret;
    for(int i=0; i<4; i++){
        ret.mat[i][i] = 1;
    }
    return ret;
}

