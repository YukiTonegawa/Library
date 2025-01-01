#ifndef _ITERATOR_BITSET_H_
#define _ITERATOR_BITSET_H_
#include <cstdint>
#include <vector>

struct iterator_bitset {
  public:
	uint64_t *ptr;
	size_t pos;
	
    iterator_bitset(uint64_t& n, size_t p) : ptr(&n), pos(p) {}

	iterator_bitset& operator = (bool val) {
		if (val) {
			*(this->ptr) |= (uint64_t)1 << this->pos;
        } else {
			*(this->ptr) &= ~((uint64_t)1 << this->pos);
        }
		return *this;
	}

	iterator_bitset& operator = (iterator_bitset& ref) {
		if (*(ref.ptr) & (uint64_t)1 << ref.pos) {
			*(this->ptr) |= (uint64_t)1 << this->pos;
        } else {
			*(this->ptr) &= ~((uint64_t)1 << this->pos);
        }
		return *this;
	}

	operator bool() const {
		return *(this->ptr) & (uint64_t)1 << this->pos;
	}
};


#endif