//
// Created by Storm Phoenix on 2020/12/20.
//

#ifndef KAGUYA_BOX_H
#define KAGUYA_BOX_H

#include <kaguya/core/Transform.h>
#include <kaguya/scene/Aggregation.h>

#include <map>

namespace kaguya {
    namespace scene {

        class Box : public Aggregation {
        public:
            Box(const std::shared_ptr<Material> material = nullptr,
                const std::shared_ptr<Medium> inside = nullptr,
                const std::shared_ptr<Medium> outside = nullptr,
                const std::shared_ptr<Matrix4> transform = nullptr);

            virtual std::vector<std::shared_ptr<Shape>> aggregation() override;

        private:
            void buildBox();

        private:
            // transform matrix
            std::shared_ptr<Matrix4> _transform;
            const std::shared_ptr<Material> _material;
            const std::shared_ptr<Medium> _inside;
            const std::shared_ptr<Medium> _outside;

            std::vector<std::shared_ptr<Shape>> objects;
        };

    }
}

#endif //KAGUYA_BOX_H
