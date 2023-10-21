#pragma once
#include <stdlib.h>
#include <utility>

namespace tq_sort {

	template<class Iter>
	struct run {
		Iter start;
		size_t len;
	};


	template<class Iter, class Iter2, class Compare>
	Iter gallopforwardright(Iter start, Iter end, Iter2 key, Compare cmp) {
		size_t len1, len2;
		Iter mid, first, last;
		len1 = 1;
		last = start;
		while (true) {
			last = len1 + last;
			if (last >= end) {
				first = last - len1;
				len1 = end - first;
				break;
			}
			if (cmp(*(last), *key) == 0) {
				first = last - len1;
				break;
			}
			len1 = len1 << 1;
		}
		while (len1 > 1) {
			len2 = len1;
			len1 = len1 >> 1;
			mid = first + len1;
			if (cmp(*(mid), *key)) {
				first = mid;
				len1 = len2 - len1;
			}

		}
		first++;
		return first;
	}

	template<class Iter, class Iter2, class Compare>
	Iter gallopforwardleft(Iter start, Iter end, Iter2 key, Compare cmp) {
		size_t len1, len2;
		Iter mid, first, last;
		len1 = 1;
		last = start;
		while (true) {
			last = len1 + last;
			if (last >= end) {
				first = last - len1;
				len1 = end - first;
				break;
			}
			if (cmp(*key, *(last))) {
				first = last - len1;
				break;
			}
			len1 = len1 << 1;
		}
		while (len1 > 1) {
			len2 = len1;
			len1 = len1 >> 1;
			mid = first + len1;
			if (cmp(*key, *(mid)) == 0) {
				first = mid;
				len1 = len2 - len1;
			}

		}
		first++;
		return first;
	}
	//end 1 more than the desired range
	template<class Iter, class Iter2, class Compare>
	Iter gallopbackleft(Iter start, Iter end, Iter2 key, Compare cmp) {
		size_t len1, len2;
		Iter mid, first, last;
		len1 = 1;
		first = end;
		while (true) {
			last = first;
			first = first - len1;
			if (first < start) {
				if (cmp(*key, *(start))) {
					return start;
				}
				first = start;
				len1 = last - start;
				break;
			}
			if (cmp(*key, *(first)) == 0) {
				break;
			}
			len1 = len1 << 1;
		}
		while (len1 > 1) {
			len2 = len1;
			len1 = len1 >> 1;
			mid = first + len1;
			if (cmp(*key, *(mid)) == 0) {
				first = mid;
				len1 = len2 - len1;
			}

		}
		first++;
		return first;
	}
	//end 1 more than the desired range
	template<class Iter, class Iter2, class Compare>
	Iter gallopbackright(Iter start, Iter end, Iter2 key, Compare cmp) {
		size_t len1, len2;
		Iter mid, first, last;
		//printf("gallopleft %d %d %d \n", *start,*end,*key);
		len1 = 1;
		first = end;
		while (true) {
			last = first;
			first = first - len1;
			if (first <= start) {
				if (cmp(*(start), *key) == 0) {
					return start;
				}
				first = start;
				len1 = last - start;
				break;
			}
			if (cmp(*(first), *key)) {
				break;
			}
			len1 = len1 << 1;
		}
		while (len1 > 1) {
			len2 = len1;
			len1 = len1 >> 1;
			mid = first + len1;
			if (cmp(*(mid), *key)) {
				first = mid;
				len1 = len2 - len1;
			}

		}
		first++;
		return first;
	}
#define tqrepeat 3


