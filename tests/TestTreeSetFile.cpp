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

#include "Debug.hpp"

#include "TreeSetFile.hpp"

#include <cstdio>
#include <chrono>
#include <exception>
#include <cmath>

#include <map>
#include <set>
#include <vector>

std::map<uint64_t, int64_t> allocated, full;

template<typename T>
bool GetRandomAllocated(T& allocator, uint64_t& ptr) {
	if(allocated.size() == 0)
		return false;
	uint64_t min = allocated.begin()->first;
	uint64_t max = allocated.rbegin()->first+allocator.blockSize;
	uint64_t bisect = RandMinMax(min, max);
	auto it = allocated.lower_bound(bisect);
	if(it == allocated.end()) {
		ptr = allocated.rbegin()->first;
	} else {
		ptr = it->first;
	}
	return true;
}

uint64_t RandV() {
	return Rand64()%987654321;
}

std::set<uint64_t> stdSet, temp;

void Cmp(TreeSetFile& fileSet) {
	bool equal = true;
	auto a = stdSet.begin();
	auto b = fileSet.begin();
	for(;a!=stdSet.end()&&b!=fileSet.end();++a, ++b) {
		if(*a != *b) {
			equal = false;
			break;
		}
	}
	if(a!=stdSet.end() || b!=fileSet.end())
		equal = false;
	if(!equal)
		printf("\n   ... FAULT\n");
	else
		printf("\n   ... OK\n");
}

void Test(TreeSetFile& fileSet, uint64_t make, uint64_t remove) {
	std::vector<uint64_t> push, pop;
	push.resize(make);
	pop.resize(remove);
	for(auto& v : push)
		v = RandV();
	for(auto& v : pop)
		v = RandV();
	
	uint64_t size = stdSet.size();
	
	Start();
	for(auto& v : push)
		stdSet.insert(v);
	End();
	printf("\n stdSet  %.0f push/s", make/DeltaTime());
	
	Start();
	for(auto& v : push)
		fileSet.insert(v);
	End();
	printf("\n fileSet %.0f push/s", make/DeltaTime());
	
	printf("\n %llu + %llu -> %llu (%.1f%% override)", size, make, stdSet.size(), 100.0*(double)(make-(stdSet.size()-size))/(double)(make));
	printf("\n size: %llu\n height: %llu (optimal: %.0f)", fileSet.size(), fileSet.root().Height(), log2(fileSet.size())*2.0);
	
	Cmp(fileSet);
	/*
	printf("\n\n Tree:\n");
	fileSet.root().Print(3);
	printf("\n End of tree\n\n");
	
	printf("\n Linear:");
	for(auto it = fileSet.begin(); it!=fileSet.end(); ++it) {
		printf("\n    %llu", *it);
	}
	printf("\n\n");
	*/
	
	size = stdSet.size();
	
	Start();
	for(auto& v : pop)
		stdSet.erase(v);
	End();
	printf("\n stdSet  %.0f pop/s", remove/DeltaTime());
	
	Start();
	for(auto& v : pop)
		fileSet.erase(v);
	End();
	printf("\n fileSet %.0f pop/s", remove/DeltaTime());
	
	printf("\n %llu - %llu -> %llu (%.1f%% miss)", size, remove, stdSet.size(), 100.0*(double)(size-stdSet.size())/(double)(remove));
	printf("\n size: %llu\n height: %llu (optimal: %.0f)", fileSet.size(), fileSet.root().Height(), log2(fileSet.size())*2.0);
	
	Cmp(fileSet);
}

int main() {
	try {
		BlockAllocator<32> allocator("32byte_block_mem.raw", "32byte_heap.raw");
		allocator.SetReservingBlocksCount(1024*8192);
		TreeSetFile fileSet(&allocator);
		fileSet.InitNewTree();
		
		
		Test(fileSet, 447327, 266476831);
		Test(fileSet, 154723, 276831);
		Test(fileSet, 125543, 22731);
		Test(fileSet, 125733, 23771);
		Test(fileSet, 118235, 235731);
		Test(fileSet, 122574, 2331);
		Test(fileSet, 145277, 75435642);
		
		fileSet.DestroyTree();
	} catch(std::exception& e) {
		printf("\n%s\n", e.what());
	}
	printf("\n");
	return 0;
}

