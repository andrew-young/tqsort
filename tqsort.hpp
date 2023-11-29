#pragma once
#include "util.h"
#include <stdlib.h>
#include <utility>
#include "quadsort.hpp"
#include <xmmintrin.h>
#include <mmintrin.h>


namespace qt_sort {
#define gallopsize 8
	template<class Iter>
	struct run {
		Iter start;
		size_t len;
		size_t unorder;//higher means less sorted
	};

	//__declspec(noinline)
	template<class Iter, class Compare>
	void insertion_sort(Iter begin, Iter unorderedstart, Iter end, Compare comp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		//if (begin == end) return;

		for (Iter cur = unorderedstart; cur != end; ++cur) {
			Iter sift = cur;
			Iter sift_1 = cur - 1;

			// Compare first so we can avoid 2 moves for an element already positioned correctly.
			if (comp(*sift, *sift_1)) {
				T tmp = *sift;

				do { *sift-- = *sift_1; } while (sift != begin && comp(tmp, *--sift_1));

				*sift = tmp;
			}
		}
	}


	template<class Iter1, class Iter2, class Iter3, class Compare, bool branchless>
	inline void head_merge(Iter3& ptd, size_t x, Iter1& ptl, Iter2& ptr, Compare cmp) {
		if constexpr (branchless) {
			//x = cmp(*ptr, *ptl) == 0; *ptd = *ptl; ptl += x; ptd[x] = *ptr; ptr =ptr+1-x; ptd++;
			x = cmp(*ptr, *ptl) == 0; *ptd = *ptl; ptl += x; ptd[x] = *ptr; ptr += !x; ptd++;
		}
		else {
			//*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;
			//x = cmp(*ptr, *ptl) == 0; *ptd = *ptl; ptl += x; ptd[x] = *ptr; ptr = ptr + 1 - x; ptd++;
			x = cmp(*ptr, *ptl) == 0; *ptd = *ptl; ptl += x; ptd[x] = *ptr; ptr += !x; ptd++;
		}
	}

	template<class Iter1, class Iter2, class Iter3, class Compare, bool branchless>
	inline void tail_merge(Iter3& tpd, size_t x, Iter1& tpl, Iter2& tpr, Compare cmp) {
		if constexpr (branchless) {
			//x = cmp(*tpr, *tpl) == 0; *tpd = *tpl; tpl += x - 1; tpd--; tpd[x] = *tpr; tpr -= x;
			x = cmp(*tpr, *tpl) == 0;	*tpd = *tpl; tpl -= !x;	tpd--; tpd[x] = *tpr; tpr -= x;
		}
		else {
			//x = cmp(*tpr, *tpl) == 0; *tpd = *tpl; tpl += x - 1; tpd--; tpd[x] = *tpr; tpr -= x;
			//*tpd-- = cmp(*tpr, *tpl) == 0 ? *tpr-- : *tpl--;
			x = cmp(*tpr, *tpl) == 0; *tpd = *tpl; tpl -= !x;	tpd--; tpd[x] = *tpr; tpr -= x;
		}
	}

	template<class Iter, class T, class Compare>
	void swap_branchless(Iter pta, T swap, size_t x, size_t y, Compare cmp) {
		x = cmp(*(pta + 1), *pta);	y = !x; swap = pta[y];	pta[0] = pta[x];	pta[1] = swap;
	}




	// class T = typename std::iterator_traits<Iter>::value_type,
	template<class Iter, class Compare, bool branchless>
	inline void parity_merge_two(Iter array, typename std::iterator_traits<Iter>::value_type* swap, size_t x, size_t y, Iter ptl, Iter ptr, typename std::iterator_traits<Iter>::value_type* pts, Compare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		ptl = array; ptr = array + 2; pts = swap;
		head_merge<Iter, Iter, T*, Compare, branchless>(pts, x, ptl, ptr, cmp);
		*pts = cmp(*ptr, *ptl) == 0 ? *ptl : *ptr;

		ptl = array + 1; ptr = array + 3; pts = swap + 3;
		tail_merge<Iter, Iter, T*, Compare, branchless>(pts, y, ptl, ptr, cmp);
		*pts = cmp(*ptr, *ptl) ? *ptl : *ptr;
	}

