# 8-Puzzle-Solver

Experiment 6 of the course "the Design and Analysis of Algorithm" (also known as "aigezen")

## Tips

1. You can download the zip and run the solver locally. But you have to access the page via HTTP protocol because the [CORS](https://developer.mozilla.org/en-US/docs/Glossary/CORS) policies from the browser prevent the script from fetching the WebAssembly. 
2. If you found that the solver is not working correctly, please check if your browser support WebAssembly. 
3. To change the goal state, simply add `?abcdefghi` after the URL or press `F12` to open the DevTools and input `goal = "abcdefghi"` in the console to change the goal state manually, which `a`~`i` are corresponding to the number in below grid (0 as the empty cell). 

| a | b | c |
| - | - | - |
| d | e | f |
| g | h | i |
