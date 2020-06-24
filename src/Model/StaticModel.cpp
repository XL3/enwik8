#include "StaticModel.hpp"

u64 StaticModel::get_frequency(u32 symbol)
{
	return cfreq[symbol];
}

void StaticModel::update_model(u32 symbol)
{
	for (u32 s = symbol; s < 0x101; ++s) 
        cfreq[s]++; 
}

void StaticModel::reset_model()
{
    for (u32 s = 0; s < 0x101; ++s) 
        cfreq[s] = s; 
}

u32 StaticModel::get_symbol(u64 subfreq)
{
    // Binary search the array for the upper bound 
    // FFFFFFFFFFFFFFFFFFFFFFFFFFF [T] TTTTTTTTTTT
    u32 l = 0, r = 0x100;
    u32 mid;

    while (l < r) {
        mid = l + (r - l) / 2;
        if (cfreq[mid] > subfreq) r = mid;
        else l = mid + 1;
    }

    return l;
}