	template<class Iter1, class Iter2, class Compare, bool branchless>
	void parity_merge_four(Iter1 array, Iter2 swap, size_t x, size_t y, Iter1 ptl, Iter1 ptr, Iter2 ptd, Compare cmp) {
		typedef typename std::iterator_traits<Iter1>::value_type T;
		ptl = array + 0; ptr = array + 4; ptd = swap;
		head_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, x, ptl, ptr, cmp);
		head_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, x, ptl, ptr, cmp);
		head_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, x, ptl, ptr, cmp);
		*ptd = cmp(*ptr, *ptl) ? *ptr : *ptl;

		ptl = array + 3; ptr = array + 7; ptd = swap + 7;
		tail_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, y, ptl, ptr, cmp);
		tail_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, y, ptl, ptr, cmp);
		tail_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, y, ptl, ptr, cmp);
		*ptd = cmp(*ptr, *ptl) == 0 ? *ptr : *ptl;
	}


	template<class Iter, class Compare, bool branchless>
	void parity_swap_eight(Iter array, typename std::iterator_traits<Iter>::value_type* swap, Compare cmp)
	{
		//,class T= typename std::iterator_traits<Iter>::value_type,
		typedef typename std::iterator_traits<Iter>::value_type T;
		Iter ptl, ptr, pta;
		T* pts, * ptls, * ptrs;
		size_t x, y;

		ptl = array;

		swap_branchless(ptl, swap[0], x, y, cmp); ptl += 2;
		swap_branchless(ptl, swap[0], x, y, cmp); ptl += 2;
		swap_branchless(ptl, swap[0], x, y, cmp); ptl += 2;
		swap_branchless(ptl, swap[0], x, y, cmp);

		if (cmp(*(array + 2), *(array + 1)) == 0 && cmp(*(array + 4), *(array + 3)) == 0 && cmp(*(array + 6), *(array + 5)) == 0)
		{
			return;
		}
		parity_merge_two<Iter, Compare, branchless>(array + 0, swap + 0, x, y, ptl, ptr, pts, cmp);
		parity_merge_two<Iter, Compare, branchless>(array + 4, swap + 4, x, y, ptl, ptr, pts, cmp);
		parity_merge_four<T*, Iter, Compare, branchless>(swap, array, x, y, ptls, ptrs, pta, cmp);
	}

	

	//__declspec(noinline)
	template<class Iter, class Compare>
	void quad_swap_four2(Iter array, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter>::value_type T;
		Iter pta;
		T swap;
		size_t x, y;

		pta = array;

		swap_branchless(pta, swap, x, y, cmp); pta += 2;
		swap_branchless(pta, swap, x, y, cmp); pta--;

		if (cmp(pta[1], *pta))
		{
			swap = pta[0]; pta[0] = pta[1]; pta[1] = swap; pta--;

			swap_branchless(pta, swap, x, y, cmp); pta += 2;
			swap_branchless(pta, swap, x, y, cmp); pta--;
			swap_branchless(pta, swap, x, y, cmp);
		}
	}
	//
	template<class Iter, class Compare>
	void quad_swap_four(Iter array, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter>::value_type T;
		Iter pta;
		T swap;
		size_t x, y;

		pta = array;

		swap_branchless(pta, swap, x, y, cmp); pta += 2;
		swap_branchless(pta, swap, x, y, cmp); pta--;

		if (cmp(pta[1], *pta))
		{
			pta--;
			swap = pta[1];
			pta[1] = pta[0];
			pta[!cmp(pta[2], pta[0])] = pta[2];
			pta += 2;
			pta[0] = pta[1]; 
			pta[cmp(pta[1], swap)] = swap;
			pta--;
			swap_branchless(pta, swap, x, y, cmp);
		}
	}



	template<class Iter1, class Iter2, class Compare, bool branchless>
	void parity_merge(Iter1 start1, size_t len1, Iter1 start2, size_t len2, Iter2 dest, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("parity_merge\n"); printarray<T>((T*)&start1[0], len1+len2);
		//size_t n = len1 + len2;
		Iter1 ptl, ptr, tpl, tpr;
		Iter2 tpd, ptd;
#if !defined __clang__
		size_t x, y;
#endif
		ptl = start1;
		ptr = start2;
		ptd = dest;
		tpl = ptl + len1 - 1;
		tpr = ptr + len2 - 1;
		tpd = dest + len1 + len2 - 1;

		if (len1 < len2)
		{
			*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;
		}

		*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;

		while (--len1)
		{
			head_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, x, ptl, ptr, cmp);
			tail_merge<Iter1, Iter1, Iter2, Compare, branchless>(tpd, y, tpl, tpr, cmp);
		}
		*tpd = cmp(*tpr, *tpl) == 0 ? *tpr : *tpl;
	}

	template<class Iter1, class Iter2, class Compare, bool branchless>
	void parity_merge_odd(Iter1 start1, size_t len1, Iter1 start2, size_t len2, Iter2 dest, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("parity_merge\n"); printarray<T>((T*)&start1[0], len1+len2);
		//size_t n = len1 + len2;
		Iter1 ptl, ptr, tpl, tpr;
		Iter2 tpd, ptd;
#if !defined __clang__
		size_t x, y;
#endif
		ptl = start1;
		ptr = start2;
		ptd = dest;
		tpl = ptl + len1 - 1;
		tpr = ptr + len2 - 1;
		tpd = dest + len1 + len2 - 1;


		//*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;
		//*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;

		while (len1--)
		{
			head_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, x, ptl, ptr, cmp);
			tail_merge<Iter1, Iter1, Iter2, Compare, branchless>(tpd, y, tpl, tpr, cmp);
		}
		//*tpd = cmp(*tpr, *tpl) == 0 ? *tpr : *tpl;
		*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;
	}
	template<class Iter1, class Iter2, class Compare, bool branchless>
	void parity_merge_even(Iter1 start1, size_t len1, Iter1 start2, size_t len2, Iter2 dest, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("parity_merge even\n"); printarray<T>((T*)&start1[0], len1+len2);
		size_t n = len1 + len2;
		Iter1 ptl, ptr, tpl, tpr;
		Iter2 tpd, ptd;
#if !defined __clang__
		size_t x, y;
#endif
		ptl = start1;
		ptr = start2;
		ptd = dest;
		tpl = ptl + len1 - 1;
		tpr = ptr + len2 - 1;
		tpd = dest + len1 + len2 - 1;


		//*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;
		//*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;

		while (--len1)
		{
			//printf("1. %d %d %d %d\n", *ptl, *ptr, *tpl, *tpr);
			head_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, x, ptl, ptr, cmp);
			//printf("2. %d %d %d %d\n", *ptl, *ptr, *tpl, *tpr);
			tail_merge<Iter1, Iter1, Iter2, Compare, branchless>(tpd, y, tpl, tpr, cmp);
			//printarray<T>((T*)&dest[0], n);
		}
		head_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, x, ptl, ptr, cmp);
		//*ptd = cmp(*ptr, *ptl) ? *ptr : *ptl;
		*tpd = cmp(*tpr, *tpl) == 0 ? *tpr : *tpl;

		//printf("end parity_merge even\n");
	}
	template<class Iter, class Compare, bool branchless>
	void parity_swap_sixteen(Iter array, typename std::iterator_traits<Iter>::value_type* swap, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter>::value_type T;
		Iter ptl, ptr,tpl,tpr;
		T* pts,*tpd;
		size_t x, y;
		quad_swap_four(array + 0, cmp);
		quad_swap_four(array + 4, cmp);
		quad_swap_four(array + 8, cmp);
		quad_swap_four(array + 12, cmp);
		if (cmp(*(array + 4), *(array + 3)) == 0 && cmp(*(array + 8), *(array + 7)) == 0 && cmp(*(array + 12), *(array + 11)) == 0)
		{
			return;
		}

		parity_merge_four<Iter, T*, Compare, branchless>(array + 0, swap + 0, x, y, ptl, ptr, pts, cmp);
		parity_merge_four<Iter, T*, Compare, branchless>(array + 8, swap + 8, x, y, ptl, ptr, pts, cmp);

		parity_merge<T*, Iter, Compare, branchless>(swap, 8, swap + 8, 8, array, cmp);
	}


	template<class Iter, class Compare, bool branchless>
	void tiny_sort(Iter start, size_t len, typename std::iterator_traits<Iter>::value_type* swap, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("tiny_insertion_sort2  %d\n", len);      printarray<T>((T*)&start[0], len);
		Iter cur, cur2, shift, shift2, ptl, ptr;
		T tmp, * ptrs, * ptls, * pts;
		bool x, y;
		switch (len)
		{
		case 0:
		case 1:
			return;
		case 2:
			swap_branchless(start, tmp, x, y, cmp);
			return;
		case 3:
			cur = start;
			swap_branchless(cur, tmp, x, y, cmp); cur++;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			swap_branchless(cur, tmp, x, y, cmp);
			return;
		case 4:
			cur = start;

			swap_branchless(cur, tmp, x, y, cmp); cur += 2;
			swap_branchless(cur, tmp, x, y, cmp); cur--;

			if (cmp(cur[1], *cur))
			{
				tmp = cur[0]; cur[0] = cur[1]; cur[1] = tmp; cur--;

				swap_branchless(cur, tmp, x, y, cmp); cur += 2;
				swap_branchless(cur, tmp, x, y, cmp); cur--;
				swap_branchless(cur, tmp, x, y, cmp);
			}
			return;


		case 5:
		{
			cur = start;

			swap_branchless(cur, tmp, x, y, cmp); cur += 2;
			swap_branchless(cur, tmp, x, y, cmp); cur--;

			if (cmp(cur[1], *cur))
			{
				tmp = cur[0]; cur[0] = cur[1]; cur[1] = tmp; cur--;

				swap_branchless(cur, tmp, x, y, cmp); cur += 2;
				swap_branchless(cur, tmp, x, y, cmp); cur--;
				swap_branchless(cur, tmp, x, y, cmp);
			}
			cur2 = start + 4;
			cur = start + 3;
			if (cmp(*cur2, *cur)) {
				tmp = *cur2;
				*cur2 = *cur;
				shift2 = cur2 - 2;
				if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
				else { *cur = tmp;  return; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; return; }
				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			}

			return;

		}
		case 6:
		{
			T* swap2;
			cur = start;
			x = cmp(cur[1], *cur); y = !x; swap[0] = cur[x]; cur[1] = cur[y]; cur++;
			x = cmp(cur[1], *cur); y = !x; swap[1] = cur[x]; swap[2] = cur[y];
			x = cmp(swap[1], *swap); y = !x; tmp = swap[y]; swap[0] = swap[x]; swap[1] = tmp;
			cur = start + 3;
			swap2 = swap + 3;
			x = cmp(cur[1], *cur); y = !x; swap2[0] = cur[x]; cur[1] = cur[y]; cur++;
			x = cmp(cur[1], *cur); y = !x; swap2[1] = cur[x]; swap2[2] = cur[y];
			x = cmp(swap2[1], *swap2); y = !x; tmp = swap2[y]; swap2[0] = swap2[x]; swap2[1] = tmp;
			parity_merge_even<T*, Iter, Compare, true>(swap, 3, swap2, 3, start, cmp);
			return;
		}
		case 7:
			cur = start;
			x = cmp(cur[1], *cur); y = !x; swap[0] = cur[x]; cur[1] = cur[y]; cur++;
			x = cmp(cur[1], *cur); y = !x; swap[1] = cur[x]; swap[2] = cur[y];
			x = cmp(swap[1], *swap); y = !x; tmp = swap[y]; swap[0] = swap[x]; swap[1] = tmp;
			cur = start + 3;
			swap_branchless(cur, tmp, x, y, cmp); cur += 2;
			swap_branchless(cur, tmp, x, y, cmp);
			parity_merge_two<Iter, Compare, true>(start + 3, swap + 3, x, y, ptl, ptr, pts, cmp);
			parity_merge_odd<T*, Iter, Compare, true>(swap, 3, swap + 3, 4, start, cmp);

			return;


		case 8:
			parity_swap_eight<Iter, std::less<T>, true>(start, swap, cmp);
			/*
			size_t x, y;
			Iter ptl, ptr;
			T* pts,*ptls,*ptrs;
			cur = start;
			swap_branchless(cur, swap[0], x, y, cmp); cur += 2;
			swap_branchless(cur, swap[0], x, y, cmp); cur += 2;
			swap_branchless(cur, swap[0], x, y, cmp); cur += 2;
			swap_branchless(cur, swap[0], x, y, cmp);

			if (cmp(*(start + 2), *(start + 1)) == 0 && cmp(*(start + 4), *(start + 3)) == 0 && cmp(*(start + 6), *(start + 5)) == 0)
			{
				return;
			}
			parity_merge_two<Iter, Compare, true>(start + 0, swap + 0, x, y, ptl, ptr, pts, cmp);
			parity_merge_two<Iter, Compare, true>(start + 4, swap + 4, x, y, ptl, ptr, pts, cmp);
			parity_merge_four<T*, Iter, Compare, true>(swap, start, x, y, ptls, ptrs, start, cmp);
			*/
			return;
		case 9:
			
			parity_swap_eight<Iter, std::less<T>, true>(start, swap, cmp);
			cur2 = start + 8;
			cur = start + 7;
			if (cmp(*cur2, *cur)) {
				tmp = *cur2;
				*cur2 = *cur;
				shift2 = cur2 - 2;
				if (cmp(tmp, *shift2)) {
					*cur = *shift2; shift = cur - 2;
					if (cmp(tmp, *shift)) {
						*shift2 = *shift; shift2 -= 2;
						if (cmp(tmp, *shift2)) {
							*shift = *shift2; shift -= 2;
							if (cmp(tmp, *shift)) {
								*shift2 = *shift; shift2 -= 2;
								if (cmp(tmp, *shift2)) {
									*shift = *shift2; shift -= 2;
									if (cmp(tmp, *shift)) {
										*shift2 = *shift; shift2 -= 2;
										x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
									}
									else { *shift2 = tmp; }
								}
								else { *shift = tmp; }
							}
							else { *shift2 = tmp; }
						}
						else { *shift = tmp; }
					}
					else { *shift2 = tmp; }
				}
				else { *cur = tmp; }
			}
			

			return;
		case 10:
			parity_swap_eight<Iter, std::less<T>, true>(start, swap, cmp);
			//printarray<T>((T*)&start[0], len);
			cur2 = start + 8;
			cur = start + 7;
			if (cmp(*cur2, *cur)) {
				tmp = *cur2;
				*cur2 = *cur;
				shift2 = cur2 - 2;
				if (cmp(tmp, *shift2)) {
					*cur = *shift2; shift = cur - 2;
					if (cmp(tmp, *shift)) {
						*shift2 = *shift; shift2 -= 2;
						if (cmp(tmp, *shift2)) {
							*shift = *shift2; shift -= 2;
							if (cmp(tmp, *shift)) {
								*shift2 = *shift; shift2 -= 2;
								if (cmp(tmp, *shift2)) {
									*shift = *shift2; shift -= 2;
									if (cmp(tmp, *shift)) {
										*shift2 = *shift; shift2 -= 2;
										x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
									}
									else { *shift2 = tmp; }
								}
								else { *shift = tmp; }
							}
							else { *shift2 = tmp; }
						}
						else { *shift = tmp; }
					}
					else { *shift2 = tmp; }
				}
				else { *cur = tmp; }
			}
			//printarray<T>((T*)&start[0], len);

			cur = start + 9;
			cur2 = start + 8;
			if (cmp(*cur, *cur2)) {
				tmp = *cur;
				*cur = *cur2;
				shift = cur - 2;
				if (cmp(tmp, *shift)) {
					*cur2 = *shift; shift2 = cur2 - 2;
					if (cmp(tmp, *shift2)) {
						*shift = *shift2; shift -= 2;
						if (cmp(tmp, *shift)) {
							*shift2 = *shift; shift2 -= 2;
							if (cmp(tmp, *shift2)) {
								*shift = *shift2; shift -= 2;
								if (cmp(tmp, *shift)) {
									*shift2 = *shift; shift2 -= 2;
									if (cmp(tmp, *shift2)) {
										*shift = *shift2; shift -= 2;
										if (cmp(tmp, *shift)) {
											*shift2 = *shift; shift2 -= 2;
											x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
										}
										else { *shift2 = tmp; }
									}
									else { *shift = tmp; }
								}
								else { *shift2 = tmp; }
							}
							else { *shift = tmp; }
						}
						else { *shift2 = tmp; }
					}
					else { *shift = tmp; }
				}
				else { *cur2 = tmp; }
			}
			return;
			//printarray<T>((T*)&start[0], len);

		case 11:
			if constexpr (branchless) {
				cur = start;
				swap_branchless(cur, tmp, x, y, cmp); cur += 2;
				swap_branchless(cur, tmp, x, y, cmp);
				parity_merge_two<Iter, Compare, true>(start + 0, swap + 0, x, y, ptl, ptr, pts, cmp);
				cur2 = swap + 4;
				cur = swap + 3;
				tmp = start[4];
				if (cmp(tmp, *cur)) {
					*cur2 = *cur;
					shift2 = cur2 - 2;
					if (cmp(tmp, *shift2)) {
						*cur = *shift2; shift = cur - 2;
						if (cmp(tmp, *shift)) {
							*shift2 = *shift; shift2 -= 2;
							x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
						}
						else { *shift2 = tmp; }
					}
					else { *cur = tmp; }
				}
				else { *cur2 = tmp; }

				T* swap2;
				cur = start + 5;

				swap_branchless(cur, tmp, x, y, cmp); cur++;
				swap_branchless(cur, tmp, x, y, cmp); cur--;
				swap_branchless(cur, tmp, x, y, cmp);

				cur = start + 8;
				swap_branchless(cur, tmp, x, y, cmp); cur++;
				swap_branchless(cur, tmp, x, y, cmp); cur--;
				swap_branchless(cur, tmp, x, y, cmp);
				parity_merge_even<T*, Iter, Compare, true>(start + 5, 3, start + 8, 3, swap + 5, cmp);
				parity_merge_odd<T*, Iter, Compare, true>(swap, 5, swap + 5, 6, start, cmp);
			}
			else {

				parity_swap_eight<Iter, std::less<T>, true>(start, swap, cmp);
				//printarray<T>((T*)&start[0], len);
				cur2 = start + 8;
				cur = start + 7;
				if (cmp(*cur2, *cur)) {
					tmp = *cur2;
					*cur2 = *cur;
					shift2 = cur2 - 2;
					if (cmp(tmp, *shift2)) {
						*cur = *shift2; shift = cur - 2;
						if (cmp(tmp, *shift)) {
							*shift2 = *shift; shift2 -= 2;
							if (cmp(tmp, *shift2)) {
								*shift = *shift2; shift -= 2;
								if (cmp(tmp, *shift)) {
									*shift2 = *shift; shift2 -= 2;
									if (cmp(tmp, *shift2)) {
										*shift = *shift2; shift -= 2;
										if (cmp(tmp, *shift)) {
											*shift2 = *shift; shift2 -= 2;
											x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
										}
										else { *shift2 = tmp; }
									}
									else { *shift = tmp; }
								}
								else { *shift2 = tmp; }
							}
							else { *shift = tmp; }
						}
						else { *shift2 = tmp; }
					}
					else { *cur = tmp; }
				}
				//printarray<T>((T*)&start[0], len);

				cur = start + 9;
				cur2 = start + 8;
				if (cmp(*cur, *cur2)) {
					tmp = *cur;
					*cur = *cur2;
					shift = cur - 2;
					if (cmp(tmp, *shift)) {
						*cur2 = *shift; shift2 = cur2 - 2;
						if (cmp(tmp, *shift2)) {
							*shift = *shift2; shift -= 2;
							if (cmp(tmp, *shift)) {
								*shift2 = *shift; shift2 -= 2;
								if (cmp(tmp, *shift2)) {
									*shift = *shift2; shift -= 2;
									if (cmp(tmp, *shift)) {
										*shift2 = *shift; shift2 -= 2;
										if (cmp(tmp, *shift2)) {
											*shift = *shift2; shift -= 2;
											if (cmp(tmp, *shift)) {
												*shift2 = *shift; shift2 -= 2;
												x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
											}
											else { *shift2 = tmp; }
										}
										else { *shift = tmp; }
									}
									else { *shift2 = tmp; }
								}
								else { *shift = tmp; }
							}
							else { *shift2 = tmp; }
						}
						else { *shift = tmp; }
					}
					else { *cur2 = tmp; }
				}
				//printarray<T>((T*)&start[0], len);
				cur2 = start + 10;
				cur = start + 9;
				if (cmp(*cur2, *cur)) {
					tmp = *cur2;
					*cur2 = *cur;
					shift2 = cur2 - 2;
					if (cmp(tmp, *shift2)) {
						*cur = *shift2; shift = cur - 2;
						if (cmp(tmp, *shift)) {
							*shift2 = *shift; shift2 -= 2;
							if (cmp(tmp, *shift2)) {
								*shift = *shift2; shift -= 2;
								if (cmp(tmp, *shift)) {
									*shift2 = *shift; shift2 -= 2;
									if (cmp(tmp, *shift2)) {
										*shift = *shift2; shift -= 2;
										if (cmp(tmp, *shift)) {
											*shift2 = *shift; shift2 -= 2;
											if (cmp(tmp, *shift2)) {
												*shift = *shift2; shift -= 2;
												if (cmp(tmp, *shift)) {
													*shift2 = *shift; shift2 -= 2;
													x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
												}
												else { *shift2 = tmp; }
											}
											else { *shift = tmp; }
										}
										else { *shift2 = tmp; }
									}
									else { *shift = tmp; }
								}
								else { *shift2 = tmp; }
							}
							else { *shift = tmp; }
						}
						else { *shift2 = tmp; }
					}
					else { *cur = tmp; }
				}
			}
			return;
			//printarray<T>((T*)&start[0], len);
		case 12:

			cur = start;

			swap_branchless(cur, tmp, x, y, cmp); cur++;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			swap_branchless(cur, tmp, x, y, cmp);

			cur = start + 3;
			swap_branchless(cur, tmp, x, y, cmp); cur++;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			swap_branchless(cur, tmp, x, y, cmp);
			parity_merge_even<T*, Iter, Compare, true>(start, 3, start + 3, 3, swap, cmp);
			//printarray<T>((T*)&swap[0], 6);

			cur = start + 6;

			swap_branchless(cur, tmp, x, y, cmp); cur++;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			swap_branchless(cur, tmp, x, y, cmp);

			cur = start + 9;
			swap_branchless(cur, tmp, x, y, cmp); cur++;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			swap_branchless(cur, tmp, x, y, cmp);
			parity_merge_even<T*, Iter, Compare, true>(start + 6, 3, start + 9, 3, swap + 6, cmp);
			//printarray<T>((T*)&start[0], len);
			//printarray<T>((T*)&swap[0], len);
			parity_merge_even<T*, Iter, Compare, true>(swap, 6, swap + 6, 6, start, cmp);
			//printarray<T>((T*)&start[0], len);
			return;

		case 13:
			cur = start;

			swap_branchless(cur, tmp, x, y, cmp); cur++;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			swap_branchless(cur, tmp, x, y, cmp);

			cur = start + 3;
			swap_branchless(cur, tmp, x, y, cmp); cur++;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			swap_branchless(cur, tmp, x, y, cmp);
			parity_merge_even<T*, Iter, Compare, true>(start, 3, start + 3, 3, swap, cmp);



			cur = start + 6;

			swap_branchless(cur, tmp, x, y, cmp); cur++;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			swap_branchless(cur, tmp, x, y, cmp);

			cur = start + 9;
			swap_branchless(cur, tmp, x, y, cmp); cur += 2;
			swap_branchless(cur, tmp, x, y, cmp); cur--;

			if (cmp(cur[1], *cur))
			{
				tmp = cur[0]; cur[0] = cur[1]; cur[1] = tmp; cur--;

				swap_branchless(cur, tmp, x, y, cmp); cur += 2;
				swap_branchless(cur, tmp, x, y, cmp); cur--;
				swap_branchless(cur, tmp, x, y, cmp);
			}
			parity_merge_odd<T*, Iter, Compare, true>(start + 6, 3, start + 9, 4, swap + 6, cmp);
			parity_merge_odd<T*, Iter, Compare, true>(swap, 6, swap + 6, 7, start, cmp);
			return;
		case 14:
			cur = start;

			swap_branchless(cur, tmp, x, y, cmp); cur++;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			swap_branchless(cur, tmp, x, y, cmp);

			cur = start + 3;
			swap_branchless(cur, tmp, x, y, cmp); cur += 2;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			if (cmp(cur[1], *cur))
			{
				tmp = cur[0]; cur[0] = cur[1]; cur[1] = tmp; cur--;

				swap_branchless(cur, tmp, x, y, cmp); cur += 2;
				swap_branchless(cur, tmp, x, y, cmp); cur--;
				swap_branchless(cur, tmp, x, y, cmp);
			}
			parity_merge_odd<T*, Iter, Compare, true>(start, 3, start + 3, 4, swap, cmp);



			cur = start + 7;

			swap_branchless(cur, tmp, x, y, cmp); cur++;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			swap_branchless(cur, tmp, x, y, cmp);

			cur = start + 10;
			swap_branchless(cur, tmp, x, y, cmp); cur += 2;
			swap_branchless(cur, tmp, x, y, cmp); cur--;

			if (cmp(cur[1], *cur))
			{
				tmp = cur[0]; cur[0] = cur[1]; cur[1] = tmp; cur--;

				swap_branchless(cur, tmp, x, y, cmp); cur += 2;
				swap_branchless(cur, tmp, x, y, cmp); cur--;
				swap_branchless(cur, tmp, x, y, cmp);
			}
			parity_merge_odd<T*, Iter, Compare, true>(start + 7, 3, start + 10, 4, swap + 7, cmp);
			parity_merge_even<T*, Iter, Compare, true>(swap, 7, swap + 7, 7, start, cmp);
			return;
		case 15:
			cur = start;

			swap_branchless(cur, tmp, x, y, cmp); cur++;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			swap_branchless(cur, tmp, x, y, cmp);

			cur = start + 3;
			swap_branchless(cur, tmp, x, y, cmp); cur += 2;
			swap_branchless(cur, tmp, x, y, cmp); cur--;
			if (cmp(cur[1], *cur))
			{
				tmp = cur[0]; cur[0] = cur[1]; cur[1] = tmp; cur--;

				swap_branchless(cur, tmp, x, y, cmp); cur += 2;
				swap_branchless(cur, tmp, x, y, cmp); cur--;
				swap_branchless(cur, tmp, x, y, cmp);
			}
			parity_merge_odd<T*, Iter, Compare, true>(start, 3, start + 3, 4, swap, cmp);

			cur = start + 7;

			swap_branchless(cur, tmp, x, y, cmp); cur += 2;
			swap_branchless(cur, tmp, x, y, cmp); cur--;

			if (cmp(cur[1], *cur))
			{
				tmp = cur[0]; cur[0] = cur[1]; cur[1] = tmp; cur--;

				swap_branchless(cur, tmp, x, y, cmp); cur += 2;
				swap_branchless(cur, tmp, x, y, cmp); cur--;
				swap_branchless(cur, tmp, x, y, cmp);
			}

			cur = start + 11;
			swap_branchless(cur, tmp, x, y, cmp); cur += 2;
			swap_branchless(cur, tmp, x, y, cmp); cur--;

			if (cmp(cur[1], *cur))
			{
				tmp = cur[0]; cur[0] = cur[1]; cur[1] = tmp; cur--;

				swap_branchless(cur, tmp, x, y, cmp); cur += 2;
				swap_branchless(cur, tmp, x, y, cmp); cur--;
				swap_branchless(cur, tmp, x, y, cmp);
			}
			parity_merge_even<T*, Iter, Compare, true>(start + 7, 4, start + 11, 4, swap + 7, cmp);
			parity_merge_odd<T*, Iter, Compare, true>(swap, 7, swap + 7, 8, start, cmp);
			return;
		case 16:
			parity_swap_sixteen<Iter, Compare, branchless>(start, swap, cmp);
		}
	}





		template<class Iter1, class Iter2, class Iter3, class Compare, bool branchless>
	void forward_merge_gallop(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 swap, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("partial_forward_merge %d %d\n", len1, len2);      printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0] , len2);

		Iter1 tpl, ptl;  // tail pointer left, array, right
		Iter2 ptr, tpr;
		Iter3  ptd;
		size_t loop, x, y;


		ptl = start1;
		ptr = start2;
		tpl = start1 + len1 - 1;
		tpr = start2 + len2 - 1;
		ptd = swap;


		while (ptl < tpl - gallopsize * 2 && ptr < tpr - gallopsize * 2)
		{

			tpl_tpr32: if (cmp(*(ptr + gallopsize * 2 - 1), *ptl))
			{
				loop = gallopsize * 2; do *ptd++ = *ptr++; while (--loop);

				if (ptr < tpr - gallopsize * 2) { goto tpl_tpr32; } break;
			}

			tpl32_tpr: if (cmp(*ptr, *(ptl + gallopsize * 2 - 1)) == 0)
			{
				loop = gallopsize * 2; do *ptd++ = *ptl++; while (--loop);

				if (ptl < tpl - gallopsize * 2) { goto tpl32_tpr; } break;
			}

			loop = gallopsize; do
			{

				if (cmp(*(ptr + 1), *ptl))
				{
					*ptd++ = *ptr++; *ptd++ = *ptr++;
				}
				else if (cmp(*ptr, *(ptl + 1)) == 0)
				{
					*ptd++ = *ptl++; *ptd++ = *ptl++;
				}
				else
				{
					x = cmp(*ptr, *ptl) == 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
					head_merge<Iter1, Iter2, Iter3, Compare, branchless>(ptd, x, ptl, ptr, cmp);
				}
			} while (--loop);
		}

		while (ptl < tpl - 1 && ptr < tpr - 1)
		{
			if (cmp(*(ptr + 1), *ptl))
			{
				*ptd++ = *ptr++; *ptd++ = *ptr++;
			}
			else if (cmp(*ptr, *(ptl + 1)) == 0)
			{
				*ptd++ = *ptl++; *ptd++ = *ptl++;
			}
			else
			{
				x = cmp(*ptr, *ptl) == 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
				head_merge<Iter1, Iter2, Iter3, Compare, branchless>(ptd, x, ptl, ptr, cmp);
			}
		}

		if (cmp(*tpr, *tpl) == 0)
		{
			while (ptl <= tpl)
			{
				*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
			}
		}
		else
		{
			while (ptr <= tpr)
			{
				*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
			}
			while (ptl <= tpl)
			{
				*ptd++ = *ptl++;
			}
		}
		//printf("end forward_merge\n");      printarray<T>((T*)&swap[0], len1+len2);
	}
	template<class Iter1, class Iter2, class Iter3, class Compare, bool branchless>
	void forward_merge(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 swap, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("partial_forward_merge %d %d\n", len1, len2);      printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0] , len2);

		Iter1 tpl, ptl;  // tail pointer left, array, right
		Iter2 ptr, tpr;
		Iter3  ptd;
		size_t loop, x, y, len;

		/*
		if (len2 == 0)
		{
			return;
		}
		*/

		//tpd = swap + len1+len2 - 1;

		ptl = start1;
		ptr = start2;
		tpl = start1 + len1 - 1;
		tpr = start2 + len2 - 1;
		ptd = swap;

		while (ptl < tpl - 1 && ptr < tpr - 1)
		{
			if (cmp(*(ptr + 1), *ptl))
			{
				*ptd++ = *ptr++; *ptd++ = *ptr++;
			}
			else if (cmp(*ptr, *(ptl + 1)) == 0)
			{
				*ptd++ = *ptl++; *ptd++ = *ptl++;
			}
			else
			{
				x = cmp(*ptr, *ptl) == 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
				head_merge<Iter1, Iter2, Iter3, Compare, branchless>(ptd, x, ptl, ptr, cmp);
			}
		}

		while (ptl <= tpl && ptr <= tpr)
		{
			//*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
			head_merge<Iter1, Iter2, Iter3, Compare, branchless>(ptd, x, ptl, ptr, cmp);
		}

	
		if (cmp(*tpr, *tpl) == 0)
		{
			while (ptl <= tpl)
			{
				*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
			}
		}
		else
		{
			while (ptr <= tpr)
			{
				*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
			}
			while (ptl <= tpl)
			{
				*ptd++ = *ptl++;
			}
		}
	
		//printf("end forward_merge\n");      printarray<T>((T*)&swap[0], len1+len2);
	}


	template<class Iter1, class Iter2, class Compare, bool branchless>
	void branchless_merge_gallop(Iter1 start1, size_t len1, Iter1 start2, size_t len2, Iter2 dest, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter2>::value_type T;
		//printf("cross_merge %d %d\n", len1, len2);     printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start1[0]+len1, len2);
		Iter1 ptl, tpl, ptr, tpr;
		Iter2 ptd, tpd;
		size_t loop, loop2;
		size_t x, y;
		ptl = start1;
		ptr = start2;
		tpl = start1 + len1 - 1;
		tpr = start2 + len2 - 1;


		ptd = dest;
		tpd = dest + len1 + len2 - 1;

		while (tpl - ptl > gallopsize && tpr - ptr > gallopsize)
		{
		ptl8_ptr:
			if (cmp(*ptr, *(ptl + (gallopsize - 1))) == 0)
			{
				loop = gallopsize; do *ptd++ = *ptl++; while (--loop);

				if (tpl - ptl > gallopsize) { goto ptl8_ptr; } break;
			}

			else {
			ptl_ptr8:
				if (cmp(*(ptr + (gallopsize - 1)), *ptl))
				{
					loop = gallopsize; do *ptd++ = *ptr++; while (--loop);

					if (tpr - ptr > gallopsize) { goto ptl_ptr8; } break;
				}
			}

		tpl_tpr8:
			if (cmp(*(tpr - (gallopsize - 1)), *tpl) == 0)
			{
				loop = gallopsize; do *tpd-- = *tpr--; while (--loop);

				if (tpr - ptr > gallopsize) { goto tpl_tpr8; } break;
			}
			else {
			tpl8_tpr:
				if (cmp(*tpr, *(tpl - (gallopsize - 1))))
				{
					loop = gallopsize; do *tpd-- = *tpl--; while (--loop);

					if (tpl - ptl > gallopsize) { goto tpl8_tpr; } break;
				}
			}



			loop = gallopsize; do
			{
				head_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, x, ptl, ptr, cmp);
				tail_merge<Iter1, Iter1, Iter2, Compare, branchless>(tpd, y, tpl, tpr, cmp);
				//*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;
				//*tpd-- = cmp(*tpr, *tpl) == 0 ? *tpr-- : *tpl--;
			} while (--loop);

		}

		while (tpl - ptl > gallopsize && tpr - ptr > gallopsize)
		{
		ptl8_ptr2:
			if (cmp(*ptr, *(ptl + (gallopsize - 1))) == 0)
			{
				loop = gallopsize; do *ptd++ = *ptl++; while (--loop);

				if (tpl - ptl > gallopsize) { goto ptl8_ptr2; } break;
			}

			else {
			ptl_ptr82:
				if (cmp(*(ptr + (gallopsize - 1)), *ptl))
				{
					loop = gallopsize; do *ptd++ = *ptr++; while (--loop);

					if (tpr - ptr > gallopsize) { goto ptl_ptr82; } break;
				}
			}

		tpl_tpr82:
			if (cmp(*(tpr - (gallopsize - 1)), *tpl) == 0)
			{
				loop = gallopsize; do *tpd-- = *tpr--; while (--loop);

				if (tpr - ptr > gallopsize) { goto tpl_tpr82; } break;
			}
			else {
			tpl8_tpr2:
				if (cmp(*tpr, *(tpl - (gallopsize - 1))))
				{
					loop = gallopsize; do *tpd-- = *tpl--; while (--loop);

					if (tpl - ptl > gallopsize) { goto tpl8_tpr2; } break;
				}
			}



			loop = gallopsize; do
			{
				//head_merge<Iter1, Iter1, Iter2, Compare, branchless>(ptd, x, ptl, ptr, cmp);
				//tail_merge<Iter1, Iter1, Iter2, Compare, branchless>(tpd, y, tpl, tpr, cmp);
				*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;
				*tpd-- = cmp(*tpr, *tpl) == 0 ? *tpr-- : *tpl--;
			} while (--loop);

		}
		
		if (cmp(*tpr, *tpl) == 0)
		{
			while (ptl <= tpl)
			{
				*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
			}
			while (ptr <= tpr)
			{
				*ptd++ = *ptr++;
			}
		}
		else
		{
			while (ptr <= tpr)
			{
				*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
			}
			while (ptl <= tpl)
			{
				*ptd++ = *ptl++;
			}
		}
		
	}

	template<class Iter1, class Iter2, class Iter3, class Compare, bool branchless>
	void branchless_merge(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 dest, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("merge %d %d\n",len1,len2);      printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0], len2);
		int n = len1 + len2;
		Iter1 ptl, tpl;// , pt2;
		Iter2 ptr, tpr;// , pt3;
		Iter3 ptd, tpd;
		//bool  x, y, z;
		size_t len, y;

		ptl = start1;
		ptr = start2;
		ptd = dest;


		tpl = start1 + len1 - 1;
		tpr = start2 + len2 - 1;
		size_t x;

		//int len3, len4;
		tpd = dest + len1 + len2 - 1;


		if (cmp(*(ptr), *(tpl)) == 0) {
			std::move(start1, start1 + len1, dest);
			std::move(start2, start2 + len2, dest + len1);
		}
		if (cmp(*(tpr), *(ptl))) {
			std::move(start2, start2 + len2, dest);
			std::move(start1, start1 + len1, dest + len2);
		}

		len = tpd - ptd + 1;
		if (len == 0) {
			return;
		}
		if (len & 1 == 0) {
			head_merge<Iter1, Iter2, Iter3, Compare, branchless>(ptd, x, ptl, ptr, cmp);
			len--;
		}
		len = len / 2;
		while (len--)
		{
			head_merge<Iter1, Iter2, Iter3, Compare, branchless>(ptd, x, ptl, ptr, cmp);
			tail_merge<Iter1, Iter2, Iter3, Compare, branchless>(tpd, y, tpl, tpr, cmp);
		}
		*tpd = cmp(*tpr, *tpl) ? *tpl : *tpr;

		//printf("end merge\n");    printarray<T>((T*)&dest[0], n);

	}


	template<class Iter1, class Iter2, class Iter3, class Compare, bool branchless>
	void branch_merge(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 swap, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("branch_merge2 %d %d\n", len1, len2);      printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0] , len2);

		Iter1 tpl, ptl,ptl2;  // tail pointer left, array, right
		Iter2 ptr, tpr,ptr2;
		Iter3  ptd, tpd,ptd2;
		size_t loop, x, y, len;

		ptl = start1;
		ptr = start2;
		tpl = start1 + len1 - 1;
		tpr = start2 + len2 - 1;
		ptd = swap;
		tpd = ptd + len1 + len2 - 1;
		tpl--;
		tpr--;
		while (ptl < tpl && ptr < tpr)
		{
			if (cmp(ptr[1], ptl[1]))
			{
				if (cmp(ptr[1], ptl[0]))
				{
					*ptd++ = *ptr++;
					*ptd++ = *ptr++;
				}
				else {
					x = cmp(*ptr, *ptl) == 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
					*ptd++ = *ptr++;
				}
			}
			else if (cmp(*ptr, ptl[1]) == 0)
			{
				*ptd++ = *ptl++;
				*ptd++ = *ptl++;
			}
			else
			{
				x = cmp(*ptr, *ptl) == 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
				*ptd++ = *ptl++;
			}
		}
		tpl++;
		tpr++;
		if (cmp(*tpr, *tpl) == 0)
		{
			while (ptl <= tpl)
			{
				*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
			}
			while (ptr <= tpr)
			{
				*ptd++ = *ptr++;
			}
		}
		else
		{
			while (ptr <= tpr)
			{
				*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
			}
			while (ptl <= tpl)
			{
				*ptd++ = *ptl++;
			}
		}
		//printf("end forward_merge\n");      printarray<T>((T*)&swap[0], len1+len2);
	}
	template<class Iter1, class Iter2, class Iter3, class Compare, bool branchless>
	void branch_merge_gallop(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 swap, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("branch_merge2 %d %d\n", len1, len2);      printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0] , len2);

		Iter1 tpl, ptl, ptl2;  // tail pointer left, array, right
		Iter2 ptr, tpr, ptr2;
		Iter3  ptd, tpd, ptd2;
		size_t loop, x, y, len;


		ptl = start1;
		ptr = start2;
		tpl = start1 + len1 - 1;
		tpr = start2 + len2 - 1;
		ptd = swap;
		tpd = ptd + len1 + len2 - 1;
		tpl=tpl-9;
		tpr=tpr-9;
		while (ptl < tpl && ptr < tpr)
		{
		ptl8_ptr:
			if (cmp(*ptr, *(ptl + (8 - 1))) == 0)
			{
				loop = 8; do *ptd++ = *ptl++; while (--loop);

				if (tpl - ptl > 8) { goto ptl8_ptr; } break;
			}

			else {
			ptl_ptr8:
				if (cmp(*(ptr + (8 - 1)), *ptl))
				{
					loop = 8; do *ptd++ = *ptr++; while (--loop);

					if (tpr - ptr > 8) { goto ptl_ptr8; } break;
				}
			}
			if (cmp(ptr[1], ptl[1]))
			{
				if (cmp(ptr[1], ptl[0]))
				{
					*ptd++ = *ptr++;
					*ptd++ = *ptr++;
				}
				else {
					x = cmp(*ptr, *ptl) == 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
					*ptd++ = *ptr++;
				}
			}
			else if (cmp(*ptr, ptl[1]) == 0)
			{
				*ptd++ = *ptl++;
				//tail_merge<Iter1, Iter2, Iter3, Compare, branchless>(tpd, y, tpl, tpr, cmp);
				*ptd++ = *ptl++;
			}
			else
			{
				x = cmp(*ptr, *ptl) == 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
				*ptd++ = *ptl++;
			}
		}
		tpl += 8;
		tpr += 8;
		while (ptl < tpl && ptr < tpr )
		{
			if (cmp(ptr[1], ptl[1]))
			{
				if (cmp(ptr[1], ptl[0]))
				{
					*ptd++ = *ptr++;
					*ptd++ = *ptr++;
				}
				else {
					x = cmp(*ptr, *ptl) == 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
					*ptd++ = *ptr++;
				}
			}
			else if (cmp(*ptr, ptl[1]) == 0)
			{
				*ptd++ = *ptl++;
				//tail_merge<Iter1, Iter2, Iter3, Compare, branchless>(tpd, y, tpl, tpr, cmp);
				*ptd++ = *ptl++;
			}
			else
			{
				x = cmp(*ptr, *ptl) == 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
				*ptd++ = *ptl++;
			}
		}
		tpl++;
		tpr++;
		if (cmp(*tpr, *tpl) == 0)
		{
			while (ptl <= tpl)
			{
				*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
			}
			while (ptr <= tpr)
			{
				*ptd++ = *ptr++;
			}
		}
		else
		{
			while (ptr <= tpr)
			{
				*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
			}
			while (ptl <= tpl)
			{
				*ptd++ = *ptl++;
			}
		}
		//printf("end forward_merge\n");      printarray<T>((T*)&swap[0], len1+len2);
	}

	template<class Iter1, class Iter2, class Iter3, class Compare, bool branchless>
	void backward_merge_gallop(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 dest, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("backward_merge %d %d\n", len1, len2);      printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0], len2);

		Iter1  ptl, tpl; // tail pointer left, array, right
		Iter2 tpr, ptr;
		Iter3 tpd;
		size_t loop, x, y;

		ptr = start2;
		ptl = start1;
		tpr = start2 + len2 - 1;
		tpl = start1 + len1 - 1;
		tpd = dest + len1 + len2 - 1;



		while (tpl > start1 + 2 * gallopsize && tpr > start2 + 2 * gallopsize)
		{

			tpl_tpr32: if (cmp(*(tpr - (2 * gallopsize - 1)), *tpl) == 0)
			{
				loop = 2 * gallopsize; do *tpd-- = *tpr--; while (--loop);

				if (tpr > start2 + 32) { goto tpl_tpr32; } break;
			}

			tpl32_tpr: if (cmp(*tpr, *(tpl - (2 * gallopsize - 1))))
			{
				loop = 2 * gallopsize; do *tpd-- = *tpl--; while (--loop);

				if (tpl > start1 + 2 * gallopsize) { goto tpl32_tpr; } break;
			}

			loop = gallopsize; do
			{

				if (cmp(*(tpr - 1), *tpl) == 0)
				{
					*tpd-- = *tpr--; *tpd-- = *tpr--;
				}
				else if (cmp(*tpr, *(tpl - 1)))
				{
					*tpd-- = *tpl--; *tpd-- = *tpl--;
				}
				else
				{
					x = cmp(*tpr, *tpl) == 0; y = !x; tpd--; tpd[x] = *tpr; tpr -= 1; tpd[y] = *tpl; tpl -= 1; tpd--;
					tail_merge<Iter1, Iter2, Iter3, Compare, branchless>(tpd, y, tpl, tpr, cmp);
				}
			} while (--loop);
		}

		while (tpr > start2 + 1 && tpl > start1 + 1)
		{

			if (cmp(*(tpr - 1), *tpl) == 0)
			{
				*tpd-- = *tpr--; *tpd-- = *tpr--;
			}
			else if (cmp(*tpr, *(tpl - 1)))
			{
				*tpd-- = *tpl--; *tpd-- = *tpl--;
			}
			else
			{
				x = cmp(*tpr, *tpl) == 0; y = !x; tpd--; tpd[x] = *tpr; tpr -= 1; tpd[y] = *tpl; tpl -= 1; tpd--;
				tail_merge<Iter1, Iter2, Iter3, Compare, branchless>(tpd, y, tpl, tpr, cmp);
			}


		}

		while (tpr >= start2 && tpl >= start1)
		{
			*tpd-- = cmp(*tpr, *tpl) ? *tpl-- : *tpr--;
			//tail_merge<Iter1, Iter2, Iter3, Compare, branchless>(tpd, y, tpl, tpr, cmp);
		}

		while (tpr >= start2)
		{
			*tpd-- = *tpr--;
		}
		//printf("end backward_merge\n");      printarray<T>((T*)&dest[0], len1+len2);
	}

	template<class Iter1, class Iter2, class Iter3, class Compare, bool branchless>
	void backward_merge(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 dest, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("backward_merge %d %d\n", len1, len2);      printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0], len2);

		Iter1  ptl, tpl; // tail pointer left, array, right
		Iter2 tpr, ptr;
		Iter3 tpd, ptd;
		size_t loop, x, y, len;

		ptr = start2;
		ptl = start1;
		tpr = start2 + len2 - 1;
		tpl = start1 + len1 - 1;
		tpd = dest + len1 + len2 - 1;
		ptd = dest;


		while (ptl < tpl - 1 && ptr < tpr - 1)
		{

			if (cmp(*(tpr - 1), *tpl) == 0)
			{
				*tpd-- = *tpr--; *tpd-- = *tpr--;
			}
			else if (cmp(*tpr, *(tpl - 1)))
			{
				*tpd-- = *tpl--; *tpd-- = *tpl--;
			}
			else
			{
				x = cmp(*tpr, *tpl) == 0; y = !x; tpd--; tpd[x] = *tpr; tpr -= 1; tpd[y] = *tpl; tpl -= 1; tpd--;
				tail_merge<Iter1, Iter2, Iter3, Compare, branchless>(tpd, y, tpl, tpr, cmp);
			}


		}
		while (tpr >= start2 && tpl >= start1)
		{
			*tpd-- = cmp(*tpr, *tpl) ? *tpl-- : *tpr--;
			//tail_merge<Iter1, Iter2, Iter3, Compare, branchless>(tpd, y, tpl, tpr, cmp);
		}
		while (ptr <= tpr)
		{
			*ptd++ = *ptr++;
		}

		//printf("end backward_merge\n");      printarray<T>((T*)&dest[0], len1+len2);
	}



	template<class Iter, class Compare, bool branchless>
	void tail_swap(Iter array,  size_t len, typename std::iterator_traits<Iter>::value_type* swap, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("tail\n"); printarray<T>((T*)&array[0], len);

		if (len <= 16)
		{
			tiny_sort<Iter, Compare, branchless>(array, len, swap, cmp);
			return;
		}

		size_t quad1, quad2, quad3, quad4, half1, half2;

		half1 = len / 2;
		quad1 = half1 / 2;
		quad2 = half1 - quad1;

		half2 = len - half1;
		quad3 = half2 / 2;
		quad4 = half2 - quad3;

		Iter pta = array;
		tiny_sort<Iter, Compare, branchless>(pta, quad1, swap, cmp); pta += quad1;
		tiny_sort<Iter, Compare, branchless>(pta, quad2, swap, cmp); pta += quad2;
		tiny_sort<Iter, Compare, branchless>(pta, quad3, swap, cmp); pta += quad3;
		tiny_sort<Iter, Compare, branchless>(pta, quad4, swap, cmp);
		if (cmp(*(array + quad1), *(array + quad1 - 1)) == 0 && cmp(*(array + half1), *(array + half1 - 1)) == 0 && cmp(*pta, *(pta - 1)) == 0)
		{
			return;
		}

		parity_merge<Iter, T*, Compare, branchless>(array, quad1, array + quad1, quad2, swap, cmp);
		parity_merge<Iter, T*, Compare, branchless>(array + half1, quad3, array + half1 + quad3, quad4, swap + half1, cmp);
		parity_merge<T*, Iter, Compare, branchless>(swap, half1, swap + half1, half2, array, cmp);
	}


	




	template<class Iter, class Iter3, class Compare, bool branchless>
	void merge_select(Iter start1, size_t len1, size_t unorder1, Iter start2, size_t len2, size_t unorder2, Iter3 dest, Compare cmp) {

		bool order = (unorder1 + unorder2) * 23 < len1 + len2;
		if (order) {
			branchless_merge_gallop<Iter, Iter3, Compare, branchless>(start1, len1, start2, len2, dest, cmp);
		}else{
			if constexpr (branchless) {
				if (len1 + len2 > 200000) {
					branchless_merge_gallop<Iter, Iter3, Compare, branchless>(start1, len1, start2, len2, dest, cmp);
				}
				else {
					branchless_merge<Iter, Iter, Iter3, Compare, branchless>(start1, len1, start2, len2, dest, cmp);
				}

			}
			else {
				if (len1 + len2 > 200000) {
					branch_merge_gallop<Iter, Iter, Iter3, Compare, branchless>(start1, len1, start2, len2, dest, cmp);
				}
				else {
					branch_merge<Iter, Iter, Iter3, Compare, branchless>(start1, len1, start2, len2, dest, cmp);
				}
			}
		}
	}



	template<class Iter, class Compare, bool branchless>
	void trimerge(Iter start1, size_t len1, size_t unorder1, Iter start2, size_t len2, size_t unorder2, Iter start3, size_t len3, size_t unorder3, typename std::iterator_traits<Iter>::value_type* swap, Compare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		Iter A, B, C;
		size_t lena, lenb, lenc, unordera, unorderb;

		if (len3 > len2) {
			if (len3 > len1) {
				C = start3;
				lenc = len3;
				A = start1;
				B = start2;
				lena = len1;
				lenb = len2;
				unordera = unorder1;
				unorderb = unorder2;
			}
			else {
				C = start1;
				lenc = len1;
				A = start2;
				B = start3;
				lena = len2;
				lenb = len3;
			}
		}
		else {
			if (len2 > len1) {

				C = start2;
				lenc = len2;
				A = start1;
				B = start3;
				lena = len1;
				lenb = len3;
				unordera = unorder1;
				unorderb = unorder3;
				/*
				if (cmp( *start3, *(start1 + len1 - 1))==0) {
					std::move(start1, start1 + len1, swap + lenc);
					std::move(start3, start3 + len3, swap + lenc+len1);
					forward_merge<T*, Iter,T*, Compare, branchless>(swap + len2, len1 + len3, start2, len2, swap, cmp);
					return;
				}
				*/
			}
			else {
				C = start1;
				lenc = len1;
				A = start2;
				B = start3;
				lena = len2;
				lenb = len3;
				unordera = unorder2;
				unorderb = unorder3;
			}
		}
		merge_select<Iter, T*, Compare, branchless>(A, lena, unordera, B, lenb, unorderb, swap + lenc, cmp);
		forward_merge_gallop<T*, Iter, T*, Compare, branchless>(C, lenc, swap + lenc, lena + lenb, swap, cmp);
	}


	template<class Iter, class Compare, bool branchless>//
	void quadmerge(Iter start1, size_t len1, size_t unorder1, Iter start2, size_t len2, size_t unorder2, Iter start3, size_t len3, size_t unorder3, Iter start4, size_t len4, size_t unorder4, typename std::iterator_traits<Iter>::value_type* swap, Compare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("quadmerge\n");     printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0], len2); printarray<T>((T*)&start3[0], len3); printarray<T>((T*)&start4[0], len4);

		if (len1 > len2 + len3 + len4) {

			trimerge<Iter, Compare, branchless>(start2, len2, unorder2, start3, len3, unorder3, start4, len4, unorder4, swap, cmp);
			qt_sort::backward_merge_gallop<T*, Iter, Iter, Compare, branchless>(start1, len1, swap, len2 + len3 + len4, start1, cmp);
		}
		else if (len4 > len1 + len2 + len3) {
			trimerge<Iter, Compare, branchless>(start1, len1, unorder1, start2, len2, unorder2, start3, len3, unorder3, swap, cmp);
			forward_merge_gallop<T*, Iter, Iter, Compare, branchless>(swap, +len1 + len2 + len3, start4, len4, start1, cmp);
		}
		else {
			merge_select<Iter, T*, Compare, branchless>(start1, len1, unorder1, start2, len2, unorder2, swap, cmp);
			merge_select<Iter, T*, Compare, branchless>(start3, len3, unorder3, start4, len4, unorder4, swap + len1 + len2, cmp);
			merge_select<T*, Iter, Compare, branchless>(swap, len1 + len2, unorder1 + unorder2, swap + len1 + len2, len3 + len4, unorder3 + unorder4, start1, cmp);

		}

	}
	template<class Iter, class Compare, bool branchless>//
	void quadmerge_small(Iter start1, size_t len1,  Iter start2, size_t len2, Iter start3, size_t len3, Iter start4, size_t len4,  typename std::iterator_traits<Iter>::value_type* swap, Compare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("quadmerge\n");     printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0], len2); printarray<T>((T*)&start3[0], len3); printarray<T>((T*)&start4[0], len4);
		branchless_merge_gallop<Iter, T*, Compare, branchless>(start1, len1, start2, len2,  swap, cmp);
		branchless_merge_gallop<Iter, T*, Compare, branchless>(start3, len3, start4, len4, swap + len1 + len2, cmp);
		branchless_merge_gallop<T*, Iter, Compare, branchless>(swap, len1 + len2,  swap + len1 + len2, len3 + len4, start1, cmp);
	}
