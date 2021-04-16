//
// Created by Storm Phoenix on 2021/4/16.
//

#include <kaguya/Config.h>
#include <kaguya/core/Transform.h>
#include <kaguya/scene/meta/Triangle.h>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/material/texture/ConstantTexture.h>
#include <kaguya/material/Lambertian.h>
#include <kaguya/scene/importer/xml/XmlSceneImporter.h>

namespace kaguya {
    namespace scene {
        namespace importer {

            using namespace material;
            using namespace material::texture;
            using namespace scene::meta;
            using namespace core::transform;

            std::string getOffset(long pos, std::string xml_file) {
                std::fstream is(xml_file);
                char buffer[1024];
                int line = 0, linestart = 0, offset = 0;
                while (is.good()) {
                    is.read(buffer, sizeof(buffer));
                    for (int i = 0; i < is.gcount(); ++i) {
                        if (buffer[i] == '\n') {
                            if (offset + i >= pos) {
                                std::stringstream ss;
                                std::string ret;
                                ss << "line " << line + 1 << ", col " << pos - linestart;
                                ss >> ret;
                                return ret;
                            }
                            ++line;
                            linestart = offset + i;
                        }
                    }
                    offset += (int) is.gcount();
                }
                return "byte offset " + std::to_string(pos);
            }

            XmlSceneImporter::XmlSceneImporter() {
                _nodeTypeMap["mode"] = Tag_Mode;
                _nodeTypeMap["scene"] = Tag_Scene;

                _nodeTypeMap["integrator"] = Tag_Integrator;
                _nodeTypeMap["sensor"] = Tag_Sensor;
                _nodeTypeMap["camera"] = Tag_Sensor;
                _nodeTypeMap["sampler"] = Tag_Sampler;
                _nodeTypeMap["film"] = Tag_Film;
                _nodeTypeMap["rfilter"] = Tag_Rfilter;
                _nodeTypeMap["emitter"] = Tag_Emitter;
                _nodeTypeMap["shape"] = Tag_Shape;
                _nodeTypeMap["bsdf"] = Tag_BSDF;
                _nodeTypeMap["ref"] = Tag_Ref;

                _nodeTypeMap["bool"] = Tag_Boolean;
                _nodeTypeMap["boolean"] = Tag_Boolean;
                _nodeTypeMap["integer"] = Tag_Integer;
                _nodeTypeMap["float"] = Tag_Float;
                _nodeTypeMap["string"] = Tag_String;
                _nodeTypeMap["rgb"] = Tag_RGB;
                _nodeTypeMap["transform"] = Tag_Transform;
                _nodeTypeMap["matrix"] = Tag_Matrix;
            }

            void handleTagBoolean(pugi::xml_node &node, ParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();


                ASSERT(value == "true" || value == "false", "Can't convert " + value + " to bool type");

                std::cout << "Handel bool value: " + name + " " + value << std::endl;

                AttrVal &attr = parentParseInfo.container[name];
                attr.type = AttrVal::Attr_Boolean;
                if (value == "true") {
                    attr.value.boolValue = true;
                } else {
                    attr.value.boolValue = false;
                }
            }

            void handleTagString(pugi::xml_node &node, ParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();

                std::cout << "Handel string value: " + name + " " + value << std::endl;

                AttrVal &attr = parentParseInfo.container[name];
                attr.type = AttrVal::Attr_String;
                attr.value.stringValue = value;
            }

            void handleTagInteger(pugi::xml_node &node, ParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();

                char *tmp = nullptr;
                int ret = strtol(value.c_str(), &tmp, 10);

                ASSERT(*tmp == '\0', "Can't convert " + value + " to int type");

                std::cout << "Handel int value: " + name + " " + value << std::endl;

                AttrVal &attr = parentParseInfo.container[name];
                attr.type = AttrVal::Attr_Integer;
                attr.value.intValue = ret;
            }

            void handleTagFloat(pugi::xml_node &node, ParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();

                char *tmp = nullptr;
#if defined(KAGUYA_DATA_DOUBLE)
                Float ret = strtod(value.c_str(), &tmp);
#else
                Float ret = strtof(value.c_str(), &tmp);
#endif

                ASSERT(*tmp == '\0', "Can't convert " + value + " to float type");

                std::cout << "Handel float value: " + name + " " + value << std::endl;

                AttrVal &attr = parentParseInfo.container[name];
                attr.type = AttrVal::Attr_Float;
                attr.value.floatValue = ret;
            }

            void handleTagMatrix(pugi::xml_node &node, ParseInfo &parentParseInfo) {
                std::string value = node.attribute("value").value();

                Matrix4F ret;
                char *tmp = nullptr;
#if defined(KAGUYA_DATA_DOUBLE)
                ret[0][0] = strtod(value.c_str(), &tmp);
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4;j ++) {
                        if (i == 0 && j == 0) {
                            continue;
                        }
                        tmp++;
                        ret[i][j] = strtod(tmp, &tmp);
                    }
                }
#else
                ret[0][0] = strtof(value.c_str(), &tmp);
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (i == 0 && j == 0) {
                            continue;
                        }
                        tmp++;
                        ret[i][j] = strtof(tmp, &tmp);
                    }
                }
