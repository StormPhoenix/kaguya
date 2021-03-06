//
// Created by Storm Phoenix on 2021/4/16.
//

#include <kaguya/Config.h>
#include <kaguya/core/Transform.h>
#include <kaguya/scene/meta/Triangle.h>
#include <kaguya/scene/accumulation/BVH.h>
#include <kaguya/scene/Cube.h>
#include <kaguya/core/light/SunLight.h>
#include <kaguya/core/light/EnvironmentLight.h>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/material/texture/ImageTexture.h>
#include <kaguya/material/texture/ConstantTexture.h>
#include <kaguya/material/texture/ChessboardTexture.h>
#include <kaguya/material/PatinaMaterial.h>
#include <kaguya/material/PlasticMaterial.h>
#include <kaguya/material/Lambertian.h>
#include <kaguya/material/Dielectric.h>
#include <kaguya/material/Mirror.h>
#include <kaguya/material/Metal.h>
#include <kaguya/core/medium/IsotropicMedium.h>
#include <kaguya/scene/importer/xml/XmlSceneImporter.h>
#include <kaguya/utils/ObjLoader.h>
#include <kaguya/tracer/PathRecorder.h>

namespace kaguya {
    namespace scene {
        namespace importer {

            using namespace utils;
            using namespace scene;
            using namespace material;
            using namespace material::texture;
            using namespace core::medium;
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
#if defined(RENDER_DATA_DOUBLE)
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
#if defined(RENDER_DATA_DOUBLE)
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

                _nodeTypeMap["texture"] = Tag_Texture;
                _nodeTypeMap["medium"] = Tag_Medium;
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

            void handleTagBoolean(pugi::xml_node &node, XmlParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();
                ASSERT(value == "true" || value == "false", "Can't convert " + value + " to bool type");
                parentParseInfo.setBoolValue(name, value == "true" ? true : false);
            }

            void handleTagString(pugi::xml_node &node, XmlParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();

                parentParseInfo.setStringValue(name, value);
            }

            void handleTagInteger(pugi::xml_node &node, XmlParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();

                char *tmp = nullptr;
                int ret = strtol(value.c_str(), &tmp, 10);
                ASSERT(*tmp == '\0', "Can't convert " + value + " to int type");

                parentParseInfo.setIntValue(name, ret);
            }

            void handleTagFloat(pugi::xml_node &node, XmlParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                std::string value = node.attribute("value").value();

                Float ret = toFloat(value);
                parentParseInfo.setFloatValue(name, ret);
            }

