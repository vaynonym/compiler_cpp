bool eq(int a, int b){
	return a==b;
}

int main(){
	bool y = eq(3,4);
	do{
		int x = 3;
		y = eq(x,x + 1);
		
	}while(y);
	return 0;
}
