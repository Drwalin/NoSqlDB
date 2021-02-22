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
    - 8B: size in bytes
	- 8B: pointer to last block (optional: only if size>blockSize-16)
    - ..: data
    - 8B: pointer to next block	
   
   other blocks:
    - ..: data
	- 8B: pointer to next block
   
   if pointer to next block = 0xFFF... then
      this is last block
   
*/

template<uint64_t blockSize>
class ByteBlockList {
public:
	
	using AllocatorType = BlockAllocator<blockSize>;
	const static uint64_t nextBlockOffset = AllocatorType::blockSize-8;
	
	/*
	   Iterator supports only types with empty constructor
	   and with size of power of two and not grater that 8 bytes
	   (only 1, 2, 4, 8 bytes)
	*/
	template<typename T>
	class Iterator {
	public:
		
		Iterator();
		Iterator(Iterator&& other);
		Iterator(const Iterator& other);
		Iterator(uint64_t block, uint64_t offset, ByteBlockList* list);
		
		inline T& operator*(int) {
			return *(T*)(Origin()+block+offset);
		}
		
		inline bool operator==(const Iterator<T>& other) const {
			return block==other.block && offset==other.offset;
		}
		inline bool operator!=(const Iterator<T>& other) const {
			return block!=other.block || offset!=other.offset;
		}
		
		inline Iterator<T> next() const {
			Iterator<T> ret = *this;
			return ret++;
		}
		
		inline Iterator<T>& operator++() {
			offset += sizeof(T);
			if(offset >= AllocatorType::blockSize) {
				block = Origin<uint64_t>()[
						(AllocatorType::blockSize-1ll)>>3];
				offset = 0;
			}
			return *this;
		}
		
		inline Iterator<T> operator++(int) {
			Iterator<T> ret = *this;
			this->operator++();
			return ret;
		}
		
		template<typename T2>
		inline T2* Origin() {return list->Origin<T2>();}
		
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
		uint64_t* p = Origin<uint64_t>()[ptr>>3];
		p[0] = 0;
		p[nextBlockOffset>>3] = -1ll;
	}
	void InitAtPosition(uint64_t ptr) {this->ptr = ptr;}
	
	template<typename T>
	Iterator<T> begin() {
		return Iterator<T>(ptr,
				ptr ? ( size>(allocator->blockSize-16) ? 16 : 8 ) : 0,
				this);
	}
	template<typename T>
	inline Iterator<T> Begin() {return begin<T>():}
	
	template<typename T>
	inline Iterator<T> end() {return Iterator<T>(-1ll, 0, this):}
	template<typename T>
	inline Iterator<T> End() {return end<T>():}
	
	
	template<typename T>
	T& at(uint64_t id);
	template<typename T>
	inline T& At(uint64_t id) {return at<T>();}
	
	
	inline uint64_t size() const {
		if(ptr == -1ll)
			return 0;
		return Origin<uint64_t>()[block>>3];
	}
	inline uint64_t Size() const {return size():}
	
	template<typename T=void>
	inline T* Origin() {return allocator->Origin<T>();}
	
private:
	
	uint64_t ptr;
	AllocatorType* allocator;
};

#include "ByteBlockList.cpp"

#endif

