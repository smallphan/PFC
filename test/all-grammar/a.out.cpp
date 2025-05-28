#include <cmath>                                
#include <iostream>                             
                                                
float Power(double n, double k) {               
  bool neg = false;                             
  if (k < 0) neg = true, k = -k;                
  long long ink = k;                            
  double ans = std::pow(n, k - (double) ink);   
  while (ink) {                                 
    if (ink & 1) ans *= n;                      
    n *= n;                                     
    ink >>= 1;                                  
  }                                             
  return neg ? (1.0 / ans) : ans;               
}                                             

float calculate_perimeter(float radius) {
  return 2 * 3.1415926 * radius;
}

int mid(int a, int b) {
  return (a + b) / 2;
}

void draw_fractal(int stop, int dep, int x1, int y1, int x2, int y2, int x3, int y3) {
  if (dep > stop) {
    return ;
  } else {
    if (dep == 1) {
      printf("tria %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %s\n", (double) x1, (double) y1, (double) x2, (double) y2, (double) x3, (double) y3, "$ffffff");
    } else if (dep == 3) {
      printf("tria %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %s\n", (double) x1, (double) y1, (double) x2, (double) y2, (double) x3, (double) y3, "$ffffff");
    } else {
      printf("tria %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %s\n", (double) x1, (double) y1, (double) x2, (double) y2, (double) x3, (double) y3, "$e6c90d");
    }
    int x12 = mid(x1, x2);
    int y12 = mid(y1, y2);
    int x23 = mid(x2, x3);
    int y23 = mid(y2, y3);
    int x31 = mid(x3, x1);
    int y31 = mid(y3, y1);
    draw_fractal(stop, dep + 1, x1, y1, x12, y12, x31, y31);
    draw_fractal(stop, dep + 1, x2, y2, x23, y23, x12, y12);
    draw_fractal(stop, dep + 1, x3, y3, x31, y31, x23, y23);
  }
}

int main() {
  int x = 400,y = 400,radius = 200;
  printf("circ %.2lf %.2lf %.2lf %s\n", (double) x, (double) y, (double) radius, "$c20e0e");
  printf("line %.2lf %.2lf %.2lf %.2lf %.2lf %s\n", (double) x, (double) y + radius, (double) x + calculate_perimeter(radius), (double) y + radius, (double) 20, "$0e5b0a");
  int sx = 100,sy = 800,edge = 200,sep = 100;
  for (int i = 0; i < 6; i++) {
    int rx = sx + i * (edge + sep),ry = sy;
    printf("rect %.2lf %.2lf %.2lf %.2lf %s\n", (double) rx, (double) ry, (double) rx + edge, (double) ry + edge, "$6915f1");
  }
  draw_fractal(3, 0, 200, 1200, 200, 1800, 1400, 1800);
  draw_fractal(4, 0, 600, 1200, 1800, 1200, 1800, 1800);
  int index = 0,cx = 800,cy = 200;
  sep= 250;
  radius= 100;
  while(++index < 5) {
    printf("circ %.2lf %.2lf %.2lf %s\n", (double) cx, (double) cy, (double) radius, "$a910ae");
    cx= cx + sep;
  }
}


