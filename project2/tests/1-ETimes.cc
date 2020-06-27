double timesDD(double a, double b){
    return a * b;
}

double timesDI(double a, int b){
    return a * b;
}

int timesII(int a, int b){
    return a * b;
}

int main() {
    double y;
    y = 3.0;
    int x = timesII(3, 4);
    y = timesII(3, 4);
    double z = timesDI(y, 3);
    double ultraZ = timesDD(4.5, 3.5);
    return 0;
}