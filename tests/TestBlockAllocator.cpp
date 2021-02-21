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

#include "BlockAllocator.hpp"

#include <cstdio>
#include <chrono>
#include <exception>

#include <map>
#include <set>
#include <vector>

BlockAllocator allocator;
std::map<uint64_t, int64_t> allocated;

bool GetRandomAllocated(uint64_t& ptr) {
	if(allocated.size() == 0)
		return false;
	uint64_t min = allocated.begin()->first;
	uint64_t max = allocated.rbegin()->first+4096;
	uint64_t bisect = RandMinMax(min, max);
	auto it = allocated.lower_bound(bisect);
	if(it == allocated.end()) {
		ptr = allocated.rbegin()->first;
	} else {
		ptr = it->first;
	}
	return true;
}

void Test(uint64_t make, uint64_t remove) {
	BlockAllocator allocator("memory.raw", "freeHeap.raw");
	for(uint64_t i=0; i<make; ++i) {
		uint64_t ptr = allocator.AllocateBlock();
		auto it = allocated.find(ptr);
		if(it == allocated.end()) {
			allocated[ptr] = 1;
		} else {
			it->second++;
		}
	}
	
	bool valid = true;
	for(auto it : allocated) {
		switch(it.second) {
		case 0: case 1:
			break;
		default:
			valid = false;
			printf("\n invalid allocation amount for object: %llu : %lld",
					it.first, it.second);
		}
	}
	
	for(uint64_t i=0; i<remove; ++i) {
		uint64_t ptr;
		if(GetRandomAllocated(ptr)) {
			allocator.FreeBlock(ptr);
			allocated[ptr]--;
			if(allocated[ptr] == 0) {
				allocated.erase(ptr);
			}
			
		} else {
			break;
		}
	}
	
	for(auto it : allocated) {
		switch(it.second) {
		case 0: case 1:
			break;
		default:
			valid = false;
			printf("\n invalid allocation amount for object: %llu : %lld",
					it.first, it.second);
		}
	}
	
	printf("\n %llu, %llu", make, remove);
	if(valid == false) {
		printf(" ... FAULT \n");
	} else {
		printf(" ... OK \n");
	}
}



int main() {
	try {
		Test(27331, 123);
		Test(33423, 334);
		Test(43424, 0);
		Test(231, 43423);
		Test(14324, 0);
		Test(14324, 0);
		Test(14324, 0);
		Test(14324, 0);
		Test(0, 121);
		Test(0, 121);
		Test(0, 121);
		Test(0, 121);
		Test(0, 54354234123434llu);
	} catch(std::exception& e) {
		printf("\n%s\n", e.what());
	}
	return 0;
}

