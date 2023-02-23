// Written by Dylan Celius
// 1/22/23

#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include "string.h"
using namespace std;



double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
double lerp(double t, double a, double b) { return a + t * (b - a); }
double dot(double v1[2], double v2[2]){ return v1[0] * v2[0] + v1[1] * v2[1]; }
double* getVector(int value){
    int k = value % 4;
	if (k == 0) {
		return new double[2] {1.0, 1.0};
    }
	else if (k == 1){
		return new double[2] {-1.0, 1.0};
    }
    else if (k == 2){
        return new double[2] {-1.0, -1.0};
    }
    else{
		return new double[2] {1.0, -1.0};
    }
}
int* generatePermutations(){
	int* p = new int[256];
	for (int i = 0; i < 256; i++) {
		p[i] = i;
	}
	int* s = new int[256];
	int index;
	for (int i = 0; i < 256; i++) {
		s[i] = -1;
	}
	for (int i = 0; i < 256; i++) {
		index = rand() % 256;
		while (s[index] != -1) {
			index = (index + 1) % 256;
		}
		s[index] = p[i];
	}
	return s;
} 
double noise(double x, double y, int* p) {
	int permx = (int) floor(x) % 256;
	int permy = (int) floor(y) % 256;
	x -= floor(x);
	y -= floor(y);
	double fadex = fade(x);
	double fadey = fade(y);
	int valtr = p[p[permx + 1] + permy + 1];
	int valtl = p[p[permx] + permy + 1];
	int valbr = p[p[permx + 1] + permy];
	int valbl = p[p[permx] + permy];
	double tr[2] = {x - 1.0, y - 1.0};
	double tl[2] = {x, y - 1.0};
	double br[2] = {x - 1.0, y};
	double bl[2] = {x, y};
	double dtr = dot(tr, getVector(valtr));
	double dtl = dot(tl, getVector(valtl));
	double dbr = dot(br, getVector(valbr));
	double dbl = dot(bl, getVector(valbl));
	return lerp(fadex, lerp(fadey, dbl, dtl), lerp(fadey, dbr, dtr));
}

float ratio(int x, int y, int originx, int originy, int a=1){
    // Ratio based on the distance to the center of the image
    return a / (a + sqrt(pow(x - originx, 2) + pow(y - originy, 2)));
}

void createImage(int ***image, int width, int height, int seed){
    double new_ratio;
    // Generate a new sequence of random numbers using the seed
    srand(seed);
    // Find random values to make a random color
    int r = rand() % 255;
    int g = rand() % 255;
    int b = rand() % 255;
    double a;
    double f;
    double min = 9999.0;
    double max = -9999.0;
    int* p = generatePermutations();
    double** map = new double*[width];
    for (int i = 0; i < width; i++){
        map[i] = new double[height];
        for (int j = 0; j < height; j++){
            // Find a ratio to multiply the color values by
            a = 1.0;
            f = 0.01;
            new_ratio = 0;
            for (int k = 0; k < 8; k++){
                new_ratio += a*noise(i * f, j * f, p);
                a *= 0.5;
                f *= 2.0;
            }
            new_ratio += 1.0;
            new_ratio *= 0.5;
            max = (max < new_ratio) ? new_ratio : max;
            min = (min > new_ratio) ? new_ratio : min;
            map[i][j] = new_ratio;
        }
    }
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            new_ratio = map[i][j];
            new_ratio = (new_ratio - min) / (max - min);
            if (new_ratio < 0.45) {
                new_ratio = 0.33;
                image[i][j][0] = new_ratio*(255-r);
                image[i][j][1] = new_ratio*(255-g);
                image[i][j][2] = new_ratio*(255-b);
            }
            else if (new_ratio < 0.55) {
                new_ratio = 0.66;
                image[i][j][0] = new_ratio*((r+255)/2);
                image[i][j][1] = new_ratio*((g+255)/2);
                image[i][j][2] = new_ratio*((b+255)/2);
            }
            else {
                new_ratio = 1.0;
                image[i][j][0] = new_ratio*r;
                image[i][j][1] = new_ratio*g;
                image[i][j][2] = new_ratio*b;
            }
        }
    }
}

