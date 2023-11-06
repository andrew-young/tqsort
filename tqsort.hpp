#pragma once
#include "util.h"
#include <stdlib.h>
#include <utility>


namespace tq_sort {

	template<class Iter>
	struct run {
		Iter start;
		size_t len;
		size_t unorder;//higher means less sorted
	};



	template<class Iter1, class Iter2, class Iter3, class cmpare,bool branchless>
	inline void head_merge(Iter3& ptd, size_t x, Iter1& ptl, Iter2& ptr, cmpare cmp) {
		if constexpr (branchless){
			x = cmp(*ptr, *ptl) == 0; *ptd = *ptl; ptl += x; ptd[x] = *ptr; ptr += !x; ptd++;
		}
		else {
			//*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;
			x = cmp(*ptr, *ptl) == 0; *ptd = *ptl; ptl += x; ptd[x] = *ptr; ptr += !x; ptd++;
		}
	}

	template<class Iter1, class Iter2, class Iter3, class cmpare, bool branchless>
	inline void tail_merge(Iter3& tpd, size_t x, Iter1& tpl, Iter2& tpr, cmpare cmp) {
		if constexpr (branchless) {
			x = cmp(*tpr, *tpl) == 0;	*tpd = *tpl;tpl -= !x;	tpd--;tpd[x] = *tpr;tpr -= x;
		}
		else {
			//*tpd-- = cmp(*tpr, *tpl) == 0 ? *tpr-- : *tpl--;
			x = cmp(*tpr, *tpl) == 0;	*tpd = *tpl; tpl -= !x;	tpd--; tpd[x] = *tpr; tpr -= x;
		}
	}

	template<class Iter, class Iter2, class cmpare>
	void swap_branchless(Iter pta, Iter2 swap, size_t x, size_t y, cmpare cmp) {
		x = cmp(*(pta + 1), *pta);	y = !x; swap = pta[y];	pta[0] = pta[x];	pta[1] = swap;
	}


	

	// class T = typename std::iterator_traits<Iter>::value_type,
	template<class Iter, class cmpare,bool branchless>
	inline void parity_merge_two(Iter array, typename std::iterator_traits<Iter>::value_type* swap, size_t x, size_t y, Iter ptl, Iter ptr, typename std::iterator_traits<Iter>::value_type* pts, cmpare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		ptl = array; ptr = array + 2; pts = swap;
		head_merge<Iter, Iter, T*, cmpare, branchless>(pts, x, ptl, ptr, cmp);
		*pts = cmp(*ptr, *ptl) == 0 ? *ptl : *ptr;

		ptl = array + 1; ptr = array + 3; pts = swap + 3;
		tail_merge<Iter, Iter, T*, cmpare, branchless>(pts, y, ptl, ptr, cmp);
		*pts = cmp(*ptr, *ptl) ? *ptl : *ptr;
	}

	template<class Iter1, class Iter2,class cmpare,bool branchless>
	void parity_merge_four(Iter1 array, Iter2 swap, size_t x, size_t y, Iter1 ptl, Iter1 ptr, Iter2 ptd, cmpare cmp) {
		typedef typename std::iterator_traits<Iter1>::value_type T;
		ptl = array + 0; ptr = array + 4; ptd = swap;
		head_merge<Iter1, Iter1, Iter2, cmpare, branchless>(ptd, x, ptl, ptr, cmp);
		head_merge<Iter1, Iter1, Iter2, cmpare, branchless>(ptd, x, ptl, ptr, cmp);
		head_merge<Iter1, Iter1, Iter2, cmpare, branchless>(ptd, x, ptl, ptr, cmp);
		*ptd = cmp(*ptr, *ptl) ? *ptr : *ptl;

		ptl = array + 3; ptr = array + 7; ptd = swap + 7;
		tail_merge<Iter1, Iter1, Iter2, cmpare, branchless>(ptd, y, ptl, ptr, cmp);
		tail_merge<Iter1, Iter1, Iter2, cmpare, branchless>(ptd, y, ptl, ptr, cmp);
		tail_merge<Iter1, Iter1, Iter2, cmpare, branchless>(ptd, y, ptl, ptr, cmp);
		*ptd = cmp(*ptr, *ptl) == 0 ? *ptr : *ptl;
	}



