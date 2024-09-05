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
// #include "common/time_loop.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


template<typename T, class Point_>
struct SparsePointRange{
    using Point = Point_;

	long dimension(){return dims;}
	//long aligned_dimension(){return aligned_dims;}

	SparsePointRange(){}

	SparsePointRange(char* filename){ // Specifically reading from .csr
		n = dims = 0;
		if(filename == NULL) return;

		std::ifstream csr_reader(filename);
		if (!csr_reader.is_open()) return;

		uint64_t num_vecs, num_dims, num_vals;
		csr_reader.read((char*)(&num_vecs), sizeof(uint64_t));
		csr_reader.read((char*)(&num_dims), sizeof(uint64_t));
		csr_reader.read((char*)(&num_vals), sizeof(uint64_t));

		n = num_vecs;
		dims = num_dims;
		std::cout << "Detected " << num_vecs << " points with dimension " << num_dims << std::endl;

		/*indptrs = (uint64_t*) malloc((num_vecs + 1) * sizeof(uint64_t));
		const int chunk_size = 1024;
		size_t amount_to_read = (num_vecs + 1) * sizeof(uint64_t);
		size_t amount_read = 0;
		while (amount_read + chunk_size < amount_to_read) {
			csr_reader.read((char*)(indptrs + amount_read), chunk_size);
			amount_read += chunk_size;
		}
		csr_reader.read((char*)(indptrs + amount_read), amount_to_read - amount_read);*/

		indptrs = (uint64_t*) malloc((num_vecs + 1) * sizeof(uint64_t));
		csr_reader.read((char*)(indptrs), (num_vecs + 1) * sizeof(uint64_t));
		indices = (unsigned int*) malloc(num_vals * sizeof(unsigned int));
		csr_reader.read((char*)(indices), num_vals * sizeof(unsigned int));
		values = (T*) malloc(num_vals * sizeof(T));
		csr_reader.read((char*)(values), num_vals * sizeof(T));
	}

	size_t size() { return n; }

	Point_ operator [] (long i) {
		return Point_(&indices[indptrs[i]], &values[indptrs[i]], indptrs[i + 1] - indptrs[i], dims, i);
	}

	//private:
	uint64_t *indptrs;
	unsigned int *indices;
	T* values;
	unsigned int dims;
	size_t n;
};
