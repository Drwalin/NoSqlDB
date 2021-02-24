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

#include "TreeSetFile.hpp"

TreeSetFile::Iterator& TreeSetFile::Iterator::operator++() {
	(*this) = (*this)++;
	return *this;
}

TreeSetFile::Iterator TreeSetFile::Iterator::operator++(int) {
	if(!*this)
		return *this;
	Iterator r = right();
	if(r)
		return r.begin();
	Iterator c = *this;
	Iterator p = parent();
	while(p) {
		Iterator l = p.left();
		if(l==c)
			return p;
		c = p;
		p = c.parent();
	}
	if(!p)
		return p;
	return p.right().begin();
}

TreeSetFile::Iterator& TreeSetFile::Iterator::operator--() {
	(*this) = (*this)--;
	return *this;
}

TreeSetFile::Iterator TreeSetFile::Iterator::operator--(int) {
	printf("\n TreeSetFile::Iterator TreeSetFile::Iterator::operator--(int) is not done!");
	if(!*this)
		return *this;
	Iterator l = left();
	if(l)
		return l.begin();
	else
		return l;
}

TreeSetFile::Iterator TreeSetFile::Iterator::sibling() {
	Iterator parent = this->parent();
	if(!parent)
		return parent;
	if(parent.left() == *this)
		return parent.right();
	return parent.left();
}

TreeSetFile::Iterator TreeSetFile::Iterator::uncle() {
	return parent().sibling();
}


TreeSetFile::Iterator TreeSetFile::Iterator::begin() {
	Iterator ret = *this;
	if(!ret)
		return ret;
	for(;;) {
		Iterator next = ret.left();
		if(!next)
			break;
		ret = next;
	}
	return ret;
}

TreeSetFile::Iterator TreeSetFile::Iterator::rbegin() {
	Iterator ret = *this;
	if(!ret)
		return ret;
	for(;;) {
		Iterator next = ret.right();
		if(!next)
			break;
		ret = next;
	}
	return ret;
}





TreeSetFile::Iterator TreeSetFile::insert(Iterator hint, uint64_t value) {
	if(hint && hint.value()==value) {
		return hint;
	}
	Iterator it(allocator->AllocateBlock(), allocator);
	it.value() = value;
	it.GetBlock().parent = hint.block;
	it.GetBlock().left = -1;
	it.GetBlock().right = -1;
	if(!hint) {
		_root().root = it.block;
	} else {
		uint64_t hv = hint.value();
		
		if(hv < value)
			hint.GetBlock().right = it.block;
		else
			hint.GetBlock().left = it.block;
	}
	
	_root().nodes++;
	
	return it;
}

TreeSetFile::Iterator TreeSetFile::insert(uint64_t value) {
	return insert(find_closest(value), value);
}

TreeSetFile::Iterator TreeSetFile::erase(Iterator it) {
	if(!it)
		return it;
	Iterator next = it.next();
	_root().nodes--;
	if(it.GetBlock().left!=-1 && it.GetBlock().right!=-1) {
		uint64_t next_value = next.value();
		erase(next);
		it.value() = next_value;
		return it;
	} else {
		Iterator l, r;
		l = it.left();
		r = it.right();
		
		Iterator parent = it.parent();
		const bool itWasLeft = (parent&&parent.GetBlock().left==it.block) ? true : false;
		
		if(l) {
			l.GetBlock().parent = parent.block;
			if(parent) {
				if(itWasLeft)
					parent.GetBlock().left = l.block;
				else
					parent.GetBlock().right = l.block;
			} else {
				_root().root = l.block;
			}
		} else if(r) {
			r.GetBlock().parent = parent.block;
			if(parent) {
				if(itWasLeft)
					parent.GetBlock().left = r.block;
				else
					parent.GetBlock().right = r.block;
			} else {
				_root().root = r.block;
			}
		} else {
			if(parent) {
				if(itWasLeft)
					parent.GetBlock().left = -1;
				else
					parent.GetBlock().right = -1;
			} else {
				_root().root = -1;
			}
		}
		allocator->FreeBlock(it.block);
	}
	
	return next;
}

TreeSetFile::Iterator TreeSetFile::erase(uint64_t value) {
	return erase(find(value));
}



TreeSetFile::Iterator TreeSetFile::find_closest(uint64_t value) {
	Iterator it = root(), next;
	if(!it)
		return it;
	while(true) {
		uint64_t v = it.value();
		if(value == v)
			break;
		else if(value < v)
			next = it.left();
		else
			next = it.right();
		if(next)
			it = next;
		else
			break;
	}
	return it;
}

TreeSetFile::Iterator TreeSetFile::find(uint64_t value) {
	Iterator it = root();
	if(!it)
		return it;
	while(it) {
		uint64_t v = it.value();
		if(value == v)
			break;
		else if(value < v)
			it = it.left();
		else
			it = it.right();
	}
	return it;
}

TreeSetFile::Iterator TreeSetFile::find_ge(uint64_t value) {
	if(!root())
		return end();
	Iterator it = find_closest(value);
	if(it.value() < value)
		return it.next();
	return it;
}

TreeSetFile::Iterator TreeSetFile::find_le(uint64_t value) {
	if(!root())
		return end();
	Iterator it = find_closest(value);
	if(it.value() > value)
		return it.prev();
	return it;
}



void TreeSetFile::InitNewTree() {
	ptr = allocator->AllocateBlock();
	_root().root = -1;
	_root().nodes = 0;
}

void TreeSetFile::DestroyTree() {
	if(ptr != -1) {
		DestroyBranch(root());
		allocator->FreeBlock(ptr);
		ptr = -1;
	}
}

void TreeSetFile::DestroyBranch(Iterator it) {
	if(!it)
		return;
	DestroyBranch(it.left());
	DestroyBranch(it.right());
	allocator->FreeBlock(it.block);
}



