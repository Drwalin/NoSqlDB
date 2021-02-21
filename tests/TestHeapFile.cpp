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
#include <exception>
#include <chrono>
#include <map>
#include <vector>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <algorithm>

#include "Debug.hpp"

HeapFile heap;

#include <queue>
std::priority_queue<uint64_t> stlHeap;

void Test(uint64_t elements, bool reopenclose=false) {
	if(reopenclose)
		heap.Open("heap.test.raw");
	
	std::vector<uint64_t> inserts;
	inserts.resize(elements);
	
	for(uint64_t& e : inserts) {
		e = Rand64();
	}
	
	Start();
	for(uint64_t& e : inserts) {
		stlHeap.push(e);
	}
	End();
	printf("\n stl pushing %llu took %.3f s -> %.2f M/s", elements, DeltaTime(), elements*0.000001/DeltaTime());
	
	Start();
	for(uint64_t& e : inserts) {
		heap.Push(e);
	}
	End();
	printf("\n my  pushing %llu took %.3f s -> %.2f M/s", elements, DeltaTime(), elements*0.000001/DeltaTime());
	
	
	if(reopenclose) {
		Start();
		heap.Close();
		End();
		printf("\n Closing after pushing took: %.3f s", DeltaTime());
		heap.Open("heap.test.raw");
	}
	
	std::vector<uint64_t> pops;
	
	pops.resize(stlHeap.size());
	Start();
	for(uint64_t i=0; i<pops.size(); ++i) {
		pops[i] = stlHeap.top();
		stlHeap.pop();
	}
	End();
	printf("\n stl poping  %llu took %.3f s -> %.2f M/s", pops.size(), DeltaTime(), pops.size()*0.000001/DeltaTime());
	
	pops.resize(heap.Size());
	Start();
	for(uint64_t i=0; i<pops.size(); ++i) {
		if(!heap.Pop(pops[i])) {
			pops.resize(i);
			break;
		}
	}
	End();
	printf("\n my  poping  %llu took %.3f s -> %.2f M/s", pops.size(), DeltaTime(), pops.size()*0.000001/DeltaTime());
	
	if(reopenclose) {
		Start();
		heap.Close();
		End();
		printf("\n Closing after poping took: %.3f s", DeltaTime());
	}
	
	std::sort(inserts.begin(), inserts.end());
	if(inserts != pops) {
		printf(" ... FAULT!   (Look for details in 'TestHeapFile.log'");
		FILE* out = fopen("TestHeapFile.log", "w+");
		fprintf(out, "\n\n\n\n New fault test desciption (%llu elements):", elements);
		for(uint64_t i=0; i<inserts.size() || i<pops.size(); ++i) {
			fprintf(out, "\n %llu : (", i);
			if(i<inserts.size())
				fprintf(out, "%.5llu", inserts[i]);
			fprintf(out, ", ");
			if(i<pops.size())
				fprintf(out, "%.5llu", pops[i]);
			fprintf(out, ")");
		}
		fclose(out);
	} else {
		printf(" ... OK!");
	}
	
	printf("\n");
}



int main() {
	srand(time(NULL));
	try {
		heap.Open("heap.test.raw");
		uint64_t elements = 734117;
		Test(elements);
		Test(elements);
		Test(elements*17);
		Test(elements*17);
		Test(elements*17);
	} catch(std::exception& e) {
		printf("\n%s", e.what());
	}
	printf("\n");
	return 0;
}

