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

#include "CachedFile.hpp"

#include <cstdio>
#include <chrono>
#include <exception>

#define DEBUG {fprintf(stderr, "\n %s:%i", __FILE__, __LINE__); fflush(stderr);}

CachedFile file;
uint64_t*& ptr = file.Data<uint64_t>();

void Test(uint64_t elements, uint64_t expected) {
	file.Open("testCachedFile.raw");
	std::chrono::high_resolution_clock::time_point a =
		std::chrono::high_resolution_clock::now();
	file.Reserve(elements*8);
	std::chrono::high_resolution_clock::time_point b =
		std::chrono::high_resolution_clock::now();
	printf("\n resizing: %f s",
			std::chrono::duration<double>(b-a).count());
	
	
	uint64_t i=0;
	uint64_t foundNotEqual = 0;
	
	std::chrono::high_resolution_clock::time_point beg =
		std::chrono::high_resolution_clock::now();
	for(;i<elements; ++i) {
		if(ptr[i] != i+1) {
			foundNotEqual++;
		}
	}
	std::chrono::high_resolution_clock::time_point end1 =
		std::chrono::high_resolution_clock::now();
	file.Close();
	std::chrono::high_resolution_clock::time_point end2 =
		std::chrono::high_resolution_clock::now();
	
	printf("\n not valid found: %llu/%llu   ... %s", foundNotEqual, expected,
			foundNotEqual!=expected?"FAULT":"OK");
	printf("\n reading: %f + %f s",
			std::chrono::duration<double>(end1-beg).count(),
			std::chrono::duration<double>(end2-end1).count());
	
	file.Open("testCachedFile.raw");
	
	beg = std::chrono::high_resolution_clock::now();
	for(uint64_t i=0; i<elements; ++i) {
		ptr[i] = i+1;
	}
	end1 = std::chrono::high_resolution_clock::now();
	file.Close();
	end2 = std::chrono::high_resolution_clock::now();
	
	printf("\n writing: %f + %f s",
			std::chrono::duration<double>(end1-beg).count(),
			std::chrono::duration<double>(end2-end1).count());
}



int main() {
	try {
		{CachedFile d("testCachedFile.raw"); d.Resize(1); d.Data<char>()[0] = 'a';};
		uint64_t elements = 789931;
		Test(elements, elements);
		Test(elements, 0);
		uint64_t mult = 5;
		Test(elements*mult, elements*(mult-1));
		Test(elements*mult, 0);
	} catch(std::exception& e) {
		printf("\n%s\n", e.what());
	}
	return 0;
}