	template<class Iter1, class Iter2, class Iter3, class Compare>
	void forward_merge(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 dest, Compare cmp)
	{
		//typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("merge %d %d\n",len1,len2);      printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0], len2);
		int n = len1 + len2;
		Iter1 ptl, tpl, pt2;
		Iter2 ptr, tpr, pt3;
		Iter3 ptd, tpd;
		bool  x, y, z;
		size_t len;
		ptl = start1;
		ptr = start2;
		ptd = dest;

		//this method allows source and dest to overlap.  these point to elements that can't be moved into dest until the end.
		Iter1 endleft2 = start1 + len1;
		Iter1 endleft1 = endleft2;
		Iter2 endright1 = start2 + len2;

		tpl = endleft1 - 1;
		tpr = endright1 - 1;


		int len3, len4;

		//prepare array2 for unguarded loops
		//runs are always atleast lenth 2
		//this will place the 1 or more elements from the end of array2 into dest array.  the last unplaced element from array2 will be the largest element still to be placed in dest array.
		if (cmp(*(tpr), *(tpl))) {
			endleft1 = gallopbackleft(ptl, endleft2, tpr, cmp);
		}
		if (ptl < endleft1) {
			tpr = gallopbackright(ptr, endright1, endleft1 - 1, cmp);
		}
		else {
			tpr = ptr;
		}

		if (tpl > ptl && tpr > ptr) {
			tpl = gallopbackleft(ptl, endleft1, tpr - 1, cmp);
		}
		else {
			tpl = ptl;
		}


		len1 = tpl + 1 - ptl;
		for (; len1 > 2 * tqrepeat + 1;)
		{
			len3 = 0;
			len4 = 0;
			for (int i = tqrepeat; i > 0; i--) {

				if (cmp(*(ptr + 1), *(ptl + 1))) {
					if (cmp(*(ptr + 1), *ptl)) {
						*ptd++ = *ptr++;
						*ptd++ = *ptr++;
						len3++;
					}
					else {
						x = cmp(*ptr, *ptl); y = !x;  ptd[y] = *ptr; ptd[x] = *ptl; ptd += 2; ptl++; ptr++;
						*ptd++ = *ptr++;
					}
				}
				else {
					if (cmp(*ptr, *(ptl + 1))) {
						x = cmp(*ptr, *ptl); y = !x;  ptd[y] = *ptr; ptd[x] = *ptl; ; ptd += 2; ptl++; ptr++;
						*ptd++ = *ptl++;
					}
					else {
						*ptd++ = *ptl++;
						*ptd++ = *ptl++;
						len4++;
					}
				}
			}

			if (len4 == tqrepeat) {
				if (cmp(*(ptr), *ptl) == 0) {
					pt2 = gallopforwardleft(ptl, tpl + 1, ptr, cmp);
					std::move(ptl, pt2, ptd);
					len3 = pt2 - ptl;
					ptd += len3;
					ptl = pt2;
					*ptd = *ptr; ptd++;	ptr++;
				}
			}
			else if (len3 == tqrepeat) {
				if (cmp(*(ptr), *ptl)) {
					pt3 = gallopforwardright(ptr, tpr + 1, ptl, cmp);
					//printf("2. ptl %d %d %d\n", *ptl, *ptr, *pt3);
					std::move(ptr, pt3, ptd);
					len4 = pt3 - ptr;
					ptd += len4;
					ptr = pt3;
					*ptd = *ptl; ptd++;	ptl++;
				}
			}

			len1 = tpl + 1 - ptl;
		}


		while (ptl < tpl)
		{
			if (cmp(*ptr, *ptl)) {
				*ptd = *ptr; ptr++; ptd++;
			}
			else {
				*ptd = *ptl; ptl++; ptd++;
			}
		}

		std::move(ptr, tpr, ptd);
		ptd += (tpr - ptr);
		std::move(tpl, endleft1, ptd);
		ptd += (endleft1 - tpl);
		std::move(tpr, endright1, ptd);
		ptd += (endright1 - tpr);
		std::move(endleft1, endleft2, ptd);

		//printf("end merge\n");    printarray<T>((T*)&dest[0], n);
	}

