int f(double x) {
	return 0;
}

int three() {
	return 3;
}

int main() {
	double x = three();
	return f(three());
}
