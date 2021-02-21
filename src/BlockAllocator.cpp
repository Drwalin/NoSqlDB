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

template<uint64_t a, uint64_t b>
BlockAllocator<a, b>::BlockAllocator() {
	preallocatedBlocks = 0;
}

template<uint64_t a, uint64_t b>
BlockAllocator<a, b>::BlockAllocator(const char* memoryFile,
		const char* heapFile) {
	Open(memoryFile, heapFile);
}

template<uint64_t a, uint64_t b>
BlockAllocator<a, b>::~BlockAllocator() {
	memoryFile.Close();
	heap.Close();
}

template<uint64_t a, uint64_t b>
bool BlockAllocator<a, b>::Open(const char* memoryFile, const char* heapFile) {
	bool valid = this->memoryFile.Open(memoryFile);
	valid &= this->heap.Open(heapFile);
	if(!valid) {
		this->memoryFile.Close();
		this->heap.Close();
		return valid;
	}
	preallocatedBlocks = this->memoryFile.Size()>>blockOffsetBits;
	return valid;
}

template<uint64_t a, uint64_t b>
uint64_t BlockAllocator<a, b>::AllocateBlock() {
	if(heap.Size() == 0)
		Reserve(reservingBlocksAtOnce);
	uint64_t ret=0;
	heap.Pop(ret);
	return ret<<blockOffsetBits;
}

template<uint64_t a, uint64_t b>
void BlockAllocator<a, b>::FreeBlock(uint64_t ptr) {
	heap.Push(ptr>>blockOffsetBits);
}

template<uint64_t a, uint64_t b>
void BlockAllocator<a, b>::Reserve(uint64_t blocks) {
	memoryFile.Reserve((preallocatedBlocks+blocks)<<blockOffsetBits);
	uint64_t i=preallocatedBlocks;
	preallocatedBlocks += blocks;
	if(heap.Size() == 0) {
		heap.BuildFromRange(i, preallocatedBlocks);
	} else {
		for(; i<preallocatedBlocks; ++i) {
			heap.Push(i);
		}
	}
}

