int boolToInt(bool b) {
    return b ? 1 : 0;
}

int main() {
    double result = 100.0;
    while (result > 1.0) {
        result = result - 11.0; 
    }

    return boolToInt(result > 10.0);
}