#pragma once
#include "ModelInterface.hpp"

class StaticModel : public ModelInterface
{
    u32 cfreq[0x101];
public:
    u64 get_frequency(u32 symbol);

    void update_model(u32 symbol);
    void reset_model();

    u32 get_symbol(u64 subfreq);
};
