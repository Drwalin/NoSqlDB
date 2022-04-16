/*
 *  This file is part of NoSqlDB.
 *  Copyright (C) 2022 Marek Zalewski aka Drwalin
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

#ifndef RED_BLACK_TREE_DOUBLE_LINKED_LIST_ALLOCATOR_HPP
#define RED_BLACK_TREE_DOUBLE_LINKED_LIST_ALLOCATOR_HPP

#include <cinttypes>
#include <cstdio>

class RedBlackTreeAllocator {
public:
	struct Node;
public:
	
	inline const static uint64_t BLOCK_SIZE = 64;
	inline const static uint64_t BLACK = 0;
	inline const static uint64_t RED = 1;
	
	inline static uint64_t RoundSize(uint64_t size) {
// 		if(size & (BLOCK_SIZE-1))
			return (size+(BLOCK_SIZE-1)) & (~(BLOCK_SIZE-1));
// 		return size;
	}
	
    RedBlackTreeAllocator();
    ~RedBlackTreeAllocator();
	
	uint64_t Allocate(uint64_t size, uint64_t* allocatedSize);
	void Free(uint64_t ptr, uint64_t size);
	
	inline void SetOrigin(void* newOrigin) {
		origin = newOrigin;
	}
	
	void MergeBlocks();
	
public:
	
    RedBlackTreeAllocator(RedBlackTreeAllocator&& rhs) = delete;
    RedBlackTreeAllocator(const RedBlackTreeAllocator& rhs) = delete;
    RedBlackTreeAllocator& operator=(RedBlackTreeAllocator&& rhs) = delete;
    RedBlackTreeAllocator& operator=(const RedBlackTreeAllocator& rhs) = delete;
	
private:
public:
	
	void Merge(Node* node);
	void Insert(Node* node);
	void Erase(Node* node);
	
	inline static struct Node* GetNodeFromPtr(void* origin, uint64_t ptr) {
		if(ptr == 0)
			return NULL;
		return (struct Node*)(((char*)origin)+ptr);
	}
	
	inline static uint64_t GetNodeOffset(void* origin, const void* node) {
		if(node == NULL)
			return 0;
		return (char*)node-(char*)origin;
	}
	
	struct __attribute__((packed)) RedBlackTreeNode {
		uint64_t leftChild;
		uint64_t rightChild;
	private:
		uint64_t parent;
	public:
		
		inline uint64_t GetColor() const {
			return parent&1;
		}
		inline void SetColor(uint64_t newColor) {
			parent = (parent&(~1llu)) | (newColor&1);
		}
		
		inline uint64_t GetParent() const {
			return parent&(~1llu);
		}
		inline void SetParent(uint64_t newParent) {
			parent = GetColor() | (newParent&(~1llu));
		}
		inline uint64_t GetLeftChild() const {
			return leftChild;
		}
		inline void SetLeftChild(uint64_t newLeftChild) {
			leftChild = newLeftChild;
		}
		inline uint64_t GetRightChild() const {
			return rightChild;
		}
		inline void SetRightChild(uint64_t newRightChild) {
			rightChild = newRightChild;
		}
		
		inline uint64_t GetSelf(void* origin) const {
			uint64_t ptr = GetNodeOffset(origin, this);
			return ptr & (~(BLOCK_SIZE-1));
		}
		
		RedBlackTreeNode() = delete;
		~RedBlackTreeNode() = delete;
	};
	
	struct __attribute__((packed)) Node {
		RedBlackTreeNode pointerTree;
		RedBlackTreeNode sizeTree;
		uint64_t size;
		uint64_t padding;
		
		template<RedBlackTreeNode Node::*field>
		inline uint64_t MinHeight(void* origin) {
			if(Left<field>(origin) && Right<field>(origin)) {
				uint64_t h=0;
				h = Left<field>(origin)->template MinHeight<field>(origin);
				uint64_t y = Right<field>(origin)->template MinHeight<field>(origin);
				if(h > y)
					h = y;
				return h+1;
			} else if(Left<field>(origin) || Right<field>(origin)) {
				return 1;
				uint64_t h=0;
				if(Left<field>(origin))
					h = Left<field>(origin)->template MinHeight<field>(origin);
				if(Right<field>(origin)) {
					uint64_t y = Right<field>(origin)->template MinHeight<field>(origin);
					if(h > y)
						h = y;
				}
				return h+1;
			}
			return 1;
		}
		
		template<RedBlackTreeNode Node::*field>
		inline uint64_t Height(void* origin) {
			uint64_t h = 0;
			if(Left<field>(origin))
				h = Left<field>(origin)->template Height<field>(origin);
			if(Right<field>(origin)) {
				uint64_t y = Right<field>(origin)->template Height<field>(origin);
				if(h < y)
					h = y;
			}
			return h+1;
		}
		
		template<RedBlackTreeNode Node::*field>
		inline uint64_t VerifyParenting(bool print, void* origin, uint64_t d=0, char C='R', void* parent_=NULL) {
			uint64_t parenting = GetNodeOffset(origin, parent_)/64;
			uint64_t self = GetNodeOffset(origin, this)/64;
			if(print) {
				for(int i=0; i<d; ++i)
					printf("    ");
				printf(" %c %lu  : parent (%lu)\n", C, self, parenting);
			}
			uint64_t sum = this->Parent<field>(origin) != parent_;
			if(Left<field>(origin))
				sum += Left<field>(origin)->template VerifyParenting<field>(print, origin, d+1, 'L', this);
			if(Right<field>(origin))
				sum += Right<field>(origin)->template VerifyParenting<field>(print, origin, d+1, 'R', this);
			return sum;
		}
		
		template<RedBlackTreeNode Node::*field>
		inline void Print(void* origin, uint64_t d=0, char C='R') {
			for(int i=0; i<d; ++i)
				printf("    ");
			printf(" %c %lu\n", C, ((size_t)this-(size_t)origin)/64);
			if(Left<field>(origin))
				Left<field>(origin)->template Print<field>(origin, d+1, 'L');
			if(Right<field>(origin))
				Right<field>(origin)->template Print<field>(origin, d+1, 'R');
		}
		
		template<RedBlackTreeNode Node::*field>
		inline uint64_t Color() const {
			return (this->*field).GetColor();
		}
		template<RedBlackTreeNode Node::*field>
		inline void Color(uint64_t color) {
			(this->*field).SetColor(color);
		}
		template<RedBlackTreeNode Node::*field>
		inline Node* Parent(void* origin) {
			return GetNodeFromPtr(origin, (this->*field).GetParent());
		}
		template<RedBlackTreeNode Node::*field>
		inline void Parent(void* origin, Node* parent) {
			(this->*field).SetParent(GetNodeOffset(origin, parent));
		}
		template<RedBlackTreeNode Node::*field>
		inline Node* Left(void* origin) {
			return GetNodeFromPtr(origin, (this->*field).GetLeftChild());
		}
		template<RedBlackTreeNode Node::*field>
		inline void Left(void* origin, Node* left) {
			(this->*field).SetLeftChild(GetNodeOffset(origin, left));
		}
		template<RedBlackTreeNode Node::*field>
		inline Node* Right(void* origin) {
			return GetNodeFromPtr(origin, (this->*field).GetRightChild());
		}
		template<RedBlackTreeNode Node::*field>
		inline void Right(void* origin, Node* right) {
			(this->*field).SetRightChild(GetNodeOffset(origin, right));
		}
		
		template<RedBlackTreeNode Node::*field>
		Node* Prev(void* origin);
		template<RedBlackTreeNode Node::*field>
		Node* Next(void* origin);
		template<RedBlackTreeNode Node::*field>
		Node* FindGreaterEqual(void* origin, uint64_t value);
		
		template<RedBlackTreeNode Node::*field>
		inline uint64_t GetValue() {
// 			if constexpr(field == POINTER_FIELD) {
				return (uint64_t)(void*)this;
// 			} else {
// 				return size;
// 			}
		}
		
		Node() = delete;
		~Node() = delete;
	};
	
	template<RedBlackTreeNode Node::*field>
	inline static uint64_t Color(const Node* const node) {
		if(node)
			return node->Color<field>();
		return BLACK;
	}
	
	template<RedBlackTreeNode Node::*field>
	inline Node* Root() {
		if constexpr(field == POINTER_FIELD)
			return pointerRoot;
		else
			return sizeRoot;
	}
	
	template<RedBlackTreeNode Node::*field>
	inline void Root(Node* newRoot) {
		if constexpr(field == &Node::pointerTree) {
			pointerRootId = GetNodeOffset(origin, newRoot);
			pointerRoot = newRoot;
		} else {
			sizeRootId = GetNodeOffset(origin, newRoot);
			sizeRoot = newRoot;
		}
		if(newRoot) {
			newRoot->Parent<field>(origin, NULL);
		}
	}
	
	template<RedBlackTreeNode Node::*field>
	inline Node* First() {
		Node* node = Root<field>();
		if(node == NULL)
			return NULL;
		while(node->Left<field>(origin))
			node = node->Left<field>(origin);
		return node;
	}
	template<RedBlackTreeNode Node::*field>
	inline Node* Last() {
		Node* node = Root<field>();
		if(node == NULL)
			return NULL;
		while(node->Right<field>(origin) != NULL)
			node = node->Right<field>(origin);
		return node;
	}
	
	template<RedBlackTreeNode Node::*field>
	void RotateLeft(Node* x);
	template<RedBlackTreeNode Node::*field>
	void RotateRight(Node* x);
	
	constexpr static inline RedBlackTreeNode Node::*POINTER_FIELD = &Node::pointerTree;
	constexpr static inline RedBlackTreeNode Node::*SIZE_FIELD = &Node::sizeTree;
	
	template<RedBlackTreeNode Node::*field>
	void RBTInsert(Node* node);
	template<RedBlackTreeNode Node::*field>
	Node* RBTInsertFixUpForRightChildUncle(Node* node);
	template<RedBlackTreeNode Node::*field>
	Node* RBTInsertFixUpForLeftChildUncle(Node* node);
	template<RedBlackTreeNode Node::*field>
	void BSTInsert(Node* node);
	template<RedBlackTreeNode Node::*field>
	void RBTErase(Node* node);
	
	template<RedBlackTreeNode Node::*field>
	Node* FindGreaterEqual(uint64_t value);
	
private:
public:
	
	uint64_t pointerRootId;
	uint64_t sizeRootId;
	void* origin;
	Node* pointerRoot;
	Node* sizeRoot;
};

#endif

