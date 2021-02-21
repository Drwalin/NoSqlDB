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

class BlockAllocator {
public:
	const static uint64_t blockSize = 4096;
	const static uint64_t blockOffsetBits = 12;
	const static uint64_t reservingBlocksAtOnce = 256;
	
	BlockAllocator(const char* memoryFile,
			const char* heapFile);
	~BlockAllocator();
	
	uint64_t AllocateBlock();
	void FreeBlock(uint64_t ptr);
	
private:
	
	void Reserve(uint64_t blocks);
	
	uint64_t reservedBlocks;
	
	void*& memory;
	CachedFile memoryFile;
	
	HeapFile heap;
};

#endif