#define tqmergeratio 2
	template<class Iter, class Compare, bool branchless>
	void mergestack(run<Iter>* ts, size_t& stacksize, typename std::iterator_traits<Iter>::value_type* swap, Compare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("mergestack %d\n", stacksize);
		while (stacksize >= 4) {
			run<Iter> run1 = ts[stacksize - 4], run2, run3, run4 = ts[stacksize - 1];
			if (run1.len <= tqmergeratio * run4.len) {
				run2 = ts[stacksize - 3];
				run3 = ts[stacksize - 2];
				quadmerge<Iter, Compare, branchless>(run1.start, run1.len, run1.unorder, run2.start, run2.len, run2.unorder, run3.start, run3.len, run3.unorder, run4.start, run4.len, run4.unorder, swap, cmp);
				ts[stacksize - 4].len = run1.len + run2.len + run3.len + run4.len;
				ts[stacksize - 4].unorder = run1.unorder + run2.unorder + run3.unorder + run4.unorder;
				stacksize = stacksize - 3;
			}
			else {
				break;
			}
		}
	}

	//__declspec(noinline) 
	template<class Iter, class Compare, bool branchless>
	void forcecollapsestack(run<Iter>* ts, typename std::iterator_traits<Iter>::value_type* swap, size_t stacksize, Compare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("forcecollapsestack %d\n", stacksize); 

		run<Iter> run1, run2, run3, run4;

		while (stacksize >= 4) {
			run1 = ts[stacksize - 4];
			run2 = ts[stacksize - 3];
			run3 = ts[stacksize - 2];
			run4 = ts[stacksize - 1];
			quadmerge<Iter, Compare, branchless>(run1.start, run1.len, run1.unorder, run2.start, run2.len, run2.unorder, run3.start, run3.len, run3.unorder, run4.start, run4.len, run4.unorder, swap, cmp);
			ts[stacksize - 4].len = run1.len + run2.len + run3.len + run4.len;
			ts[stacksize - 4].unorder = run1.unorder + run2.unorder + run3.unorder + run4.unorder;
			stacksize = stacksize - 3;
		}

		switch (stacksize) {
		case 3:
			run3 = ts[stacksize - 1];
			run2 = ts[stacksize - 2];
			run1 = ts[stacksize - 3];;
			//forward_merge<Iter, Iter, T*, Compare, branchless>(run3.start, run3.len, run2.start, run2.len, swap, cmp);
			merge_select<Iter, T*, Compare, branchless>(run3.start, run3.len, run3.unorder, run2.start, run2.len, run2.unorder, swap, cmp);
			backward_merge_gallop<Iter, T*, Iter, Compare, branchless>(run1.start, run1.len, swap, run2.len + run3.len, run1.start, cmp);
			break;
		case 2:

			run2 = ts[stacksize - 1];
			run1 = ts[stacksize - 2];
			std::move(run2.start, run2.start + run2.len, swap);
			backward_merge_gallop<Iter, T*, Iter, Compare, branchless>(run1.start, run1.len, swap, run2.len, run1.start, cmp);
		}
	}

	template<class Iter, class Compare, bool branchless>
	void forcecollapsestack_small(run<Iter>* ts, typename std::iterator_traits<Iter>::value_type* swap, size_t stacksize, Compare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("forcecollapsestack %d\n", stacksize); 

		run<Iter> run1, run2, run3, run4;

		while (stacksize >= 4) {
			run1 = ts[stacksize - 4];
			run2 = ts[stacksize - 3];
			run3 = ts[stacksize - 2];
			run4 = ts[stacksize - 1];
			quadmerge_small<Iter, Compare, branchless>(run1.start, run1.len, run2.start, run2.len, run3.start, run3.len, run4.start, run4.len, swap, cmp);
			ts[stacksize - 4].len = run1.len + run2.len + run3.len + run4.len;
			stacksize = stacksize - 3;
		}

		switch (stacksize) {
		case 3:
			run3 = ts[stacksize - 1];
			run2 = ts[stacksize - 2];
			run1 = ts[stacksize - 3];;
			branchless_merge_gallop<Iter, T*, Compare, branchless>(run3.start, run3.len, run2.start, run2.len, swap, cmp);
			backward_merge<Iter, T*, Iter, Compare, branchless>(run1.start, run1.len, swap, run2.len + run3.len, run1.start, cmp);
			break;
		case 2:

			run2 = ts[stacksize - 1];
			run1 = ts[stacksize - 2];
			std::move(run2.start, run2.start + run2.len, swap);
			backward_merge<Iter, T*, Iter, Compare, branchless>(run1.start, run1.len, swap, run2.len, run1.start, cmp);
			//printf("asdf\n"); printarray<T>((T*)&run1.start[0], run1.len + run2.len);
		}
		
	}

