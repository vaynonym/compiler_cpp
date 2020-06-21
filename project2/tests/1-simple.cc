bool test(bool a, bool b) {
    return a == b;
}

int main() {
    bool b = test(true, false);
    bool x = b == false;

    return 0;
}