//
// Created by Storm Phoenix on 2021/5/4.
//

#include <kaguya/scene/importer/xml/XmlParseInfo.h>

namespace kaguya {
    namespace scene {
        namespace importer {

#define GET_PARSE_INFO_VALUE_FUNC_DEFINE(Type, TypeUpperCase, TypeLowerCase) \
    Type XmlParseInfo::get##TypeUpperCase##Value(const std::string name, const Type defaultValue) { \
        if (container.count(name) > 0) {          \
            return container[name].value.TypeLowerCase##Value;                                        \
        } else {                                  \
            return defaultValue;                                          \
        }                                         \
    }

#define SET_PARSE_INFO_VALUE_FUNC_DEFINE(Type, TypeUpperCase, TypeLowerCase) \
    void XmlParseInfo::set##TypeUpperCase##Value(const std::string name, const Type value) {\
        XmlAttrVal &attrVal = container[name];               \
        attrVal.type = XmlAttrVal::Attr_##TypeUpperCase;                     \
        attrVal.value.TypeLowerCase##Value = value;                          \
    }

            GET_PARSE_INFO_VALUE_FUNC_DEFINE(bool, Bool, bool);

            SET_PARSE_INFO_VALUE_FUNC_DEFINE(bool, Bool, bool);

            GET_PARSE_INFO_VALUE_FUNC_DEFINE(int, Int, int);

            SET_PARSE_INFO_VALUE_FUNC_DEFINE(int, Int, int);

            GET_PARSE_INFO_VALUE_FUNC_DEFINE(Spectrum, Spectrum, spectrum);

            SET_PARSE_INFO_VALUE_FUNC_DEFINE(Spectrum, Spectrum, spectrum);

            GET_PARSE_INFO_VALUE_FUNC_DEFINE(float, Float, float);

            SET_PARSE_INFO_VALUE_FUNC_DEFINE(float, Float, float);

            GET_PARSE_INFO_VALUE_FUNC_DEFINE(Transform, Transform, transform);

            SET_PARSE_INFO_VALUE_FUNC_DEFINE(Transform, Transform, transform);

            GET_PARSE_INFO_VALUE_FUNC_DEFINE(std::string, String, string);

            SET_PARSE_INFO_VALUE_FUNC_DEFINE(std::string, String, string);

            SET_PARSE_INFO_VALUE_FUNC_DEFINE(Vector3F, Vector, vector);

            GET_PARSE_INFO_VALUE_FUNC_DEFINE(SpectrumTexture::Ptr, SpectrumTexture, spectrumTexture);

            SET_PARSE_INFO_VALUE_FUNC_DEFINE(SpectrumTexture::Ptr, SpectrumTexture, spectrumTexture);
        }
    }
}