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

#include "MultiBlockAllocator.hpp"

#include <string>

MultiBlockAllocator::MultiBlockAllocator() {
	preallocatedBlocks = 0;
}

MultiBlockAllocator::MultiBlockAllocator(const char* fileNameBase) {
	preallocatedBlocks = 0;
	Open(fileNameBase);
}

MultiBlockAllocator::~MultiBlockAllocator() {
	Close();
}

bool MultiBlockAllocator::Open(const char* fileNameBase) {
	Close();
	std::string base = fileNameBase;
	bool valid = true;
	valid &= memory.Open((base+"_memory.raw").c_str());
	valid &= blockSizeAssociation.Open((base+"_block_size_association.raw").c_str());
	for(uint64_t i=minBlockSizeBits; i<=maxBlockSizeBits; ++i) {
		valid &= memory.Open((base+"_heap"+std::to_string(i)+".raw").c_str());
	}
	if(!valid) {
		Close();
		return false;
	}
	preallocatedBlocks = memory.Size()>>maxBlockSize;
	return valid;
}

void MultiBlockAllocator::Close() {
	memory.Close();
	for(auto& it : heap)
		it.Close();
	blocksDestinySizes.Close();
	preallocatedBlocks = 0;
}

uint64_t MultiBlockAllocator::Allocate(uint64_t size) {
	if(!*this)
		return -1;
	if(size < minBlockSize || size > maxBlockSize)
		return -1;
	uint64_t i = minBlockSizeBits;
	for(; i<maxBlockSizeBits && size>(1<<i); ++i) {
	}
	return InternalAllocate(i);
	
	
	
	if(heap.Size() == 0)
		Reserve(reservingBlocksAtOnce);
	uint64_t ret=0;
	heap.Pop(ret);
	return ret<<blockOffsetBits;
}

void MultiBlockAllocator::InternalAllocate(uint64_t sizeBits) {
	Reserve(sizeBits);
	uint64_t ptr = -1;
	heap[sizeBits].Pop(ptr);
	return ptr;
}


void MultiBlockAllocator::Free(uint64_t ptr) {
	uint64_t sizeBits = blockSizeAssociation.Origin<uint8_t>(ptr>>maxBlockSize);
	heap[sizeBits].Push(ptr);
}

void MultiBlockAllocator::Reserve(uint64_t sizeBits) {
	HeapFile& heap = this->heap[sizeBits];
	if(heap.Size() != 0)
		return;
	heap.BuildFromRange(preallocatedBlocks<<maxBlockSizeBits,
			maxBlockSize>>sizeBits,
			1<<sizeBits);
	blockSizeAssociation.Resize(preallocatedBlocks+1);
	blockSizeAssociation.Origin<uint8_t>(preallocatedBlocks)[0] = sizeBits;
	preallocatedBlocks += 1;
	memory.Resize(preallocatedBlocks<<maxBlockSizeBits);
}