	template<class Iter1, class Iter2, class Iter3, class Compare>
	void back_merge(Iter1 start1, size_t len1, Iter2 start2, size_t len2, Iter3 dest, Compare cmp)
	{
		//typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("back merge\n");     printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0], len2);
		int n = len1 + len2;
		Iter1 ptl, tpl, pt2;
		Iter2 ptr, tpr, pt3;
		Iter3 ptd, tpd;
		bool  x, y, z;
		size_t len;
		ptl = start1;
		ptr = start2;
		ptd = dest;
		tpl = start1 + len1 - 1;
		tpr = start2 + len2 - 1;
		tpd = dest + len1 + len2 - 1;
		Iter1  beginleft1 = ptl, beginleft2 = ptl;
		Iter2 beginright1 = ptr;
		int len3, len4;

		if (cmp(*(ptl), *(ptr))) {
			beginleft2 = gallopforwardleft(ptl, tpl + 1, ptr, cmp);
		}
		if (beginleft2 <= tpl) {
			ptr = gallopforwardright(ptr, tpr + 1, beginleft2, cmp);
		}
		else {
			ptr = tpr + 1;
		}

		if (ptl <= tpl && ptr <= tpr) {
			ptl = gallopforwardleft(beginleft2, tpl + 1, ptr, cmp);
		}
		else {
			ptl = tpl + 1;
		}



		len1 = tpl + 1 - ptl;//+1 prevents negative values in unsigned size type
		//printf("%d \n", len1);
		for (; len1 > 2 * tqrepeat + 2;)
		{
			len3 = 0;
			len4 = 0;
			for (int i = tqrepeat; i > 0; i--) {


				if (cmp(*(tpr - 1), *(tpl - 1))) {
					if (cmp(*tpr, *(tpl - 1))) {
						*tpd-- = *tpl--;
						*tpd-- = *tpl--;
						len3++;
					}
					else {
						x = cmp(*tpr, *tpl); y = !x; tpd--;  tpd[y] = *tpr; tpd[x] = *tpl; tpd--; tpl--; tpr--;
						*tpd-- = *tpl--;
					}
				}
				else {
					if (cmp(*(tpr - 1), *tpl)) {
						x = cmp(*tpr, *tpl); y = !x; tpd--;  tpd[y] = *tpr; tpd[x] = *tpl; tpd--; tpl--; tpr--;
						*tpd-- = *tpr--;

					}
					else {
						*tpd-- = *tpr--;
						*tpd-- = *tpr--;
						len4++;

					}

				}
			}

			if (len3 == tqrepeat) {

				if (cmp(*(tpr), *tpl)) {
					pt2 = gallopbackleft(ptl, tpl + 1, tpr, cmp);
					len3 = tpl - pt2;//;
					tpd -= len3;
					std::move(pt2, tpl + 1, tpd);
					tpd--;

					tpl = pt2 - 1;
				}
			}
			else if (len4 == tqrepeat) {
				if (cmp(*(tpr), *tpl) == 0) {

					pt3 = gallopbackright(ptr, tpr + 1, tpl, cmp);
					len3 = tpr - pt3;// 
					tpd -= len3;
					std::move(pt3, tpr + 1, tpd);
					tpd--;

					tpr = pt3 - 1;
				}
			}

			len1 = tpl + 1 - ptl;
		}

		while (ptl <= tpl)
		{
			if (cmp(*tpr, *tpl)) {
				*tpd = *tpl; tpl--; tpd--;
			}
			else {
				*tpd = *tpr; tpr--; tpd--;
			}
		}
		tpd -= (tpr - ptr);
		std::move(ptr, tpr + 1, tpd);
		tpd -= (ptl - beginleft2);
		std::move(beginleft2, ptl, tpd);
		tpd -= (ptr - beginright1);
		std::move(beginright1, ptr, tpd);
		std::move(beginleft1, beginleft2, dest);


		//printf("end back merge\n");    printarray<T>((T*)&dest[0], n);
	}


