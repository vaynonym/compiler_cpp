bool doubleEq(double x, double expected) {
    return x == expected;
}

int doubleCmp(double a, double b) {
    if (doubleEq(a, b)) return 0;
    else {
        if (a < b) return -1;
        else return 1;
    }

    return -100000;
}

bool quadCmp(double a, double b) {
    return doubleCmp(a, b) == doubleCmp(a, b);
}

int trueInt() {
    return 100;
}

int falseInt() {
    return 10;
}

int boolToInt(bool b) {
    return b ? trueInt() : falseInt();
}

double three() {
    return 3.0;
}

double nine() {
    return 9.0;
}

void test() {
    int a = 3;
    a = a + 5;
    return;
    return;
}

int main() {
    double a = three();
    double b = nine();

    int twc = 0;//a <=> b;
    int cmp = doubleCmp(a, b);

    test();

    bool result = twc == cmp;
    result = result && quadCmp(a, b);

    test();

    return boolToInt(result);
}