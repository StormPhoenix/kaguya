//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_BSDF_H
#define KAGUYA_BSDF_H

#include <kaguya/core/Core.h>
#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/Interaction.h>

namespace kaguya {
    namespace core {

        class BSDF {
        public:
            BSDF(const Interaction &insect);

            void addBXDF(std::shared_ptr<BXDF> bxdf);

            /**
             * 向量 v 转化到局部坐标系
             * @param v
             * @return
             */
            Vector3 toObjectSpace(const Vector3 &v) const;

            /**
             * 向量 v 转化到世界坐标系
             * @param v
             * @return
             */
            Vector3 toWorldSpace(const Vector3 &v) const;

            Spectrum f(const Vector3 &worldWo, const Vector3 &worldWi, BXDFType type = BSDF_ALL) const;

            Spectrum sampleF(const Vector3 &worldWo, Vector3 *worldWi, double *pdf, BXDFType type = BSDF_ALL);

        private:
            int _bxdfCount = 0;
            // 存储 bxdf
            std::shared_ptr<BXDF> _bxdfs[MAX_BXDF_NUM];
            // 入射光方向与交点法线构成的切线空间
            Vector3 _tanY;
            Vector3 _tanX;
            Vector3 _tanZ;
        };

    }
}

#endif //KAGUYA_BSDF_H
