#include <bits/stdc++.h>
#include "Matrix.h"
#include "Vector.h"
#include "bitmap_image.hpp"

using namespace std;

static unsigned long int g_seed = 1;

inline int random_value()
{
g_seed = (214013 * g_seed + 2531011);
return (g_seed >> 16) & 0x7FFF;
}

class Triangle{
    public:
        Points p1, p2, p3;
        Triangle(Points p1, Points p2, Points p3){
            this->p1 = p1;
            this->p2 = p2;
            this->p3 = p3;
        }
        int r,g,b;
};

void Stage_1(fstream& scene){
    stack<Matrix> s;
    Matrix m;
    m = m.Identity();
    bool end = false;
    fstream stage1;
    stage1.open("stage1.txt", ios::out);
    while(!end){
        string cmd;
        scene >> cmd;
        if(cmd == "end"){
            end = true;
        }
        else if(cmd == "triangle"){
            for(int i=0; i<3; i++){
                Points tr(1);
                scene >> tr.mat[0][0] >> tr.mat[1][0] >> tr.mat[2][0];
                tr = m * tr;
                tr.print(stage1);
            }
            stage1 << endl;
        }

        else if(cmd == "translate"){
            double tx, ty, tz;
            scene >> tx >> ty >> tz;
            Matrix t;
            t = t.Identity();
            t.mat[0][3] = tx;
            t.mat[1][3] = ty;
            t.mat[2][3] = tz;
            m = m * t;
            // cout << "M:" << endl;
            // m.print();
        }
        else if(cmd == "scale"){
            double sx, sy, sz;
            scene >> sx >> sy >> sz;
            Matrix t;
            t = t.Identity();
            t.mat[0][0] = sx;
            t.mat[1][1] = sy;
            t.mat[2][2] = sz;
            m = m * t;
            // cout << "M:" << endl;
            // m.print();
        }
        else if(cmd == "rotate"){
            double angle, rx, ry, rz;
            scene >> angle >> rx >> ry >> rz;
            double rad = angle * M_PI / 180;
            double c = cos(rad);
            double s = sin(rad);
            Matrix t;
            t = t.Identity();
            Vector v(rx, ry, rz);
            v = Normalize(v);
            Vector c1 = Rodrigues(Vector(1,0,0), v, rad);
            Vector c2 = Rodrigues(Vector(0,1,0), v, rad);
            Vector c3 = Rodrigues(Vector(0,0,1), v, rad);
            t.mat[0][0] = c1.x;
            t.mat[0][1] = c2.x;
            t.mat[0][2] = c3.x;
            t.mat[1][0] = c1.y;
            t.mat[1][1] = c2.y;
            t.mat[1][2] = c3.y;
            t.mat[2][0] = c1.z;
            t.mat[2][1] = c2.z;
            t.mat[2][2] = c3.z;
            m = m * t;
            // cout << "M:" << endl;
            // m.print();
        }
        else if(cmd == "push"){
            s.push(m);
            // m = m.Identity();
        }
        else if(cmd == "pop"){
            m = s.top();
            s.pop();
        }
        else{
            cout << "Invalid command" << endl;
        }
    }
    stage1.close();
}

void Stage_2(Vector eye, Vector l, Vector u, Vector r){
    Matrix T;
    T = T.Identity();
    T.mat[0][3] = -eye.x;
    T.mat[1][3] = -eye.y;
    T.mat[2][3] = -eye.z;
    Matrix R;
    R = R.Identity();
    R.mat[0][0] = r.x;
    R.mat[0][1] = r.y;
    R.mat[0][2] = r.z;
    R.mat[1][0] = u.x;
    R.mat[1][1] = u.y;  
    R.mat[1][2] = u.z;
    R.mat[2][0] = -l.x;
    R.mat[2][1] = -l.y;
    R.mat[2][2] = -l.z;
    Matrix V = R * T;

    fstream stage1;
    stage1.open("stage1.txt", ios::in);
    fstream stage2;
    stage2.open("stage2.txt", ios::out);
    while(true){
        for(int i=0; i<3; i++){
            Points tr(1);
            stage1 >> tr.mat[0][0] >> tr.mat[1][0] >> tr.mat[2][0];
            if(stage1.eof()) break;
            tr = V * tr;
            tr.print(stage2);
        }
        if(stage1.eof()) break;
        stage2 << endl;
    }
    stage1.close();
    stage2.close();
}

