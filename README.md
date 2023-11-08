# QTsort
A stable mergsort algorithm in c++;  
This sorting algorithm combines ideas of quadsort and timsort.  Gaining the benifits of both.  
In all of my test cases its faster then both timsort and quadsort.  

Timsort  - https://github.com/timsort/cpp-TimSort
Quadsort - https://github.com/scandum/quadsort

Like Timsort this creates runs and uses a stack to remember runs to merge them.  
Like Quadsort it merges 4 runs at a time.  
It merges 4 consecutive runs when run1 < 2 * run4. where run4 is the last run on the stack.  
Much of the merging procedures come from quadsort.

Benchmark
----------

Sorted % - array is sorted than each element has a percent chance of being changed to a random value.  Sortedness 0% is 100% random.  

Integers
---------------
arraysize = 10000000, number of arrays = 1, Time in seconds
---------------------
<img src="/img/intlarge.png">

arraysize = 100, number of arrays = 100000, Time in seconds
---------------------
<img src="/img/intsmall.png" width="1024">

strings - length 7
---------------
arraysize = 10000000, number of arrays = 1, Time in seconds
---------------------
<img src="/img/largestring.png" height="432" width="1024">

arraysize = 100, number of arrays = 100000, Time in seconds
---------------------
<img src="/img/smallstring.png" height="432" width="1024">


Sortedness - array is sorted than each element has a percent chance of being changed to a random value.  Sortedness 0% is 100% random.  
pdfsort_branchless used for integers.  
Quadsort - I ported to c++. used the branchless version of head_merge_branchless.  

Integers
---------------
arraysize = 10000000, number of arrays = 1, Time in seconds

| Algorithm |Sortedness %0 |Sortedness %80 |Sortedness %95 |
|---|--- |--- |--- |
| timsort  | 1.109 |0.371 |0.169 |
| pdqsort  | 0.154 |0.251 |0.245 |
| tqsort   | 0.272 |0.270 |0.122 |
| quadsort | 0.300 |0.306 |0.287 |


arraysize = 100, number of arrays = 100000, Time in seconds
---------------------

| Algorithm |Sortedness %0 |Sortedness %90 |
|---|--- |--- |
| timsort  | 0.726 |0.141 |
| pdqsort  | 0.147 |0.090 |
| tqsort   | 0.102 |0.071 |
| quadsort | 0.123 |0.084 |

strings - length 7
---------------
arraysize = 10000000, number of arrays = 1, Time in seconds
---------------------

Algorithm | Sortedness %0 | Sortedness %90 |
|---|--- |--- |
| timsort  | 2.128 |0.746 |
| pdqsort  | 1.938 |1.135 |
| tqsort   | 1.689 |0.553 |
| quadsort | 1.818 |1.118 |


arraysize = 100, number of arrays = 100000, Time in seconds
---------------------

Algorithm | Sortedness %0 | Sortedness %90 |
|---|--- |--- |
| timsort  | 0.883 |0.255 |
| pdqsort  | 0.311 |0.184 |
| tqsort   | 0.296 |0.184 |
| quadsort | 0.311 |0.193 |