#define smallthreshold 150
	template<class Iter, class Compare, bool branchless>
	void sortloop_small(Iter start, const size_t len, Compare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		run<Iter> ts[4];
		size_t stacksize = 0;
		run<Iter> newrun;
		Iter end, pta, end2;

		T alignas(64)swap[smallthreshold];
		end = start + len;
		pta = start;
		end2 = end - 64;

		while (end2 - pta >= 0) {
			newrun.start = pta;
			parity_swap_sixteen<Iter, Compare, branchless>(pta, swap, cmp); pta += 16;
			parity_swap_sixteen<Iter, Compare, branchless>(pta, swap, cmp); pta += 16;
			parity_swap_sixteen<Iter, Compare, branchless>(pta, swap, cmp); pta += 16;
			parity_swap_sixteen<Iter, Compare, branchless>(pta, swap, cmp); pta += 16;

			pta -= 64;

			newrun.unorder = cmp(*(pta + 16), *(pta + 15)) + cmp(*(pta + 32), *(pta + 31)) +cmp(*(pta + 48), *(pta + 47));

			if (newrun.unorder) {
				parity_merge<Iter, T*, Compare, branchless>(pta, 16, pta + 16, 16, swap, cmp);
				parity_merge<Iter, T*, Compare, branchless>(pta + 32, 16, pta + 48, 16, swap + 32, cmp);
				parity_merge<T*, Iter, Compare, branchless>(swap, 32, swap + 32, 32, pta, cmp);
			}
			pta += 64;


			while (pta < end && cmp(*(pta), *(pta - 1)) == 0) {
				pta++;
			}
			newrun.len = (size_t)(pta - newrun.start);
			//printf("newrun %d\n",pta - newrun.start);  printarray<T>((T*)&newrun.start[0], pta - newrun.start);

			ts[stacksize] = newrun;
			stacksize++;

		}
		if (end - pta > 0) {
			newrun.start = pta;
			newrun.len = (size_t)(end - pta);
			qt_sort::tail_swap<Iter, Compare, branchless>(newrun.start,  newrun.len, swap, cmp);
			ts[stacksize] = newrun;	stacksize++;
			//printf("tail newrun %d\n", newrun.len); printarray<T>((T*)&pta[0], end - pta);

		}
		forcecollapsestack_small<Iter, Compare, branchless>(ts, swap, stacksize, cmp);
	}

	template<class Iter, class Compare, bool branchless>
	void sortloop(Iter start, const size_t len, Compare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		run<Iter> ts[64];
		size_t stacksize = 0;
		run<Iter> newrun;
		Iter end, pta, runstart, end2;
		T alignas(64)swapstack[128];

		T* swap = new T[len];// (T*)malloc(len * sizeof(T));
		//swap3 = reinterpret_cast<T*> ((reinterpret_cast<std::size_t>(swap) + 63) & -64);

		end = start + len;
		pta = start;

		end2 = end - 64;

		while (end2 - pta >= 0) {
			newrun.start = pta;
			
			parity_swap_sixteen<Iter, Compare, branchless>(pta, swapstack, cmp); pta += 16;
			parity_swap_sixteen<Iter, Compare, branchless>(pta, swapstack, cmp); pta += 16;
			parity_swap_sixteen<Iter, Compare, branchless>(pta, swapstack, cmp); pta += 16;
			parity_swap_sixteen<Iter, Compare, branchless>(pta, swapstack, cmp); pta += 16;
		
			pta -= 64;

			newrun.unorder = cmp(*(pta + 16), *(pta + 15)) + cmp(*(pta + 32), *(pta + 31)) + cmp(*(pta + 48), *(pta + 47));

			if (newrun.unorder) {
				parity_merge<Iter, T*, Compare, branchless>(pta, 16, pta + 16, 16, swapstack, cmp);
				parity_merge<Iter, T*, Compare, branchless>(pta + 32, 16, pta + 48, 16, swapstack + 32, cmp);
				parity_merge<T*, Iter, Compare, branchless>(swapstack, 32, swapstack + 32, 32, pta, cmp);
			}
			pta += 64;


			while (pta < end && cmp(*(pta), *(pta - 1)) == 0) {
				pta++;
			}
			newrun.len = (size_t)(pta - newrun.start);
			//printf("newrun %d\n",pta - newrun.start);  printarray<T>((T*)&newrun.start[0], pta - newrun.start);

			ts[stacksize] = newrun;
			stacksize++;



			mergestack<Iter, Compare, branchless>(ts, stacksize, swap, cmp);

		}
		if (end - pta > 0) {
			newrun.start = pta;
			newrun.len = (size_t)(end - pta);
			newrun.unorder = (size_t)1;
			qt_sort::tail_swap<Iter, Compare, branchless>(newrun.start,  newrun.len, swap, cmp);
			ts[stacksize] = newrun;	stacksize++;
			//printf("tail newrun\n"); printarray<T>((T*)&pta[0], end - pta);

		}
		forcecollapsestack<Iter, Compare, branchless>(ts, swap, stacksize, cmp);
		delete swap;
	}


}

