bool lessThanOrEqualDD(double a, double b){
    return a <= b;
}

bool lessThanOrEqualDI(double a, int b){
    return a <= b;
}

bool lessThanOrEqualII(int a, int b){
    return a <= b;
}

int main() {
    bool x = lessThanOrEqualII(3, 4);
    bool y = lessThanOrEqualII(3, 4);
    bool z = lessThanOrEqualDI(3.5, 3);
    return 0;
}