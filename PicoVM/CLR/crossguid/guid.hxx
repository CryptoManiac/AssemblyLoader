#ifndef __GUID_HXX__
#define __GUID_HXX__

/*
The MIT License (MIT)

Copyright (c) 2014 Graeme Hill (http://graemehill.ca)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdint>

// Class to represent a GUID/UUID. Each instance acts as a wrapper around a
// 16 byte value that can be passed around by value. It also supports
// conversion to string (via the stream operator <<) and conversion from a
// string via constructor.
class Guid
{
  public:

    // create a guid from vector of bytes
    Guid(const std::vector<uint8_t> &bytes);

    // create a guid frob bytes range
    Guid(std::vector<uint8_t>::const_iterator first, std::vector<uint8_t>::const_iterator last);

    // create a guid from array of bytes
    Guid(const uint8_t *bytes);

    // create a guid from string
    Guid(const std::string &fromString);

    // create empty guid
    Guid();

    // copy constructor
    Guid(const Guid &other);

    // move constructor
    Guid(Guid &&other) noexcept;

    // overload assignment operator
    Guid &operator=(const Guid &other);

    // overload equality and inequality operator
    bool operator==(const Guid &other) const;
    bool operator!=(const Guid &other) const;

    // overload comparison operator (required by std::map)
    bool operator<(const Guid &other) const;

    void swap(Guid& other) noexcept;

    // Convert to string
    std::string str() const;
    const char *c_str() const;

  private:

    // actual data
    std::vector<uint8_t> _bytes = {0,0,0,0, 0,0, 0,0, 0,0, 0,0,0,0,0,0};

    // make the << operator a friend so it can access _bytes
    friend std::ostream &operator<<(std::ostream &s, const Guid &guid);
};

#endif