	template<class Iter1, class Iter2, class Compare>
	void merge_small(Iter1 start1, size_t len1, Iter1 start2, size_t len2, Iter2 dest, Compare cmp)
	{
		//typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("merge\n");     printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0], len2);
		int n = len1 + len2;
		Iter1 ptl, ptr, tpl, tpr, pt2, last, pt3;
		Iter2 ptd, tpd;

		size_t len;
		ptl = start1;
		ptr = start2;
		ptd = dest;
		tpl = start1 + len1 - 1;
		tpr = start2 + len2 - 1;
		tpd = dest + len1 + len2 - 1;


		len = tpd - ptd + 1;
		for (; len > 2; len -= 2)
		{
			*ptd++ = cmp(*ptl, *ptr) > 0 ? *ptl++ : *ptr++;
			*tpd-- = cmp(*tpl, *tpr) <= 0 ? *tpl-- : *tpr--;
		}
		*ptd = cmp(*ptl, *ptr) > 0 ? *ptl : *ptr;
		*tpd = cmp(*tpl, *tpr) <= 0 ? *tpl : *tpr;

	}


	template<class Iter, class Compare>
	void insertion_sort(Iter begin, Iter unorderedstart, Iter end, Compare comp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		//printarray<T>((T*)&begin[0], 4);
		if (begin == end) return;

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

	template<class Iter, class Compare>
	inline void insertion_sort8(Iter begin, Compare comp) {

		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("insert\n"); printarray<T>((T*)&begin[0], 8);
		Iter shift = begin + 1;
		Iter shift2 = begin;
		Iter cur = begin;
		T tmp;
		bool x;
		//2
		do {
			if (comp(*shift, *shift2)) {}
			else { break; }
			tmp = *shift;
			*shift = *shift2;
			*shift2 = tmp;
		} while (false);
		shift2 += 2;
		//3
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;

			x = !comp(tmp, *shift2);
			*shift = *shift2;
			shift2[x] = tmp;
			shift = shift2 + 3;;
			shift2 = shift - 1;

		} while (false);
		//4
		do {
			if (comp(*shift, *shift2)) {}
			else { shift2 += 2; break; }
			cur = shift;
			tmp = *shift;
			*shift = *shift2;
			shift -= 2;
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  shift = cur; shift2 = cur + 1; break; }
			x = !comp(tmp, *shift2);
			*shift = *shift2;
			shift2[x] = tmp;
			shift = cur; shift2 = cur + 1;
		} while (false);
		//5
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			cur = shift2;
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  shift2 = cur; shift = cur + 1; break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; shift2 = cur; shift = cur + 1; break; }
			x = !comp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			shift2 = cur; shift = cur + 1;
		} while (false);
		//6
		do {
			if (comp(*shift, *shift2)) {}
			else { shift2 += 2; break; }
			cur = shift;
			tmp = *shift;
			*shift = *shift2;
			shift -= 2;
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  shift = cur; shift2 = cur + 1; break; }
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; shift = cur; shift2 = cur + 1; break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  shift = cur; shift2 = cur + 1; break; }
			x = !comp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			shift = cur; shift2 = cur + 1;
		} while (false);
		//7
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			cur = shift2;
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  shift2 = cur; shift = cur + 1; break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; shift2 = cur; shift = cur + 1; break; }
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  shift2 = cur; shift = cur + 1; break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; shift2 = cur; shift = cur + 1; break; }

			x = !comp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			shift2 = cur; shift = cur + 1;
		} while (false);
		//8
		do {
			if (comp(*shift, *shift2)) {}
			else { break; }
			cur = shift;
			tmp = *shift;
			*shift = *shift2;
			shift -= 2;
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  break; }
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; break; }
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  break; }

			x = !comp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
		} while (false);

	}


	template<class Iter, class Compare>
	inline void insertion_sort7(Iter begin, Compare comp) {

		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("insert\n"); printarray<T>((T*)&begin[0], 8);
		Iter shift = begin + 1;
		Iter shift2 = begin;
		Iter cur = begin;
		T tmp;
		bool x;
		//2
		do {
			if (comp(*shift, *shift2)) {}
			else { break; }
			tmp = *shift;
			*shift = *shift2;
			*shift2 = tmp;
		} while (false);

		shift2 += 2;
		//3
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;

			x = !comp(tmp, *shift2);
			*shift = *shift2;
			shift2[x] = tmp;
			shift = shift2 + 3;;
			shift2 = shift - 1;

		} while (false);

		//4
		do {
			if (comp(*shift, *shift2)) {}
			else { shift2 += 2; break; }
			cur = shift;
			tmp = *shift;
			*shift = *shift2;
			shift -= 2;
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  shift = cur; shift2 = cur + 1; break; }
			x = !comp(tmp, *shift2);
			*shift = *shift2;
			shift2[x] = tmp;
			shift = cur; shift2 = cur + 1;
		} while (false);


		//printf("%d %d %d\n", *shift, *shift2,*cur);
		//5
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			cur = shift2;
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  shift2 = cur; shift = cur + 1; break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; shift2 = cur; shift = cur + 1; break; }
			x = !comp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			shift2 = cur; shift = cur + 1;
		} while (false);

		//printf("%d %d %d\n", *shift, *shift2, *cur);
		//6
		do {
			if (comp(*shift, *shift2)) {}
			else { shift2 += 2; break; }
			cur = shift;
			tmp = *shift;
			*shift = *shift2;
			shift -= 2;
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  shift = cur; shift2 = cur + 1; break; }
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; shift = cur; shift2 = cur + 1; break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  shift = cur; shift2 = cur + 1; break; }
			x = !comp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			shift = cur; shift2 = cur + 1;
		} while (false);

		//7
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			cur = shift2;
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; break; }
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; break; }

			x = !comp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
		} while (false);



	}


	template<class Iter, class Compare>
	inline void insertion_sort6(Iter begin, Compare comp) {

		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("insert\n"); printarray<T>((T*)&begin[0], 8);
		Iter shift = begin + 1;
		Iter shift2 = begin;
		Iter cur = begin;
		T tmp;
		bool x;
		//2
		/*
		x = comp(*shift, *shift2);
		y = !x;
		tmp = shift2[y];
		*shift2 = shift2[x];
		*shift = tmp;
		*/

		do {
			if (comp(*shift, *shift2)) {}
			else { break; }
			tmp = *shift;
			*shift = *shift2;
			*shift2 = tmp;
		} while (false);



		shift2 += 2;
		//3
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;

			x = !comp(tmp, *shift2);
			*shift = *shift2;
			shift2[x] = tmp;
			shift = shift2 + 3;;
			shift2 = shift - 1;

		} while (false);

		//4
		do {
			if (comp(*shift, *shift2)) {}
			else { shift2 += 2; break; }
			cur = shift;
			tmp = *shift;
			*shift = *shift2;
			shift -= 2;
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  shift = cur; shift2 = cur + 1; break; }
			x = !comp(tmp, *shift2);
			*shift = *shift2;
			shift2[x] = tmp;
			shift = cur; shift2 = cur + 1;
		} while (false);


		//printf("%d %d %d\n", *shift, *shift2,*cur);
		//5
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			cur = shift2;
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  shift2 = cur; shift = cur + 1; break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; shift2 = cur; shift = cur + 1; break; }
			x = !comp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
			shift2 = cur; shift = cur + 1;
		} while (false);


		//6
		do {
			if (comp(*shift, *shift2)) {}
			else { break; }
			cur = shift;
			tmp = *shift;
			*shift = *shift2;
			shift -= 2;
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; break; }
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp;  break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  break; }

			x = !comp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
		} while (false);

	}


	template<class Iter, class Compare>
	inline void insertion_sort5(Iter begin, Compare comp) {

		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("insert\n"); printarray<T>((T*)&begin[0], 8);
		Iter shift = begin + 1;
		Iter shift2 = begin;
		Iter cur = begin;
		T tmp;
		bool x;
		//2
		/*
		x = comp(*shift, *shift2);
		y = !x;
		tmp = shift2[y];
		*shift2 = shift2[x];
		*shift = tmp;
		*/

		do {
			if (comp(*shift, *shift2)) {}
			else { break; }
			tmp = *shift;
			*shift = *shift2;
			*shift2 = tmp;
		} while (false);



		shift2 += 2;
		//3
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;

			x = !comp(tmp, *shift2);
			*shift = *shift2;
			shift2[x] = tmp;
			shift = shift2 + 3;;
			shift2 = shift - 1;

		} while (false);

		//4
		do {
			if (comp(*shift, *shift2)) {}
			else { shift2 += 2; break; }
			cur = shift;
			tmp = *shift;
			*shift = *shift2;
			shift -= 2;
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  shift = cur; shift2 = cur + 1; break; }
			x = !comp(tmp, *shift2);
			*shift = *shift2;
			shift2[x] = tmp;
			shift = cur; shift2 = cur + 1;
		} while (false);


		//printf("%d %d %d\n", *shift, *shift2,*cur);
		//5
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			cur = shift2;
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;
			if (comp(tmp, *shift2)) { *shift = *shift2; shift -= 2; }
			else { *shift = tmp; break; }
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp; break; }

			x = !comp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
		} while (false);

	}

	template<class Iter, class Compare>
	inline void insertion_sort4(Iter begin, Compare comp) {

		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("insert\n"); printarray<T>((T*)&begin[0], 8);
		Iter shift = begin + 1;
		Iter shift2 = begin;
		Iter cur = begin;
		T tmp;
		bool x;
		//2

		do {
			if (comp(*shift, *shift2)) {}
			else { break; }
			tmp = *shift;
			*shift = *shift2;
			*shift2 = tmp;
		} while (false);



		shift2 += 2;
		//3
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;

			x = !comp(tmp, *shift2);
			*shift = *shift2;
			shift2[x] = tmp;
			shift = shift2 + 3;;
			shift2 = shift - 1;

		} while (false);

		//4
		do {
			if (comp(*shift, *shift2)) {}
			else { break; }
			cur = shift;
			tmp = *shift;
			*shift = *shift2;
			shift -= 2;
			if (comp(tmp, *shift)) { *shift2 = *shift; shift2 -= 2; }
			else { *shift2 = tmp;  break; }

			x = !comp(tmp, *shift2); *shift = *shift2; shift2[x] = tmp;
		} while (false);

	}
	template<class Iter, class Compare>
	inline void insertion_sort3(Iter begin, Compare comp) {

		typedef typename std::iterator_traits<Iter>::value_type T;
		//printf("insert\n"); printarray<T>((T*)&begin[0], 8);
		Iter shift = begin + 1;
		Iter shift2 = begin;
		T tmp;
		bool x;
		//2

		do {
			if (comp(*shift, *shift2)) {}
			else { break; }
			tmp = *shift;
			*shift = *shift2;
			*shift2 = tmp;
		} while (false);



		shift2 += 2;
		//3
		do {
			if (comp(*shift2, *shift)) {}
			else { shift += 2; break; }
			tmp = *shift2;
			*shift2 = *shift;
			shift2 -= 2;

			x = !comp(tmp, *shift2);
			*shift = *shift2;
			shift2[x] = tmp;

		} while (false);
	}

	template<class Iter, class Compare>
	inline void insertion_sort2(Iter begin, Compare cmp) {

		typedef typename std::iterator_traits<Iter>::value_type T;
		//printarray<T>((T*)&begin[0], 2);
		T tmp;
		bool x;
		tmp = *(begin + 1);
		if (cmp(tmp, *begin)) {
			*(begin + 1) = *begin;
			*begin = tmp;
		}
	}


	template<class Iter, class Compare>
	inline void insertion_sort_small(Iter begin, size_t n, Compare cmp) {

		typedef typename std::iterator_traits<Iter>::value_type T;
		switch (n) {
		case 0:
		case 1:
			break;
		case 2:
			insertion_sort2(begin, cmp); break;
		case 3:
			insertion_sort3(begin, cmp); break;
		case 4:
			insertion_sort4(begin, cmp); break;
		case 5:
			insertion_sort5(begin, cmp); break;
		case 6:
			insertion_sort6(begin, cmp); break;
		case 7:
			insertion_sort7(begin, cmp); break;
		case 8:
			insertion_sort8(begin, cmp);
			break;
		}
	}

	template<class Iter, class Compare>
	void tail_swap(Iter array, size_t len, Compare cmp)
	{
		typedef typename std::iterator_traits<Iter>::value_type T;
		if (len <= 8) {
			insertion_sort_small(array, len, cmp);
		}
		else if (len <= 16) {
			insertion_sort8(array, cmp);
			insertion_sort(array, array + 8, array + len, cmp);
		}
		else if (len <= 24) {

			T swap[16];

			insertion_sort8(array, cmp);
			insertion_sort8(array + 8, cmp);
			merge_small(array, 8, array + 8, 8, swap, cmp);
			insertion_sort_small(array + 16, len - 16, cmp);
			forward_merge(swap, 16, array + 16, len - 16, array, cmp);

		}
		else {
			T swap[32];
			int len2 = len - 24;

			insertion_sort8(array, cmp);
			insertion_sort8(array + 8, cmp);
			merge_small(array, 8, array + 8, 8, swap, cmp);

			insertion_sort8(array + 16, cmp);
			insertion_sort(array + 16, array + 24, array + 24 + len2, cmp);

			forward_merge(swap, 16, array + 16, 8 + len2, array, cmp);
		}

	}
	template<class Iter, class T, class Compare>
	void quadmerge(Iter start1, size_t len1, Iter start2, size_t len2, Iter start3, size_t len3, Iter start4, size_t len4, T* swap, Compare cmp) {
		//printf("quadmerge\n");     printarray<T>((T*)&start1[0], len1); printarray<T>((T*)&start2[0], len2); printarray<T>((T*)&start3[0], len3); printarray<T>((T*)&start4[0], len4);

		if (len1 > len2 + len3 + len4) {
			if (len2 < len4) {
				forward_merge(start2, len2, start3, len3, swap + len4, cmp);
				forward_merge(swap + len4, len2 + len3, start4, len4, swap, cmp);

			}
			else {
				forward_merge(start3, len3, start4, len4, swap + len1, cmp);
				forward_merge(start2, len2, swap + len1, len3 + len4, swap, cmp);
			}
			back_merge(start1, len1, swap, len2 + len3 + len4, start1, cmp);
		}
		else if (len4 > len1 + len2 + len3) {
			if (len2 < len1) {
				forward_merge(start2, len2, start3, len3, swap + len1, cmp);
				forward_merge(swap + len1, len2 + len3, start1, len1, swap, cmp);

			}
			else {
				forward_merge(start1, len1, start2, len2, swap + len3, cmp);
				forward_merge(swap + len3, len1 + len2, start3, len3, swap, cmp);
			}
			forward_merge(swap, +len1 + len2 + len3, start4, len4, start1, cmp);
		}
		else {
			forward_merge(start1, len1, start2, len2, swap, cmp);
			forward_merge(start3, len3, start4, len4, swap + len1 + len2, cmp);
			forward_merge(swap, len1 + len2, swap + len1 + len2, len3 + len4, start1, cmp);

		}

	}


