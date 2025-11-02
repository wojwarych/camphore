# camphore
Small clone of `ls` POSIX utility

## Usage
Built with cmd `clang -std=c99 -o <source_file> main.c file_items.c` will produce a binary that can be used.  
Tested and worked on WSL Ubuntu 24.04.03 LTS.

### Flags currently implemented
```
-l
  print the output of the directory in list format
-a
  print all the items of directory, including ones starting with '.'
-R
  recursive mode
```