	template<class Iter, class cmpare,bool branchless>
	void parity_swap_eight(Iter array, typename std::iterator_traits<Iter>::value_type* swap, cmpare cmp)
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
		parity_merge_two<Iter, cmpare, branchless>(array + 0, swap + 0, x, y, ptl, ptr, pts, cmp);
		parity_merge_two<Iter, cmpare, branchless>(array + 4, swap + 4, x, y, ptl, ptr, pts, cmp);
		parity_merge_four<T*, Iter, cmpare, branchless>(swap, array, x, y, ptls, ptrs, pta, cmp);
	}

	template<class Iter, class cmpare>
	void quad_swap_four(Iter array, cmpare cmp)
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

	template<class Iter1, class Iter2, class cmpare,bool branchless>
	void parity_merge( Iter1 start1, size_t len1,Iter1 start2, size_t len2, Iter2 dest, cmpare cmp)
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
		tpl = ptl+len1 - 1;
		tpr = ptr + len2-1;
		tpd = dest + len1 + len2 - 1;

		if (len1 < len2)
		{
			*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;
		}

		*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;

		while (--len1)
		{
			head_merge<Iter1, Iter1, Iter2,cmpare,branchless>(ptd, x, ptl, ptr, cmp);
			tail_merge<Iter1, Iter1, Iter2, cmpare, branchless>(tpd, y, tpl, tpr, cmp);
		}
		*tpd = cmp(*tpr, *tpl) == 0 ? *tpr : *tpl;
	}


	template<class Iter, class cmpare,bool branchless>
	void parity_swap_sixteen(Iter array, typename std::iterator_traits<Iter>::value_type* swap, cmpare cmp)
	{
		typedef typename std::iterator_traits<Iter>::value_type T;
		Iter ptl, ptr;
		T* pts;
		size_t x, y;
		quad_swap_four(array + 0, cmp);
		quad_swap_four(array + 4, cmp);
		quad_swap_four(array + 8, cmp);
		quad_swap_four(array + 12, cmp);
		if (cmp(*(array + 4), *(array + 3)) == 0 && cmp(*(array + 8), *(array + 7)) == 0 && cmp(*(array + 12), *(array + 11)) == 0)
		{
			return;
		}
		
		parity_merge_four<Iter, T*, cmpare, branchless>(array + 0, swap + 0, x, y, ptl, ptr, pts, cmp);
		parity_merge_four<Iter, T*, cmpare, branchless>(array + 8, swap + 8, x, y, ptl, ptr, pts, cmp);

		parity_merge<T* ,Iter, cmpare, branchless>(swap, 8, swap+8, 8, array, cmp);
	}

	template<class Iter, class cmpare>
	void tiny_sort(Iter array, size_t nmemb, cmpare cmp)
	{
		typedef typename std::iterator_traits<Iter>::value_type T;
		T swap;
		Iter pta;
		size_t x, y;

		switch (nmemb)
		{
		case 4:
			pta = array;
			x = cmp(pta[1], *pta); y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta += 2;
			x = cmp(pta[1], *pta); y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta--;

			if (cmp(pta[1], *pta))
			{
				swap = pta[0]; pta[0] = pta[1]; pta[1] = swap; pta--;

				x = cmp(pta[1], *pta); y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta += 2;
				x = cmp(pta[1], *pta); y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta--;
				x = cmp(pta[1], *pta); y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap;
			}
			return;
		case 3:
			pta = array;
			x = cmp(pta[1], *pta); y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta++;
			x = cmp(pta[1], *pta); y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap;
		case 2:
			pta = array;
			x = cmp(pta[1], *pta); y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap;
		case 1:
		case 0:
			return;
		}
	}




	template<class Iter, class cmpare>
	void insertion_sort(Iter begin, Iter unorderedstart, Iter end, cmpare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		if (begin == end) return;

		for (Iter cur = unorderedstart; cur != end; ++cur) {
			Iter sift = cur;
			Iter sift_1 = cur - 1;

			// cmpare first so we can avoid 2 moves for an element already positioned correctly.
			if (cmp(*sift, *sift_1)) {
				T tmp = *sift;

				do { *sift-- = *sift_1; } 
				while (sift != begin && cmp(tmp, *--sift_1));

				*sift = tmp;
			}
		}
	}


	//faster than branchless for  strings not ints
	template<class Iter, class Iter2, class cmpare>
	inline void insertion_sort16b(Iter start, Iter unorderedstart,Iter2 swap, cmpare cmp) {

		typedef typename std::iterator_traits<Iter>::value_type T;
		Iter cur;
		Iter cur2;
		Iter shift;
		Iter shift2;
		switch (unorderedstart - start) {
		case 1:
		case 2:
		case 3:
			tq_sort::parity_swap_sixteen<Iter, std::less<T>, false>(start, swap, std::less<T>());
			break;
		case 4:
			cur = start + 3;
			cur2 = unorderedstart;
			goto insert16_5b;
			break;
		case 5:
			cur = unorderedstart;
			cur2 = start+4;
			goto insert16_6b;
			break;
		case 6:
			cur = start + 5;
			cur2 = unorderedstart;
			goto insert16_7b;
			break;
		case 7:
			cur = unorderedstart;
			cur2 = start+6;
			goto insert16_8b;
			break;
		case 8:
			cur = start + 7;
			cur2 = unorderedstart;
			goto insert16_9b;
			break;
		case 9:
			cur = unorderedstart;
			cur2 = start+8;
			goto insert16_10b;
			break;
		case 10:
			cur = start + 9;
			cur2 = unorderedstart;
			goto insert16_11b;
			break;
		case 11:
			cur = unorderedstart;
			cur2 = start+10;
			goto insert16_12c;
			break;
		case 12:
			cur = start + 11;
			cur2 = unorderedstart;
			goto insert16_13b;
			break;
		case 13:
			cur = unorderedstart;
			cur2 = start+12;
			goto insert16_14b;
			break;
		case 14:
			cur = start + 13;
			cur2 = unorderedstart;
			goto insert16_15b;
			break;
		case 15:
			cur = unorderedstart;
			cur2 = start+14;
			goto insert16_16b;
			break;

		}
	insert16_2c:
		if (cmp(*unorderedstart, *start)) {
			goto insert16_2b;
		}
	insert16_3a:
		cur2 = start + 2;
		cur = start + 1;
		if (cmp(*cur2, *cur)) {
			goto insert16_3b;
		}
	insert16_4a:
		cur += 2;
	insert16_4c:
		if (cmp(*cur, *cur2)) {
			goto insert16_4b;
		}
	insert16_5a:
		cur2 += 2;
	insert16_5c:
		
		if (cmp(*cur2, *cur)) {
			goto insert16_5b;
		}
	insert16_6a:
		cur += 2;
	insert16_6c:
		if (cmp(*cur, *cur2)) {
			goto insert16_6b;
		}
	insert16_7a:
		cur2 += 2;
	insert16_7c:
		
		if (cmp(*cur2, *cur)) {
			goto insert16_7b;
		}
	insert16_8a:
		cur += 2;
	insert16_8c:
		
		if (cmp(*cur, *cur2)) {
			goto insert16_8b;
		}
	insert16_9a:
		cur2 += 2;
	insert16_9c:
		
		if (cmp(*cur2, *cur)) {
			goto insert16_9b;
		}
	insert16_10a:
		cur += 2;
	insert16_10c:
		
		if (cmp(*cur, *cur2)) {
			goto insert16_10b;
		}
	insert16_11a:
		cur2 += 2;
	insert16_11c:
		
		if (cmp(*cur2, *cur)) {
			goto insert16_11b;
		}
	insert16_12a:
		cur += 2;
	insert16_12c:
		
		if (cmp(*cur, *cur2)) {
			goto insert16_12b;
		}
	insert16_13a:
		cur2 += 2;
	insert16_13c:
		
		if (cmp(*cur2, *cur)) {
			goto insert16_13b;
		}
	insert16_14a:
		cur += 2;
	insert16_14c:
		
		if (cmp(*cur, *cur2)) {
			goto insert16_14b;
		}
	insert16_15a:
		cur2 += 2;
	insert16_15c:
		
		if (cmp(*cur2, *cur)) {
			goto insert16_15b;
		}
	insert16_16a:
		cur += 2;
	insert16_16c:
		
		if (cmp(*cur, *cur2)) {
			goto insert16_16b;
		}
		return;
		//printf("insert\n"); printarray<T>((T*)&start[0], 8);
	
		
		T tmp;
		bool x;
		//2
		insert16_2b:
			tmp = *unorderedstart;
			*unorderedstart = *start;
			*start = tmp;
			goto insert16_3a;
		
		insert16_3b:
			tmp = *cur2;
			*cur2 = *cur;
			shift2 = cur2 - 2;

			x = !cmp(tmp, *shift2);
			*cur = *shift2;
			shift2[x] = tmp;
			goto insert16_4a;
		//4
		
		insert16_4b:
			tmp = *cur;
			*cur = *cur2;
			shift = cur - 2;
			if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
			else { *cur2 = tmp; goto insert16_5a;			}
			x = !cmp(tmp, *shift2);
			*shift = *shift2;
			shift2[x] = tmp;
			goto insert16_5a;
		//5
		insert16_5b:
			tmp = *cur2;
			*cur2 = *cur;
			shift2 = cur2 - 2;
			if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
			else { *cur = tmp;  goto insert16_6a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_6a;	}
			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			goto insert16_6a;
		//6
	insert16_6b:
			tmp = *cur;
			*cur = *cur2;
			shift = cur - 2;
			if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
			else { *cur2 = tmp; goto insert16_7a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_7a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_7a;	}
			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			goto insert16_7a;
		//7
	insert16_7b:
			tmp = *cur2;
			*cur2 = *cur;
			shift2 = cur2 - 2;
			if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
			else { *cur = tmp;  goto insert16_8a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_8a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  goto insert16_8a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_8a;	}

			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			goto insert16_8a;
		//8
	insert16_8b:
			tmp = *cur;
			*cur = *cur2;
			shift = cur - 2;
			if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
			else { *cur2 = tmp; goto insert16_9a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  goto insert16_9a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_9a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_9a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_9a;	}

			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;

			goto insert16_9a;
		//9
	insert16_9b:
			tmp = *cur2;
			*cur2 = *cur;
			shift2 = cur2 - 2;
			if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
			else { *cur = tmp; goto insert16_10a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_10a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_10a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_10a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_10a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_10a;	}

			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			goto insert16_10a;
		//10
	insert16_10b:
			tmp = *cur;
			*cur = *cur2;
			shift = cur - 2;
			if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
			else { *cur2 = tmp; goto insert16_11a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;   goto insert16_11a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  goto insert16_11a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;   goto insert16_11a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  goto insert16_11a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  goto insert16_11a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  goto insert16_11a;	}

			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;

			goto insert16_11a;
		//11
	insert16_11b:
			tmp = *cur2;
			*cur2 = *cur;
			shift2 = cur2 - 2;
			if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
			else { *cur = tmp;  goto insert16_12a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_12a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_12a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_12a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_12a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_12a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_12a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_12a;	}

			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			goto insert16_12a;
		//12
	insert16_12b:
			tmp = *cur;
			*cur = *cur2;
			shift = cur - 2;
			if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
			else { *cur2 = tmp; goto insert16_13a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_13a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_13a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_13a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_13a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_13a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_13a;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_13a;	}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_13a;}

			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;

			goto insert16_13a;
		//13
	insert16_13b:
			tmp = *cur2;
			*cur2 = *cur;
			shift2 = cur2 - 2;
			if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
			else { *cur = tmp; goto insert16_14a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  goto insert16_14a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;   goto insert16_14a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  goto insert16_14a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;   goto insert16_14a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  goto insert16_14a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;   goto insert16_14a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  goto insert16_14a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;   goto insert16_14a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  goto insert16_14a;
			}

			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			goto insert16_14a;
		//14
	insert16_14b:
			tmp = *cur;
			*cur = *cur2;
			shift = cur - 2;
			if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
			else { *cur2 = tmp;  goto insert16_15a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  goto insert16_15a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  goto insert16_15a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  goto insert16_15a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_15a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_15a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_15a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_15a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;   goto insert16_15a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  goto insert16_15a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  goto insert16_15a;
			}

			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;

			goto insert16_15a;
		//15
	insert16_15b:
			tmp = *cur2;
			*cur2 = *cur;
			shift2 = cur2 - 2;
			if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
			else { *cur = tmp;  goto insert16_16a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_16a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  goto insert16_16a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_16a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  goto insert16_16a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_16a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_16a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_16a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; goto insert16_16a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_16a;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  goto insert16_16a;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; goto insert16_16a;
			}

			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			goto insert16_16a;
		//16
	insert16_16b:
			tmp = *cur;
			*cur = *cur2;
			shift = cur - 2;
			if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
			else { *cur2 = tmp; return;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; return;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  return;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  return;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;   return;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; return;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  return;	}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; return;}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  return;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; return;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; return;
			}
			if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; return;
			}
			if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; return;
			}

			x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;		
	}
