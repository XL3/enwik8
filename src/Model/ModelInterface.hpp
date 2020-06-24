#pragma once

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

// Abstract probability model class
class ModelInterface
{
public:
	// Returns the cumulative occurence count for a given symbol
	virtual u64 get_frequency(u32 symbol) = 0;

	// Updates the probability model on occurence of a given symbol
	virtual void update_model(u32 symbol) = 0;

	// Resets the probability model to an initial state
	virtual void reset_model() = 0;

	// Returns the upper bound symbol for a given occurence count
	virtual u32 get_symbol(u64 subfreq) = 0;
};