void Stage_3(double fovY, double aspectRatio, double near, double far){
    double fovX = fovY * aspectRatio;
    double t = near * tan((fovY*M_PI/180.0)/2);
    double r = near * tan((fovX*M_PI/180.0)/2);
    Matrix P;
    P.mat[0][0] = near/r;
    P.mat[1][1] = near/t;
    P.mat[2][2] = -(far+near)/(far-near);
    P.mat[2][3] = -(2*far*near)/(far-near);
    P.mat[3][2] = -1;

    fstream stage2;
    stage2.open("stage2.txt", ios::in);
    fstream stage3;
    stage3.open("stage3.txt", ios::out);
    while(true){
        for(int i=0; i<3; i++){
            Points tr(1);
            stage2 >> tr.mat[0][0] >> tr.mat[1][0] >> tr.mat[2][0];
            if(stage2.eof()) break;
            tr = P * tr;
            tr.print(stage3);
        }
        if(stage2.eof()) break;
        stage3 << endl;
    }
    stage2.close();
    stage3.close();
}

void Z_Buffer(double Screen_width, double Screen_height){
    vector<Triangle> triangles;
    
    // file inputs , creating triangles and specifying their colors and pushing them into a vector
    fstream stage3;
    stage3.open("stage3.txt", ios::in);
    fstream z_buffer;
    z_buffer.open("z_buffer.txt", ios::out);
    while(true){
        Points p1, p2, p3;
        stage3 >> p1.mat[0][0] >> p1.mat[1][0] >> p1.mat[2][0];
        stage3 >> p2.mat[0][0] >> p2.mat[1][0] >> p2.mat[2][0];
        stage3 >> p3.mat[0][0] >> p3.mat[1][0] >> p3.mat[2][0];
        // cout << "p1: " << p1.mat[0][0] << " " << p1.mat[1][0] << " " << p1.mat[2][0] << endl;
        // cout << "p2: " << p2.mat[0][0] << " " << p2.mat[1][0] << " " << p2.mat[2][0] << endl;
        // cout << "p3: " << p3.mat[0][0] << " " << p3.mat[1][0] << " " << p3.mat[2][0] << endl;
        if(stage3.eof()) break;
        Triangle t(p1, p2, p3);
        t.r = random_value() % 256;
        t.g = random_value() % 256;
        t.b = random_value() % 256;
        triangles.push_back(t);
    }

    //Specifying the top and bottom of the screen
    double dx = 2.0 / Screen_width;
    double dy = 2.0 / Screen_height;
    double Top_y = 1.0 - dy/2;
    double Left_x = -1.0 + dx/2;
    double Bottom_y = -1.0 + dy/2;
    double Right_x = 1.0 - dx/2;
    double **z_arr = new double*[(int)Screen_height];
    double z_min = -1.0;
    double z_max = 1.0;

    for(int i=0; i<Screen_height; i++){
        z_arr[i] = new double[(int)Screen_width];
        for(int j=0; j<Screen_width; j++){
            z_arr[i][j] = 1.0;
        }
    }

    bitmap_image image(Screen_width, Screen_height);
    for(int i=0; i<Screen_height; i++){
        for(int j=0; j<Screen_width; j++){
            image.set_pixel(j, i, 0, 0, 0); // creating a bit image and setting all pixels to black
        }
    }

    for(int i=0; i<triangles.size(); i++){
        double max_y = max(triangles[i].p1.mat[1][0], max(triangles[i].p2.mat[1][0], triangles[i].p3.mat[1][0]));
        double min_y = min(triangles[i].p1.mat[1][0], min(triangles[i].p2.mat[1][0], triangles[i].p3.mat[1][0]));
        int Top_scanline = round(((Top_y - min(Top_y,max_y)) / dy));
        // cout << "Top_scanline: " << Top_scanline << endl;
        int Bottom_scanline = (int)Screen_height - round(((max(Bottom_y,min_y) - Bottom_y) / dy));
        // cout << "Bottom_scanline: " << Bottom_scanline << endl;
        for(int row=Top_scanline; row<=Bottom_scanline; row++){
            double current_scanline_y = Top_y - row*dy;
            // cout << "current_scanline_y: " << current_scanline_y << endl;
            double max_x = -1000, min_x = 1000;
            double z_a, z_b;
            Points p, q;
            for(int k=0; k<3; k++){
                if(k==0){
                    p = triangles[i].p1;
                    q = triangles[i].p2;
                }
                else if(k==1){
                    p = triangles[i].p2;
                    q = triangles[i].p3;
                }
                else{
                    p = triangles[i].p3;
                    q = triangles[i].p1;
                }
                // cout << "p: " << p.mat[0][0] << " " << p.mat[1][0] << " " << p.mat[2][0] << endl;
                // cout << "q: " << q.mat[0][0] << " " << q.mat[1][0] << " " << q.mat[2][0] << endl;
                double x = p.mat[0][0] + (current_scanline_y - p.mat[1][0]) * ((q.mat[0][0] - p.mat[0][0]) / (q.mat[1][0] - p.mat[1][0]));
                // cout << "x: " << x << endl;
                if(x>=min(p.mat[0][0], q.mat[0][0]) && x<=max(p.mat[0][0], q.mat[0][0])){
                    if(p.mat[1][0] != q.mat[1][0]){
                        double z = p.mat[2][0] + (current_scanline_y - p.mat[1][0]) * ((q.mat[2][0] - p.mat[2][0]) / (q.mat[1][0] - p.mat[1][0]));
                        if(x < min_x){
                            min_x = x;
                            z_a = z;
                        }
                        if(x > max_x){
                            max_x = x;
                            z_b = z;
                        }
                    }
                    else{
                        if(p.mat[1][0] == current_scanline_y){
                            if(p.mat[0][0] < min_x){
                                min_x = p.mat[0][0];
                                z_a = p.mat[2][0];
                            }
                            if(p.mat[0][0] > max_x){
                                max_x = p.mat[0][0];
                                z_b = p.mat[2][0];
                            }
                            if(q.mat[0][0] < min_x){
                                min_x = q.mat[0][0];
                                z_a = q.mat[2][0];
                            }
                            if(q.mat[0][0] > max_x){
                                max_x = q.mat[0][0];
                                z_b = q.mat[2][0];
                            }
                        }
                    }
                }

                // cout << "min_x: " << min_x << " max_x: " << max_x << endl;
                int left_intersecting_col = round((max(Left_x, min_x) - Left_x) / dx);
                int right_intersecting_col = (int)Screen_width - round((Right_x - min(Right_x, max_x))/dx);

                for(int col=left_intersecting_col; col<=right_intersecting_col; col++){
                    double current_scanline_x = Left_x + col*dx;
                    double z = z_a + (current_scanline_x - min_x) * ((z_b - z_a) / (max_x - min_x));
                    if(z>=z_min&&z<=z_max){
                        if(z < z_arr[row][col]){
                            z_arr[row][col] = z;
                            image.set_pixel(col, row, triangles[i].r, triangles[i].g, triangles[i].b);
                        }
                    }
                }
            }
        }
    }

    for(int i=0; i<Screen_height; i++){
        for(int j=0; j<Screen_width; j++){
            if(z_arr[i][j]<z_max)
                z_buffer << z_arr[i][j] << "\t";
        }
        z_buffer << endl;
    }

    stage3.close();
    z_buffer.close();
    for(int i=0; i<Screen_height; i++){
        delete[] z_arr[i];
    }
    delete[] z_arr;
    image.save_image("out.bmp");
    image.clear();
}

int main() {
    
    double Screen_width, Screen_height;
    double eyeX, eyeY, eyeZ;
    double lookX, lookY, lookZ;
    double upX, upY, upZ;
    double fovY, aspectRatio, near, far;

    //read data from screen.txt and configure.txt
    fstream config;
    config.open("config.txt", ios::in);
    config >> Screen_width >> Screen_height;
    config.close();
    fstream scene;
    scene.open("scene.txt", ios::in);
    scene >> eyeX >> eyeY >> eyeZ;
    scene >> lookX >> lookY >> lookZ;
    scene >> upX >> upY >> upZ;
    scene >> fovY >> aspectRatio >> near >> far;
    Vector eye(eyeX, eyeY, eyeZ);
    Vector look(lookX, lookY, lookZ);
    Vector up(upX, upY, upZ);
    Vector l = Normalize(look - eye);
    Vector r = Normalize(l | up);
    Vector u = r | l;
    
    Stage_1(scene);
    scene.close();
    Stage_2(eye, l, u, r);
    Stage_3(fovY, aspectRatio, near, far);
    Z_Buffer(Screen_width, Screen_height);
    return 0;
}