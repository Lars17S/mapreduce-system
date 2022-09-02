# mapreduce-system

MapReduce system created as a schoolar project. 

This program sorts all the words in an input TXT file, in this case the Alice in Wonderland book. It can then search using Binary Search for any word given as input and output the lines where the word is found. MapReduce is used to make the process faster and more efficient.

How to compile
```
gcc mapper.c -lpthread -o mapper
gcc reduce.c -o reduce
gcc find.c -o find
```

How to use
```
Run ./find and input the word you want to search.
```
