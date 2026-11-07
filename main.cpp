#include <print>
#include <string>
#include <array>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <ranges>
#include <span>
#include <cassert>

/*
> = increases memory pointer, or moves the pointer to the right 1 block.
< = decreases memory pointer, or moves the pointer to the left 1 block.
+ = increases value stored at the block pointed to by the memory pointer
- = decreases value stored at the block pointed to by the memory pointer
[ = like c while(cur_block_value != 0) loop.
] = if block currently pointed to's value is not zero, jump back to [
. = like c putchar(). print 1 character to the console
*/

static constexpr auto CODE = std::to_array<char>({
    #embed "examples/hello_world.bf"
});

using jump_map_t = std::unordered_map<std::size_t, std::size_t>;

jump_map_t make_jump_map(std::span<const char> code) {
    jump_map_t map;

    std::vector<std::size_t> opening_stack{};

    using namespace std::views;
    for (const auto [idx, c]: enumerate(code)) {
        switch (c) {
        case '[': opening_stack.push_back(idx); break;
        case ']': 
        {
            assert(!opening_stack.empty());
            const std::size_t opening = opening_stack.back();
            opening_stack.pop_back();
            map[opening] = idx;
            map[idx] = opening;
            break;
        }
        default: break;
        }
    }

    return map;
}

static constexpr std::size_t MEMORY_SIZE = 30'000;

struct State {
    std::size_t instruction_pointer;
    std::size_t data_pointer;
    std::array<std::uint8_t, MEMORY_SIZE> memory;
};

[[nodiscard]]
constexpr std::uint8_t& get_data_pointer(State& state) noexcept {
    assert(state.data_pointer < MEMORY_SIZE);
    return state.memory[state.data_pointer];
}

[[nodiscard]]
constexpr std::uint8_t read_data_pointer(const State& state) noexcept {
    assert(state.data_pointer < MEMORY_SIZE);
    return state.memory[state.data_pointer];
}

std::string run() {
    std::string out;
    State state{};
    auto jump_map = make_jump_map(CODE);

    while (state.instruction_pointer < CODE.size()) {
        const char instruction = CODE[state.instruction_pointer];

        switch (instruction) {
        case '>': ++state.data_pointer; break;
        case '<': --state.data_pointer; break;
        case '+': ++get_data_pointer(state); break;
        case '-': --get_data_pointer(state); break;
        case '.': out.push_back(read_data_pointer(state)); break;
        case '[': 
            if (read_data_pointer(state) == 0)
                state.instruction_pointer = jump_map[state.instruction_pointer];
            break;
        case ']': 
            if (read_data_pointer(state) != 0)
                state.instruction_pointer = jump_map[state.instruction_pointer];
            break;
        default: break;
        }
        ++state.instruction_pointer;
    }

    return out;
}

int main() {
    std::print("{}", run());     
    return 0;
}
