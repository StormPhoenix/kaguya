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
            BSDF(const Interaction &interaction);

            void addBXDF(BXDF *bxdf);

            /**
             * 向量 v 转化到局部坐标系
             * @param v
             * @return
             */
            Vector3F toObjectSpace(const Vector3F &v) const;

            /**
             * 向量 v 转化到世界坐标系
             * @param v
             * @return
             */
            Vector3F toWorldSpace(const Vector3F &v) const;

            /**
             * 计算 f(p, wi, wo)
             * @param worldWo
             * @param worldWi
             * @param type
             * @return
             */
            Spectrum f(const Vector3F &worldWo, const Vector3F &worldWi, BXDFType type = BSDF_ALL) const;

            /**
             * 随机采样 worldWi f(p, wo, wi)
             * @param worldWo
             * @param worldWi
             * @param pdf
             * @param type
             * @param sampleType 返回采样的 BXDF 类型
             * @return
             */
            Spectrum sampleF(const Vector3F &worldWo, Vector3F *worldWi, Float *pdf,
                             Sampler *const sampler,
                             BXDFType type = BSDF_ALL, BXDFType *sampleType = nullptr) const;

            /**
             * 计算 worldWi worldWo 的采样概率
             * @param worldWo
             * @param worldWi
             * @param type
             * @return
             */
            Float samplePdf(const Vector3F &worldWo, const Vector3F &worldWi, BXDFType type = BSDF_ALL) const;

            /**
             * 计算属于 bxdfType 类型集合的材质有多少个
             * @param bxdfType
             * @return
             */
            int allIncludeOf(BXDFType bxdfType);

            /**
             * 计算包含 bxdfType 类型集合的材质有多少个
             * @param bxdfType
             * @return
             */
            int hasAllOf(BXDFType bxdfType);

            /**
             * 计算包含 bxdfType 中任一类型集合的材质有多少个
             * @param bxdfType
             * @return
             */
            int hasAnyOf(const BXDFType bxdfType) const;

        private:
            ~BSDF() {}

        private:
            int _bxdfCount = 0;
            // 存储 bxdf
            BXDF *_bxdfs[MAX_BXDF_NUM];
            // 入射光方向与交点法线构成的切线空间
            Vector3F _tanY;
            Vector3F _tanX;
            Vector3F _tanZ;
            Vector3F _geoNormal;
        };

    }
}

#endif //KAGUYA_BSDF_H
