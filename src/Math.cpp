#include <Math.hpp>

namespace tp
{

void flipUVsAroundX(tp::FourUVs& UVs)
{
    swap(UVs[0], UVs[1]);
    swap(UVs[3], UVs[2]);
}

} // namespace tp