#pragma once

#include <cassert>
#include "Traits.hpp"
#include "Math.hpp"

namespace Core
{
    template<UnsignedIntegral I>
    constexpr Bool IsPowerOf2(I value) noexcept
    {
        if(value == 0)
            return false;

        //0b100 -> 0b011
        //0b101 -> 0b100
        return (value & (value - 1)) == 0;
    }

    template<UnsignedIntegral I>
    constexpr I GetPowerOf2(I value) noexcept
    {
        if(IsPowerOf2(value))
            return value;

        I output = 0;
        while(value != 0)
        {
            output <<= 1;
            output |= 0b1;

            value >>= 1;
        }

        return output + 1;
    }

    template<UnsignedIntegral I>
    constexpr Bool Align(I& size, I alignment) noexcept
    {
        assert(alignment != 0);

        if(alignment >= size)
        {
            size = alignment;
            return true;
        }

        if(size % alignment == 0)
            return true;

        auto div = size / alignment;
        if(!SaturatingAdd(div, static_cast<decltype(div)>(1), div))
            return false;

        return SaturatingMul(div, alignment, size);
    }

    template<UnsignedIntegral I>
    constexpr Bool AlignAddress(UInt8*& address, I buffer_size, I required_alignment) noexcept
    {
        assert(required_alignment != 0);

        DeviceSize int_address = *reinterpret_cast<DeviceSize*>(address);
        auto rem = int_address % required_alignment;
        if(rem > buffer_size)
            return false;

        int_address += rem;
        address = reinterpret_cast<UInt8*>(int_address);

        return true;
    }

    template<UnsignedIntegral I>
    constexpr Bool IsAlignedAddress(UInt8* address, I alignment) noexcept
    {
        assert(alignment != 0);

        DeviceSize int_address = *reinterpret_cast<DeviceSize*>(address);

        return int_address % alignment == 0;
    }

