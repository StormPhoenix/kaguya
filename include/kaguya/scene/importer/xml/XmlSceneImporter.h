//
// Created by Storm Phoenix on 2021/4/16.
//

#ifndef KAGUYA_XMLSCENEIMPORTER_H
#define KAGUYA_XMLSCENEIMPORTER_H

#include <ext/pugixml/pugixml.hpp>
#include <kaguya/scene/meta/Shape.h>
#include <kaguya/core/Transform.h>
#include <kaguya/scene/importer/SimpleSceneImporter.h>
#include <kaguya/scene/importer/xml/XmlParseInfo.h>
#include <kaguya/utils/memory/MemoryAllocator.h>

namespace RENDER_NAMESPACE {
    namespace scene {
        namespace importer {
            using meta::Shape;

            // XML Tag
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

            class XmlSceneImporter : public SimpleSceneImporter {
            public:
                XmlSceneImporter(MemoryAllocator &allocator);

                std::shared_ptr<SimpleScene> importScene(std::string sceneDir) override;

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

                const Material createDiffuseMaterial(XmlParseInfo &info);

                const Material createMirrorMaterial(XmlParseInfo &info);

                const Material createGlassMaterial(XmlParseInfo &info);

                const Material createDielectricMaterial(XmlParseInfo &info);

                const Material createRoughConductorMaterial(XmlParseInfo &info);

                const Material createCoatingMaterial(XmlParseInfo &info);

                const Material createPlasticMaterial(XmlParseInfo &info);

            private:
                MemoryAllocator &_allocator;
                std::map<std::string, TagType> _nodeTypeMap;
                std::shared_ptr<SimpleScene> _scene;
                std::map<std::string, Material> _materialMap;
                std::vector<Intersectable::Ptr> _shapes;
                std::string _inputSceneDir;
            };

        }
    }
}

#endif //KAGUYA_XMLSCENEIMPORTER_H
