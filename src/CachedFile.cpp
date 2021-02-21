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

#ifndef CACHED_FILE_CPP
#define CACHED_FILE_CPP

#include "CachedFile.hpp"

#include <fstream>
#include <cstdio>
#define DEBUG {fprintf(stderr, "\n %s:%i", __FILE__, __LINE__); fflush(stderr);}
bool CachedFile::Open(const char* fileName) {
	Close();
	{
		std::ifstream f(fileName, std::ios::binary);
		char c;
		if(!(f.good() && f>>c)) {
			f.close();
			std::ofstream d(fileName, std::ios::binary);
			c = 0;
			d.write(&c, 1);
			d.close();
		}
		f.close();
	}
	
	file = new boost::iostreams::mapped_file(fileName);
	if(file->is_open()) {
		ptr = file->data();
		size = file->size();
		return true;
	} else {
		delete file;
		file = NULL;
		ptr = NULL;
		size = 0;
		return false;
	}
	return true;
}

void CachedFile::Close() {
	if(file) {
		file->close();
		delete file;
		file = NULL;
		size = 0;
		ptr = NULL;
	}
}

uint64_t CachedFile::Resize(uint64_t newSize) {
	if(file && newSize > 0) {
		file->resize(newSize);
		size = newSize;
		ptr = file->data();
	}
	return size;
}

uint64_t CachedFile::Reserve(uint64_t minSize) {
	if(file && size < minSize) {
		file->resize(minSize);
		size = minSize;
		ptr = file->data();
	}
	return size;
}

#endif

