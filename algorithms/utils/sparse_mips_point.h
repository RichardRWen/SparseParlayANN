// This code is part of the Problem Based Benchmark Suite (PBBS)
// Copyright (c) 2011 Guy Blelloch and the PBBS team
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include <algorithm>
#include <iostream>

#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "parlay/internal/file_map.h"
#include "../bench/parse_command_line.h"
#include "NSGDist.h"

#include "../bench/parse_command_line.h"
#include "types.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


float sparse_mips_distance(const unsigned int *ind1, const uint8_t *val1, unsigned len1, const unsigned int *ind2, const uint8_t *val2, unsigned len2) {
	int result = 0;
	int i = 0, j = 0;
	while (true) {
		if (ind1[i] == ind2[j]) {
			result += ((int32_t*)val1)[i++] * ((int32_t*)val2)[j++];
			if (i >= (int)len1 || j >= (int)len2) break;
		}
		else if (ind1[i] < ind2[j]) {
			if (++i >= (int)len1) break;
		}
		else {
			if (++j >= (int)len2) break;
		}
	}
	return -((float)result);
}

float sparse_mips_distance(const unsigned int *ind1, const int8_t *val1, unsigned len1, const unsigned int *ind2, const int8_t *val2, unsigned len2) {
	int result = 0;
	int i = 0, j = 0;
	while (true) {
		if (ind1[i] == ind2[j]) {
			result += ((int32_t*)val1)[i++] * ((int32_t*)val2)[j++];
			if (i >= (int)len1 || j >= (int)len2) break;
		}
		else if (ind1[i] < ind2[j]) {
			if (++i >= (int)len1) break;
		}
		else {
			if (++j >= (int)len2) break;
		}
	}
	return -((float)result);
}

float sparse_mips_distance(const unsigned int *ind1, const float *val1, unsigned len1, const unsigned int *ind2, const float *val2, unsigned len2) {
	float result = 0;
	int i = 0, j = 0;
	while (true) {
		if (ind1[i] == ind2[j]) {
			result += val1[i++] * val2[j++];
			if (i >= (int)len1 || j >= (int)len2) break;
		}
		else if (ind1[i] < ind2[j]) {
			if (++i >= (int)len1) break;
		}
		else {
			if (++j >= (int)len2) break;
		}
	}
	return -result;
}

template<typename T>
struct SparseMipsPoint {
	using distanceType = float; 

	static bool is_metric() {return false;}

	float distance(SparseMipsPoint<T> x) {
		return sparse_mips_distance(this->indices, this->values, this->len, x.indices, x.values, x.len);
	}

	void prefetch() {
		int l1 = (len * sizeof(unsigned int)) / 64;
		for (int i = 0; i < l1; i++)
			__builtin_prefetch((char*)indices + i * 64);
		int l2 = (len * sizeof(T)) / 64;
		for (int i = 0; i < l2; i++)
			__builtin_prefetch((char*)values + i * 64);
	}

	long id() {return id_;}

	SparseMipsPoint(const unsigned int *indices, const T* values, size_t len, unsigned int dims, long id)
		: indices(indices), values(values), len(len), dims(dims), id_(id) {}

	bool operator==(SparseMipsPoint<T> q){
		for (int i = 0; i < len; i++) {
			if (indices[i] != q.indices[i] || values[i] != q.values[i]) {
				return false;
			}
		}
		return true;
	}
	/*bool operator==(SparseMipsPoint<T> q){ // allows zeros in vector
		int i, j;
		for (i = j = 0; i < len && j < q.len;) {
			if (indices[i] != q.indices[j]) {
				if (values[i] == 0 || q.values[j] == 0) {
					while (values[i] == 0 && i < len) i++;
					while (q.values[j] == 0 && j < q.len) j++;
					continue;
				}
				return false;
			}
			else if (values[i] != q.values[j]) return false;
			i++;
			j++;
		}
		while (i < len) if (values[i] != 0) return false;
		while (j < q.len) if (q.values[j] != 0) return false;
		return true;
	}*/

	std::string to_string() {
		std::string s = "";
		for (unsigned int i = 0, j = 0; i < dims; i++) {
			if (indices[j] == i) s += std::to_string(values[j++]) + " ";
			else s += "0 ";
		}
		return s;
	}

	private:
	const unsigned int *indices;
	const T *values;
	size_t len;
	unsigned int dims;
	long id_;
};
