#ifndef __HEXSTR_HXX__
#define __HEXSTR_HXX__

#include <string>
#include <vector>

std::string HexStr(std::vector<uint8_t>::const_iterator first, std::vector<uint8_t>::const_iterator last, bool insert_spaces = false);

#endif
