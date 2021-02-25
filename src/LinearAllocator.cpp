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

#include "LinearAllocator.hpp"

LinearAllocator::LinearAllocator() {}

LinearAllocator::LinearAllocator(const char* linearMemoryFile,
		TreeSetFile& allocatedRanges) {
	Open(linearMemoryFile, allocatedRanges);
}

LinearAllocator::~LinearAllocator() {
	Close();
}

bool LinearAllocator::Open(const char* linearMemoryFile,
		TreeSetFile& allocatedRanges) {
	Close();
	memoryFile.Open(linearMemoryFile);
	allocated = allocatedRanges;
	if(!*this) {
		allocated = TreeSetFile();
		memoryFile.Close();
		return false;
	}
	return true;
}

void LinearAllocator::Close() {
	memoryFile.Close();
	allocated = TreeSetFile();
}



uint64_t LinearAllocator::reserved() {
	return memoryFile.Size();
}

uint64_t LinearAllocator::used() {
	uint64_t ret = 0;
	for(auto it = allocated.begin(); it; ++it) {
		auto next = it.next();
		if(!next)
			break;
		ret += *next - *it;
		it = next.next();
	}
	return ret;
}



uint64_t LinearAllocator::Allocate(uint64_t size) {
	if(!*this)
		return 0;
	if(size==0 || size>maxSize)
		return 0;
	size = (size+7)&(-8);
	size += 8;
	return InternalAllocate(size)+8;
}

uint64_t LinearAllocator::InternalAllocate(uint64_t size) {
	auto it = allocated.begin();
	if(it) {
		if(*it >= size) {
			allocated.insert(0);
			allocated.insert(size);
			Origin<uint64_t>()[0] = size;
			return 0;
		}
		it = it.next();
		while(it) {
			auto next = it.next();
			if(!next) {
				break;
			} else {
				uint64_t ptr = *it;
				uint64_t diff = *next - ptr;
				if(diff > size) {
					*it += size;
					Origin<uint64_t>(ptr)[0] = size;
					return ptr;
				} else if(diff == size) {
					next = allocated.erase(it);
					allocated.erase(next);
					Origin<uint64_t>(ptr)[0] = size;
					return ptr;
				}
			}
			it = next.next();
		}
	}
	
	auto rbegin = allocated.rbegin();
	uint64_t last = rbegin ? *rbegin : 0;
	uint64_t newMinSize = last + size;
	if(memoryFile.Size() < newMinSize+8) {
		uint64_t newSize = (newMinSize+allocationUnitSize-1) & (-allocationUnitSize);
		memoryFile.Reserve(newSize);
	}
	
	uint64_t ptr;
	if(rbegin) {
		ptr = *rbegin;
		*rbegin += size;
	} else {
		ptr = 0;
		allocated.insert(0);
		allocated.insert(size);
	}
	Origin<uint64_t>(ptr)[0] = size;
	return ptr;
}


void LinearAllocator::Free(uint64_t ptr) {
	if(!*this)
		return;
	if(ptr==-1)
		return;
	ptr &= -8;
	if(ptr==0)
		return;
	ptr -= 8;
	uint64_t data_size = Origin<uint64_t>(ptr)[0];
	InternalFree(ptr, data_size);
}

void LinearAllocator::InternalFree(uint64_t ptr, uint64_t size) {
	auto le = allocated.find_le(ptr);
	auto ge = le.next();
	
	if(!le || !ge) {
		fprintf(stderr, "\n trying to free not allocated pointers");
		fflush(stderr);
		return;
	}
	
	if(*ge < ptr+size) {
		fprintf(stderr, "\n invalid allocated memory tree:"
				"\n   freeing %llu sizeof %llu(%llu)"
				"\n   found values: %llu <> %llu",
				ptr, size-8, size, *le, *ge);
		fflush(stderr);
		return;
	}
	
	if(*le == ptr && *ge == (ptr+size)) {
		ge = allocated.erase(le);
		allocated.erase(ge);
	} else if(*le == ptr) {
		*le += size;
	} else if(*ge == (ptr+size)) {
		*ge -= size;
	} else {
		allocated.insert(ptr);
		allocated.insert(ptr+size);
	}
}

