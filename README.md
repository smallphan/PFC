# Pixel Format Compiler (PFC)

A compiler that translates image description code into actual images. It provides a simple and intuitive way to create images through programming.

## Features

- Draw basic shapes (lines, circles, triangles, rectangles)
- Support for custom colors (RGB hex format)
- Float-point coordinate system
- Function definitions and calls
- Control structures (if, for, while)
- Variable scoping
- Intermediate code generation
- Proxy code generation (C++)

## Prerequisites

Before building the project, ensure you have the following dependencies installed:

```bash
# Install Cairo graphics library development files
sudo apt-get install libcairo2-dev

# Install build essentials if not already installed
sudo apt-get install build-essential
```

## Building

To build the project:

```bash
# Clone the repository
git clone https://github.com/smallphan/PFC.git
cd compile

# Build all targets
make

# Build specific targets
make pfc-draw     # Build drawing component only
make pfc          # Build lexical analyzer only
```

## Installation

Add the bin directory to your system PATH:

```bash
# Add to ~/.bashrc or ~/.zshrc
export PATH=$PATH:/path/to/compile/bin
```

Then reload your shell or run:
```bash
source ~/.bashrc  # or source ~/.zshrc
```

## Usage

Basic command syntax:
```bash
pfc [options] input_file
```

Options:
- `-h` Show help message and exit
- `-o <filename>` Set output image filename
- `-d` Generate intermediate code
- `-c` Generate proxy code (C++)
- `-l` Generate lexical analysis results
- `-s <width> <height>` Set image dimensions

Examples:
```bash
# Show help
pfc -h

# Compile input.pf to output.png
pfc -o output input.pf

# Generate 800x600 image
pfc -s 800 600 input.pf

# Generate intermediate code
pfc -d input.pf

# Show lexical analysis
pfc -l input.pf
```

## Example Code

```
def calculate_perimeter(float radius) -> float {
  return 2 * 3.1415926 * radius;
}

def mid(int a, int b) -> int {
  return (a + b) / 2;
}

def draw_fractal(int stop, int dep, int x1, int y1, int x2, int y2, int x3, int y3) -> void {
  if (dep > stop) {
    return;
  } else {
    if (dep == 1) {
      draw triangle(vec(x1, y1), vec(x2, y2), vec(x3, y3), #ffffff);
    } else if (dep == 3) {
      draw triangle(vec(x1, y1), vec(x2, y2), vec(x3, y3), #ffffff);
    } else {
      draw triangle(vec(x1, y1), vec(x2, y2), vec(x3, y3), #e6c90d);
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

def main() -> int {
  
  // Draw a circle, then unfold its edge.
  int x = 40, y = 40, radius = 20;
  draw circle(vec(x, y), radius, #c20e0e);
  draw line(vec(x, y + radius), vec(x + calculate_perimeter(radius), y + radius), 2, #0e5b0a);

  // Draw a row of rectangles.
  int sx = 10, sy = 80, edge = 20, sep = 10;
  for (int i = 0; i < 6; i++) {
    int rx = sx + i * (edge + sep), ry = sy;
    draw rectangle(vec(rx, ry), vec(rx + edge, ry + edge), #6915f1);
  }

  // Draw two fractal shapes.
  draw_fractal(3, 0, 20, 120, 20, 180, 140, 180);
  draw_fractal(4, 0, 60, 120, 180, 120, 180, 180);

  // Draw a row of circles.
  int index = 0, cx = 80, cy = 20; 
  sep = 25; radius = 10;
  while (++index < 5) {
    draw circle(vec(cx, cy), radius, #a910ae);
    cx = cx + sep;
  }
  
}
```
