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

#ifndef TREE_SET_FILE_HPP
#define TREE_SET_FILE_HPP

#include "BlockAllocator.hpp"

class TreeSetFile {
public:
	
	struct Root {
		uint64_t root;
		uint64_t nodes;
	};
	
	struct Block {
		uint64_t value;
		uint64_t parent;
		uint64_t left, right;
	};
	
	class Iterator {
	public:
		
		Iterator() : block(-1), allocator(NULL) {}
		Iterator(Iterator&& other) : block(other.block), allocator(other.allocator) {}
		Iterator(const Iterator& other) : block(other.block), allocator(other.allocator) {}
		Iterator(uint64_t block, TreeSetFile* set) : block(block), allocator(set->allocator) {}
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
		
		inline uint64_t& operator*() {return GetBlock().value;}
		
		inline bool operator==(const Iterator& other) const {
			return block==other.block;
		}
		inline bool operator!=(const Iterator& other) const {
			return block!=other.block;
		}
		
		inline operator bool() const {return block!=-1;}
		
		Iterator next() const;
		Iterator& operator++();
		Iterator operator++(int);
		
		inline Iterator prev() const;
		Iterator& operator--();
		Iterator operator--(int);
		
		inline Block& GetBlock() {return *allocator->Origin<Block>(block);}
		inline const Block& GetBlock() const {return *allocator->Origin<Block>(block);}
		
		inline uint64_t& value() {return GetBlock().value;}
		inline uint64_t value() const {return GetBlock().value;}
		inline Iterator left() const {return Iterator(GetBlock().left, allocator);}
		inline Iterator right() const {return Iterator(GetBlock().right, allocator);}
		inline Iterator parent() const {return Iterator(GetBlock().parent, allocator);}
		
		inline Iterator grandParent() {return parent().parent();}
		Iterator sibling();
		
		Iterator uncle();
		
		Iterator begin();
		Iterator rbegin();
		inline Iterator end() {return Iterator(-1,allocator);}
		inline Iterator rend() {return end();}
		
		friend class TreeSetFile;
		
		void Print(uint64_t tab) {
			if(!*this)
				return;
			printf("\n");
			for(uint64_t i=0; i<tab; ++i)
				printf(" ");
			printf("%llu", value());
			left().Print(tab+1);
			right().Print(tab+1);
		}
		
		uint64_t Height() {
			if(!*this)
				return 0;
			uint64_t l = left().Height()+1;
			uint64_t r = right().Height()+1;
			if(l>r)
				return l;
			return r;
		}
		
	private:
		
		uint64_t block;
		BlockAllocator<32>* allocator;
	};
	
	TreeSetFile() : ptr(0), allocator(NULL) {}
	TreeSetFile(BlockAllocator<32>* allocator) : ptr(0), allocator(allocator) {}
	TreeSetFile(uint64_t treeManagerNode, BlockAllocator<32>* allocator) : ptr(treeManagerNode), allocator(allocator) {}
	TreeSetFile(TreeSetFile&& other) : ptr(other.ptr), allocator(other.allocator) {}
	TreeSetFile(const TreeSetFile& other) : ptr(other.ptr), allocator(other.allocator) {}
	~TreeSetFile() {ptr=0; allocator=NULL;}
	
	inline TreeSetFile& operator=(TreeSetFile&& other) {ptr=other.ptr; allocator=other.allocator; return*this;}
	inline TreeSetFile& operator=(const TreeSetFile& other) {ptr=other.ptr; allocator=other.allocator; return*this;}
	
	inline operator bool() const {return ptr!=-1 && (bool)allocator && (bool)*allocator;}
	
	
	Iterator insert(Iterator hint, uint64_t value);	// return Iterator to new element
	Iterator insert(uint64_t value);				// return Iterator to new element
	
	Iterator erase(Iterator it);		// return Iterator to next element after removed
	Iterator erase(uint64_t value);		// return Iterator to next element after removed
	
	Iterator find_closest(uint64_t value);
	Iterator find(uint64_t value);
	Iterator find_ge(uint64_t value);	// returns iterator to first element not lower than value
	Iterator find_le(uint64_t value);	// returns iterator to first element not grater then value
	
	inline Iterator begin() {return root().begin();}
	inline Iterator rbegin() {return root().rbegin();}
	inline Iterator end() {return Iterator(-1,allocator);}
	inline Iterator rend() {return end();}
	
	inline Iterator root() {return Iterator(_root().root, allocator);}
	
	
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
	
	Root& _root() {return *allocator->Origin<Root>(ptr);}
	const Root& _root() const {return *allocator->Origin<Root>(ptr);}
	
	void InitNewTree();
	void DestroyTree();
	void DestroyBranch(Iterator it);
	
	uint64_t size() const {return _root().nodes;}
	
private:
	
	uint64_t ptr;
	BlockAllocator<32>* allocator;
};

#endif

