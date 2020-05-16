int main() {
  int x = 0;
  const int y = x;
  constinit char z = 12312;
  constinit const string something = "hello";

  const int y, b = x, c;
  constinit int& z, hallo;
  constinit const int& test1, test2 = 3, test3;

  constinit int & z, hallo;
  constinit const int & test1, test2 = 3, test3;

  const constinit int a = 0;
}