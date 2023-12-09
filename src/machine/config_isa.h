#ifndef MACHINE_CONFIG_ISA_H
#define MACHINE_CONFIG_ISA_H

#include <QtGlobal>
#include <QMetaType>

namespace machine {

struct ConfigIsaWord {
    constexpr ConfigIsaWord() : bits(0) {};
    constexpr ConfigIsaWord(const quint64 &abits) : bits(abits) {};
    constexpr ConfigIsaWord(const ConfigIsaWord &isaWord) = default;    //> Copy constructor
    constexpr ConfigIsaWord &operator=(const ConfigIsaWord &isaWord) = default; //> Assign constructor

    constexpr static ConfigIsaWord empty() {return ConfigIsaWord(); };

    constexpr ConfigIsaWord &operator&=(const ConfigIsaWord &isaWord) {
        bits &= isaWord.bits;
        return *this;
    }
    constexpr ConfigIsaWord &operator|=(const ConfigIsaWord &isaWord) {
        bits |= isaWord.bits;
        return *this;
    }
    constexpr ConfigIsaWord operator~() const {
        ConfigIsaWord ans(~bits);
        return ans;
    }
    friend constexpr ConfigIsaWord operator|(ConfigIsaWord lhs, const ConfigIsaWord& rhs)
    {
        lhs |= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }
    friend constexpr ConfigIsaWord operator&(ConfigIsaWord lhs, const ConfigIsaWord& rhs)
    {
        lhs &= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }
    constexpr friend bool operator==(const ConfigIsaWord& lhs, const ConfigIsaWord& rhs) {
        return lhs.bits == rhs.bits;
    }

    static constexpr ConfigIsaWord byChar(char ch) {
        if (ch >= 'A' && ch <= 'Z')
            ch -= 'A';
        else if (ch >= 'a' && ch <= 'z')
            ch -= 'a';
        else
            ch = 0;
        auto abits = static_cast<typeof(bits)>(1) << ch;
        return ConfigIsaWord(abits);
    };

    constexpr bool isEmpty() const {
        return bits == 0;
    };

    constexpr bool contains(char ch) const {
        return !(*this & byChar(ch)).isEmpty();
    };

    constexpr bool contains(ConfigIsaWord &isaWord) const {
        return (*this & isaWord) == isaWord;
    };

    ConfigIsaWord &modify(ConfigIsaWord &mask, ConfigIsaWord &val) {
        (*this) &= ~mask | val;
        (*this) |= mask & val;
        return *this;
    }

    constexpr auto toUnsigned() const {
        return bits;
    };

    quint64 bits;
};

}

Q_DECLARE_METATYPE(machine::ConfigIsaWord)

#endif // MACHINE_CONFIG_ISA_H