#define forwardloop 8
	template<class Iter1, class Iter2, class Iter3, class cmpare,bool branchless>
	void forward_merge(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 swap, cmpare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("partial_forward_merge %d %d\n", len1, len2);      printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0] , len2);

		Iter1 tpl, ptl;  // tail pointer left, array, right
		Iter2 ptr, tpr;
		Iter3  ptd;
		size_t loop, x, y;

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

		//remove  mabye
		/*
		if (cmp(*ptr, *(tpl)) == 0)
		{
			if (start1 == swap) {

			}
			else {
				std::move(start1, start1 + len1, swap);
				std::move(start2, start2 + len2, swap+len1);
			}
			return;
		}
		*/


		while (ptl < tpl - forwardloop * 2 && ptr < tpr - forwardloop * 2)
		{

			tpl_tpr32: if (cmp(*(ptr + forwardloop * 2 - 1), *ptl))
			{
				loop = forwardloop * 2; do *ptd++ = *ptr++; while (--loop);

				if (ptr < tpr - forwardloop * 2) { goto tpl_tpr32; } break;
			}

			tpl32_tpr: if (cmp(*ptr, *(ptl + forwardloop * 2 - 1)) == 0)
			{
				loop = forwardloop * 2; do *ptd++ = *ptl++; while (--loop);

				if (ptl < tpl - forwardloop * 2) { goto tpl32_tpr; } break;
			}

			loop = forwardloop; do
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
					head_merge<Iter1, Iter2, Iter3, cmpare, branchless>(ptd, x, ptl, ptr, cmp);
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
				head_merge<Iter1, Iter2, Iter3, cmpare, branchless>(ptd, x, ptl, ptr, cmp);
			}
		}

		while (ptl <= tpl && ptr <= tpr)
		{
			*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
		}

		while (ptl <= tpl)
		{
			*ptd++ = *ptl++;
		}
		while (ptr <= tpr)
		{
			*ptd++ = *ptr++;
		}
	//printf("end forward_merge\n");      printarray<T>((T*)&swap[0], len1+len2);
	}
