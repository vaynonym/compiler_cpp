bool eq(int a, int b){
	return a==b;
}

int main(){
	bool y = eq(3,4);

	int x = 100;
	int i = 0;
	do {
		x = x - i;
		int y;
		for (y = 0; y < 3; y++) {
			i = i + y;
		}
	} while (x > i);
	return x;

	do{
		int x = 3;
		do {
			y = eq(x,x + 1);

			int i;
			for (i = 10; x < i; i++) {
				x = x + i;
				{
					if (x == 13) {
						return -10;
					}
					else {
						{
							x = x - 3;
						}
					}
				}
				y = false;
			}
			return x;
		} while (y);
		
	}while(y);
	return 0;
}
