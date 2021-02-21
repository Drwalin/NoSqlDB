/*
 *  This file is part of NoSqlDB.
 *  Copyright (C) 2020 Marek Zalewski aka Drwalin
 *
 *  ICon3 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ICon3 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HEAP_FILE_HPP
#define HEAP_FILE_HPP

#include "CachedFile.hpp"

class HeapFile {
public:
	
	const static uint64_t blockSize = 4096;
	const static uint64_t blockSizeBits = 12;
	
	HeapFile();
	HeapFile(const char* fileName);
	~HeapFile();
	
	bool Open(const char* fileName);
	void Close();
	
	inline void Push(uint64_t value) {
		heap[0]++;
		if((file.Size()>>3) <= heap[0]) {
			file.Reserve((((heap[0]>>blockSizeBits)+1)<<blockSizeBits)<<3);
		}
		uint64_t i, j, v;
		for(i=heap[0], j=i>>1; i>1; i=j, j>>=1) {
			v = heap[j];
			if(value >= v) {
				break;
			}
			heap[i] = v;
		}
		heap[i] = value;
	}
	
	inline bool Pop(uint64_t& result) {
		if((file.Size()<<3) == 0 || heap[0] == 0)
			return false;
		result = heap[1];
		uint64_t size = heap[0];
		uint64_t last = heap[size];
		heap[0]--;
		uint64_t i, j, v;
		for(i=1, j=2; j<=size; i=j, j<<=1) {
			v = heap[j];
			if((j < size) && (v > heap[j+1])) {
				++j;
				v = heap[j];
			}
			if(last <= v)
				break;
			heap[i] = v;
			
		}
		heap[i] = last;
		return true;
	}
	
	inline uint64_t Size() const {
		return heap[0];
	}
		
private:
	
	uint64_t*& heap;
	CachedFile file;
};

#endif

