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
| timsort  | 1.092 |0.371 |0.170 |
| pdqsort  | 0.156 |0.248 |0.248 |
| tqsort   | 0.292 |0.285 |0.140 |
| quadsort | 0.311 |0.294 |0.286 |

arraysize = 100, number of arrays = 100000, Time in seconds
---------------------

| Algorithm |Sortedness %0 |Sortedness %90 |
|---|--- |--- |
| timsort  | 0.403 |0.129 |
| pdqsort  | 0.141 |0.108 |
| tqsort   | 0.107 |0.078 |
| quadsort | 0.153 |0.095 |

strings - length 7
---------------
arraysize = 10000000, number of arrays = 1, Time in seconds
---------------------

Algorithm | Sortedness %0 | Sortedness %90 |
|---|--- |--- |
| timsort  | 2.119 |0.754 |
| pdqsort  | 2.073 |1.147 |
| tqsort   | 1.797 |0.687 |
| quadsort | 1.838 |1.110 |

arraysize = 100, number of arrays = 100000, Time in seconds
---------------------

Algorithm | Sortedness %0 | Sortedness %90 |
|---|--- |--- |
| timsort  | 0.547 |0.266 |
| pdqsort  | 0.303 |0.188 |
| tqsort   | 0.313 |0.172 |
| quadsort | 0.313 |0.203 |
