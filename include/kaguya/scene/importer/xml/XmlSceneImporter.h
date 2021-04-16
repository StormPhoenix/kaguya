//
// Created by Storm Phoenix on 2021/4/16.
//

#ifndef KAGUYA_XMLSCENEIMPORTER_H
#define KAGUYA_XMLSCENEIMPORTER_H

#include <pugixml/pugixml.hpp>
#include <kaguya/core/Transform.h>
#include <kaguya/scene/importer/SceneImporter.h>

namespace kaguya {
    namespace scene {
        namespace importer {

            /**
             * XML 标签对应的场景元素类型
             */
            typedef enum TagType {
                Tag_Scene,
                Tag_Mode,

                // Scene components
                Tag_Integrator,
                Tag_Emitter,
                Tag_Sensor,
                Tag_Sampler,
                Tag_Film,
                Tag_Rfilter,
                Tag_Shape,
                Tag_BSDF,
                Tag_Ref,

                Tag_Integer,
                Tag_Float,
                Tag_Boolean,
                Tag_String,
                Tag_Transform,
                Tag_Matrix,
                Tag_RGB,
            } TagType;

            typedef struct AttrVal {

                enum {
                    Attr_Spectrum,
                    Attr_Boolean,
                    Attr_Integer,
                    Attr_Float,
                    Attr_Transform,
                    Attr_String,
                } type;

                struct Val {
                    Spectrum spectrumValue;
                    bool boolValue;
                    int intValue;
                    Float floatValue;
                    transform::Transform transformValue;
                    std::string stringValue;
                } value;

                AttrVal() {}
            } AttrVal;

            typedef struct ParseInfo {
                transform::Transform transformMat;
                std::map<std::string, AttrVal> container;
                std::string materialId;
            } ParseInfo;

            class XmlSceneImporter : public SceneImporter {
            public:
                XmlSceneImporter();

                std::shared_ptr<Scene> importScene(std::string sceneFile) override;

            private:
                void handleXmlNode(pugi::xml_node &node,
                                   ParseInfo &parseInfo,
                                   ParseInfo &parentParseInfo);

                void parseXml(pugi::xml_node &node, ParseInfo &parseInfo);

                void handleTagSensor(pugi::xml_node &node, ParseInfo &parseInfo);

                void handleTagBSDF(pugi::xml_node &node, ParseInfo &parseInfo);

                void handleTagShape(pugi::xml_node &node, ParseInfo &parseInfo);

                void handleTagRef(pugi::xml_node &node, ParseInfo &info);

                void handleTagRGB(pugi::xml_node &node, ParseInfo &parentParseInfo);

                std::shared_ptr<std::vector<Shape::Ptr>> createRectangleShape(ParseInfo &info);

                Material::Ptr createDiffuseMaterial(ParseInfo &info);

            private:
                std::map<std::string, TagType> _nodeTypeMap;
                std::shared_ptr<Scene> _scene;
                std::map<std::string, Material::Ptr> _materialMap;
                std::shared_ptr<std::vector<Geometry::Ptr>> _geometries;
            };

        }
    }
}

#endif //KAGUYA_XMLSCENEIMPORTER_H