    template<UnsignedIntegral I>
    constexpr DeviceSize CountBits(I value) noexcept
    {
        DeviceSize result = 0;
        while(value != 0)
        {
            switch(value & 0b1111'1111)
            {
                case 0:
                    result += 0;
                    break;
                case 1:
                    result += 1;
                    break;
                case 2:
                    result += 1;
                    break;
                case 3:
                    result += 2;
                    break;
                case 4:
                    result += 1;
                    break;
                case 5:
                    result += 2;
                    break;
                case 6:
                    result += 2;
                    break;
                case 7:
                    result += 3;
                    break;
                case 8:
                    result += 1;
                    break;
                case 9:
                    result += 2;
                    break;
                case 10:
                    result += 2;
                    break;
                case 11:
                    result += 3;
                    break;
                case 12:
                    result += 2;
                    break;
                case 13:
                    result += 3;
                    break;
                case 14:
                    result += 3;
                    break;
                case 15:
                    result += 4;
                    break;
                case 16:
                    result += 1;
                    break;
                case 17:
                    result += 2;
                    break;
                case 18:
                    result += 2;
                    break;
                case 19:
                    result += 3;
                    break;
                case 20:
                    result += 2;
                    break;
                case 21:
                    result += 3;
                    break;
                case 22:
                    result += 3;
                    break;
                case 23:
                    result += 4;
                    break;
                case 24:
                    result += 2;
                    break;
                case 25:
                    result += 3;
                    break;
                case 26:
                    result += 3;
                    break;
                case 27:
                    result += 4;
                    break;
                case 28:
                    result += 3;
                    break;
                case 29:
                    result += 4;
                    break;
                case 30:
                    result += 4;
                    break;
                case 31:
                    result += 5;
                    break;
                case 32:
                    result += 1;
                    break;
                case 33:
                    result += 2;
                    break;
                case 34:
                    result += 2;
                    break;
                case 35:
                    result += 3;
                    break;
                case 36:
                    result += 2;
                    break;
                case 37:
                    result += 3;
                    break;
                case 38:
                    result += 3;
                    break;
                case 39:
                    result += 4;
                    break;
                case 40:
                    result += 2;
                    break;
                case 41:
                    result += 3;
                    break;
                case 42:
                    result += 3;
                    break;
                case 43:
                    result += 4;
                    break;
                case 44:
                    result += 3;
                    break;
                case 45:
                    result += 4;
                    break;
                case 46:
                    result += 4;
                    break;
                case 47:
                    result += 5;
                    break;
                case 48:
                    result += 2;
                    break;
                case 49:
                    result += 3;
                    break;
                case 50:
                    result += 3;
                    break;
                case 51:
                    result += 4;
                    break;
                case 52:
                    result += 3;
                    break;
                case 53:
                    result += 4;
                    break;
                case 54:
                    result += 4;
                    break;
                case 55:
                    result += 5;
                    break;
                case 56:
                    result += 3;
                    break;
                case 57:
                    result += 4;
                    break;
                case 58:
                    result += 4;
                    break;
                case 59:
                    result += 5;
                    break;
                case 60:
                    result += 4;
                    break;
                case 61:
                    result += 5;
                    break;
                case 62:
                    result += 5;
                    break;
                case 63:
                    result += 6;
                    break;
                case 64:
                    result += 1;
                    break;
                case 65:
                    result += 2;
                    break;
                case 66:
                    result += 2;
                    break;
                case 67:
                    result += 3;
                    break;
                case 68:
                    result += 2;
                    break;
                case 69:
                    result += 3;
                    break;
                case 70:
                    result += 3;
                    break;
                case 71:
                    result += 4;
                    break;
                case 72:
                    result += 2;
                    break;
                case 73:
                    result += 3;
                    break;
                case 74:
                    result += 3;
                    break;
                case 75:
                    result += 4;
                    break;
                case 76:
                    result += 3;
                    break;
                case 77:
                    result += 4;
                    break;
                case 78:
                    result += 4;
                    break;
                case 79:
                    result += 5;
                    break;
                case 80:
                    result += 2;
                    break;
                case 81:
                    result += 3;
                    break;
                case 82:
                    result += 3;
                    break;
                case 83:
                    result += 4;
                    break;
                case 84:
                    result += 3;
                    break;
                case 85:
                    result += 4;
                    break;
                case 86:
                    result += 4;
                    break;
                case 87:
                    result += 5;
                    break;
                case 88:
                    result += 3;
                    break;
                case 89:
                    result += 4;
                    break;
                case 90:
                    result += 4;
                    break;
                case 91:
                    result += 5;
                    break;
                case 92:
                    result += 4;
                    break;
                case 93:
                    result += 5;
                    break;
                case 94:
                    result += 5;
                    break;
                case 95:
                    result += 6;
                    break;
                case 96:
                    result += 2;
                    break;
                case 97:
                    result += 3;
                    break;
                case 98:
                    result += 3;
                    break;
                case 99:
                    result += 4;
                    break;
                case 100:
                    result += 3;
                    break;
                case 101:
                    result += 4;
                    break;
                case 102:
                    result += 4;
                    break;
                case 103:
                    result += 5;
                    break;
                case 104:
                    result += 3;
                    break;
                case 105:
                    result += 4;
                    break;
                case 106:
                    result += 4;
                    break;
                case 107:
                    result += 5;
                    break;
                case 108:
                    result += 4;
                    break;
                case 109:
                    result += 5;
                    break;
                case 110:
                    result += 5;
                    break;
                case 111:
                    result += 6;
                    break;
                case 112:
                    result += 3;
                    break;
                case 113:
                    result += 4;
                    break;
                case 114:
                    result += 4;
                    break;
                case 115:
                    result += 5;
                    break;
                case 116:
                    result += 4;
                    break;
                case 117:
                    result += 5;
                    break;
                case 118:
                    result += 5;
                    break;
                case 119:
                    result += 6;
                    break;
                case 120:
                    result += 4;
                    break;
                case 121:
                    result += 5;
                    break;
                case 122:
                    result += 5;
                    break;
                case 123:
                    result += 6;
                    break;
                case 124:
                    result += 5;
                    break;
                case 125:
                    result += 6;
                    break;
                case 126:
                    result += 6;
                    break;
                case 127:
                    result += 7;
                    break;
                case 128:
                    result += 1;
                    break;
                case 129:
                    result += 2;
                    break;
                case 130:
                    result += 2;
                    break;
                case 131:
                    result += 3;
                    break;
                case 132:
                    result += 2;
                    break;
                case 133:
                    result += 3;
                    break;
                case 134:
                    result += 3;
                    break;
                case 135:
                    result += 4;
                    break;
                case 136:
                    result += 2;
                    break;
                case 137:
                    result += 3;
                    break;
                case 138:
                    result += 3;
                    break;
                case 139:
                    result += 4;
                    break;
                case 140:
                    result += 3;
                    break;
                case 141:
                    result += 4;
                    break;
                case 142:
                    result += 4;
                    break;
                case 143:
                    result += 5;
                    break;
                case 144:
                    result += 2;
                    break;
                case 145:
                    result += 3;
                    break;
                case 146:
                    result += 3;
                    break;
                case 147:
                    result += 4;
                    break;
                case 148:
                    result += 3;
                    break;
                case 149:
                    result += 4;
                    break;
                case 150:
                    result += 4;
                    break;
                case 151:
                    result += 5;
                    break;
                case 152:
                    result += 3;
                    break;
                case 153:
                    result += 4;
                    break;
                case 154:
                    result += 4;
                    break;
                case 155:
                    result += 5;
                    break;
                case 156:
                    result += 4;
                    break;
                case 157:
                    result += 5;
                    break;
                case 158:
                    result += 5;
                    break;
                case 159:
                    result += 6;
                    break;
                case 160:
                    result += 2;
                    break;
                case 161:
                    result += 3;
                    break;
                case 162:
                    result += 3;
                    break;
                case 163:
                    result += 4;
                    break;
                case 164:
                    result += 3;
                    break;
                case 165:
                    result += 4;
                    break;
                case 166:
                    result += 4;
                    break;
                case 167:
                    result += 5;
                    break;
                case 168:
                    result += 3;
                    break;
                case 169:
                    result += 4;
                    break;
                case 170:
                    result += 4;
                    break;
                case 171:
                    result += 5;
                    break;
                case 172:
                    result += 4;
                    break;
                case 173:
                    result += 5;
                    break;
                case 174:
                    result += 5;
                    break;
                case 175:
                    result += 6;
                    break;
                case 176:
                    result += 3;
                    break;
                case 177:
                    result += 4;
                    break;
                case 178:
                    result += 4;
                    break;
                case 179:
                    result += 5;
                    break;
                case 180:
                    result += 4;
                    break;
                case 181:
                    result += 5;
                    break;
                case 182:
                    result += 5;
                    break;
                case 183:
                    result += 6;
                    break;
                case 184:
                    result += 4;
                    break;
                case 185:
                    result += 5;
                    break;
                case 186:
                    result += 5;
                    break;
                case 187:
                    result += 6;
                    break;
                case 188:
                    result += 5;
                    break;
                case 189:
                    result += 6;
                    break;
                case 190:
                    result += 6;
                    break;
                case 191:
                    result += 7;
                    break;
                case 192:
                    result += 2;
                    break;
                case 193:
                    result += 3;
                    break;
                case 194:
                    result += 3;
                    break;
                case 195:
                    result += 4;
                    break;
                case 196:
                    result += 3;
                    break;
                case 197:
                    result += 4;
                    break;
                case 198:
                    result += 4;
                    break;
                case 199:
                    result += 5;
                    break;
                case 200:
                    result += 3;
                    break;
                case 201:
                    result += 4;
                    break;
                case 202:
                    result += 4;
                    break;
                case 203:
                    result += 5;
                    break;
                case 204:
                    result += 4;
                    break;
                case 205:
                    result += 5;
                    break;
                case 206:
                    result += 5;
                    break;
                case 207:
                    result += 6;
                    break;
                case 208:
                    result += 3;
                    break;
                case 209:
                    result += 4;
                    break;
                case 210:
                    result += 4;
                    break;
                case 211:
                    result += 5;
                    break;
                case 212:
                    result += 4;
                    break;
                case 213:
                    result += 5;
                    break;
                case 214:
                    result += 5;
                    break;
                case 215:
                    result += 6;
                    break;
                case 216:
                    result += 4;
                    break;
                case 217:
                    result += 5;
                    break;
                case 218:
                    result += 5;
                    break;
                case 219:
                    result += 6;
                    break;
                case 220:
                    result += 5;
                    break;
                case 221:
                    result += 6;
                    break;
                case 222:
                    result += 6;
                    break;
                case 223:
                    result += 7;
                    break;
                case 224:
                    result += 3;
                    break;
                case 225:
                    result += 4;
                    break;
                case 226:
                    result += 4;
                    break;
                case 227:
                    result += 5;
                    break;
                case 228:
                    result += 4;
                    break;
                case 229:
                    result += 5;
                    break;
                case 230:
                    result += 5;
                    break;
                case 231:
                    result += 6;
                    break;
                case 232:
                    result += 4;
                    break;
                case 233:
                    result += 5;
                    break;
                case 234:
                    result += 5;
                    break;
                case 235:
                    result += 6;
                    break;
                case 236:
                    result += 5;
                    break;
                case 237:
                    result += 6;
                    break;
                case 238:
                    result += 6;
                    break;
                case 239:
                    result += 7;
                    break;
                case 240:
                    result += 4;
                    break;
                case 241:
                    result += 5;
                    break;
                case 242:
                    result += 5;
                    break;
                case 243:
                    result += 6;
                    break;
                case 244:
                    result += 5;
                    break;
                case 245:
                    result += 6;
                    break;
                case 246:
                    result += 6;
                    break;
                case 247:
                    result += 7;
                    break;
                case 248:
                    result += 5;
                    break;
                case 249:
                    result += 6;
                    break;
                case 250:
                    result += 6;
                    break;
                case 251:
                    result += 7;
                    break;
                case 252:
                    result += 6;
                    break;
                case 253:
                    result += 7;
                    break;
                case 254:
                    result += 7;
                    break;
                case 255:
                    result += 8;
                    break;
            }

            value >>= 8;
        }

        return result;
    }
};