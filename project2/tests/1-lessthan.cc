bool func(int a, int b){
	return a < b;
}


int main(){
	int x = 4;
	double y = 3.4;
	bool b = func(x,6) == x < y;
	return 0;
}