#define forwardloop2 8
	template<class Iter1, class Iter2, class Iter3, class cmpare, bool branchless>
	void uneven_merge(Iter1 startsmall, size_t lensmall, Iter2 startbig, size_t lenbig, Iter3 swap, cmpare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("partial_forward_merge %d %d\n", lensmall, lenbig);      printarray<T>((T*)&startsmall[0], lensmall); printarray<T>((T*)&startbig[0] , len2);

		Iter1 tpl, ptl;  // tail pointer left, array, right
		Iter2 ptr, tpr;
		Iter3  ptd;
		size_t loop, x, y;


		ptl = startsmall;
		ptr = startbig;
		tpl = startsmall + lensmall - 1;
		tpr = startbig + lenbig - 1;
		ptd = swap;


		while (ptr < tpr - forwardloop2 && ptl < tpl )
		{

		tpl_tpr32: 
			if (cmp(*(ptr + forwardloop2  - 1), *ptl))
			{
				loop = forwardloop2 ; do *ptd++ = *ptr++; while (--loop);

				if (ptr < tpr - forwardloop2) { goto tpl_tpr32; } break;
			}


			if (cmp(*(ptr + 1), *ptl))
			{
				*ptd++ = *ptr++; *ptd++ = *ptr++;
			}
			else if (ptl < tpl && cmp(*ptr, *(ptl + 1)) == 0)
			{
				*ptd++ = *ptl++; *ptd++ = *ptl++;
			}
			else
			{
				x = cmp(*ptr, *ptl) == 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
				head_merge<Iter1, Iter2, Iter3, cmpare, branchless>(ptd, x, ptl, ptr, cmp);
			}
		}
		while (ptl <= tpl && ptr <= tpr)
		{
			*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
		}

		while (ptl <= tpl)
		{
			*ptd++ = *ptl++;
		}
		while (ptr <= tpr)
		{
			*ptd++ = *ptr++;
		}
		//printf("end forward_merge\n");      printarray<T>((T*)&swap[0], lensmall+len2);
	}

