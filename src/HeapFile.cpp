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

#include "HeapFile.hpp"

HeapFile::HeapFile() {
}

HeapFile::HeapFile(const char* fileName) {
	Open(fileName);
}

HeapFile::~HeapFile() {
	file.Close();
}

bool HeapFile::Open(const char* fileName) {
	Close();
	file.Open(fileName);
	uint64_t size = file.Size();
	if(Origin() == NULL)
		return false;
	if(size < 8) {
		file.Resize(blockSize);
		Origin()[0] = 0;
	}
	return true;
}

void HeapFile::Close() {
	file.Close();
}

void HeapFile::BuildFromRange(uint64_t min, uint64_t max) {
	file.Reserve(((max-min)+1)<<3);
	Origin()[0] = max-min;
	for(uint64_t i=1; i<=Origin()[0]; ++i, ++min) {
		Origin()[i] = min;
	}
}

void HeapFile::BuildFromRange(uint64_t min, uint64_t elements, uint64_t step) {
	file.Reserve(((elements)+1)<<3);
	Origin()[0] = elements;
	for(uint64_t i=1; i<=elements; ++i, min+=step) {
		Origin()[i] = min;
	}
}

void HeapFile::Push(uint64_t value) {
	Origin()[0]++;
	if((file.Size()>>3) <= Origin()[0]) {
		file.Reserve((((Origin()[0]>>blockSizeBits)+1)<<blockSizeBits)<<3);
	}
	uint64_t i, j, v;
	for(i=Origin()[0], j=i>>1; i>1; i=j, j>>=1) {
		v = Origin()[j];
		if(value >= v) {
			break;
		}
		Origin()[i] = v;
	}
	Origin()[i] = value;
}

bool HeapFile::Pop(uint64_t& result) {
	if((file.Size()<<3) == 0 || Origin()[0] == 0)
		return false;
	result = Origin()[1];
	uint64_t size = Origin()[0];
	uint64_t last = Origin()[size];
	Origin()[0]--;
	uint64_t i, j, v;
	for(i=1, j=2; j<=size; i=j, j<<=1) {
		v = Origin()[j];
		if((j < size) && (v > Origin()[j+1])) {
			++j;
			v = Origin()[j];
		}
		if(last <= v)
			break;
		Origin()[i] = v;
		
	}
	Origin()[i] = last;
	return true;
}
