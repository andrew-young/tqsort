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
| timsort  | 1.112 |0.362 |0.166 |
| pdqsort  | 0.500 |0.357 |0.231 |
| tqsort   | 0.281 |0.265 |0.121 |
| quadsort | 0.297 |0.306 |0.284 |


arraysize = 100, number of arrays = 100000, Time in seconds
---------------------

| Algorithm |Sortedness %0 |Sortedness %90 |
|---|--- |--- |
| timsort  | 0.722 |0.143 |
| pdqsort  | 0.184 |0.092 |
| tqsort   | 0.102 |0.065 |
| quadsort | 0.130 |0.082 |

strings - length 7
---------------
arraysize = 10000000, number of arrays = 1, Time in seconds
---------------------

Algorithm | Sortedness %0 | Sortedness %90 |
|---|--- |--- |
| timsort  | 2.074 |0.747 |
| pdqsort  | 2.016 |1.109 |
| tqsort   | 1.622 |0.535 |
| quadsort | 1.803 |1.127 |

arraysize = 100, number of arrays = 100000, Time in seconds
---------------------

Algorithm | Sortedness %0 | Sortedness %90 |
|---|--- |--- |
| timsort  | 0.872 |0.263 |
| pdqsort  | 0.310 |0.186 |
| tqsort   | 0.296 |0.182 |
| quadsort | 0.304 |0.197 |