#define crossgallop 8

	template<class Iter1, class Iter2, class cmpare,bool branchless>
	void cross_merge(Iter1 start1, size_t len1, Iter1 start2, size_t len2, Iter2 dest, cmpare cmp)
	{
		typedef typename std::iterator_traits<Iter2>::value_type T;
		//printf("cross_merge %d %d\n", len1, len2);     printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start1[0]+len1, len2);
		Iter1 ptl, tpl, ptr, tpr;
		Iter2 ptd, tpd;
		size_t loop,loop2;
		size_t x, y;
		ptl = start1;
		ptr = start2;
		tpl = start1+len1 - 1;
		tpr = start2 + len2-1;


		ptd = dest;
		tpd = dest + len1 + len2 - 1;

		while (tpl - ptl > crossgallop && tpr - ptr > crossgallop)
		{
		ptl8_ptr:
			if (cmp(*ptr, *(ptl + (crossgallop - 1))) == 0)
			{
				loop = crossgallop; do *ptd++ = *ptl++; while (--loop);

				if (tpl - ptl > crossgallop) { goto ptl8_ptr; } break;
			}

			else {
			ptl_ptr8:
				if (cmp(*(ptr + (crossgallop - 1)), *ptl))
				{
					loop = crossgallop; do *ptd++ = *ptr++; while (--loop);

					if (tpr - ptr > crossgallop) { goto ptl_ptr8; } break;
				}
			}

		tpl_tpr8:
			if (cmp(*(tpr - (crossgallop - 1)), *tpl) == 0)
			{
				loop = crossgallop; do *tpd-- = *tpr--; while (--loop);

				if (tpr - ptr > crossgallop) { goto tpl_tpr8; } break;
			}
			else {
			tpl8_tpr:
				if (cmp(*tpr, *(tpl - (crossgallop - 1))))
				{
					loop = crossgallop; do *tpd-- = *tpl--; while (--loop);

					if (tpl - ptl > crossgallop) { goto tpl8_tpr; } break;
				}
			}



			loop = crossgallop; do
			{
				head_merge<Iter1, Iter1, Iter2, cmpare, branchless>(ptd, x, ptl, ptr, cmp);
				tail_merge<Iter1, Iter1, Iter2, cmpare, branchless>(tpd, y, tpl, tpr, cmp);
				//*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;
				//*tpd-- = cmp(*tpr, *tpl) == 0 ? *tpr-- : *tpl--;
			} while (--loop);

		}

		while (tpl - ptl > crossgallop && tpr - ptr > crossgallop)
		{
		ptl8_ptr2: 
			if (cmp(*ptr, *(ptl + (crossgallop - 1))) == 0)
			{
				loop = crossgallop; do *ptd++ = *ptl++; while (--loop);

				if (tpl - ptl > crossgallop) { goto ptl8_ptr2; } break;
			}

			else {
			ptl_ptr82:
				if (cmp(*(ptr + (crossgallop - 1)), *ptl))
				{
					loop = crossgallop; do *ptd++ = *ptr++; while (--loop);

					if (tpr - ptr > crossgallop) { goto ptl_ptr82; } break;
				}
			}
			
		tpl_tpr82: 
			if (cmp(*(tpr - (crossgallop - 1)), *tpl) == 0)
			{
				loop = crossgallop; do *tpd-- = *tpr--; while (--loop);

				if (tpr - ptr > crossgallop) { goto tpl_tpr82; } break;
			}
			else {
			tpl8_tpr2: 
				if (cmp(*tpr, *(tpl - (crossgallop - 1))))
				{
					loop = crossgallop; do *tpd-- = *tpl--; while (--loop);

					if (tpl - ptl > crossgallop) { goto tpl8_tpr2; } break;
				}
			}
			
			
		
			loop = crossgallop; do
			{
				//head_merge(ptd, x, ptl, ptr, cmp);
				//tail_merge(tpd, y, tpl, tpr, cmp);
				*ptd++ = cmp(*ptr, *ptl) ? *ptr++ : *ptl++;
				*tpd-- = cmp(*tpr, *tpl) == 0 ? *tpr-- : *tpl--;
			} while (--loop);
		
		}
		/*
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
		*/
		
		while (ptl <= tpl && ptr <= tpr)
		{
			*ptd++ = cmp(*ptr, *ptl) == 0 ? *ptl++ : *ptr++;
		}

		while (ptl <= tpl)
		{
			*ptd++ = *ptl++;
		}
		while (ptr <= tpr)
		{
			*ptd++ = *ptr++;
		}
		
	}


	template<class Iter1, class Iter2,class Iter3,class cmpare,bool branchless>
	void backward_merge(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 dest,cmpare cmp)
	{
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("backward_merge %d %d\n", len1, len2);      printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0], len2);

		Iter1  ptl, tpl; // tail pointer left, array, right
		Iter2 tpr,ptr;
		Iter3 tpd;
		size_t loop, x, y;

		if (len2 == 0)
		{
			return;
		}
		ptr = start2;
		ptl = start1;
		tpr = start2 + len2 - 1;
		tpl = start1 + len1 - 1;
		tpd = dest + len1+len2 - 1;

		/*
		if (cmp(*start2, *tpl) == 0)
		{
			return;
		}
		*/
		//memcpy(start2, (&start1[0]+len1), (len2) * sizeof(T));

	
		while (tpl > start1 + 32 && tpr > start2 + 32)
		{

		tpl_tpr32: if (cmp(*(tpr - 31), *tpl) == 0)
		{
			loop = 32; do *tpd-- = *tpr--; while (--loop);

			if (tpr > start2 + 32) { goto tpl_tpr32; } break;
		}

	tpl32_tpr: if (cmp(*tpr, *(tpl - 31)))
	{
		loop = 32; do *tpd-- = *tpl--; while (--loop);

		if (tpl > start1 + 32) { goto tpl32_tpr; } break;
	}

	loop = 16; do
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
			tail_merge<Iter1, Iter2, Iter3, cmpare, branchless>(tpd, y, tpl, tpr, cmp);
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
				tail_merge<Iter1, Iter2, Iter3, cmpare, branchless>(tpd, y, tpl, tpr, cmp);
			}


		}

		while (tpr >= start2 && tpl >= start1)
		{
			*tpd-- = cmp(*tpr, *tpl) ? *tpl-- : *tpr--;
		}

		while (tpr >= start2)
		{
			*tpd-- = *tpr--;
		}
		while (tpl >= start1)
		{
			*tpd-- = *tpl--;
		}
		//printf("end backward_merge\n");      printarray<T>((T*)&dest[0], len1+len2);
	}


	//insertion sort is fast for strings
	//branchless is fast for ints
	
	

	template<class Iter, class cmpare,bool branchless>
	void tail_swap(Iter array, typename std::iterator_traits<Iter>::value_type* swap, size_t len, cmpare cmp)
	{
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("tail\n"); printarray<T>((T*)&array[0], len);
		if constexpr (branchless) {
			if (len < 5)
			{
				tiny_sort(array, len, cmp);
				return;
			}
			if (len < 8)
			{
				quad_swap_four(array, cmp);
				insertion_sort(array, array + 4, array + len, cmp);
				return;
			}
			if (len < 12)
			{
				parity_swap_eight<Iter, cmpare, branchless>(array, swap, cmp);
				insertion_sort(array, array + 8, array + len, cmp);
				return;
			}
			if (len >= 16 && len < 24)
			{
				parity_swap_sixteen<Iter, cmpare, branchless>(array, swap, cmp);
				insertion_sort(array, array + 16, array + len, cmp);
				return;
			}
		}
		else {
			if (len <= 16)
			{
				insertion_sort(array, array + 1, array + len, cmp);
				return;
			}
		}
		size_t quad1, quad2, quad3, quad4, half1, half2;

		half1 = len / 2;
		quad1 = half1 / 2;
		quad2 = half1 - quad1;

		half2 = len - half1;
		quad3 = half2 / 2;
		quad4 = half2 - quad3;

		Iter pta = array;
		if constexpr (branchless) {
			tail_swap<Iter,cmpare,true>(pta, swap, quad1, cmp); pta += quad1;
			tail_swap<Iter, cmpare, true>(pta, swap, quad2, cmp); pta += quad2;
			tail_swap<Iter, cmpare, true>(pta, swap, quad3, cmp); pta += quad3;
			tail_swap<Iter, cmpare, true>(pta, swap, quad4, cmp);
		}
		else {
			/*
			insertion_sort(pta, pta + 1, pta + quad1, cmp); pta += quad1;
			insertion_sort(pta, pta + 1, pta + quad2, cmp); pta += quad2;
			insertion_sort(pta, pta + 1, pta + quad3, cmp); pta += quad3;
			insertion_sort(pta, pta + 1, pta + quad4, cmp);
			*/
			
			tail_swap<Iter, cmpare, true>(pta, swap, quad1, cmp); pta += quad1;
			tail_swap<Iter, cmpare, true>(pta, swap, quad2, cmp); pta += quad2;
			tail_swap<Iter, cmpare, true>(pta, swap, quad3, cmp); pta += quad3;
			tail_swap<Iter, cmpare, true>(pta, swap, quad4, cmp);
			
		}
		if (cmp(*(array + quad1), *(array + quad1 - 1)) == 0 && cmp(*(array + half1), *(array + half1 - 1)) == 0 && cmp(*pta, *(pta - 1)) == 0)
		{
			return;
		}

		parity_merge<Iter, T*, cmpare, branchless>(array, quad1, array+quad1,quad2, swap, cmp);
		parity_merge<Iter, T*, cmpare, branchless>( array + half1, quad3, array + half1+quad3,quad4, swap + half1, cmp);
		parity_merge<T*, Iter, cmpare, branchless>( swap, half1,swap+half1, half2, array, cmp);
	}


	template<class Iter1, class Iter2, class Iter3, class cmpare,bool branchless>
	void merge(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 dest, cmpare cmp)
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


		if (cmp(*(ptr), *(ptl)) == 0) {
			*ptd++ = *ptl++;
			while (ptl <= tpl && cmp(*(ptr), *(ptl)) == 0) {
				*ptd++ = *ptl++;
			}
			if (ptl > tpl) {
				std::move(ptr, tpr + 1, ptd);
				return;
			}
		}
		else {
			*ptd++ = *ptr++;
			while (ptr <= tpr && cmp(*(ptr), *(ptl))) {
				*ptd++ = *ptr++;
			}
			if (ptr > tpr) {
				std::move(ptl, tpl + 1, ptd);
				return;
			}
		}
		///////////////////
		if (cmp(*(tpr), *(tpl))) {
			*tpd-- = *tpl--;
			while (cmp(*(tpr), *(tpl))) {
				*tpd-- = *tpl--;
			}
		}
		else {
			*tpd-- = *tpr--;
			while (cmp(*(tpr), *(tpl)) == 0) {
				*tpd-- = *tpr--;
			}
		}

		len = tpd - ptd + 1;
		if (len == 0) {
			return;
		}
		if (len & 1 == 0) {
			head_merge<Iter1, Iter2, Iter3, cmpare, branchless>(ptd, x, ptl, ptr, cmp);
			len--;
		}
		len = len / 2;
		while (len--)
		{
			head_merge<Iter1, Iter2, Iter3, cmpare, branchless>(ptd, x, ptl, ptr, cmp);
			tail_merge<Iter1, Iter2, Iter3, cmpare, branchless>(tpd, y, tpl, tpr, cmp);
		}

		//*ptd = cmp(*ptl, *ptr) > 0 ? *ptl : *ptr;
		*tpd = cmp(*tpr, *tpl) ? *tpl : *tpr;

		//printf("end merge\n");    printarray<T>((T*)&dest[0], n);

	}

	template<class Iter1, class Iter2, class Iter3, class cmpare,bool branchless>
	void forward_merge2(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 swap, cmpare cmp) {
		//forward_merge(start1, len1, start2, len2, swap, cmp);
		//return;
		if (len1 > len2) {
			forward_merge<Iter1, Iter2, Iter3, cmpare, branchless>(start2, len2, start1, len1, swap, cmp);
			return;
		}
		else if (len2 >= len1) {
			forward_merge<Iter1, Iter2, Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
			return;
		}
		
	}




	template<class Iter,  class Iter3, class cmpare,bool branchless>
	void merge2(Iter start1, size_t len1,size_t unorder1, Iter start2, size_t len2, size_t unorder2, Iter3 swap, cmpare cmp) {
		//merge<Iter, Iter, Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
		//return;
		/*
		if (len1+len2 < 64) {
			merge<Iter, Iter, Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
			return;
		}*/
		if (len1 > 2 * len2) {
			uneven_merge<Iter, Iter, Iter3, cmpare, branchless>(start2, len2, start1, len1, swap, cmp);
			//merge<Iter, Iter, Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
			return;
		}
		else if (len2 > 2 * len1) {
			uneven_merge<Iter, Iter, Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
			//merge<Iter, Iter, Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
			return;
		}
		bool order = (unorder1 + unorder2) * 23 < len1 + len2;
		if constexpr (!branchless) {
			if (cmp(*start2, start1[15]) && cmp(start2[15], *start1) == 0 && cmp(*(start2 + len2 - 16), *(start1 + len1 - 1)) && cmp(*(start2 + len2 - 1), *(start1 + len1 - 16)) == 0)
			{
				if (order) {
					forward_merge2<Iter, Iter, Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
				}
				else {
				//cross_merge(start1, len1, start2, len2, swap, cmp);

					merge<Iter, Iter, Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
				}
			}
			else {
				cross_merge<Iter, Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
				//forward_merge(start1, len1, start2, len2, swap, cmp);
				//merge(start1, len1, start2, len2, swap, cmp);
			}
			return;
		}
		else {
			if (cmp(*start2, start1[15]) && cmp(start2[15], *start1) == 0 && cmp(*(start2 + len2 - 16), *(start1 + len1 - 1)) && cmp(*(start2 + len2 - 1), *(start1 + len1 - 16)) == 0)
			{
				//merge(start1, len1, start2, len2, swap, cmp);
				//cross_merge(start1, len1, start2, len2, swap, cmp);
				if (order) {
					//printf("%d %d %d\n", (unorder1 + unorder2) * 43, len1 + len2, order);
					//forward_merge(start1, len1, start2, len2, swap, cmp);
					cross_merge<Iter,  Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
				}
				else {
					//cross_merge(start1, len1, start2, len2, swap, cmp);

					merge<Iter, Iter, Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
				}
			}
			else {
				cross_merge<Iter,  Iter3, cmpare, branchless>(start1, len1, start2, len2, swap, cmp);
				//forward_merge(start1, len1, start2, len2, swap, cmp);
			}
		}
	}


	template<class Iter, class cmpare,bool branchless>
	void trimerge(Iter start1, size_t len1, size_t unorder1,Iter start2, size_t len2, size_t unorder2, Iter start3, size_t len3, size_t unorder3, typename std::iterator_traits<Iter>::value_type* swap, cmpare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		Iter A, B, C;
		size_t lena, lenb, lenc,unordera,unorderb;
		
		if (len3 >  len2) {
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
					forward_merge<T*, Iter,T*, cmpare, branchless>(swap + len2, len1 + len3, start2, len2, swap, cmp);
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
		merge2<Iter,T*,cmpare,branchless>(A, lena,unordera, B, lenb, unorderb, swap+lenc, cmp);
		forward_merge<T*,Iter,T*,cmpare,branchless>(swap + lenc, lena + lenb, C, lenc, swap, cmp);
	}


	template<class Iter, class cmpare, bool branchless>//
	void quadmerge(Iter start1, size_t len1, size_t unorder1, Iter start2, size_t len2, size_t unorder2, Iter start3, size_t len3, size_t unorder3, Iter start4, size_t len4, size_t unorder4, typename std::iterator_traits<Iter>::value_type* swap, cmpare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("quadmerge\n");     printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0], len2); printarray<T>((T*)&start3[0], len3); printarray<T>((T*)&start4[0], len4);

		if (len1 > len2 + len3 + len4) {
			trimerge<Iter, cmpare, branchless>(start2, len2,unorder2, start3,len3,unorder3, start4, len4, unorder4, swap, cmp);
			tq_sort::backward_merge<T*, Iter, Iter, cmpare, branchless>(swap, len2 + len3 + len4, start1, len1, start1, cmp);
		}
		else if (len4 > len1 + len2 + len3) {
			trimerge<Iter, cmpare, branchless>(start1, len1, unorder1, start2,len2, unorder2, start3, len3, unorder3, swap, cmp);
			forward_merge<T*, Iter, Iter, cmpare, branchless>(swap, +len1 + len2 + len3, start4, len4, start1, cmp);
		}
		else {
			merge2<Iter,T*,cmpare,branchless>(start1, len1, unorder1, start2, len2,  unorder2, swap, cmp);
			merge2<Iter, T*, cmpare, branchless>(start3, len3, unorder3, start4, len4, unorder4, swap + len1 + len2, cmp);
			merge2<T*, Iter, cmpare, branchless>(swap, len1 + len2,  unorder1+ unorder2, swap + len1 + len2, len3 + len4, unorder3 + unorder4, start1, cmp);

		}

	}
