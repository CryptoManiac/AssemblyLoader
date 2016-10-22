#include <string>
#include <sstream>
#include <iomanip>

#include "HexStr.hxx"

using namespace std;

string HexStr(vector<uint8_t>::const_iterator first, vector<uint8_t>::const_iterator last, bool insert_spaces) {
    ostringstream ss;
    ss << hex << setfill('0');
    while (first != last) {
        ss << setw(2) << static_cast<int>(*first++);
        if (insert_spaces && first != last) {
            ss << " ";
        }
    }
    return ss.str();
}
