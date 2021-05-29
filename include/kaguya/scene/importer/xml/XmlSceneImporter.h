//
// Created by Storm Phoenix on 2021/4/16.
//

#ifndef KAGUYA_XMLSCENEIMPORTER_H
#define KAGUYA_XMLSCENEIMPORTER_H

#include <ext/pugixml/pugixml.hpp>
#include <kaguya/core/Transform.h>
#include <kaguya/scene/importer/SceneImporter.h>
#include <kaguya/scene/importer/xml/XmlParseInfo.h>

namespace RENDER_NAMESPACE {
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

                Tag_Texture,
                Tag_Medium,
                Tag_Integer,
                Tag_Float,
                Tag_Boolean,
                Tag_String,
                Tag_Transform,
                Tag_Matrix,
                Tag_Vector,
                Tag_LookAt,
                Tag_RGB,
            } TagType;

            class XmlSceneImporter : public SceneImporter {
            public:
                XmlSceneImporter();

                std::shared_ptr<Scene> importScene(std::string sceneDir) override;

            private:
                void handleXmlNode(pugi::xml_node &node,
                                   XmlParseInfo &parseInfo,
                                   XmlParseInfo &parentXmlParseInfo);

                void parseXml(pugi::xml_node &node, XmlParseInfo &parseInfo);

                void handleTagMode(pugi::xml_node &node, XmlParseInfo &parseInfo);

                void handleTagSensor(pugi::xml_node &node, XmlParseInfo &parseInfo);

                void handleTagBSDF(pugi::xml_node &node, XmlParseInfo &parseInfo, XmlParseInfo &parentInfo);

                void handleTagTexture(pugi::xml_node &node, XmlParseInfo &parseInfo, XmlParseInfo &parentInfo);

                void handleTagShape(pugi::xml_node &node, XmlParseInfo &parseInfo);

                void handleTagRef(pugi::xml_node &node, XmlParseInfo &parent);

                void handleTagRGB(pugi::xml_node &node, XmlParseInfo &parentXmlParseInfo);

                void handleTagEmitter(pugi::xml_node &node, XmlParseInfo &info, XmlParseInfo &parent);

                void handleTagMedium(pugi::xml_node &node, XmlParseInfo &info, XmlParseInfo &parent);

                void handleTagLookAt(pugi::xml_node &node, XmlParseInfo &parent);

                void handleTagVector(pugi::xml_node &node, XmlParseInfo &parent);

                void handleTagIntegrator(pugi::xml_node &node, XmlParseInfo &info);

                std::shared_ptr<std::vector<Shape::Ptr>> createRectangleShape(XmlParseInfo &info);

                std::shared_ptr<std::vector<Shape::Ptr>> createCubeShape(XmlParseInfo &info);

                std::shared_ptr<std::vector<Shape::Ptr>> createObjMeshes(XmlParseInfo &info);

                Material createDiffuseMaterial(XmlParseInfo &info);

                Material createMirrorMaterial(XmlParseInfo &info);

                Material createGlassMaterial(XmlParseInfo &info);

                Material createDielectricMaterial(XmlParseInfo &info);

                Material createRoughConductorMaterial(XmlParseInfo &info);

                Material createCoatingMaterial(XmlParseInfo &info);

                Material createPlasticMaterial(XmlParseInfo &info);

            private:
                std::map<std::string, TagType> _nodeTypeMap;
                std::shared_ptr<Scene> _scene;
                std::map<std::string, Material::Ptr> _materialMap;
                std::vector<Intersectable::Ptr> _shapes;
                std::string _inputSceneDir;
            };

        }
    }
}

#endif //KAGUYA_XMLSCENEIMPORTER_H
