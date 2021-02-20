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
	
	HeapFile(const char* fileName);
	~HeapFile();
	
	inline void Push(uint64_t value) {
		if((file.Size()>>3) <= heap[0]+1) {
			file.Reserve((heap[0]+512)<<3);
		}
		heap[0]++;
		uint64_t i;
		for(i=heap[0]; i>1; i>>=1) {
			if(value >= heap[i>>1])
				break;
			heap[i] = heap[i>>1];
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
		uint64_t i, j;
		for(i=1, j=2; j<=size; i=j, j<<=1) {
			if(j < size) {
				if(heap[j] > heap[j+1])
					j=j+1;
			}
			if(last <= heap[j])
				break;
			heap[i] = heap[j];
			
		}
		heap[i] = last;
		return true;
	}
		
private:
	
	uint64_t*& heap;
	CachedFile file;
};

#endif

