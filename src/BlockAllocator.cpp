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

#include "BlockAllocator.hpp"

BlockAllocator::BlockAllocator(const char* memoryFile,
			const char* heapFile) :
		memory(this->memoryFile.Data()) {
	this->memoryFile.Open(memoryFile);
	this->heap.Open(heapFile);
	reservedBlocks = this->memoryFile.Size()>>blockOffsetBits;
}

BlockAllocator::~BlockAllocator() {
	memoryFile.Close();
	heap.Close();
}

uint64_t BlockAllocator::AllocateBlock() {
	if(heap.Size() == 0)
		Reserve(reservingBlocksAtOnce);
	uint64_t ret=0;
	heap.Pop(ret);
	return ret<<blockOffsetBits;
}

void BlockAllocator::FreeBlock(uint64_t ptr) {
	heap.Push(ptr>>blockOffsetBits);
}

void BlockAllocator::Reserve(uint64_t blocks) {
	memoryFile.Reserve((reservedBlocks+blocks)<<blockOffsetBits);
	uint64_t i=reservedBlocks;
	reservedBlocks += blocks;
	for(; i<reservedBlocks; ++i) {
		heap.Push(i);
	}
}