#define tqmergeratio 2
	template<class Iter, class T, class Compare>
	void mergestack(run<Iter>* ts, size_t& stacksize, T* swap, Compare cmp) {
		//printf("mergestack %d\n", stacksize);
		while (stacksize >= 4) {
			run<Iter> run1 = ts[stacksize - 4], run2, run3, run4 = ts[stacksize - 1];
			if (run1.len <= tqmergeratio * run4.len) {
				run2 = ts[stacksize - 3];
				run3 = ts[stacksize - 2];
				quadmerge(run1.start, run1.len, run2.start, run2.len, run3.start, run3.len, run4.start, run4.len, swap, cmp);
				ts[stacksize - 4].len = run1.len + run2.len + run3.len + run4.len;
				stacksize = stacksize - 3;
			}
			else {
				break;
			}
		}
	}




	template<class Iter1, class Iter2, class Compare>
	void forcecollapsestack(run<Iter1>* ts, Iter2 swap, size_t stacksize, Compare cmp) {
		typedef typename std::iterator_traits<Iter1>::value_type T;
		//printf("forcecollapsestack %d\n", stacksize); 

		run<Iter1> run1, run2, run3, run4;
		while (stacksize >= 4) {
			run1 = ts[stacksize - 4];
			run2 = ts[stacksize - 3];
			run3 = ts[stacksize - 2];
			run4 = ts[stacksize - 1];
			quadmerge(run1.start, run1.len, run2.start, run2.len, run3.start, run3.len, run4.start, run4.len, swap, cmp);
			ts[stacksize - 4].len = run1.len + run2.len + run3.len + run4.len;
			stacksize = stacksize - 3;
		}

		switch (stacksize) {
		case 3:
			run3 = ts[stacksize - 1];
			run2 = ts[stacksize - 2];
			run1 = ts[stacksize - 3];;
			forward_merge(run2.start, run2.len, run3.start, run3.len, swap, cmp);
			back_merge(run1.start, run1.len, swap, run2.len + run3.len, run1.start, cmp);
			break;
		case 2:

			run2 = ts[stacksize - 1];
			run1 = ts[stacksize - 2];
			std::move(run2.start, run2.start + run2.len, swap);
			back_merge(run1.start, run1.len, swap, run2.len, run1.start, cmp);
		}
	}


	template<class Iter, class Compare>
	void quad_swap_merge_32(Iter start, typename std::iterator_traits<Iter>::value_type* swap, Compare cmp)
	{
		bool b1, b2, b3;
		typedef typename std::iterator_traits<Iter>::value_type T;
		insertion_sort8(start, cmp);
		start += 8;
		insertion_sort8(start, cmp);

		start += 8;
		insertion_sort8(start, cmp);
		start += 8;
		insertion_sort8(start, cmp);

		b1 = cmp(*(start - 16), *(start - 17));
		b2 = cmp(*(start - 8), *(start - 9));
		b3 = cmp(*(start), *(start - 1));
		start -= 24;
		if (b1 | b2 | b3) {
			merge_small(start + 0, 8, start + 8, 8, swap, cmp);
			merge_small(start + 16, 8, start + 24, 8, swap + 16, cmp);

			merge_small(swap, 16, swap + 16, 16, start, cmp);
		}
	}

	template<class Iter, class Compare>
	void tqsortloop(Iter start, const size_t len, Compare cmp) {
		typedef typename std::iterator_traits<Iter>::value_type T;
		run<Iter> ts[64];

		size_t stacksize = 0;
		Iter end, pta, runstart;
		T swapbase1[128];

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
		while (end - pta >= 32) {
			runstart = pta;
			//create run of size 32
			quad_swap_merge_32(pta, swapbase1, cmp);
			pta += 32;
			//extend run
			while (pta < end && cmp(*(pta), *(pta - 1)) == 0) {
				pta++;
			}

			run newrun = run{ runstart,(size_t)(pta - runstart) };
			//printf("newrun\n"); printarray<T>((T*)&pt[0], pta - pt);
			ts[stacksize] = newrun;
			stacksize++;

			mergestack(ts, stacksize, swap, cmp);

		}
		if (end - pta > 0) {
			tail_swap(pta, end - pta, cmp);
			ts[stacksize] = run{ pta,(size_t)(end - pta) };;
			stacksize++;
		}
		forcecollapsestack(ts, swap, stacksize, cmp);

	}
}


template<class Iter, class Compare>
void tqsort(Iter start, size_t len, Compare cmp) {

	//typedef typename std::iterator_traits<Iter>::value_type T;
	if (len < 32) {
		tq_sort::tail_swap(start, len, cmp);
	}
	else {
		tq_sort::tqsortloop(start, len, cmp);
	}
}


