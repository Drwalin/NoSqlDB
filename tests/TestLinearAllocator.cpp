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

#include "Debug.hpp"

#include "LinearAllocator.hpp"

#include <cstdio>
#include <chrono>
#include <exception>
#include <cmath>

#include <map>
#include <set>
#include <vector>

LinearAllocator allocator;
struct Ptr {
	uint64_t linearPtr;
	uint64_t* regular_ptr;
	uint64_t bytes;
	Ptr() {linearPtr=0;bytes=0;regular_ptr=NULL;}
};
std::map<uint64_t, Ptr> allocated;

bool useRegularMemory = false;

uint64_t NotEqual(Ptr ptr) {
	if(!useRegularMemory)
		return 0;
	uint64_t elements = ptr.bytes>>3;
	uint64_t* linear = allocator.Origin<uint64_t>(ptr.linearPtr);
	uint64_t* regular = ptr.regular_ptr;
	uint64_t ret = 0;
	for(uint64_t i=0; i<elements; ++i) {
		if(linear[i] != regular[i])
			++ret;
	}
	return ret;
}

void AllocateNew(uint64_t elements) {
	Ptr ptr;
	ptr.bytes = elements<<3;
	ptr.linearPtr = allocator.Allocate(ptr.bytes);
	if(!useRegularMemory)
		ptr.regular_ptr = new uint64_t[elements];
	else
		ptr.regular_ptr = NULL;
	uint64_t* linear = allocator.Origin<uint64_t>(ptr.linearPtr);
	uint64_t* regular = ptr.regular_ptr;
	for(uint64_t i=0; i<elements; ++i) {
		linear[i] = Rand64();
		if(useRegularMemory)
			regular[i] = linear[i];
	}
	allocated[ptr.linearPtr] = ptr;
}

void RemoveRandom() {
	if(allocated.size() == 0)
		return;
	uint64_t id = Rand64()%allocated.size();
	auto it = allocated.begin();
	std::advance(it, id);
	Ptr ptr = it->second;
	allocator.Free(ptr.linearPtr);
	if(ptr.regular_ptr)
		delete[] ptr.regular_ptr;
	allocated.erase(it);
}



void Test(uint64_t make, uint64_t minSize, uint64_t sizeVariance, uint64_t remove) {
	DEBUG;
	for(uint64_t i=0; i<make; ++i)
		AllocateNew((Rand64()%sizeVariance)+minSize);
	
	DEBUG;
	uint64_t used = allocator.used();
	uint64_t reserved = allocator.reserved();
	printf("\n used %llu / %llu (%.2f%%)", used, reserved, 100.0*used/(double)reserved);
	DEBUG;
	
	for(uint64_t i=0; i<remove; ++i)
		RemoveRandom();
	DEBUG;
	
	
	uint64_t notEqual = 0;
	/*
	for(auto ptr : allocated) {
		notEqual += NotEqual(ptr.second);
	}
	DEBUG;
	*/
	used = allocator.used();
	reserved = allocator.reserved();
	printf("\n used %llu / %llu (%.2f%%)", used, reserved, 100.0*used/(double)reserved);
	DEBUG;
	
	if(notEqual == 0) {
		printf("\n ... OK\n\n");
	} else {
		printf("\n not equal values: %llu ... FAULT\n\n", notEqual);
	}
}

int main() {
	BlockAllocator<32> ballocator("32byte_block_mem.raw", "32byte_heap.raw");
	TreeSetFile fileSet(&ballocator);
	fileSet.InitNewTree();
	allocator.Open("linear_memory.raw", fileSet);
	
	try {
		Test(10425, 1032, 3453, 3041);
		Test(43242, 16, 24, 345);
		
		
		Test(12345, 16, 8, 2000);
		Test(12345, 32, 32, 2000);
		Test(12345, 80, 64, 2000);
		Test(12345, 136, 128, 2000);
		Test(12345, 264, 256, 2000);
		Test(12345, 550, 550, 2000);
		Test(12345, 1200, 1200, 2000);
		Test(12345, 16, 8, 2000);
		Test(12345, 16, 8, 2000);
		Test(12345, 16, 8, 2000);
		Test(23457, 48, 48, 2000);
		Test(123, 432, 4325, 123455);
		
		
		fileSet.DestroyTree();
	} catch(std::exception& e) {
		printf("\n%s\n", e.what());
	}
	printf("\n");
	return 0;
}

