//
// Created by Storm Phoenix on 2021/5/4.
//

#ifndef KAGUYA_XMLPARSEINFO_H
#define KAGUYA_XMLPARSEINFO_H

#include <kaguya/common.h>
#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/core/Transform.h>
#include <kaguya/core/medium/Medium.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace RENDER_NAMESPACE {
    namespace scene {
        namespace importer {

            using namespace core;
            using namespace core::transform;
            using namespace core::medium;
            using namespace material;
            using namespace material::texture;

#define GET_PARSE_INFO_VALUE_FUNC_DECLARE(Type, TypeUpperCase) \
    Type get##TypeUpperCase##Value(const std::string name, const Type defaultValue);                                                               \

#define SET_PARSE_INFO_VALUE_FUNC_DECLARE(Type, TypeUpperCase) \
    void set##TypeUpperCase##Value(const std::string name, const Type value);

            typedef struct XmlAttrVal {
                enum AttrType {
                    Attr_Spectrum,
                    Attr_Bool,
                    Attr_Int,
                    Attr_Float,
                    Attr_Transform,
                    Attr_String,
                    Attr_Vector,
                    Attr_SpectrumTexture,
                    Attr_None,
                } type;

                struct Val {
                    Spectrum spectrumValue;
                    SpectrumTexture::Ptr spectrumTextureValue;
                    bool boolValue;
                    int intValue;
                    Float floatValue;
                    Transform transformValue;
                    std::string stringValue;
                    Vector3F vectorValue;
                } value;

                XmlAttrVal() {}
            } XmlAttrVal;

            typedef struct XmlParseInfo {
                Transform transformMat;
                Material currentMaterial;
                Medium::Ptr currentExteriorMedium = nullptr;
                Medium::Ptr currentInteriorMedium = nullptr;
                bool hasAreaLight = false;

                XmlAttrVal get(std::string name) {
                    return container[name];
                }

                XmlAttrVal::AttrType getType(std::string name) {
                    if (container.count(name) > 0) {
                        return container[name].type;
                    } else {
                        return XmlAttrVal::Attr_None;
                    }
                }

                void set(std::string name, XmlAttrVal value) {
                    container[name] = value;
                }

                bool attrExists(std::string name) {
                    return container.count(name) > 0;
                }

                GET_PARSE_INFO_VALUE_FUNC_DECLARE(SpectrumTexture::Ptr, SpectrumTexture)

                SET_PARSE_INFO_VALUE_FUNC_DECLARE(SpectrumTexture::Ptr, SpectrumTexture)

                GET_PARSE_INFO_VALUE_FUNC_DECLARE(bool, Bool)

                SET_PARSE_INFO_VALUE_FUNC_DECLARE(bool, Bool)

                GET_PARSE_INFO_VALUE_FUNC_DECLARE(int, Int)

                SET_PARSE_INFO_VALUE_FUNC_DECLARE(int, Int)

                GET_PARSE_INFO_VALUE_FUNC_DECLARE(Spectrum, Spectrum)

                SET_PARSE_INFO_VALUE_FUNC_DECLARE(Spectrum, Spectrum)

                GET_PARSE_INFO_VALUE_FUNC_DECLARE(float, Float)

                SET_PARSE_INFO_VALUE_FUNC_DECLARE(float, Float)

                GET_PARSE_INFO_VALUE_FUNC_DECLARE(Transform, Transform)

                SET_PARSE_INFO_VALUE_FUNC_DECLARE(Transform, Transform)

                GET_PARSE_INFO_VALUE_FUNC_DECLARE(std::string, String)

                SET_PARSE_INFO_VALUE_FUNC_DECLARE(std::string, String)

                SET_PARSE_INFO_VALUE_FUNC_DECLARE(Vector3F, Vector)

                GET_PARSE_INFO_VALUE_FUNC_DECLARE(Vector3F, Vector)

            private:
                std::map<std::string, XmlAttrVal> container;
            } XmlParseInfo;

        }
    }
}

#endif //KAGUYA_XMLPARSEINFO_H
