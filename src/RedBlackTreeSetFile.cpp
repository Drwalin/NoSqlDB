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

RedBlackTreeSetFile::Iterator& RedBlackTreeSetFile::Iterator::operator++() {
	(*this) = this->operator++(int);
	return *this;
}

RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::Iterator::operator++(int) {
	if(!*this)
		return *this;
	Iterator r = right();
	if(r)
		return r.begin();
	else
		return r;
}

RedBlackTreeSetFile::Iterator& RedBlackTreeSetFile::Iterator::operator--() {
	(*this) = this->operator--(int);
	return *this;
}

RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::Iterator::operator--(int) {
	if(!*this)
		return *this;
	Iterator l = left();
	if(l)
		return l.begin();
	else
		return l;
}

RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::Iterator::sibling() {
	Iterator parent = parent();
	if(!parent)
		return parent;
	if(parent.left() == *this)
		return parent.right();
	return parent.left();
}

RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::Iterator::uncle() {
	return parent().sibling();
}


RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::Iterator::begin() {
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

RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::Iterator::rbegin() {
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







RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::insert(Iterator hint, uint64_t value);
RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::insert(uint64_t value) {
	return insert(find_close(value), value);
}

RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::erase(Iterator it) {
	if(!it)
		return end();
	Iterator next = it.next();
	
	...
	
	return next;
}

RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::erase(uint64_t value) {
	return erase(find(value));
}



RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::find_closest(uint64_t value) {
	Iterator it = root(), next;
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

RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::find(uint64_t value) {
	Iterator it = root();
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

RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::find_ge(uint64_t value) {
	Iterator it = find_close(value);
	if(it.value() < value)
		return it.next();
	return it;
}

RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::find_le(uint64_t value) {
	Iterator it = find_close(value);
	if(it.value() > value)
		return it.prev();
	return it;
}




