//
// Created by Storm Phoenix on 2021/4/16.
//

#include <kaguya/Config.h>
#include <kaguya/core/Transform.h>
#include <kaguya/scene/meta/Triangle.h>
#include <kaguya/scene/accumulation/BVH.h>
#include <kaguya/scene/Cube.h>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/material/texture/ConstantTexture.h>
#include <kaguya/material/Lambertian.h>
#include <kaguya/material/Dielectric.h>
#include <kaguya/scene/importer/xml/XmlSceneImporter.h>
#include <kaguya/utils/ObjLoader.h>

namespace kaguya {
    namespace scene {
        namespace importer {

            using namespace utils;
            using namespace scene;
            using namespace material;
            using namespace material::texture;
            using namespace scene::acc;
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

            inline Float toFloat(const std::string &val) {
                char *tmp = nullptr;
#if defined(KAGUYA_DATA_DOUBLE)
                Float v = strtod(val.c_str(), &tmp);
#else
                Float v = strtof(val.c_str(), &tmp);
#endif
                ASSERT(*tmp == '\0', "Can't convert " + val + " to float type");
                return v;
            }

            inline Vector3F toVector(const std::string &val) {
                Vector3F ret;
                char *tmp;
#if defined(KAGUYA_DATA_DOUBLE)
                ret[0] = strtod(val.c_str(), &tmp);
                for (int i = 1; i < 3; i++) {
                    tmp++;
                    ret[i] = strtod(tmp, &tmp);
                }
#else
                ret[0] = strtof(val.c_str(), &tmp);
                for (int i = 1; i < 3; i++) {
                    tmp++;
                    ret[i] = strtof(tmp, &tmp);
                }
#endif
                return ret;
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
                _nodeTypeMap["vector"] = Tag_Vector;
                _nodeTypeMap["lookat"] = Tag_LookAt;
            }

            void handleTagBoolean(pugi::xml_node &node, ParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();
                ASSERT(value == "true" || value == "false", "Can't convert " + value + " to bool type");

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

                AttrVal &attr = parentParseInfo.container[name];
                attr.type = AttrVal::Attr_Integer;
                attr.value.intValue = ret;
            }

            void handleTagFloat(pugi::xml_node &node, ParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();

                Float ret = toFloat(value);
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
                        ret[j][i] = strtod(tmp, &tmp);
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
                        ret[j][i] = strtof(tmp, &tmp);
                    }
                }
#endif
                parentParseInfo.transformMat = transform::Transform(ret);
            }

            void handleTagTransform(pugi::xml_node &node, ParseInfo &parseInfo, ParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();

                AttrVal &attrVal = parentParseInfo.container[name];
                attrVal.type = AttrVal::Attr_Transform;
                attrVal.value.transformValue = parseInfo.transformMat;
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
            }

            void handleTagSampler(pugi::xml_node &node, ParseInfo &parseInfo) {
                Config::Sampler::type = node.attribute("type").value();
                Config::Sampler::sampleCount = parseInfo.container["sampleCount"].value.intValue;
            }

            void XmlSceneImporter::handleTagSensor(pugi::xml_node &node, ParseInfo &parseInfo) {
                Config::Camera::type = node.attribute("type").value();
                ASSERT(Config::Camera::type == "perspective", "Only support perspective camera for now.");

                Float fov = parseInfo.container["fov"].value.floatValue;
                transform::Transform toWorldMat = parseInfo.container["toWorld"].value.transformValue;
                std::shared_ptr<transform::Transform> toWorld = std::make_shared<transform::Transform>(toWorldMat);

                // TODO check near far 属性
                _scene->setCamera(std::make_shared<Camera>(toWorld, fov));
            }

