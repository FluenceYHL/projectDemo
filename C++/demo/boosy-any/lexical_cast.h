#ifndef LEXICAL_CAST_H
#define LEXICAL_CAST_H

#include <mutex>
#include <string>
#include <cstdio>
#include <cctype>
#include <cstring>

namespace YHL {

    template<typename from, typename to>
    struct convertor {};

    // string to int
    template<typename from>
    struct convertor<from, int> {
        static int convert(const std::string& _from) {
            return std::atoi(_from.c_str());
        }
        static int convert(const char* _from) {
            return std::atoi(_from);
        }
    };

    // string to long
    template<typename from>
    struct convertor<from, long> {
        static long convert(const std::string& _from) {
            return std::atol(_from.c_str());
        }
        static long convert(const char* _from) {
            return std::atol(_from);
        }
    };

    // string to long long
    template<typename from>
    struct convertor<from, long long> {
        static long long convert(const std::string& _from) {
            return std::atoll(_from.c_str());
        }
        static long long convert(const char* _from) {
            return std::atoll(_from);
        }
    };

    // string to double
    template<typename from>
    struct convertor<from, double> {
        static double convert(const std::string& _from) {
            return std::atof(_from.c_str());
        }
        static double convert(const char* _from) {
            return std::atof(_from);
        }
    };

    // string tot float
    template<typename from>
    struct convertor<from, float> {
        static float convert(const std::string& _from) {
            return static_cast<float>(std::atof(_from.c_str()));
        }
        static float convert(const char* _from) {
            return static_cast<float>(std::atof(_from));
        }
    };

    // int to bool
    template<typename from>
    struct convertor<from, bool> {
        static bool convert(const from& _from) {
            return std::is_integral<from>::value and !!_from;
        }
    };

    struct helper {
        static bool convert_to_bool(const char* from, const int len) {
            if(len not_eq 4 and len not_eq 5)
                throw std::logic_error("invalid from\n");
            if(len == 4) {
                static const char* pattern = "true";
                for(int i = 0;i < 4; ++i)
                    if(from[i] not_eq pattern[i] and
                            from[i] not_eq std::toupper(pattern[i]))
                        return false;
                return true;
            }
            static const char* pattern = "false";  // constexpr char *
            if(strcmp(pattern, from) == 0)
                return false;
            throw std::logic_error("invalid from of false");
        }
    };

    // string to bool
    template< >
    struct convertor<std::string, bool> {
        static bool convert(const std::string& from) {
            return helper::convert_to_bool(from.c_str(), from.size());
        }
    };

    // char* to bool
    template< >
    struct convertor<char*, bool> {
        static bool convert(char *from) {
            return helper::convert_to_bool(from, strlen(from));
        }
    };

    // const char* to bool
    template< >
    struct convertor<const char*, bool> {
        static bool convert(const char *from) {
            return helper::convert_to_bool(from, strlen(from));
        }
    };

    // integer to string
    template<typename from>
    struct convertor<std::string, from> {
        static std::string convert(const from& _from) {
            return std::to_string(_from);
        }
    };

    template<typename from, typename to>
    inline typename std::enable_if<std::is_same<from, to>::value, from>::type lexicast(const from& _from) {
        return _from;
    }

    template<typename from, typename to>
    inline typename std::enable_if<!std::is_same<from, to>::value, to>::type lexicast(const from& _from) {
        return convertor<from, to>::convert(_from);
    }

}

#endif // LEXICAL_CAST_H
