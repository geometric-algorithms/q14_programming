# Simple arrangement of lines

Problem statement:
(50 = 25+25 points) Let L be a set of n lines in R2 .
	(a) Compute the arrangement A(L) using incremental construction.
	Input: A set L of n lines in general position (no two parallel, no three concurrent).
	Output: The full arrangement A(L), i.e., the subdivision of the plane induced by the lines.
	Time complexity: O(n2 ) time.

	(b) Given another line ℓ, report the set of cells in A(L) that are intersected by ℓ.
	Input: The arrangement A(L) and a query line ℓ.
	Output: All faces (cells) of the arrangement that intersect ℓ.
	Time complexity: Proportional to the number of intersected cells, plus logarithmic overhead if a point location data structure is used to begin the walk along ℓ.
# Instructions to run the code

There are 2 sub-folders for each part of the problem. execute `g++ main.cpp` to compile the code and `./a.out` to run the program
Or u can just run `./test.sh`

# Inputs
for (a), Input format is as follows:

```n
x11 y11 x12 y12
x21 y21 x22 y22
...
xn1 yn1 xn2 yn2
```

for (b):
```n
x11 y11 x12 y12
x21 y21 x22 y22
...
xn1 yn1 xn2 yn2
qx1 qy1 qx2 qy2
```

