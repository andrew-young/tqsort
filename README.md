# tqsort
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
<img src="/img/intlarge.png" height="432" width="1024">

arraysize = 100, number of arrays = 100000, Time in seconds
---------------------
<img src="/img/intsmall.png" height="432" width="1024">

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
| timsort  | 1.078 |0.358 |0.167 |
| pdqsort  | 0.492 |0.369 |0.250 |
| tqsort   | 0.272 |0.278 |0.134 |
| quadsort | 0.297 |0.311 |0.286 |

arraysize = 100, number of arrays = 100000, Time in seconds
---------------------

| Algorithm |Sortedness %0 |Sortedness %90 |
|---|--- |--- |
| timsort  | 0.732 |0.144 |
| pdqsort  | 0.193 |0.100 |
| tqsort   | 0.098 |0.075 |
| quadsort | 0.125 |0.087 |

strings - length 7
---------------
arraysize = 10000000, number of arrays = 1, Time in seconds
---------------------

Algorithm | Sortedness %0 | Sortedness %90 |
|---|--- |--- |
| timsort  | 2.105 |0.755 |
| pdqsort  | 1.965 |1.114 |
| tqsort   | 1.657 |0.701 |
| quadsort | 1.831 |1.108 |

arraysize = 100, number of arrays = 100000, Time in seconds
---------------------

Algorithm | Sortedness %0 | Sortedness %90 |
|---|--- |--- |
| timsort  | 0.873 |0.256 |
| pdqsort  | 0.312 |0.186 |
| tqsort   | 0.317 |0.199 |
| quadsort | 0.324 |0.191 |
