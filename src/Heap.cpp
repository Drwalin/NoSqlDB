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

HeapFile::HeapFile(const char* fileName) :
		file(fileName),
		heap(file.Data<uint64_t>()) {
	uint64_t size = file.size();
	if(size < 8) {
		file.Resize(4096);
		file.Data<uint64_t>()[0] = 0;
	}
}

HeapFile::~HeapFile() {
	file.Close();
}

