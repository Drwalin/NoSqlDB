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

#ifndef CACHED_FILE_HPP
#define CACHED_FILE_HPP

#include <cinttypes>

#ifdef CACHED_FILE_CPP
#include <boost\iostreams\device\mapped_file.hpp>
#else
namespace boost {
	namespace iostreams {
		class mapped_file;
	};
};
#endif

// opening file for read and write even if it does not exists
class CachedFile {
public:
	
	CachedFile() : ptr(NULL), file(NULL), size(0) {
	}
	CachedFile(const char* fileName) : ptr(NULL), file(NULL), size(0) {
		Open(fileName);
	}
	CachedFile(CachedFile&& other) : ptr(other.ptr), file(other.file), size(0) {
	}
	CachedFile(const CachedFile&) = delete;
	~CachedFile() {
		Close();
	}
	
	CachedFile& operator=(const CachedFile&) = delete;
	
	bool Open(const char* fileName);
	inline uint64_t Size() const {return size;}
	template<typename T=void>
	inline T*& Data() {return *(T**)&ptr;}
	void Close();
	
	inline bool IsOpen() const {return file!=NULL;}
	
	uint64_t Resize(uint64_t newSize);
	uint64_t Reserve(uint64_t minSize);
	
private:
	
	uint64_t size;
	void* ptr;
	boost::iostreams::mapped_file* file;
};

#endif
