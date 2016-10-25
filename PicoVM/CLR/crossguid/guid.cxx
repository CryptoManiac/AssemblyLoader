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

#include "guid.hxx"
#include <iomanip>


using namespace std;

// overload << so that it's easy to convert to a string
ostream &operator<<(ostream &s, const Guid &guid)
{
  return s << hex << setfill('0')
    << setw(2) << (int)guid._bytes[0]
    << setw(2) << (int)guid._bytes[1]
    << setw(2) << (int)guid._bytes[2]
    << setw(2) << (int)guid._bytes[3]
    << "-"
    << setw(2) << (int)guid._bytes[4]
    << setw(2) << (int)guid._bytes[5]
    << "-"
    << setw(2) << (int)guid._bytes[6]
    << setw(2) << (int)guid._bytes[7]
    << "-"
    << setw(2) << (int)guid._bytes[8]
    << setw(2) << (int)guid._bytes[9]
    << "-"
    << setw(2) << (int)guid._bytes[10]
    << setw(2) << (int)guid._bytes[11]
    << setw(2) << (int)guid._bytes[12]
    << setw(2) << (int)guid._bytes[13]
    << setw(2) << (int)guid._bytes[14]
    << setw(2) << (int)guid._bytes[15];
}

string Guid::str() const {
    ostringstream ss;
    ss << *this;
    return ss.str();
}

const char *Guid::c_str() const {
  return str().c_str();
}

// create a guid from vector of bytes
Guid::Guid(const vector<uint8_t> &bytes)
{
  _bytes = bytes;
}

// create a guid from array of bytes
Guid::Guid(const uint8_t *bytes)
{
  _bytes.assign(bytes, bytes + 16);
}

// create a guid frob bytes range
Guid::Guid(vector<uint8_t>::const_iterator first, vector<uint8_t>::const_iterator last)
{
  _bytes.assign(first, last);
}

// converts a single hex char to a number (0 - 15)
uint8_t hexDigitToChar(char ch)
{
  if (ch > 47 && ch < 58)
    return ch - 48;

  if (ch > 96 && ch < 103)
    return ch - 87;

  if (ch > 64 && ch < 71)
    return ch - 55;

  return 0;
}

// converts the two hexadecimal characters to an uint8_t (a byte)
uint8_t hexPairToChar(char a, char b)
{
  return hexDigitToChar(a) * 16 + hexDigitToChar(b);
}

// create a guid from string
Guid::Guid(const string &fromString)
{
  _bytes.clear();

  char charOne = '\0', charTwo;
  bool lookingForFirstChar = true;

  for (const char &ch : fromString)
  {
    if (ch == '-')
      continue;

    if (lookingForFirstChar)
    {
      charOne = ch;
      lookingForFirstChar = false;
    }
    else
    {
      charTwo = ch;
      auto byte = hexPairToChar(charOne, charTwo);
      _bytes.push_back(byte);
      lookingForFirstChar = true;
    }
  }

}

// create empty guid
Guid::Guid() : _bytes(vector<uint8_t>(16, 0))
{
}

// copy constructor
Guid::Guid(const Guid &other) : _bytes(other._bytes)
{
}

// move constructor
Guid::Guid(Guid &&other) noexcept : _bytes(other._bytes)
{
  other._bytes.clear();
}

// overload assignment operator
Guid &Guid::operator=(const Guid &other)
{
  Guid(other).swap(*this);
  return *this;
}

// overload equality operator
bool Guid::operator==(const Guid &other) const
{
  return _bytes == other._bytes;
}

// overload inequality operator
bool Guid::operator!=(const Guid &other) const
{
  return !((*this) == other);
}

// overload comparison operator (required by std::map)
bool Guid::operator<(const Guid &other) const
{
  for(uint32_t n = 0; n < 16; ++n) {
    if (other._bytes[n] > _bytes[n]) {
      return true;
    }
  }

  return false;
}

void Guid::swap(Guid& other) noexcept
{
  _bytes.swap(other._bytes);
}