///////////////
//TQSORT
//////////////


template<class Iter, class T = typename std::iterator_traits<Iter>::value_type, class Compare = std::less<T>>
void tqsort(Iter start, size_t len, Compare cmp) {
	if (len <= 64) {
		T swap[64];
		qt_sort::tail_swap<Iter, Compare, std::is_integral<T>::value || std::is_floating_point<T>::value>(start,  len, swap, cmp);
	}
	else if (len < smallthreshold) {
		qt_sort::sortloop_small<Iter, Compare, std::is_integral<T>::value || std::is_floating_point<T>::value>(start, len, cmp);
	}
	else {
		qt_sort::sortloop<Iter, Compare, std::is_integral<T>::value || std::is_floating_point<T>::value>(start, len, cmp);
		//qt_sort::tqquad_swap<Iter, Compare, std::is_integral<T>::value || std::is_floating_point<T>::value>(start, len, cmp);
	}
}
template<class Iter, class T = typename std::iterator_traits<Iter>::value_type, class Compare = std::less<T>>
void tqsort_branching(Iter start, size_t len, Compare cmp) {
	if (len < 64) {
		T swap[64];
		qt_sort::tail_swap<Iter, Compare, false>(start, len, swap, cmp);
	}
	else if (len < smallthreshold) {
		qt_sort::sortloop_small<Iter, Compare, false>(start, len, cmp);
	}
	else {
		qt_sort::sortloop<Iter, Compare, false>(start, len, cmp);
	}
}
template<class Iter, class T = typename std::iterator_traits<Iter>::value_type, class Compare = std::less<T>>
void tqsort_branchless(Iter start, size_t len, Compare cmp) {
	if (len < 64) {
		T swap[64];
		qt_sort::tail_swap<Iter, Compare, true>(start, len, swap, cmp);
	}
	else if (len < smallthreshold) {
		qt_sort::sortloop_small<Iter, Compare, true>(start, len, cmp);
	}
	else {
		qt_sort::sortloop<Iter, Compare, true>(start, len, cmp);
	}
}

