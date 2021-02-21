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

#ifndef BLOCK_ALLOCATOR_HPP
#define BLOCK_ALLOCATOR_HPP

#include "CachedFile.hpp"
#include "HeapFile.hpp"

// TODO: Add BitmapFile class to store info about all allocated and free blocks

uint64_t BitsForBlockSizeCorrect(uint64_t value) constexpr {
	uint64_t i;
	for(i=3; i<21 && value>(1<<i); ++i) {
	}
	return i;
}

template<uint64_t __blockSize = 4096>
class BlockAllocator {
public:
	const uint64_t blockOffsetBits = BitsForBlockSizeCorrect(__blockSize);
	const uint64_t blockSize = 1<<blockOffsetBits;
	const uint64_t reservingBlocksAtOnce = 256;
	
	BlockAllocator();
	BlockAllocator(const char* memoryFile, const char* heapFile);
	~BlockAllocator();
	
	bool Open(const char* memoryFile, const char* heapFile);
	void Close();
	
	uint64_t AllocateBlock();
	void FreeBlock(uint64_t ptr);
	
	template<typename T=void>
	inline T* Origin() {return (T*)memoryFile.ptr;}
	template<typename T=void>
	inline const T* Origin() const {return (T*)memoryFile.ptr;}
	
private:
	
	void Reserve(uint64_t blocks);
	
	
	uint64_t reservedBlocks;
	
	CachedFile memoryFile;
	HeapFile heap;
};

#include "BlockAllocator.cpp"

#endif

