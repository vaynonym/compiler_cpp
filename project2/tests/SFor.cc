int loop(int a, int b){
    int x = 4;
    x++;
    for(a = 0; a <= 2; a++){
        b++;
    }

    return a + b;
}

int main() {
    bool c = loop(0, 0) <= 10;
    return 0;
}