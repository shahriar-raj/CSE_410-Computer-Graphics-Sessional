#include <bits/stdc++.h>

using namespace std;

class Points{
    public:
        double mat[4][1];
        Points();
        Points(int n);
        // Points operator*(const Points &m);
        void print(fstream& file);
};

Points::Points(){
    for(int i=0; i<4; i++){
            mat[i][0] = 0;
    }
}

Points::Points(int n){
    for(int i=0; i<4; i++){
            mat[i][0] = n;
    }
}

 void Points::print(fstream& file){
    for(int i=0; i<3; i++){
        file << fixed << setprecision(7) << mat[i][0]/mat[3][0] << " ";
    }
    file << endl;
}


