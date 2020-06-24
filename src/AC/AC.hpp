#pragma once
#include <unordered_map>
#include <fstream>
#include "Model/ModelInterface.hpp"

// Arithmetic Coding
class AC
{
public:
	// Derived Probability Model class
	static ModelInterface *model;

    // Encodes a given buffer
    static bool encode(const char* buffer, const char* encoded);

    // Decodes a given buffer
    static bool decode(const char* buffer, const char* decoded);
};

