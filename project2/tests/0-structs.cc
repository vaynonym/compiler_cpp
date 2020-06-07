struct Point {
  double x;
  double y;
};

int main() {
  Point p;
  p.x = 1.0;
  p.y = 1;

  double sqrMag = p.x * p.x + p.y * p.y;
  int z = p.x;
}