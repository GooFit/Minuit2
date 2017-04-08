Reconversion of ROOT's Minuit2 to standalone package by Henry Schreiner.

Since this process will need to be repeated to capture the latest changes, a discription of the original process is provided. This was guided heavily by the current standalone package (the CMake files were originally developed for it).

* I removed the old makeiles/cmake files
* I removed the build directory
* I removed /doc as well
* I created the CMakeLists.txt files
* I removed extra files in inc, added Math and Fit from /math 
* I added any cxx files needed for Math symbols
* I ran the provided script to delete the unneeded header files
* I restored any header files called from other header files

To reconvert, the process should be easier, and only changes will need to be merged.
