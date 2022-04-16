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

#ifndef GENERIC_RED_BLACK_TREE_HPP
#define GENERIC_RED_BLACK_TREE_HPP

#include <cinttypes>
#include <cstdio>

namespace Generic {

	inline const static uint64_t BLACK = 0;
	inline const static uint64_t RED = 1;
	
	namespace Prototypes {
		struct Tree {
			void* root;
			
			inline void* Root();
			inline void Root(void* newRoot);
		};

		struct Node {
			Node* left, *right, *parent;
			uint64_t value;
			uint64_t color;
			
			inline static uint64_t Color(Tree* tree, void* node);
			inline static void Color(Tree* tree, void* node, uint64_t newColor);
			inline static void* Left(Tree* tree, void* node);
			inline static void Left(Tree* tree, void* node, void* newLeft);
			inline static void* Right(Tree* tree, void* node);
			inline static void Right(Tree* tree, void* node, void* newRight);
			inline static void* Parent(Tree* tree, void* node);
			inline static void Parent(Tree* tree, void* node, void* newParent);
			inline static uint64_t Value(Tree* tree, void* node);
		};
	}
	
	template<typename Tree, typename NodeAccessor>
	struct NodeImpl {
		NodeImpl() = delete;
		~NodeImpl() = delete;
		
		using Node = NodeImpl;
		
		inline uint64_t Color(Tree* tree);
		inline void Color(Tree* tree, uint64_t newColor);
		inline Node* Left(Tree* tree);
		inline void Left(Tree* tree, Node* newLeft);
		inline Node* Right(Tree* tree);
		inline void Right(Tree* tree, Node* newRight);
		inline Node* Parent(Tree* tree);
		inline void Parent(Tree* tree, Node* newParent);
		inline uint64_t Value(Tree* tree);
		
		inline Node* Prev(Tree* tree);
		inline Node* Next(Tree* tree);
		
		inline Node* LeftMost(Tree* tree);
		inline Node* RightMost(Tree* tree);

		inline Node* FindGreaterEqual(Tree* tree, uint64_t value);
		
		
		
		
		
		
		inline uint64_t MinHeight(Tree* tree) {
			if(Left(tree) && Right(tree)) {
				uint64_t h=0;
				h = Left(tree)->MinHeight(tree);
				uint64_t y = Right(tree)->MinHeight(tree);
				if(h > y)
					h = y;
				return h+1;
			} else if(Left(tree) || Right(tree)) {
				return 1;
				uint64_t h=0;
				if(Left(tree))
					h = Left(tree)->MinHeight(tree);
				if(Right(tree)) {
					uint64_t y = Right(tree)->MinHeight(tree);
					if(h > y)
						h = y;
				}
				return h+1;
			}
			return 1;
		}
		
		inline uint64_t Height(Tree* tree) {
			uint64_t h = 0;
			if(Left(tree))
				h = Left(tree)->Height(tree);
			if(Right(tree)) {
				uint64_t y = Right(tree)->Height(tree);
				if(h < y)
					h = y;
			}
			return h+1;
		}
		
		inline uint64_t VerifyParenting(bool print, Tree* tree, uint64_t d=0, char C='R', Node* parent=NULL) {
			if(print) {
				for(int i=0; i<d; ++i)
					printf("    ");
				printf(" %c %p  : parent (%p)\n", C, this, parent);
			}
			uint64_t sum = this->Parent(tree) != parent;
			if(Left(tree))
				sum += Left(tree)->VerifyParenting(print, tree, d+1, 'L', this);
			if(Right(tree))
				sum += Right(tree)->VerifyParenting(print, tree, d+1, 'R', this);
			return sum;
		}
	};
	
	template<typename Tree, typename NodeAccessor>
	class RedBlackTree {
	public:
		
		using Node = NodeImpl<Tree, NodeAccessor>;
		
		template<typename T=Node>
		inline void Insert(T* node) { InsertImpl((Node*)node); }
		template<typename T=Node>
		inline void Erase(T* node) { EraseImpl((Node*)node); }
		
		template<typename T=Node>
		inline T* Root() { return (T*)(tree->Root()); }
		template<typename T=Node>
		inline void Root(T* newRoot) { tree->Root((Node*)newRoot); }
		
		template<typename T=Node>
		inline uint64_t Color(T* node) {
			if(node)
				return node->Color(tree);
			return BLACK;
		}
		
		template<typename T=Node>
		inline T* First() {
			if(Root())
				return (T*)(Root()->LeftMost(tree));
			return NULL;
		}
		template<typename T=Node>
		inline T* Last() {
			if(Root())
				return (T*)(Root()->RightMost(tree));
			return NULL;
		}
		
		template<typename T=Node>
		inline T* FindGreaterEqual(uint64_t value) {
			if(Root())
				return (T*)(Root()->FindGreaterEqual(tree, value));
			return NULL;
		}
		
	private:
		
		void InsertImpl(Node* node);
		void EraseImpl(Node* node);
		
		void BSTInsert(Node* node);
		Node* RBTInsertFixUpForRightChildUncle(Node* node);
		Node* RBTInsertFixUpForLeftChildUncle(Node* node);
		void RotateLeft(Node* x);
		void RotateRight(Node* x);
		
// 		void RBTErase(Node* node);
		
	public:
		
		Tree* tree;
		
	public:
		
		struct Iterator {
			Tree* tree;
			Node* node;
			Iterator(Tree*t, Node*n) {
				tree = t;
				node = n;
			}
			inline Iterator() = default;
			inline Iterator(Iterator&&) = default;
			inline Iterator& operator=(Iterator&) = default;
			inline Iterator& operator=(Iterator&&) = default;
			inline Node* operator*() { return node; }
			inline Iterator& operator++(int) { if(node) node = node->Next(tree); return *this; }
			inline Iterator& operator++() { if(node) node = node->Next(tree); return *this; }
			inline bool operator==(const Iterator& other) const {
				return node == other.node;
			}
			inline bool operator!=(const Iterator& other) const {
				return node != other.node;
			}
		};
		
		inline Iterator begin() { return {tree, First()}; }
		inline Iterator end() { return {tree, NULL}; }
		
		inline Iterator begin() const { return {(Tree*)tree, First()}; }
		inline Iterator end() const { return {(Tree*)tree, NULL}; }
	};
}

#include "GenericRedBlackTree.impl.hpp"

#endif

