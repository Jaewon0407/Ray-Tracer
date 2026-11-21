Look further into negative or positive z axis for viewing direction towards the viewport

## Build & run

```bash
g++ -std=c++20 -O2 -pthread src/main.cpp -o src/main
cd src && ./main
```

The image is written to `data/output.ppm`. Rendering uses every CPU core by default; set `cam.num_threads` in `main.cpp` to change that.