#endif
                std::cout << "Handel matrix " << std::endl;
                parentParseInfo.transformMat = transform::Transform(ret);
            }

            void handleTagTransform(pugi::xml_node &node, ParseInfo &parseInfo, ParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();

                AttrVal &attrVal = parentParseInfo.container[name];
                attrVal.type = AttrVal::Attr_Transform;
                attrVal.value.transformValue = parseInfo.transformMat;

                std::cout << "Handel transform value: " + name << std::endl;
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        std::cout << attrVal.value.transformValue.mat()[i][j] << " ";
                    }
                    std::cout << std::endl;
                }
            }

            void handleTagFilm(pugi::xml_node &node, ParseInfo &parseInfo) {
                std::string type = node.attribute("type").value();
                Config::Camera::filmType = type;
                Config::Camera::filename = parseInfo.container["filename"].value.stringValue;
                Config::Camera::width = parseInfo.container["width"].value.intValue;
                Config::Camera::height = parseInfo.container["height"].value.intValue;
                Config::Camera::fileFormat = parseInfo.container["fileFormat"].value.stringValue;
                Config::Camera::pixelFormat = parseInfo.container["pixelFormat"].value.stringValue;
                Config::Camera::gamma = parseInfo.container["gamma"].value.floatValue;
                Config::Camera::banner = parseInfo.container["banner"].value.boolValue;
                Config::Camera::rfilter = parseInfo.container["rfilter"].value.stringValue;

                std::cout << "Handel film value: " << std::endl;
            }

            void handleTagSampler(pugi::xml_node &node, ParseInfo &parseInfo) {
                Config::Sampler::type = node.attribute("type").value();
                Config::Sampler::sampleCount = parseInfo.container["sampleCount"].value.intValue;
                std::cout << "Handel sampler value: " << std::endl;
            }

            void XmlSceneImporter::handleTagSensor(pugi::xml_node &node, ParseInfo &parseInfo) {
                Config::Camera::type = node.attribute("type").value();
                Float fov = parseInfo.container["fov"].value.floatValue;
                transform::Transform toWorldMat = parseInfo.container["toWorld"].value.transformValue;
                std::shared_ptr<transform::Transform> toWorld = std::make_shared<transform::Transform>(toWorldMat);

                // TODO check near far 属性
                _scene->setCamera(std::make_shared<Camera>(toWorld, fov));
            }

            void XmlSceneImporter::handleTagBSDF(pugi::xml_node &node, ParseInfo &parseInfo) {
                std::string type = node.attribute("type").value();
                std::string id = node.attribute("id").value();

                Material::Ptr material = nullptr;
                if (type == "diffuse") {
                    material = createDiffuseMaterial(parseInfo);
                } else {
                    // TODO
                    ASSERT(type == "diffuse", "Only support diffuse material for now");
                }
                _materialMap[id] = material;
            }

            Material::Ptr XmlSceneImporter::createDiffuseMaterial(ParseInfo &info) {
                Material::Ptr material = nullptr;

                auto type = info.container["reflectance"].type;
                if (type == AttrVal::Attr_Spectrum) {
                    Spectrum albedo = info.container["reflectance"].value.spectrumValue;
                    Texture<Spectrum>::Ptr texture = std::make_shared<ConstantTexture<Spectrum>>(albedo);
                    material = std::make_shared<Lambertian>(texture);
                } else {
                    // TODO
                    ASSERT(type == AttrVal::Attr_Spectrum, "Only support spectrum reflectance for now.");
                }
                return material;
            }

            std::shared_ptr<std::vector<Shape::Ptr>> XmlSceneImporter::createRectangleShape(ParseInfo &info) {
                auto transformMat = info.container["toWorld"].value.transformValue;
                Transform::Ptr toWorld = std::make_shared<Transform>(transformMat.mat());

                const int vertexNum = 4;
                const int triangleNum = 2;
                std::vector<Point3F> vertices;
                vertices.resize(vertexNum);

                Normal3F normal(0, 0, 1);

                vertices[0] = Point3F(-1, -1, 0);
                vertices[1] = Point3F(1, -1, 0);
                vertices[2] = Point3F(1, 1, 0);
                vertices[3] = Point3F(-1, 1, 0);

                auto tri1 = std::make_shared<Triangle>(vertices[0], vertices[1], vertices[2], normal, normal, normal,
                                                       Vector2F(0, 0), Vector2F(1, 0), Vector2F(1, 1));

                auto tri2 = std::make_shared<Triangle>(vertices[2], vertices[3], vertices[0], normal, normal, normal,
                                                       Vector2F(1, 1), Vector2F(0, 1), Vector2F(0, 0));

                std::shared_ptr<std::vector<Shape::Ptr>> ret = std::make_shared<std::vector<Shape::Ptr>>();
                ret->push_back(tri1);
                ret->push_back(tri2);

                return ret;
            }

            void XmlSceneImporter::handleTagShape(pugi::xml_node &node, ParseInfo &parseInfo) {
                std::string type = node.attribute("type").value();
                std::shared_ptr<std::vector<Shape::Ptr>> shapes = nullptr;

                if (type == "rectangle") {
                    shapes = createRectangleShape(parseInfo);
                } else {
                    ASSERT(type == "rectangle", "Only support rectangle shape for now");
                }

                Material::Ptr material = nullptr;
                if (_materialMap.count(parseInfo.materialId) > 0) {
                    material = _materialMap[parseInfo.materialId];
                }

                for (auto it = shapes->begin(); it != shapes->begin(); it++) {
                    auto geometry = std::make_shared<Geometry>(*it, material);
                    _geometries->push_back(geometry);
                }

                // TODO light
            }

            void XmlSceneImporter::handleTagRef(pugi::xml_node &node, ParseInfo &info) {
                std::string id = node.attribute("id").value();
                ASSERT(_materialMap.count(id) > 0, "Material " + id + " Not Exists.!");
                info.materialId = id;
            }

            void XmlSceneImporter::handleTagRGB(pugi::xml_node &node, ParseInfo &parentParseInfo) {
                if (strcmp(node.name(), "spectrum") == 0) {
                    // TODO: Fix Spectrum declared with wavelength
                    ASSERT(false, "No implemented!");
                } else if (strcmp(node.name(), "rgb") == 0 || strcmp(node.name(), "color") == 0) {
                    Spectrum ret;
                    std::string colorValue = node.attribute("value").value();
                    char *tmp;
#if defined(KAGUYA_DATA_DOUBLE)
                    ret[0] = strtod(colorValue.c_str(), &tmp);
                    for (int i = 1; i < 3; i++) {
                        tmp++;
                        ret[i] = strtod(tmp, &tmp);
                    }
#else
                    ret[0] = strtof(colorValue.c_str(), &tmp);
                    for (int i = 1; i < 3; i++) {
                        tmp++;
                        ret[i] = strtof(tmp, &tmp);
                    }
#endif
                    AttrVal &attrVal = parentParseInfo.container["reflectance"];
                    attrVal.type = AttrVal::Attr_Spectrum;
                    attrVal.value.spectrumValue = ret;
                }
            }

            void XmlSceneImporter::handleXmlNode(pugi::xml_node &node,
                                                 ParseInfo &parseInfo,
                                                 ParseInfo &parentParseInfo) {
                TagType tagType = _nodeTypeMap[node.name()];
                switch (tagType) {
                    case Tag_Boolean:
                        handleTagBoolean(node, parentParseInfo);
                        break;
                    case Tag_Integer:
                        handleTagInteger(node, parentParseInfo);
                        break;
                    case Tag_Float:
                        handleTagFloat(node, parentParseInfo);
                        break;
                    case Tag_Matrix:
                        handleTagMatrix(node, parentParseInfo);
                        break;
                    case Tag_Transform:
                        handleTagTransform(node, parseInfo, parentParseInfo);
                        break;
                    case Tag_String:
                        handleTagString(node, parentParseInfo);
                        break;
                    case Tag_Sampler:
                        handleTagSampler(node, parseInfo);
                        break;
                    case Tag_Film:
                        handleTagFilm(node, parseInfo);
                        break;
                    case Tag_Sensor:
                        handleTagSensor(node, parseInfo);
                        break;
                    case Tag_BSDF:
                        handleTagBSDF(node, parseInfo);
                        break;
                    case Tag_Shape:
                        handleTagShape(node, parseInfo);
                        break;
                    case Tag_Ref:
                        handleTagRef(node, parentParseInfo);
                        break;
                    case Tag_RGB:
                        handleTagRGB(node, parentParseInfo);
                        break;
                    default:
                        std::cout << "Unsupport tag type: " << tagType << std::endl;
                }
            }

            void XmlSceneImporter::parseXml(pugi::xml_node &node, ParseInfo &parentParseInfo) {
                ParseInfo parseInfo;
                std::map<std::string, AttrVal> attrContainer;
                for (pugi::xml_node &child : node.children()) {
                    parseXml(child, parseInfo);
                }

                handleXmlNode(node, parseInfo, parentParseInfo);
            }

            std::shared_ptr<Scene> XmlSceneImporter::importScene(std::string xml_file) {
                pugi::xml_document xml_doc;
                pugi::xml_parse_result ret = xml_doc.load_file(xml_file.c_str());

                ASSERT(ret, "Error while parsing \"" + xml_file + "\": " + ret.description()
                            + " (at " + getOffset(ret.offset, xml_file) + ")");

                _scene = std::make_shared<Scene>();

                ParseInfo parseInfo;
                parseXml(*xml_doc.begin(), parseInfo);

                return nullptr;
            }
        }
    }
}