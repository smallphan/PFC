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
def calculate(float num) -> float {
    return num * num * 3.1415926;
}

def main() -> int {
    // Draw a red line
    draw line(vec(10, 10), vec(40, 40), #ff0000, 3);

    // Draw 10 green circles
    for (int i = 1; i <= 10; i++) {
        draw circle(vec(50 + i * 10, 50 + i * 10), #00ff00, 5);
    }

    // Draw shapes based on conditions
    int num = 10;
    if (num == 10) {
        draw triangle(vec(60, 20), vec(60, 40), vec(120, 30), #0000ff);
    } else {
        draw rectangle(vec(20, 80), vec(40, 150), #fff000);
    }
}
```
