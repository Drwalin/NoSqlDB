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

#ifndef LINEAR_ALLOCATOR_HPP
#define LINEAR_ALLOCATOR_HPP

#include "CachedFile.hpp"
#include "TreeSetFile.hpp"

/*
 *  LinearAllocator uses NULL pointer invalid value instead of internal
 *  -1 pointer invalid value 
 *  
 */

class LinearAllocator {
public:
	
	const static uint64_t maxSize = 1llu<<60;
	const static uint64_t allocationUnitSize = 1<<24;	// 16 MiB
	
	LinearAllocator();
	LinearAllocator(const char* linearMemoryFile, TreeSetFile& allocatedRanges);
	~LinearAllocator();
	
	bool Open(const char* linearMemoryFile, TreeSetFile& allocatedRanges);
	void Close();
	
	inline operator bool() const {return memoryFile && allocated;}
	
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
	
	uint64_t reserved();
	uint64_t used();
	
private:
	
	uint64_t InternalAllocate(uint64_t size);
	void InternalFree(uint64_t ptr, uint64_t size);
	
	CachedFile memoryFile;
	TreeSetFile allocated;
};

class Pointer {
public:
	
	Pointer() : ptr(0), alloc(NULL) {}
	Pointer(uint64_t ptr, class LinearAllocator* alloc) : ptr(ptr), alloc(alloc) {}
	Pointer(Pointer&& ptr) : ptr(ptr.ptr), alloc(ptr.alloc) {}
	Pointer(const Pointer& ptr) : ptr(ptr.ptr), alloc(ptr.alloc) {}
	~Pointer() {}
	
	inline void* operator*() {if(ptr&&alloc) return alloc->Origin(ptr); return NULL;}
	
	inline Pointer& operator=(const Pointer& r) {ptr=r.ptr; alloc=r.alloc; return *this;}
	inline Pointer& operator=(Pointer&& r) {ptr = r.ptr; alloc = r.alloc; return *this;}
	inline Pointer& operator=(void* r) {
		if(!alloc) {
			ptr = 0;
		} else {
			ptr = (uint64_t)r - (uint64_t)alloc->Origin();
		}
		return *this;
	}
	
	inline operator bool() const {return ptr && alloc && *alloc;}
	
	template<typename T=void>
	inline T* Origin() {return alloc->Origin<T>();}
	template<typename T=void>
	inline const T* Origin() const {return alloc->Origin<T>();}
	
	template<typename T=void>
	inline T* Origin(uint64_t offset) {return alloc->Origin<T>(offset);}
	template<typename T=void>
	inline const T* Origin(uint64_t offset) const {return alloc->Origin<T>(offset);}
	
	friend class LinearAllocator;
	
	inline const LinearAllocator* Alloc() const {return alloc;}
	inline const uint64_t Ptr() const {return ptr;}
	
	void SetTo(void* dst) const {
		if(dst && alloc && *alloc) {
			*((uint64_t*)dst) = ptr;
		} else {
			*((uint64_t*)dst) = 0;
		}
	}
	
private:
	
	uint64_t ptr;
	class LinearAllocator* alloc;
};

#endif