void printImage(int ***image, int width, int height){
    // Open file
    ofstream image_file;
    time_t now = time(0);
    tm *local = localtime(&now);
    // Base filename on time, date, and file dimensions
    string filename = to_string(local->tm_hour) + "-" + to_string(local->tm_min) + "-" + to_string(local->tm_sec);
    filename.append("_" + to_string(1 + local->tm_mon) + "-" + to_string(local->tm_mday) + "-" + to_string(1900 + local->tm_year));
    filename.append("_" + to_string(width) + "x" + to_string(height));
    filename = filename.append(".ppm");
    image_file.open(filename);
    string pixel;
    string line;
    // Attach header lines
    image_file << "P3" << endl;
    image_file << width << " " << height << endl;
    image_file << "255" << endl;
    // Append 1 pixel's data to each line
    for(int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            pixel = "";
            for (int k = 0; k < 3; k++){
                pixel += to_string(image[i][j][k]);
                // Don't add unnecessary whitespace
                if (k < 2){
                    pixel += " ";
                }
            }
            image_file << pixel << endl;
        }
    }
    // Close the file
    image_file.close();
}

int main(int argc, char *argv[]){
    // Catch for no arguments
    if (argc <= 1) {
        cerr << "Invalid arguments, please enter a image description filename" << endl;
        return -1;
    }

    // Catch for invalid filename
    ifstream image_descriptor;
    try{
        image_descriptor.open(argv[1]);
    }
    catch (exception e) {
        cerr << "Invalid filename" << endl;
        return -1;
    }
    string line;
    string keyword;
    bool keyword_found = false;
    int index;
    int width;
    int height;
    srand(time(0));
    int seed = rand();
    if (image_descriptor.is_open()){
        // Read each line in the file
        while (getline(image_descriptor, line)){
            index = line.find(" ");
            // If there is no keyword and value combo, skip
            if (index != string::npos){
                // Read keyword
                keyword = line.substr(0, index);
                if (keyword.compare("imsize") == 0){
                    keyword_found = true;
                    // Try to read in width and height
                    try{
                        line.erase(0, index + 1);
                        index = line.find(" ");
                        width = stoi(line.substr(0, index));
                        line.erase(0, index + 1);
                        index = line.find("\n");
                        height = stoi(line.substr(0, index));
                        if (width <= 0 || height <= 0) throw invalid_argument("dimensions cannot be 0 or negative");
                    }
                    catch (exception e){
                        cout << "Invalid arguments, please recheck image descriptor guidelines" << endl;
                        image_descriptor.close();
                        return -1;
                    }
                }
                // Read for seed keyword
                if (keyword.compare("seed") == 0){
                    // Try to save seed
                    try{
                        line.erase(0, index + 1);
                        seed = stoi(line);
                    }
                    catch (exception e){
                        cout << "Invalid seed, must be an integer, using random seed" << endl;
                    }
                }
            }
        }
        // If imsize was not found in file, return with an error
        if (!keyword_found){
            cerr << "Invalid arguments, please recheck image descriptor guidelines" << endl;
            image_descriptor.close();
            return -1;            
        }
        // Close file
        image_descriptor.close();
    }
    else{
        cerr << "Invalid filename" << endl;
        return -1;
    }

    // Intialize memory block for new picture
    int ***newImage = new int**[width];
    for(int i = 0; i < width; i++){
        newImage[i] = new int*[height];
        for (int j = 0; j < height; j++){
            newImage[i][j] = new int[3];
            for (int k = 0; k < 3; k++){
                // Instantiate each pixel and rgb value to 255
                newImage[i][j][k] = 255;
            }
        }
    }
    // Run the new picture array through the image creator
    try {
        createImage(newImage, width, height, seed);
    }
    catch (exception e){
        cerr << "Something went wrong during image manipulation";
        cerr << e.what() << endl;
    }

    // Save array to file
    try{
        printImage(newImage, width, height);
    }
    catch (exception e){
        cerr << "Something went wrong during image saving";
        cerr << e.what() << endl;
    }

    return 0;
}