            void XmlSceneImporter::handleTagBSDF(pugi::xml_node &node, ParseInfo &parseInfo, ParseInfo &parentInfo) {
                std::string type = node.attribute("type").value();
                std::string id = node.attribute("id").value();

                Material::Ptr material = nullptr;
                if (type == "diffuse") {
                    material = createDiffuseMaterial(parseInfo);
                } else if (type == "dielectric") {
                    material = createDielectricMaterial(parseInfo);
                } else {
                    // TODO
                    ASSERT(false, "Only support diffuse material for now");
                }

                if (id == "") {
                    parentInfo.currentMaterial = material;
                } else {
                    _materialMap[id] = material;
                }
            }

            Material::Ptr XmlSceneImporter::createDielectricMaterial(ParseInfo &info) {
                Material::Ptr material = nullptr;
                auto intIORType = info.container["intIOR"].type;
                auto extIORType = info.container["extIOR"].type;
                ASSERT(intIORType == AttrVal::Attr_Float && extIORType == AttrVal::Attr_Float,
                       "Only support float type IOR for now");

                // thetaT
                Float intIOR = info.container["intIOR"].value.floatValue;
                // thetaI
                Float extIOR = info.container["extIOR"].value.floatValue;
                // 临时用 1.0 spectrum 代替
                Texture<Spectrum>::Ptr texture = std::make_shared<ConstantTexture<Spectrum>>(1.0);
                material = std::make_shared<Dielectric>(texture, extIOR, intIOR);
                return material;
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
                                                       Vector2F(0, 0), Vector2F(1, 0), Vector2F(1, 1),
                                                       toWorld);

                auto tri2 = std::make_shared<Triangle>(vertices[2], vertices[3], vertices[0], normal, normal, normal,
                                                       Vector2F(1, 1), Vector2F(0, 1), Vector2F(0, 0),
                                                       toWorld);

                std::shared_ptr<std::vector<Shape::Ptr>> ret = std::make_shared<std::vector<Shape::Ptr>>();
                ret->push_back(tri1);
                ret->push_back(tri2);

