#include "Math.hpp"

namespace tp
{

void flipUVsAroundX(FourUVs& UVs)
{
    auto t = UVs[0];
    swap(UVs[0], UVs[1]);
    swap(UVs[3], UVs[2]);
}

} // namespace tp