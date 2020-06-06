int main() {
  int x = 3;
  double y = 3.9;
  int compare = x <=> y;

  bool compResult = compare <= 0;
  int z = 3.0 <=> compResult;
}