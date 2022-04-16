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

/*
 *  FNV-1a hash algorithm to be used:
 *  http://isthe.com/chongo/tech/comp/fnv/#FNV-1a
 */

#ifndef HASH_HPP
#define HASH_HPP

#include <cinttypes>

namespace hash {
	
	const static uint32_t FNV_32_PRIME= (2<<24) + (2<<8) + 0x93;
	const static uint64_t FNV_64_PRIME = (2llu<<40) + (2llu<<8) + 0xB3llu;
	
	const static uint32_t FNV_32_OFFSET_BASIS = 2166136261;
	const static uint64_t FNV_64_OFFSET_BASIS = 14695981039346656037llu;
	
	inline void FNV1a64(uint64_t& hval, uint64_t data) {
		hval ^= data;
#if defined(NO_FNV_GCC_OPTIMIZATION)
		hval *= FNV_64_PRIME;
#else
		hval += (hval << 1) + (hval << 4) + (hval << 5) + (hval << 7) +
			(hval << 8) + (hval << 40);
#endif
	}
	
	
	inline uint64_t FNV1a64(uint64_t value) {
		uint64_t hval = FNV_64_OFFSET_BASIS;
		FNV1a64(hval, value);
		return hval;
	}
	
	inline uint64_t hash(uint64_t value) {
		return FNV1a64(value);
	}
	
	
	inline uint64_t FNV1a64(const char* str, uint64_t len) {
		uint64_t hval = FNV_64_OFFSET_BASIS;
		uint64_t end = len&(-8);
		for(uint64_t i=0; i<end; i+=8) {
			FNV1a64(hval, *(uint64_t*)(str+i));
		}
		uint64_t rest = len-end;
		if(rest) {
			uint64_t v=0;
			char* ptr = (char*)&v;
			for(uint64_t i=0; i<rest; ++i) {
				ptr[i] = str[end+i];
			}
			FNV1a64(hval, v);
		}
		return hval;
	}
	
	inline uint64_t hash(const char* str, uint64_t len) {
		return FNV1a64(str, len);
	}
	
};

#endif

