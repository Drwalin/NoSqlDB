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



RedBlackTreeSetFile::Iterator RedBlackTreeSetFile::Iterator::find(uint64_t value) {
	
}






