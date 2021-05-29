//
// Created by Graphics on 2021/5/29.
//

#include <kaguya/material/Material.h>
#include <kaguya/material/Dielectric.h>
#include <kaguya/material/Lambertian.h>
#include <kaguya/material/Metal.h>
#include <kaguya/material/Mirror.h>
#include <kaguya/material/OrenNayar.h>
#include <kaguya/material/PatinaMaterial.h>
#include <kaguya/material/PlasticMaterial.h>
#include <kaguya/material/SubsurfaceMaterial.h>

namespace RENDER_NAMESPACE {
    namespace material {
        void Material::evaluateBSDF(SurfaceInteraction &insect, MemoryAllocator &allocator, TransportMode mode) {
            auto func = [&](auto ptr) { return ptr->evaluateBSDF(insect, allocator, mode); };
            return proxyCall(func);
        }

        bool Material::isSpecular() {
            auto func = [&](auto ptr) { return ptr->isSpecular(); };
            return proxyCall(func);
        }

        bool Material::isTwoSided() {
            auto func = [&](auto ptr) { return ptr->isTwoSided(); };
            return proxyCall(func);
        }

        void Material::setTwoSided(bool twoSided) {
            auto func = [&](auto ptr) { return ptr->setTwoSided(twoSided); };
            return proxyCall(func);
        }
    }
}