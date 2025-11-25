#include "Core/Utils/FreeBlockList.hpp"

int main(int argc, char** argv)
{
    Core::FreeBlockList<std::uint32_t, Core::DefaultFreeBlockListNodeStorage<std::uint32_t>> list(
        //16,
        1024);

    auto blk1 = list.Acquire(Core::BlockRequirement<std::uint32_t>{.alignment = 64, .size = 128});
    auto blk2 = list.Acquire(Core::BlockRequirement<std::uint32_t>{.alignment = 16, .size = 512});
    auto blk4 = list.Acquire(Core::BlockRequirement<std::uint32_t>{.alignment = 24, .size = 9});
    auto blk3 = list.Acquire(Core::BlockRequirement<std::uint32_t>{.alignment = 8, .size = 16});

    list.Release(*blk3);
    list.Release(*blk1);
    list.Release(*blk4);
    list.Release(*blk2);
}