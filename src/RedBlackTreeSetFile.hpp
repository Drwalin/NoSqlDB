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

#ifndef RED_BLACK_TREE_SET_FILE_HPP
#define RED_BLACK_TREE_SET_FILE_HPP

class RedBlackTreeSetFile {
public:
	
	struct Block {
		uint64_t value;
		uint64_t parent;
		uint64_t left, right;
		
		bool IsRed() const {return !(parent&1);}
		bool IsBlack() const {return parent&1;}
		uint64_t Parent() {return parent & -2;}
		void SetRed() {parent &= -2;}
		void SetBlack() {parent |= 1;}
	};
	
	class Iterator {
	public:
		
		Iterator() : block(-1), set(NULL) {}
		Iterator(Iterator&& other) : block(other.block), set(other.set) {}
		Iterator(const Iterator& other) : block(other.block), set(other.set) {}
		Iterator(uint64_t block, RedBlackTreeSet* set) : block(block), allocator(set.allocator) {}
		Iterator(uint64_t block, BlockAllocator<32>* allocator) : block(block), allocator(allocator) {}
		
		inline Iterator& operator=(const Iterator& other) {
			block = other.block;
			allocator = other.allocator;
			return *this;
		}
		inline Iterator& operator=(Iterator&& other) {
			block = other.block;
			allocator = other.allocator;
			return *this;
		}
		
		inline uint64_t& operator*(int) {return *Origin(block);}
		
		inline bool operator==(const Iterator<TI>& other) const {
			return block==other.block;
		}
		inline bool operator!=(const Iterator<TI>& other) const {
			return block!=other.block;
		}
		
		inline operator bool() const {return block!=-1;}
		
		inline Iterator next() const { Iterator ret = *this; return ret++;}
		inline Iterator& operator++();
		inline Iterator operator++(int);
		
		inline Iterator prev() const { Iterator ret = *this; return ret--;}
		inline Iterator& operator--();
		inline Iterator operator--(int);
		
		inline Block& GetBlock() {return *alocator->Origin<Block>(block);}
		
		inline uint64_t& value() {return GetBlock().value;}
		inline Iterator left() {return Iterator(GetBlock().left, allocator);}
		inline Iterator right() {return Iterator(GetBlock().right, allocator);}
		inline Iterator parent() {return Iterator(GetBlock().parent, allocator);}
		
		inline bool red() const {return GetBlock().IsRed();}
		inline bool black() const {return GetBlock().IsBlack();}
		
		inline Iterator grandParent() {return parent().parent();}
		Iterator sibling();
		
		Iterator uncle();
		
		Iterator begin();
		Iterator rbegin();
		inline Iterator end() {return Iterator(-1,allocator);}
		inline Iterator rend() {return end();}
		
	private:
		
		uint64_t block;
		BlockAllocator<32>* allocator;
	};
	
	
	Iterator insert(Iterator hint, uint64_t value);	// return Iterator to new element
	Iterator insert(uint64_t value);				// return Iterator to new element
	
	Iterator erase(Iterator it);		// return Iterator to next element after removed
	Iterator erase(uint64_t value);		// return Iterator to next element after removed
	
	Iterator find_closest(uint64_t value);
	Iterator find(uint64_t value);
	Iterator find_ge(uint64_t value);	// returns iterator to first element not lower than value
	Iterator find_le(uint64_t value);	// returns iterator to first element not grater then value
	
	inline Iterator begin() {root().begin();}
	inline Iterator rbegin() {root().rbegin();}
	inline Iterator end() {return Iterator(-1,allocator);}
	inline Iterator rend() {return end();}
	
	inline Iterator root() {return Iterator(ptr, allocator);}
	
	
	template<typename T=Block>
	inline T* Origin() {return allocator->Origin<T>();}
	template<typename T=Block>
	inline const T* Origin() const {return allocator->Origin<T>();}
	
	template<typename T=Block>
	inline T* Origin(uint64_t offset) {return allocator->Origin<T>(offset);}
	template<typename T=Block>
	inline const T* Origin(uint64_t offset) const {
		return allocator->Origin<T>(offset);
	}
	
	Iterator
	
private:
	
	uint64_t ptr;
	BlockAllocator<32>* allocator;
};

#endif

