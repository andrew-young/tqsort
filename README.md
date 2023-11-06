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
| timsort  | 1.122 |0.375 |0.169 |
| pdqsort  | 0.150 |0.249 |0.258 |
| tqsort   | 0.271 |0.266 |0.118 |
| quadsort | 0.302 |0.306 |0.288 |


arraysize = 100, number of arrays = 100000, Time in seconds
---------------------

| Algorithm |Sortedness %0 |Sortedness %90 |
|---|--- |--- |
| timsort  | 0.719 |0.143 |
| pdqsort  | 0.152 |0.094 |
| tqsort   | 0.099 |0.066 |
| quadsort | 0.130 |0.082 |

strings - length 7
---------------
arraysize = 10000000, number of arrays = 1, Time in seconds
---------------------

Algorithm | Sortedness %0 | Sortedness %90 |
|---|--- |--- |
| timsort  | 2.097 |0.744 |
| pdqsort  | 1.924 |1.126 |
| tqsort   | 1.693 |0.536 |
| quadsort | 1.842 |1.089 |

arraysize = 100, number of arrays = 100000, Time in seconds
---------------------

Algorithm | Sortedness %0 | Sortedness %90 |
|---|--- |--- |
| timsort  | 0.877 |0.255 |
| pdqsort  | 0.310 |0.187 |
| tqsort   | 0.300 |0.180 |
| quadsort | 0.302 |0.194 |
