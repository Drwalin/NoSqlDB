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

#include <cstdio>
#define DEBUG {fprintf(stderr, "\n %s:%i", __FILE__, __LINE__); fflush(stderr);}
HeapFile::HeapFile() : heap(file.Data<uint64_t>()) {
}

HeapFile::HeapFile(const char* fileName) : heap(file.Data<uint64_t>()) {
	Open(fileName);
}

HeapFile::~HeapFile() {
	file.Close();
}

bool HeapFile::Open(const char* fileName) {
	Close();
	file.Open(fileName);
	uint64_t size = file.Size();
	if(heap == NULL)
		return false;
	if(size < 8) {
		file.Resize(blockSize);
		heap[0] = 0;
	}
	return true;
}

void HeapFile::Close() {
	file.Close();
}


