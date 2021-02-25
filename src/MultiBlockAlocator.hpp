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

#ifndef MULTI_BLOCK_ALLOCATOR_HPP
#define MULTI_BLOCK_ALLOCATOR_HPP

#include "CachedFile.hpp"
#include "HeapFile.hpp"

/*
 *  
 *  Uses -1 instead of NULL pointer
 *  
 */

class MultiBlockAllocator {
public:
	
	const static uint64_t maxBlockSize = 1024*1024*16;
	const static uint64_t minBlockSize = 16;
	
	const static uint64_t maxBlockSizeBits = 24;
	const static uint64_t minBlockSizeBits = 4;
	
	MultiBlockAllocator();
	MultiBlockAllocator(const char* fileNameBase);
	~MutliBlockAllocator();
	
	inline operator bool() const {return (bool)memory;}
	
	bool Open(const char* fileNameBase);
	void Close();
	
	uint64_t Allocate(uint64_t size);
	void Free(uint64_t ptr);
	
	
	template<typename T=void>
	inline T* Origin() {return memoryFile.Origin<T>();}
	template<typename T=void>
	inline const T* Origin() const {return memoryFile.Origin<T>();}
	
	template<typename T=void>
	inline T* Origin(uint64_t offset) {return memoryFile.Origin<T>(offset);}
	template<typename T=void>
	inline const T* Origin(uint64_t offset) const {return memoryFile.Origin<T>(offset);}
	
private:
	
	uint64_t InternalAllocate(uint64_t sizeBits);
	void Reserve(uint64_t sizeBits);
	
	uint64_t preallocatedBlocks;
	
	CachedFile memory;
	HeapFile heap[maxBlockSizeBits];
	CachedFile blockSizeAssociation;	// byte array[reserved max blocks] -> size of block used
};

#endif

