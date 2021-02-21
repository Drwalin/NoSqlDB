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

#ifndef HEAP_FILE_HPP
#define HEAP_FILE_HPP

#include "CachedFile.hpp"

class HeapFile {
public:
	
	const static uint64_t blockSize = 4096;
	const static uint64_t blockSizeBits = 12;
	
	HeapFile();
	HeapFile(const char* fileName);
	~HeapFile();
	
	bool Open(const char* fileName);
	void Close();
	
	void Push(uint64_t value);
	bool Pop(uint64_t& result);
	void BuildFromRange(uint64_t min, uint64_t max); // excluding max
	
	inline uint64_t Size() const {return Origin()[0];}
	
	inline uint64_t* Origin() {return file.Origin<uint64_t>();}
	inline const uint64_t* Origin() const {return file.Origin<uint64_t>();}
	
private:
	
	CachedFile file;
};

#endif

