/*
 * Unicode manipulation functions
 *
 * Copyright (C) 2012-2014 Lu Wang <coolwanglu@gmail.com>
 */

#include <iostream>
#include <GlobalParams.h>
#include <map>
#include <unordered_map>
#include <algorithm>

#include "pdf2htmlEX-config.h"
#include "unicode.h"

namespace pdf2htmlEX {

using std::cerr;
using std::endl;
using std::ostream;
using std::unordered_map;
using std::none_of;
using std::string;

const static unordered_map<string, Unicode> ligature_map {
        {"AE", 198}, //Æ
        {"TH", 222}, //Þ
        {"ss", 223}, //ß
        {"ae", 230}, //æ
        {"th", 254}, //þ
        {"IJ", 306}, //Ĳ
        {"ij", 307}, //ĳ
        {"OE", 338}, //Œ
        {"oe", 339}, //œ
        {"hv", 405}, //ƕ
        {"OI", 418}, //Ƣ
        {"oi", 419}, //ƣ
        {"DZ", 452}, //Ǆ
        {"Dz", 453}, //ǅ
        {"dz", 454}, //ǆ
        {"LJ", 455}, //Ǉ
        {"Lj", 456}, //ǈ
        {"lj", 457}, //ǉ
        {"NJ", 458}, //Ǌ
        {"Nj", 459}, //ǋ
        {"nj", 460}, //ǌ
        {"DZ", 497}, //Ǳ
        {"Dz", 498}, //ǲ
        {"dz", 499}, //ǳ
        {"db", 568}, //ȸ
        {"qp", 569}, //ȹ
        {"OE", 630}, //ɶ
        {"dz", 675}, //ʣ
        {"dz", 677}, //ʥ
        {"ts", 678}, //ʦ
        {"ls", 682}, //ʪ
        {"lz", 683}, //ʫ
        {"AE", 7425}, //ᴁ
        {"ue", 7531}, //ᵫ
        {"th", 7546}, //ᵺ
        {"SS", 7838}, //ẞ
        {"LL", 7930}, //Ỻ
        {"ll", 7931}, //ỻ
        {"ff", 64256}, //ﬀ
        {"fi", 64257}, //ﬁ
        {"fl", 64258}, //ﬂ
        {"ffi", 64259}, //ﬃ
        {"ffl", 64260}, //ﬄ
        {"st", 64261}, //ﬅ
        {"st", 64262} //ﬆ
};

Unicode map_to_private(CharCode code)
{
    Unicode private_mapping = (Unicode)(code + 0xE000);
    if(private_mapping > 0xF8FF)
    {
        private_mapping = (Unicode)((private_mapping - 0xF8FF) + 0xF0000);
        if(private_mapping > 0xFFFFD)
        {
            private_mapping = (Unicode)((private_mapping - 0xFFFFD) + 0x100000);
            if(private_mapping > 0x10FFFD)
            {
                cerr << "Warning: all private use unicode are used" << endl;
            }
        }
    }
    return private_mapping;
}

Unicode unicode_from_font (CharCode code, GfxFont * font)
{
    if(!font->isCIDFont())
    {
        char * cname = dynamic_cast<Gfx8BitFont*>(font)->getCharName(code);
        // may be untranslated ligature
        if(cname)
        {
            Unicode ou = globalParams->mapNameToUnicodeText(cname);
            if(!is_illegal_unicode(ou))
                return ou;
        }
    }

    return map_to_private(code);
}

Unicode check_unicode(Unicode * u, int len, CharCode code, GfxFont * font)
{
    if(len == 0)
        return map_to_private(code);

    if(len == 1)
    {
        if(!is_illegal_unicode(*u))
            return *u;
    }

    if((len == 2 || len == 3) && none_of(u, u + len, is_illegal_unicode)) {
        //possible that it's ligature or digraph -> map to unicode char
        string c;
        for(int i = 0; i < len; i++) {
            c += (char) *(u+i);
        }

        unordered_map<string, Unicode>::const_iterator it = ligature_map.find(c);
        if (it != ligature_map.end()) {
            //ligature matched
            return it->second;
        }
        else {
            //map this shit to private zone
            return map_to_private(code);
        }
    }

    return unicode_from_font(code, font);
}

} //namespace pdf2htmlEX
