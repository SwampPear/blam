#include <cstdint>


namespace Blam {

struct Token {
    const char* src;  // source string
    uint16_t pos;     // position
    uint16_t len;     // length
};

}  // namespace Token