#define tqmergeratio 2
	template<class Iter, class cmpare,bool branchless>
	void mergestack(run<Iter>* ts, size_t& stacksize, typename std::iterator_traits<Iter>::value_type* swap, cmpare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("mergestack %d\n", stacksize);
		while (stacksize >= 4) {
			run<Iter> run1 = ts[stacksize - 4], run2, run3, run4 = ts[stacksize - 1];
			if (run1.len <= tqmergeratio * run4.len) {
				run2 = ts[stacksize - 3];
				run3 = ts[stacksize - 2];
				quadmerge<Iter,cmpare,branchless>(run1.start, run1.len, run1.unorder , run2.start, run2.len, run2.unorder, run3.start, run3.len, run3.unorder, run4.start, run4.len, run4.unorder, swap, cmp);
				ts[stacksize - 4].len = run1.len + run2.len + run3.len + run4.len;
				ts[stacksize - 4].unorder= run1.unorder + run2.unorder + run3.unorder + run4.unorder;
				stacksize = stacksize - 3;
			}
			else {
				break;
			}
		}
	}



	template<class Iter, class cmpare,bool branchless>
	void forcecollapsestack(run<Iter>* ts, typename std::iterator_traits<Iter>::value_type* swap, size_t stacksize, cmpare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("forcecollapsestack %d\n", stacksize); 

		run<Iter> run1, run2, run3, run4;

		while (stacksize >= 4) {
			run1 = ts[stacksize - 4];
			run2 = ts[stacksize - 3];
			run3 = ts[stacksize - 2];
			run4 = ts[stacksize - 1];
			quadmerge<Iter, cmpare, branchless>(run1.start, run1.len, run1.unorder,run2.start, run2.len, run2.unorder, run3.start, run3.len, run3.unorder, run4.start, run4.len, run4.unorder, swap, cmp);
			ts[stacksize - 4].len = run1.len + run2.len + run3.len + run4.len;
			ts[stacksize - 4].unorder = run1.unorder + run2.unorder + run3.unorder + run4.unorder;
			stacksize = stacksize - 3;
		}

		switch (stacksize) {
		case 3:
			run3 = ts[stacksize - 1];
			run2 = ts[stacksize - 2];
			run1 = ts[stacksize - 3];;
			forward_merge<Iter, Iter, T*, cmpare, branchless>(run3.start, run3.len, run2.start, run2.len, swap, cmp);
			backward_merge<Iter, T*, Iter, cmpare, branchless>(run1.start, run1.len,swap, run2.len + run3.len , run1.start, cmp);
			break;
		case 2:

			run2 = ts[stacksize - 1];
			run1 = ts[stacksize - 2];
			std::move(run2.start, run2.start + run2.len, swap);
			backward_merge<Iter, T*, Iter,  cmpare, branchless>(run1.start, run1.len, swap, run2.len, run1.start, cmp);
		}
	}

	template<class Iter>
	void reverserun(Iter start, Iter end) {
		while (start < end) {
			std::iter_swap(start++, end--);
		}
	}

	template<class Iter, class cmpare,bool branchless>
	void tqsortloop(Iter start, const size_t len, cmpare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		run<Iter> alignas(64)ts[64];
		size_t stacksize = 0;
		bool b1,b2,b3;
		size_t b;
		run<Iter> newrun;
		Iter end, pta, runstart,end2;
		T alignas(64)swapbase1[128];

		T* swap, * swapbase2 = NULL;
		//swap3 = reinterpret_cast<T*> ((reinterpret_cast<std::size_t>(swap) + 63) & -64);
		if (len >= 128) {
			swapbase2 = (T*)malloc(len * sizeof(T));
			swap = swapbase2;
		}
		else {
			swap = swapbase1;
		}
		end = start + len;
		pta = start;
		if constexpr (branchless) {
			end2 = end - 32;
		}
		else {
			end2 = end - 64;
		}
	
		end2 = end - 64;
		int i;
		while (end2 - pta >= 0) {
			newrun.start = pta;

			int i, loop;

			for (i = 3; i >= 0; i--) {
				runstart = pta;
				
				Iter cur;
				Iter cur2;
				Iter shift;
				Iter shift2;
				if (cmp(runstart[1], *runstart)) {
					tq_sort::parity_swap_sixteen<Iter, std::less<T>, false>(runstart, swap, std::less<T>());
					goto unordered;
				}
				cur2 = runstart + 2;
				cur = runstart + 1;
				if (cmp(*cur2, *cur)) {
					tq_sort::parity_swap_sixteen<Iter, std::less<T>, false>(runstart, swap, std::less<T>());
					goto unordered;
				}
				cur += 2;
				if (cmp(*cur, *cur2)) {
					tq_sort::parity_swap_sixteen<Iter, std::less<T>, false>(runstart, swap, std::less<T>());
					goto unordered;
				}

				//loopless insertion sort

				cur2 += 2;
				if (cmp(*cur2, *cur)) {
					goto insert16_5b;
				}
			insert16_6a:
				cur += 2;
				if (cmp(*cur, *cur2)) {
					goto insert16_6b;
				}
			insert16_7a:
				cur2 += 2;
				if (cmp(*cur2, *cur)) {
					goto insert16_7b;
				}
			insert16_8a:
				cur += 2;
				if (cmp(*cur, *cur2)) {
					goto insert16_8b;
				}
			insert16_9a:
				cur2 += 2;
				if (cmp(*cur2, *cur)) {
					goto insert16_9b;
				}
			insert16_10a:
				cur += 2;
				if (cmp(*cur, *cur2)) {
					goto insert16_10b;
				}
			insert16_11a:
				cur2 += 2;
				if (cmp(*cur2, *cur)) {
					goto insert16_11b;
				}
			insert16_12a:
				cur += 2;
				if (cmp(*cur, *cur2)) {
					goto insert16_12b;
				}
			insert16_13a:
				cur2 += 2;
				if (cmp(*cur2, *cur)) {
					goto insert16_13b;
				}
			insert16_14a:
				cur += 2;
				if (cmp(*cur, *cur2)) {
					goto insert16_14b;
				}
			insert16_15a:
				cur2 += 2;
				if (cmp(*cur2, *cur)) {
					goto insert16_15b;
				}
			insert16_16a:
				cur += 2;
				if (cmp(*cur, *cur2)) {
					goto insert16_16b;
				}
				goto insertend;

				T tmp;
				bool x;
				//5
			insert16_5b:
				tmp = *cur2;
				*cur2 = *cur;
				shift2 = cur2 - 2;
				if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
				else { *cur = tmp;  goto insert16_6a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_6a; }
				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
				goto insert16_6a;
				//6
			insert16_6b:
				tmp = *cur;
				*cur = *cur2;
				shift = cur - 2;
				if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
				else { *cur2 = tmp; goto insert16_7a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_7a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_7a; }
				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
				goto insert16_7a;
				//7
			insert16_7b:
				tmp = *cur2;
				*cur2 = *cur;
				shift2 = cur2 - 2;
				if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
				else { *cur = tmp;  goto insert16_8a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_8a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp;  goto insert16_8a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_8a; }

				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
				goto insert16_8a;
				//8
			insert16_8b:
				tmp = *cur;
				*cur = *cur2;
				shift = cur - 2;
				if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
				else { *cur2 = tmp; goto insert16_9a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp;  goto insert16_9a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_9a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_9a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_9a; }

				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;

				goto insert16_9a;
				//9
			insert16_9b:
				tmp = *cur2;
				*cur2 = *cur;
				shift2 = cur2 - 2;
				if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
				else { *cur = tmp; goto insert16_10a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_10a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_10a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_10a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_10a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_10a; }

				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
				goto insert16_10a;
				//10
			insert16_10b:
				tmp = *cur;
				*cur = *cur2;
				shift = cur - 2;
				if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
				else { *cur2 = tmp; goto insert16_11a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp;   goto insert16_11a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp;  goto insert16_11a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp;   goto insert16_11a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp;  goto insert16_11a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp;  goto insert16_11a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp;  goto insert16_11a; }

				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;

				goto insert16_11a;
				//11
			insert16_11b:
				tmp = *cur2;
				*cur2 = *cur;
				shift2 = cur2 - 2;
				if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
				else { *cur = tmp;  goto insert16_12a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_12a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_12a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_12a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_12a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_12a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_12a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_12a; }

				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
				goto insert16_12a;
				//12
			insert16_12b:
				tmp = *cur;
				*cur = *cur2;
				shift = cur - 2;
				if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
				else { *cur2 = tmp; goto insert16_13a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_13a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_13a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_13a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_13a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_13a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_13a; }
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_13a; }
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_13a; }

				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;

				goto insert16_13a;
				//13
			insert16_13b:
				tmp = *cur2;
				*cur2 = *cur;
				shift2 = cur2 - 2;
				if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
				else {	*cur = tmp; goto insert16_14a;	}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else {*shift2 = tmp;  goto insert16_14a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else {*shift = tmp;   goto insert16_14a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else {*shift2 = tmp;  goto insert16_14a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else {*shift = tmp;   goto insert16_14a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else {*shift2 = tmp;  goto insert16_14a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else {*shift = tmp;   goto insert16_14a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else {*shift2 = tmp;  goto insert16_14a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else {*shift = tmp;   goto insert16_14a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else {*shift2 = tmp;  goto insert16_14a;}

				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
				goto insert16_14a;
				//14
			insert16_14b:
				tmp = *cur;
				*cur = *cur2;
				shift = cur - 2;
				if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
				else { *cur2 = tmp;  goto insert16_15a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp;  goto insert16_15a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp;  goto insert16_15a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp;  goto insert16_15a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_15a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_15a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_15a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else {		*shift = tmp; goto insert16_15a;		}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else {	*shift2 = tmp;   goto insert16_15a;	}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else {				*shift = tmp;  goto insert16_15a;			}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else {	*shift2 = tmp;  goto insert16_15a;	}

				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;

				goto insert16_15a;
				//15
			insert16_15b:
				tmp = *cur2;
				*cur2 = *cur;
				shift2 = cur2 - 2;
				if (cmp(tmp, *shift2)) { *cur = *shift2; shift = cur - 2; }
				else {	*cur = tmp;  goto insert16_16a;	}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_16a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp;  goto insert16_16a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_16a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp;  goto insert16_16a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_16a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_16a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_16a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insert16_16a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insert16_16a;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp;  goto insert16_16a;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else {	*shift2 = tmp; goto insert16_16a; }

				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
				goto insert16_16a;
				//16
			insert16_16b:
				tmp = *cur;
				*cur = *cur2;
				shift = cur - 2;
				if (cmp(tmp, *shift)) { *cur2 = *shift; shift2 = cur2 - 2; }
				else { *cur2 = tmp; goto insertend;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insertend;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insertend;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insertend;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insertend;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insertend;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp;  goto insertend;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insertend;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp;  goto insertend;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insertend;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insertend;}
				if (cmp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
				else { *shift = tmp; goto insertend;}
				if (cmp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
				else { *shift2 = tmp; goto insertend; }

				x = !cmp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;

			insertend:
				//printf("insert\n"); printarray<T>((T*)&runstart[0], 8);
				pta=runstart+16;

			}
		unordered:
			//printf("insert\n"); printarray<T>((T*)&runstart[0], 16);
			for (; i > 0; i--) {
				pta += 16;
				parity_swap_sixteen<Iter, cmpare, branchless>(pta, swap, cmp);
				//printarray<T>((T*)&pta[0], 16);
				
			}
			pta -= 48;

			b1 = cmp(*(pta + 16), *(pta + 15));
			b2 = cmp(*(pta + 32), *(pta + 31));
			b3 = cmp(*(pta + 48), *(pta + 47));

			newrun.unorder = b1 + b2 + b3;

			if (newrun.unorder) {
				parity_merge<Iter, T*, cmpare, branchless>(pta, 16, pta + 16, 16, swap, cmp);
				parity_merge<Iter, T*, cmpare, branchless>(pta + 32, 16, pta + 48, 16, swap + 32, cmp);
				parity_merge<T*, Iter, cmpare, branchless>(swap, 32, swap + 32, 32, pta, cmp);
			}
			pta += 64;
			

			while (pta < end && cmp(*(pta), *(pta - 1)) == 0) {
				pta++;
			}
				
			newrun.len = (size_t)(pta - newrun.start);
			//printf("newrun %d\n",pta - newrun.start);  printarray<T>((T*)&newrun.start[0], pta - newrun.start);

			ts[stacksize] = newrun;
			stacksize++;
			


			mergestack<Iter,cmpare,branchless>(ts, stacksize, swap, cmp);

		}
		if (end - pta > 0) {
			newrun.start = pta;
			newrun.len = (size_t)(end - pta);
			newrun.unorder=(size_t)1;
			tq_sort::tail_swap<Iter, cmpare, branchless>(newrun.start, swap, newrun.len, cmp);
			ts[stacksize] = newrun;	stacksize++;
			//printf("newrun\n"); printarray<T>((T*)&pta[0], end - pta);

		}
		forcecollapsestack<Iter,cmpare,branchless>(ts, swap, stacksize, cmp);
		free(swapbase2);
	}
}









///////////////
//TQSORT
//////////////


template<class Iter, class T = typename std::iterator_traits<Iter>::value_type, class cmpare = std::less<T>>
void tqsort(Iter start, size_t len, cmpare cmp) {
	if (len < 64) {
		T swap[64];
		tq_sort::tail_swap<Iter, cmpare, std::is_integral<T>::value || std::is_floating_point<T>::value>(start, swap, len, cmp);
	}
	else {
		tq_sort::tqsortloop<Iter, cmpare, std::is_integral<T>::value || std::is_floating_point<T>::value>(start, len, cmp);
	}
}
template<class Iter, class T = typename std::iterator_traits<Iter>::value_type, class cmpare = std::less<T>>
void tqsort_branching(Iter start, size_t len, cmpare cmp) {
	if (len < 64) {
		T swap[64];
		tq_sort::tail_swap<Iter,cmpare,false>(start, swap, len, cmp);
	}
	else {
		tq_sort::tqsortloop<Iter, cmpare, false>(start, len, cmp);
	}
}
template<class Iter, class T = typename std::iterator_traits<Iter>::value_type, class cmpare = std::less<T>>
void tqsort_branchless(Iter start, size_t len, cmpare cmp) {
	if (len < 64) {
		T swap[64];
		tq_sort::tail_swap<Iter, cmpare, true>(start, swap, len, cmp);
	}
	else {
		tq_sort::tqsortloop<Iter, cmpare, true>(start, len, cmp);
	}
}
