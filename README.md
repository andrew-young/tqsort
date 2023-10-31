# tqsort
A stable mergsort algorithm
sorting algorithm combining ideas of quadsort and timsort

integers

arraysize = 10000000, number of arrays = 1
---------------------
| Algorithm |Sortedness %0 |Sortedness %80 |Sortedness %95 | Time(s) |
-----------------------
| timsort  | 1.082 |0.355 |0.170 |
| pdqsort  | 0.147 |0.262 |0.263 |
| tqsort   | 0.292 |0.261 |0.154 |
| quadsort | 0.292 |0.308 |0.302 |

arraysize = 100, number of arrays = 100000
---------------------
| Algorithm |Sortedness %0 |Sortedness %90 | Time(s) |
-----------------------
| timsort  | 0.398 |0.141 |
| pdqsort  | 0.154 |0.107 |
| tqsort   | 0.110 |0.078 |
| quadsort | 0.122 |0.076 |


strings - length 7

arraysize = 10000000, number of arrays = 1
---------------------
Algorithm | Sortedness %0 | Sortedness %90 | Time(s)
---------------------- -
| timsort  | 2.062 |0.739 |
| pdqsort  | 1.972 |1.141 |
| tqsort   | 1.692 |0.679 |
| quadsort | 1.817 |1.082 |

arraysize = 100, number of arrays = 100000
---------------------
Algorithm | Sortedness %0 | Sortedness %90 | Time(s)
---------------------- -
| timsort  | 0.535 |0.243 |
| pdqsort  | 0.309 |0.200 |
| tqsort   | 0.308 |0.185 |
| quadsort | 0.311 |0.186 |
