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

#include "BlockAllocator.hpp"

BlockAllocator::BlockAllocator(const char* memoryFile,
			const char* bitmapFile) :
		memory(this->memoryFile.Data()), bitmap(this->bitmapFile.Data<uint64_t>()){
	this->memoryFile.Open(memoryFile);
	this->bitmapFile.Open(bitmapFile);
}

BlockAllocator::~BlockAllocator() {
	memoryFile.Close();
	bitmapFile.Close();
	memory = NULL;
}

uint64_t BlockAllocator::AllocateBlock() {
	uint64_t bitmapSize = bitmapFile.Size()>>3;
	for(uint64_t i=0; i<bitmapSize; ++i) {
		if(bitmap[i] != (uint64_t)(-1)) {
			for(int j=0; j<64; ++j) {
				if(bitmap[i]&(1<<j) == 0) {
					bitmap[i] |= (1<<j);
					return ((i<<6)+j) << blockOffsetBits;
				}
			}
		}
	}
	bitmapFile.Reserve((bitmapSize<<3) + 64);
	for(uint64_t i=bitmapSize; i<(bitmapFile.Size()>>3); ++i) {
		bitmap[i] = 0;
	}
	bitmap[bitmapSize] = 1;
	return bitmapSize<<(6+blockOffsetBits);
}

void BlockAllocator::FreeBlock(uint64_t ptr) {
	uint64_t blockId = ptr>>blockOffsetBits;
	if(bitmapFile.Size() > blockId>>3) {
		if(bitmap[blockId>>6] & (1<<(blockId&63))) {
			bitmap[blockId>>6] ^= 1<<(blockId&63);
		}
	}
}

