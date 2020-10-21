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

            void addBXDF(BXDF *bxdf);

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

            /**
             * 计算 f(p, wi, wo)
             * @param worldWo
             * @param worldWi
             * @param type
             * @return
             */
            Spectrum f(const Vector3 &worldWo, const Vector3 &worldWi, BXDFType type = BSDF_ALL) const;

            /**
             * 随机采样 worldWi f(p, wo, wi)
             * @param worldWo
             * @param worldWi
             * @param pdf
             * @param type
             * @param sampleType 返回采样的 BXDF 类型
             * @return
             */
            Spectrum sampleF(const Vector3 &worldWo, Vector3 *worldWi, double *pdf,
                             BXDFType type = BSDF_ALL, BXDFType *sampleType = nullptr);

            /**
             * 计算 worldWi worldWo 的采样概率
             * @param worldWo
             * @param worldWi
             * @param type
             * @return
             */
            double samplePdf(const Vector3 &worldWo, const Vector3 &worldWi, BXDFType type = BSDF_ALL) const;

            /**
             * 计算有多少个 bxdfType 类型的材质
             * @param bxdfType
             * @return
             */
            int belongToType(BXDFType bxdfType);

        private:
            ~BSDF() {}

        private:
            int _bxdfCount = 0;
            // 存储 bxdf
            BXDF *_bxdfs[MAX_BXDF_NUM];
            // 入射光方向与交点法线构成的切线空间
            Vector3 _tanY;
            Vector3 _tanX;
            Vector3 _tanZ;
        };

    }
}

#endif //KAGUYA_BSDF_H