            void handleTagMatrix(pugi::xml_node &node, XmlParseInfo &parentParseInfo) {
                std::string value = node.attribute("value").value();

                Matrix4F ret;
                char *tmp = nullptr;
#if defined(RENDER_DATA_DOUBLE)
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

            void handleTagTransform(pugi::xml_node &node, XmlParseInfo &parseInfo, XmlParseInfo &parentParseInfo) {
                std::string name = node.attribute("name").value();
                parentParseInfo.setTransformValue(name, parseInfo.transformMat);
            }

            void handleTagFilm(pugi::xml_node &node, XmlParseInfo &parseInfo) {
                std::string type = node.attribute("type").value();
                Config::Camera::filmType = type;
                Config::Camera::filename = parseInfo.getStringValue("filename", Config::Camera::filename);
                Config::Camera::width = parseInfo.getIntValue("width", Config::Camera::width);
                Config::Camera::height = parseInfo.getIntValue("height", Config::Camera::height);
                Config::Camera::fileFormat = parseInfo.getStringValue("fileFormat", Config::Camera::fileFormat);
                Config::Camera::pixelFormat = parseInfo.getStringValue("pixelFormat", Config::Camera::pixelFormat);
                Config::Camera::gamma = parseInfo.getFloatValue("gamma", Config::Camera::gamma);
                Config::Camera::banner = parseInfo.getBoolValue("banner", Config::Camera::banner);
                Config::Camera::rfilter = parseInfo.getStringValue("rfilter", Config::Camera::rfilter);
            }

            void handleTagSampler(pugi::xml_node &node, XmlParseInfo &parseInfo) {
                Config::Sampler::type = node.attribute("type").value();
                Config::Sampler::sampleCount = parseInfo.getIntValue("sampleCount", Config::Sampler::sampleCount);
            }

            void XmlSceneImporter::handleTagSensor(pugi::xml_node &node, XmlParseInfo &parseInfo) {
                Config::Camera::type = node.attribute("type").value();
                ASSERT(Config::Camera::type == "perspective", "Only support perspective camera for now.");

                Float fov = parseInfo.getFloatValue("fov", 45.);
                transform::Transform toWorldMat = parseInfo.getTransformValue("toWorld", Transform());
                std::shared_ptr<transform::Transform> toWorld = std::make_shared<transform::Transform>(
                        toWorldMat.mat());

                _scene->setCamera(std::make_shared<Camera>(toWorld, fov));
            }

            void XmlSceneImporter::handleTagMode(pugi::xml_node &node, XmlParseInfo &parseInfo) {
                const std::string type = node.attribute("type").value();
                if (type == "final") {
                    SET_TRACE_PATH_FLAG(false)
                    return;
                } else if (type == "trace") {
                    SET_TRACE_PATH_FLAG(true)
                    int row = parseInfo.getIntValue("row", -1);
                    int col = parseInfo.getIntValue("col", -1);
                    int traceCount = parseInfo.getIntValue("traceCount", 0);
                    ADD_TRACE_CRITERIA(Point2I(col, row), traceCount);
                } else {
                    ASSERT(false, "Mode type not supported: <" + type + ">. ");
                }
            }

            void XmlSceneImporter::handleTagTexture(pugi::xml_node &node,
                                                    XmlParseInfo &parseInfo,
                                                    XmlParseInfo &parentInfo) {
                const std::string type = node.attribute("type").value();
                const std::string name = node.attribute("name").value();
                if (type == "checkerboard") {
                    Spectrum color0 = parseInfo.getSpectrumValue("color0", Spectrum(0.));
                    Spectrum color1 = parseInfo.getSpectrumValue("color1", Spectrum(0.));

                    Float uScale = parseInfo.getFloatValue("uscale", 1.0);
                    Float vScale = parseInfo.getFloatValue("vscale", 1.0);

                    Texture<Spectrum>::Ptr texture = std::make_shared<ChessboardTexture<Spectrum>>(color0, color1,
                                                                                                   uScale, vScale);
                    parentInfo.setSpectrumTextureValue(name, texture);
                } else if (type == "bitmap") {
                    ASSERT(parseInfo.attrExists("filename"), "Bitmap path not exists. ");
                    std::string filename = parseInfo.getStringValue("filename", "");
                    TextureMapping2D::Ptr mapping = std::shared_ptr<UVMapping2D>();
                    Texture<Spectrum>::Ptr texture =
                            std::make_shared<ImageTexture<Spectrum>>(_inputSceneDir + filename, mapping);
                    parentInfo.setSpectrumTextureValue(name, texture);
                } else {
                    ASSERT(false, "Texture type not supported: " + type);
                }
            }

            void
            XmlSceneImporter::handleTagBSDF(pugi::xml_node &node, XmlParseInfo &parseInfo, XmlParseInfo &parentInfo) {
                std::string type = node.attribute("type").value();
                std::string id = node.attribute("id").value();

                Material::Ptr material = nullptr;
                if (type == "diffuse") {
                    material = createDiffuseMaterial(parseInfo);
                } else if (type == "dielectric") {
                    material = createDielectricMaterial(parseInfo);
                } else if (type == "mirror") {
                    material = createMirrorMaterial(parseInfo);
                } else if (type == "glass") {
                    material = createGlassMaterial(parseInfo);
                } else if (type == "roughconductor" || type == "conductor") {
                    material = createRoughConductorMaterial(parseInfo);
                } else if (type == "twosided") {
                    ASSERT(parseInfo.currentMaterial != nullptr,
                           "BSDF twosided should have {currentMaterial} attribute. ");
                    material = parseInfo.currentMaterial;
                    material->setTwoSided(true);
                } else if (type == "coating") {
                    material = createCoatingMaterial(parseInfo);
                } else if (type == "plastic") {
                    material = createPlasticMaterial(parseInfo);
                } else {
                    ASSERT(false, "Material " + type + " not supported for now");
                }

                if (id == "") {
                    parentInfo.currentMaterial = material;
                } else {
                    _materialMap[id] = material;
                }
            }

            Material::Ptr XmlSceneImporter::createRoughConductorMaterial(XmlParseInfo &info) {
                // Roughness
                Float alpha = 0.01;
                if (info.attrExists("alpha")) {
                    auto alphaType = info.getType("alpha");
                    ASSERT(alphaType == XmlAttrVal::Attr_Float, "Only support float type for alpha. ")
                    alpha = info.getFloatValue("alpha", 0.01);
                } else {
                    alpha = info.getFloatValue("alpha", 0.01);
                }


                std::string distributionType = info.getStringValue("distribution", "beckmann");
                ASSERT(distributionType == "beckmann" || distributionType == "ggx",
                       "Microfacet distribution unsupported: " + distributionType);

                Spectrum R = info.getSpectrumValue("specularReflectance", Spectrum(1.0));
                Spectrum Eta = info.getSpectrumValue("eta", Spectrum(0.200438));
                Spectrum K = info.getSpectrumValue("k", Spectrum(0.200438));

                Texture<Float>::Ptr texAlpha = std::make_shared<ConstantTexture<Float>>(alpha);
                Texture<Spectrum>::Ptr texR = std::make_shared<ConstantTexture<Spectrum>>(R);
                Texture<Spectrum>::Ptr texEta = std::make_shared<ConstantTexture<Spectrum>>(Eta);
                Texture<Spectrum>::Ptr texK = std::make_shared<ConstantTexture<Spectrum>>(K);
                return std::make_shared<Metal>(texAlpha, texEta, texR, texK, distributionType);
            }

            Material::Ptr XmlSceneImporter::createGlassMaterial(XmlParseInfo &info) {
                Float extIOR = 1.;
                Float intIOR = 1.5;
                Texture<Spectrum>::Ptr texR = std::make_shared<ConstantTexture<Spectrum>>(1.0);
                Texture<Spectrum>::Ptr texT = std::make_shared<ConstantTexture<Spectrum>>(1.0);
                return std::make_shared<Dielectric>(texR, texT, extIOR, intIOR);
            }

            Material::Ptr XmlSceneImporter::createPlasticMaterial(XmlParseInfo &info) {
                Material::Ptr material = nullptr;
                auto intIORType = info.getType("intIOR");
                auto extIORType = info.getType("extIOR");
                ASSERT(intIORType == XmlAttrVal::Attr_Float && extIORType == XmlAttrVal::Attr_Float,
                       "Only support float type IOR for now");

                // thetaT
                Float intIOR = info.getFloatValue("intIOR", 1.5);
                // thetaI
                Float extIOR = info.getFloatValue("extIOR", 1);

                ASSERT(info.getType("diffuseReflectance") == XmlAttrVal::Attr_Spectrum &&
                       info.getType("specularReflectance") == XmlAttrVal::Attr_Spectrum &&
                       info.getType("alpha") == XmlAttrVal::Attr_Float,
                       "PlasticMaterial parameter error: type not supported");

                auto diffuseReflectance = info.getSpectrumValue("diffuseReflectance", Spectrum(1.0));
                auto specularReflectance = info.getSpectrumValue("specularReflectance", Spectrum(1.0));
                auto alpha = info.getFloatValue("alpha", 0.01);

                Texture<Spectrum>::Ptr Kd = std::make_shared<ConstantTexture<Spectrum>>(diffuseReflectance);
                Texture<Spectrum>::Ptr Ks = std::make_shared<ConstantTexture<Spectrum>>(specularReflectance);

                Texture<Float>::Ptr etaI = std::make_shared<ConstantTexture<Float>>(extIOR);
                Texture<Float>::Ptr etaT = std::make_shared<ConstantTexture<Float>>(intIOR);

                material = std::make_shared<PlasticMaterial>(Kd, Ks, etaI, etaT, alpha);
                return material;
            }

            // Rename method
            Material::Ptr XmlSceneImporter::createCoatingMaterial(XmlParseInfo &info) {
                Material::Ptr material = nullptr;

                ASSERT(info.attrExists("diffuseReflectance") &&
                       info.attrExists("specularReflectance") && info.attrExists("alpha"),
                       "CoatingMaterial parameter error: type not supported");

                // Kd
                Texture<Spectrum>::Ptr Kd = nullptr;
                XmlAttrVal::AttrType kdType = info.getType("diffuseReflectance");
                if (kdType == XmlAttrVal::Attr_Spectrum) {
                    Spectrum diffuseReflectance = info.getSpectrumValue("diffuseReflectance", Spectrum(1.0));
                    Kd = std::make_shared<ConstantTexture<Spectrum>>(diffuseReflectance);
                } else if (kdType == XmlAttrVal::Attr_SpectrumTexture) {
                    Kd = info.getSpectrumTextureValue("diffuseReflectance", nullptr);
                } else {
                    ASSERT(false, "Unsupported Kd type. ");
                }

                // Ks
                Texture<Spectrum>::Ptr Ks = nullptr;
                XmlAttrVal::AttrType ksType = info.getType("specularReflectance");
                if (ksType == XmlAttrVal::Attr_Spectrum) {
                    Spectrum specularReflectance = info.getSpectrumValue("specularReflectance", Spectrum(1.0));
                    Ks = std::make_shared<ConstantTexture<Spectrum>>(specularReflectance);
                } else if (ksType == XmlAttrVal::Attr_SpectrumTexture) {
                    Ks = info.getSpectrumTextureValue("specularReflectance", nullptr);
                } else {
                    ASSERT(false, "Unsupported Ks type. ");
                }

                auto alpha = info.getFloatValue("alpha", 0.1);

                Texture<Float>::Ptr roughness = std::make_shared<ConstantTexture<Float>>(alpha);
                material = std::make_shared<PatinaMaterial>(Kd, Ks, roughness);
                return material;
            }

            Material::Ptr XmlSceneImporter::createDielectricMaterial(XmlParseInfo &info) {
                Material::Ptr material = nullptr;
                auto intIORType = info.getType("intIOR");
                auto extIORType = info.getType("extIOR");
                ASSERT(intIORType == XmlAttrVal::Attr_Float && extIORType == XmlAttrVal::Attr_Float,
                       "Only support float type IOR for now");

                Float roughness = info.getFloatValue("alpha", 0.0);
                // thetaT
                Float intIOR = info.getFloatValue("intIOR", 1.5);
                // thetaI
                Float extIOR = info.getFloatValue("extIOR", 1.0);
                // 临时用 1.0 spectrum 代替
                Texture<Spectrum>::Ptr texR = std::make_shared<ConstantTexture<Spectrum>>(1.0);
                Texture<Spectrum>::Ptr texT = std::make_shared<ConstantTexture<Spectrum>>(1.0);
                material = std::make_shared<Dielectric>(texR, texT, extIOR, intIOR, roughness);
                return material;
            }

            Material::Ptr XmlSceneImporter::createDiffuseMaterial(XmlParseInfo &info) {
                Material::Ptr material = nullptr;

                if (!info.attrExists("reflectance")) {
                    // Create default diffuse material
                    Texture<Spectrum>::Ptr texture = std::make_shared<ConstantTexture<Spectrum>>(0.);
                    material = std::make_shared<Lambertian>(texture);
                } else {
                    auto type = info.getType("reflectance");
                    if (type == XmlAttrVal::Attr_Spectrum) {
                        Spectrum albedo = info.getSpectrumValue("reflectance", 0);
                        Texture<Spectrum>::Ptr texture = std::make_shared<ConstantTexture<Spectrum>>(albedo);
                        material = std::make_shared<Lambertian>(texture);
                    } else if (type == XmlAttrVal::Attr_SpectrumTexture) {
                        Texture<Spectrum>::Ptr texture = info.getSpectrumTextureValue("reflectance", nullptr);
                        ASSERT(texture, "Texture can't be nullptr. ");
                        material = std::make_shared<Lambertian>(texture);
                    } else {
                        // TODO
                        ASSERT(type == XmlAttrVal::Attr_Spectrum, "Reflectance type not supported .");
                    }
                }
                return material;
            }

            Material::Ptr XmlSceneImporter::createMirrorMaterial(XmlParseInfo &info) {
                return std::make_shared<Mirror>();
            }

            std::shared_ptr<std::vector<Shape::Ptr>> XmlSceneImporter::createRectangleShape(XmlParseInfo &info) {
                auto transformMat = info.getTransformValue("toWorld", Transform());
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

            std::shared_ptr<std::vector<Shape::Ptr>> XmlSceneImporter::createCubeShape(XmlParseInfo &info) {
                auto transform = info.getTransformValue("toWorld", Transform());
                Cube::Ptr cubePtr = std::make_shared<Cube>(transform.mat());
                return cubePtr->triangles();
            }

            std::shared_ptr<std::vector<Shape::Ptr>> XmlSceneImporter::createObjMeshes(XmlParseInfo &info) {
                auto transform = info.getTransformValue("toWorld", Transform());
                Transform::Ptr toWorld = std::make_shared<Transform>(transform.mat());

                // face normal
                bool faceNormal = false;
                faceNormal = info.getBoolValue("faceNormals", false);

                std::string filename = info.getStringValue("filename", "");
                ASSERT(filename != "", "Obj filename can't be empty. ");

                std::vector<Vector3F> vertices;
                std::vector<Normal3F> normals;
                std::vector<Point2F> texcoords;
                std::vector<TriMesh::TriIndex> indics;
                bool good = io::ObjLoader::loadObj(_inputSceneDir + filename, vertices, normals, texcoords, indics);
                ASSERT(good, "Load *.obj model failed: " + filename);
                std::cout << "\tLoading mesh: " << filename << std::endl;

                TriMesh::Ptr tris = std::make_shared<TriangleMesh>(vertices, normals, texcoords, indics, toWorld,
                                                                   faceNormal);
                return tris->triangles();
            }

            void XmlSceneImporter::handleTagShape(pugi::xml_node &node, XmlParseInfo &parseInfo) {
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
                Medium::Ptr exteriorMedium = parseInfo.currentExteriorMedium;
                Medium::Ptr interiorMedium = parseInfo.currentInteriorMedium;

                Spectrum radiance(0.0);
                if (parseInfo.hasAreaLight) {
                    auto radianceType = parseInfo.getType("radiance");
                    if (radianceType == XmlAttrVal::Attr_Spectrum) {
                        radiance = parseInfo.getSpectrumValue("radiance", 0);
                    } else {
                        ASSERT(false, "Only support spectrum radiance for now.");
                    }
                }

                for (auto it = shapes->begin(); it != shapes->end(); it++) {
                    AreaLight::Ptr light = nullptr;
                    if (parseInfo.hasAreaLight) {
                        light = std::make_shared<DiffuseAreaLight>(radiance, *it,
                                                                   MediumInterface(interiorMedium.get(),
                                                                                   exteriorMedium.get()),
                                                                   true);
                        _scene->addLight(light);
                    }

                    Geometry::Ptr geometry = std::make_shared<Geometry>(*it, material, interiorMedium, exteriorMedium,
                                                                        light);
                    _shapes.push_back(geometry);
                }
            }

            void XmlSceneImporter::handleTagRef(pugi::xml_node &node, XmlParseInfo &parent) {
                std::string id = node.attribute("id").value();
                ASSERT(_materialMap.count(id) > 0, "Material " + id + " Not Exists.!");
                parent.currentMaterial = _materialMap[id];
            }

            void XmlSceneImporter::handleTagRGB(pugi::xml_node &node, XmlParseInfo &parentParseInfo) {
                if (strcmp(node.name(), "spectrum") == 0) {
                    // TODO: Fix Spectrum declared with wavelength
                    ASSERT(false, "No implemented!");
                } else if (strcmp(node.name(), "rgb") == 0 || strcmp(node.name(), "color") == 0) {
                    Spectrum ret;
                    std::string colorValue = node.attribute("value").value();
                    char *tmp;
#if defined(RENDER_DATA_DOUBLE)
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
                    parentParseInfo.setSpectrumValue(name, ret);
                }
            }

            void XmlSceneImporter::handleTagVector(pugi::xml_node &node, XmlParseInfo &parent) {
                std::string name = node.attribute("name").value();
                std::string x_str = node.attribute("x").value();
                std::string y_str = node.attribute("y").value();
                std::string z_str = node.attribute("z").value();

                Float x = toFloat(x_str);
                Float y = toFloat(y_str);
                Float z = toFloat(z_str);

                Vector3F v(x, y, z);
                parent.setVectorValue(name, v);
            }

            void XmlSceneImporter::handleTagEmitter(pugi::xml_node &node, XmlParseInfo &info, XmlParseInfo &parent) {
                std::string type = node.attribute("type").value();
                if (type == "area") {
                    parent.hasAreaLight = true;
                    auto radianceType = info.getType("radiance");
                    if (radianceType == XmlAttrVal::Attr_Spectrum) {
                        parent.set("radiance", info.get("radiance"));
                    } else {
                        ASSERT(false, "Only support spectrum type radiance.");
                    }
                    std::cout << "\tCreate light: area light" << std::endl;
                } else if (type == "point") {
                    transform::Transform toWorldMat = info.getTransformValue("toWorld", Transform());
                    std::shared_ptr<transform::Transform> toWorld = std::make_shared<transform::Transform>(toWorldMat);
                    Spectrum intensity = info.getSpectrumValue("intensity", 0);
                    Light::Ptr pointLight = std::make_shared<PointLight>(intensity, toWorld, MediumInterface(nullptr,
                                                                                                             nullptr));
                    _scene->addLight(pointLight);
                    std::cout << "\tCreate light: point light" << std::endl;
                } else if (type == "spot") {
                    transform::Transform toWorldMat = info.getTransformValue("toWorld", Transform());
                    std::shared_ptr<transform::Transform> toWorld = std::make_shared<transform::Transform>(toWorldMat);
                    Spectrum intensity = info.getSpectrumValue("intensity", Spectrum(0));
                    Float totalAngle = info.getFloatValue("totalAngle", 60.);
                    Float falloffAngle = info.getFloatValue("falloffAngle", 50.);
                    Light::Ptr spotLight = std::make_shared<SpotLight>(
                            intensity, toWorld,
                            MediumInterface(nullptr, nullptr),
                            falloffAngle, totalAngle);
                    _scene->addLight(spotLight);
                    std::cout << "\tCreate light: spot light" << std::endl;
                } else if (type == "envmap") {
                    transform::Transform toWorldMat = info.getTransformValue("toWorld", Transform());
                    std::shared_ptr<transform::Transform> toWorld = toWorldMat.ptr();

                    ASSERT(info.attrExists("filename"), "Environment light type should has envmap. ");
                    std::string envmapPath = info.getStringValue("filename", "");

                    EnvironmentLight::Ptr envLight = std::make_shared<EnvironmentLight>(1., _inputSceneDir + envmapPath,
                                                                                        MediumInterface(nullptr,
                                                                                                        nullptr),
                                                                                        toWorld);
                    _scene->addLight(envLight);
                    _scene->addInfiniteLight(envLight);
                    std::cout << "\tCreate environment light. " << std::endl;
                } else if (type == "sunsky") {
                    ASSERT(info.attrExists("sunDirection") && info.attrExists("intensity"),
                           "Sunsky parameter incomplete. ");
                    ASSERT(info.getType("intensity") == XmlAttrVal::Attr_Spectrum,
                           "<emitter> Only support spectrum intensity. ");
                    Spectrum intensity = info.getSpectrumValue("intensity", Spectrum(0.0));
                    Vector3F sunDirection = -info.getVectorValue("sunDirection", Vector3F(0, 1, 0));
                    SunLight::Ptr sunLight = std::make_shared<SunLight>(intensity, sunDirection);
                    _scene->addLight(sunLight);
                    _scene->addInfiniteLight(sunLight);
                    std::cout << "\tCreate sun light. " << std::endl;
                } else {
                    ASSERT(false, "Emitter type not supported: <" + type + ">. ");
                }
            }

            void XmlSceneImporter::handleTagMedium(pugi::xml_node &node, XmlParseInfo &info, XmlParseInfo &parent) {
                std::string mediumType = node.attribute("type").value();
                std::string mediumName = node.attribute("name").value();
                Medium::Ptr medium = nullptr;
                if (mediumType == "homogeneous") {
                    ASSERT(info.attrExists("sigmaS") && info.attrExists("sigmaA"), "Medium parameter missed. ")
                    Spectrum sigmaS = info.getSpectrumValue("sigmaS", Spectrum(0.1));
                    Spectrum sigmaA = info.getSpectrumValue("sigmaA", Spectrum(0.1));
                    Float g = info.getFloatValue("g", 0.0f);
                    medium = std::make_shared<IsotropicMedium>(sigmaA, sigmaS, g);
                } else {
                    ASSERT(false, "Medium type not supported. ");
                }

                if (medium != nullptr) {
                    if (mediumName == "exterior") {
                        parent.currentExteriorMedium = medium;
                    } else if (mediumName == "interior") {
                        parent.currentInteriorMedium = medium;
                    }
                }
            }

            void XmlSceneImporter::handleTagIntegrator(pugi::xml_node &node, XmlParseInfo &info) {
                std::string type = node.attribute("type").value();
                Config::Tracer::type = type;
                Config::Tracer::maxDepth = info.getIntValue("maxDepth", Config::Tracer::maxDepth);
                Config::Tracer::sampleNum = info.getIntValue("sampleNum", Config::Tracer::sampleNum);
                Config::Tracer::delta = info.getIntValue("delta", Config::Tracer::delta);
                Config::Tracer::strictNormals = info.getBoolValue("strictNormals", Config::Tracer::strictNormals);
                Config::Tracer::photonCount = info.getIntValue("photonCount", Config::Tracer::photonCount);
                Config::Tracer::initialRadius = info.getFloatValue("initialRadius", Config::Tracer::initialRadius);
                Config::Tracer::radiusDecay = info.getFloatValue("alpha", Config::Tracer::radiusDecay);
            }

            void XmlSceneImporter::handleTagLookAt(pugi::xml_node &node, XmlParseInfo &parent) {
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
                                                 XmlParseInfo &parseInfo,
                                                 XmlParseInfo &parentParseInfo) {
                TagType tagType = _nodeTypeMap[node.name()];
                switch (tagType) {
                    case Tag_Mode:
                        handleTagMode(node, parseInfo);
                        break;
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
                    case Tag_Texture:
                        handleTagTexture(node, parseInfo, parentParseInfo);
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
                    case Tag_Medium:
                        handleTagMedium(node, parseInfo, parentParseInfo);
                        break;
                    default:
                        std::cout << "\tUnsupported tag: <" << node.name() << ">" << std::endl;
                }
            }

            void XmlSceneImporter::parseXml(pugi::xml_node &node, XmlParseInfo &parentParseInfo) {
                XmlParseInfo parseInfo;
                std::map<std::string, XmlAttrVal> attrContainer;
                for (pugi::xml_node &child : node.children()) {
                    parseXml(child, parseInfo);
                }

                handleXmlNode(node, parseInfo, parentParseInfo);
            }

            std::shared_ptr<Scene> XmlSceneImporter::importScene(std::string sceneDir) {
                _inputSceneDir = sceneDir;
                std::string xml_file = sceneDir + "scene.xml";
                std::cout << "Loading scene file: " << xml_file << std::endl;

                pugi::xml_document xml_doc;
                pugi::xml_parse_result ret = xml_doc.load_file(xml_file.c_str());

                ASSERT(ret, "Error while parsing \"" + xml_file + "\": " + ret.description()
                            + " (at " + getOffset(ret.offset, xml_file) + ")");
                _scene = std::make_shared<Scene>();

                XmlParseInfo parseInfo;
                parseXml(*xml_doc.begin(), parseInfo);

                std::cout << "\tBuilding bvh acceleration structure ... " << std::endl;
                std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(_shapes);
                _scene->setWorld(bvh);
                _scene->setSceneName(Config::Camera::filename);

                const std::vector<InfiniteLight::Ptr> &infiniteLights = _scene->getInfiniteLights();
                for (auto it = infiniteLights.begin(); it != infiniteLights.end(); it++) {
                    (*it)->worldBound(_scene);
                }

                std::cout << "Loading finished. " << std::endl;
                return _scene;
            }
        }
    }
}