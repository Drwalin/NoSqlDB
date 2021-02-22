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

#ifndef BYTE_BLOCK_LIST_HPP
#define BYTE_BLOCK_LIST_HPP

#include "Allocator.hpp"

/*
   linked block list structure:
   
   first block:
    - 8B: pointer to next block	
    - 8B: size in bytes
	- 8B: pointer to last block (optional: only if size>blockSize-16)
    - ..: data
   
   other blocks:
	- 8B: pointer to next block
    - ..: data
   
   if pointer to next block = 0xFFF... then
      this is last block
   
*/

template<typename T, uint64_t blockSize=64>
class ByteBlockList {
public:
	
	using AllocatorType = BlockAllocator<blockSize>;
	const static uint64_t nextBlockOffset = AllocatorType::blockSize-8;
	
	/*
	   Iterator supports only types with empty constructor
	   and with size of power of two and not grater that 8 bytes
	   (only 1, 2, 4, 8 bytes)
	*/
	template<typename TI=T>
	class Iterator {
	public:
		
		Iterator();
		Iterator(Iterator&& other);
		Iterator(const Iterator& other);
		Iterator(uint64_t block, uint64_t offset, ByteBlockList* list);
		
		inline TI& operator*(int) {return *Origin(block+offset);}
		
		inline bool operator==(const Iterator<TI>& other) const {
			return block==other.block && offset==other.offset;
		}
		inline bool operator!=(const Iterator<TI>& other) const {
			return block!=other.block || offset!=other.offset;
		}
		
		inline Iterator<TI> next() const {
			Iterator<TI> ret = *this;
			return ret++;
		}
		
		inline Iterator<TI>& operator++() {
			offset += sizeof(TI);
			if(offset >= AllocatorType::blockSize) {
				block = Origin<uint64_t>(AllocatorType::blockSize-8);
				offset = 0;
			}
			return *this;
		}
		
		inline Iterator<TI> operator++(int) {
			Iterator<TI> ret = *this;
			this->operator++();
			return ret;
		}
		
		inline TI* Origin() {return list->Origin<TI>();}
		inline TI* Origin(uint64_t offset) {return list->Origin<TI>(offset);}
		
	private:
		
		uint64_t offset;
		uint64_t block;
		ByteBlockList* list;
	};
	
	
	ByteBlockList(AllocatorType& allocator) {
		this->allocator = &allocator;
		ptr = -1ll;
	}
	~ByteBlockList();
	
	void Free(); // iterate over all blocks and free them
	void InitEmpty() {
		ptr = allocator->AllocateBlock();
		uint64_t* p = Origin<uint64_t>(ptr);
		p[0] = 0;
		p[nextBlockOffset>>3] = -1ll;
	}
	void InitAtPosition(uint64_t ptr) {this->ptr = ptr;}
	
	template<typename TI=T>
	Iterator<TI> begin() {
		return Iterator<TI>(ptr,
				ptr ? ( size>(allocator->blockSize-16) ? 16 : 8 ) : 0,
				this);
	}
	template<typename TI=T> inline Iterator<TI> Begin() {return begin<TI>():}
	
	template<typename TI=T>
	inline Iterator<TI> end() {return Iterator<TI>(-1ll, 0, this):}
	template<typename TI=T> inline Iterator<TI> End() {return end<TI>():}
	
	inline uint64_t size() const {
		if(ptr == -1ll)
			return 0;
		return Origin<uint64_t>(block);
	}
	inline uint64_t Size() const {return size():}
	
	template<typename T=void>
	inline T* Origin() {return allocator->Origin<T>();}
	template<typename T=void>
	inline T* Origin() const {return allocator->Origin<T>();}
	
	template<typename T=void>
	inline T* Origin(uint64_t offset) {return allocator->Origin<T>(offset);}
	template<typename T=void>
	inline const T* Origin(uint64_t offset) const {return allocator->Origin<T>(offset);}
	
private:
	
	uint64_t ptr;
	AllocatorType* allocator;
};

#include "ByteBlockList.cpp"

#endif