                return ret;
            }

            std::shared_ptr<std::vector<Shape::Ptr>> XmlSceneImporter::createCubeShape(ParseInfo &info) {
                auto transform = info.container["toWorld"].value.transformValue;
                Cube::Ptr cubePtr = std::make_shared<Cube>(transform.mat());
                return cubePtr->triangles();
            }

            std::shared_ptr<std::vector<Shape::Ptr>> XmlSceneImporter::createObjMeshes(ParseInfo &info) {
                auto transformMat = info.container["toWorld"].value.transformValue;
                Transform::Ptr toWorld = std::make_shared<Transform>(transformMat.mat());

                // face normal
                bool faceNormal = false;
                if (info.container.count("faceNormals") == 1) {
                    faceNormal = info.container["faceNormals"].value.boolValue;
                }

                std::string filename = info.container["filename"].value.stringValue;

                std::vector<Vector3F> vertices;
                std::vector<Normal3F> normals;
                std::vector<Point2F> texcoords;
                std::vector<TriMesh::TriIndex> indics;
                bool good = io::ObjLoader::loadObj(Config::sceneDir + filename, vertices, normals, texcoords, indics);
                ASSERT(good, "Load *.obj model failed: " + filename);
                std::cout << "\tLoading mesh: " << filename << std::endl;

                TriMesh::Ptr tris = std::make_shared<TriangleMesh>(vertices, normals, texcoords, indics, toWorld,
                                                                   faceNormal);
                return tris->triangles();
            }

            void XmlSceneImporter::handleTagShape(pugi::xml_node &node, ParseInfo &parseInfo) {
                std::string type = node.attribute("type").value();
                std::shared_ptr<std::vector<Shape::Ptr>> shapes = nullptr;

                if (type == "rectangle") {
                    shapes = createRectangleShape(parseInfo);
                } else if (type == "cube") {
                    shapes = createCubeShape(parseInfo);
                } else if (type == "obj") {
                    shapes = createObjMeshes(parseInfo);
                } else {
                    ASSERT(false, "Only support rectangle shape for now");
                }

                Material::Ptr material = parseInfo.currentMaterial;
                AreaLight::Ptr light = nullptr;
                if (parseInfo.hasAreaLight) {
                    auto radianceType = parseInfo.container["radiance"].type;
                    if (radianceType == AttrVal::Attr_Spectrum) {
                        Spectrum radiance = parseInfo.container["radiance"].value.spectrumValue;
                        for (auto it = shapes->begin(); it != shapes->end(); it++) {
                            light = std::make_shared<DiffuseAreaLight>(radiance, *it, nullptr, true);
                            _scene->addLight(light);
                        }
                    } else {
                        ASSERT(false, "Only support spectrum radiance for now.");
                    }
                }

                for (auto it = shapes->begin(); it != shapes->end(); it++) {
                    Geometry::Ptr geometry = std::make_shared<Geometry>(*it, material);
                    if (parseInfo.hasAreaLight) {
                        geometry->setAreaLight(light);
                    }
                    _shapes.push_back(geometry);
                }
            }

            void XmlSceneImporter::handleTagRef(pugi::xml_node &node, ParseInfo &info) {
                std::string id = node.attribute("id").value();
                ASSERT(_materialMap.count(id) > 0, "Material " + id + " Not Exists.!");
                info.currentMaterial = _materialMap[id];
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
                    std::string name = node.attribute("name").value();
                    AttrVal &attrVal = parentParseInfo.container[name];
                    attrVal.type = AttrVal::Attr_Spectrum;
                    attrVal.value.spectrumValue = ret;
                }
            }

            void XmlSceneImporter::handleTagVector(pugi::xml_node &node, ParseInfo &parent) {
                std::string name = node.attribute("name").value();
                std::string x_str = node.attribute("x").value();
                std::string y_str = node.attribute("y").value();
                std::string z_str = node.attribute("z").value();

                Float x = toFloat(x_str);
                Float y = toFloat(y_str);
                Float z = toFloat(z_str);

                Vector3F v(x, y, z);
                AttrVal &attrVal = parent.container[name];
                attrVal.type = AttrVal::Attr_Vector;
                attrVal.value.vectorValue = v;
            }

            void XmlSceneImporter::handleTagEmitter(pugi::xml_node &node, ParseInfo &info, ParseInfo &parent) {
                std::string type = node.attribute("type").value();
                if (type == "area") {
                    parent.hasAreaLight = true;
                    auto radianceType = info.container["radiance"].type;
                    if (radianceType == AttrVal::Attr_Spectrum) {
                        parent.container["radiance"] = info.container["radiance"];
                    } else {
                        ASSERT(false, "Only support spectrum type radiance.");
                    }
                    std::cout << "\tCreate light: area light" << std::endl;
                } else if (type == "point") {
                    transform::Transform toWorldMat = info.container["toWorld"].value.transformValue;
                    std::shared_ptr<transform::Transform> toWorld = std::make_shared<transform::Transform>(toWorldMat);
                    Spectrum intensity = info.container["intensity"].value.spectrumValue;
                    Light::Ptr pointLight = std::make_shared<PointLight>(intensity, toWorld, MediumBoundary(nullptr,
                                                                                                            nullptr));
                    _scene->addLight(pointLight);
                    std::cout << "\tCreate light: point light" << std::endl;
                } else if (type == "spot") {
                    transform::Transform toWorldMat = info.container["toWorld"].value.transformValue;
                    std::shared_ptr<transform::Transform> toWorld = std::make_shared<transform::Transform>(toWorldMat);
                    Spectrum intensity = info.container["intensity"].value.spectrumValue;
                    Float totalAngle = info.container["totalAngle"].value.floatValue;
                    Float falloffAngle = info.container["falloffAngle"].value.floatValue;
                    Light::Ptr spotLight = std::make_shared<SpotLight>(
                            intensity, toWorld,
                            MediumBoundary(nullptr, nullptr),
                            falloffAngle, totalAngle);
                    _scene->addLight(spotLight);
                    std::cout << "\tCreate light: spot light" << std::endl;
                } else {
                    ASSERT(false, "Only support area type for now");
                }
            }

            void XmlSceneImporter::handleTagIntegrator(pugi::xml_node &node, ParseInfo &info) {
                std::string type = node.attribute("type").value();
                Config::Tracer::type = type;
                Config::Tracer::maxDepth = info.container["maxDepth"].value.intValue;
                Config::Tracer::sampleNum = info.container["sampleNum"].value.intValue;
                Config::Tracer::delta = info.container["delta"].value.intValue;
                Config::Tracer::strictNormals = info.container["strictNormals"].value.boolValue;
            }

            void XmlSceneImporter::handleTagLookAt(pugi::xml_node &node, ParseInfo &parent) {
                const Vector3F origin = toVector(node.attribute("origin").value());
                const Vector3F target = toVector(node.attribute("target").value());
                const Vector3F up = toVector(node.attribute("up").value());

                Matrix4F mat;
                mat[3][0] = origin[0];
                mat[3][1] = origin[1];
                mat[3][2] = origin[2];
                mat[3][3] = 1;

                Vector3F forward = NORMALIZE(target - origin);
                Vector3F left = CROSS(up, forward);
                Vector3F realUp = CROSS(forward, left);
                mat[0][0] = left[0];
                mat[0][1] = left[1];
                mat[0][2] = left[2];
                mat[0][3] = 0;

                mat[1][0] = realUp[0];
                mat[1][1] = realUp[1];
                mat[1][2] = realUp[2];
                mat[1][3] = 0;

                mat[2][0] = forward[0];
                mat[2][1] = forward[1];
                mat[2][2] = forward[2];
                mat[2][3] = 0;

                parent.transformMat = transform::Transform(mat);
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
                    case Tag_Vector:
                        handleTagVector(node, parentParseInfo);
                        break;
                    case Tag_Transform:
                        handleTagTransform(node, parseInfo, parentParseInfo);
                        break;
                    case Tag_String:
                        handleTagString(node, parentParseInfo);
                        break;
                        /*
                    case Tag_Sampler:
                        handleTagSampler(node, parseInfo);
                        break;
                         */
                    case Tag_Film:
                        handleTagFilm(node, parseInfo);
                        break;
                    case Tag_Sensor:
                        handleTagSensor(node, parseInfo);
                        break;
                    case Tag_BSDF:
                        handleTagBSDF(node, parseInfo, parentParseInfo);
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
                    case Tag_Emitter:
                        handleTagEmitter(node, parseInfo, parentParseInfo);
                        break;
                    case Tag_LookAt:
                        handleTagLookAt(node, parentParseInfo);
                        break;
                    case Tag_Integrator:
                        handleTagIntegrator(node, parseInfo);
                        break;
                    default:
                        std::cout << "\tUnsupported tag: <" << node.name() << ">" << std::endl;
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

            std::shared_ptr<Scene> XmlSceneImporter::importScene(std::string sceneDir) {
                std::string xml_file = sceneDir + "scene.xml";
                std::cout << "Loading scene file: " << xml_file << std::endl;

                pugi::xml_document xml_doc;
                pugi::xml_parse_result ret = xml_doc.load_file(xml_file.c_str());

                ASSERT(ret, "Error while parsing \"" + xml_file + "\": " + ret.description()
                            + " (at " + getOffset(ret.offset, xml_file) + ")");
                _scene = std::make_shared<Scene>();

                ParseInfo parseInfo;
                parseXml(*xml_doc.begin(), parseInfo);

                std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(_shapes);
                _scene->setWorld(bvh);
                _scene->setSceneName(Config::Camera::filename);

                std::cout << "Loading finished. " << std::endl;
                return _scene;
            }
        }
    }
}