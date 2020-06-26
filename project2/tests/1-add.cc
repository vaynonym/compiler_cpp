int dop(int a, int b){
	return a + b;
}

bool eq(int a, int b){
	return a == b;
}

int main(){
	int x = 3;
	x = 4;
	
	double y = 3.4 + dop(x,7);
	bool b = eq(x, x);
	return 0;
}

