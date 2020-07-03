int add(int a, int b) {
	return a + b;
}

int sub(int a, int b) {
	return a - b;
}

int main() {
	int x = 2;
	double y = add(x, 5 + 3);

	{
		int x = 3;
		int z = sub(x, 5 + 3);
	}

	x = add(x, 5 + 3);

	return x;
}
