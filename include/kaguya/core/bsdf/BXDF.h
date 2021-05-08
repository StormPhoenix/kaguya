//
// Created by Storm Phoenix on 2020/10/14.
//

#ifndef KAGUYA_BXDF_H
#define KAGUYA_BXDF_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace core {
        namespace bsdf {
            using sampler::Sampler;

            typedef enum TransportMode {
                IMPORTANCE,
                RADIANCE
            } TransportMode;

            enum BXDFType {
                BSDF_REFLECTION = 1 << 0,
                BSDF_DIFFUSE = 1 << 1,
                BSDF_GLOSSY = 1 << 2,
                BSDF_SPECULAR = 1 << 3,
                BSDF_TRANSMISSION = 1 << 4,
                BSDF_ALL = BSDF_REFLECTION | BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_TRANSMISSION
            };

            class BXDF {
            public:
                /**
                 * BXDF 类型
                 * @param type
                 */
                BXDF(BXDFType type);

                /**
                 * 计算从 w_i 方向入射、w_o 方向出射的反射系数，默认为标准右手坐标系
                 * @param wo 出射方向
                 * @param wi 入射方向
                 * @return 辐射系数
                 */
                virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const = 0;

                /**
                 * 针对 w_o 出射方向，采样 wi 入射方向，并返回概率 pdf 以及对应的辐射系数。
                 * 默认为标准右手坐标系，按照 hemisphere-cosine 方式采样 wi
                 * @param wo 出射方向
                 * @param wi 入射方向
                 * @param pdf 采样概率
                 * @return 辐射系数
                 */
                virtual Spectrum sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                         Sampler *const sampler, BXDFType *sampleType);

                /**
                 * wo 与 wi 之间的采样 PDF
                 * @param wo 出射方向
                 * @param wi 入射方向
                 * @return
                 */
                virtual Float samplePdf(const Vector3F &wo, const Vector3F &wi) const;

                virtual ~BXDF() {}

                /**
                 * 判断自身 type 是否包含在 bxdfType 之中
                 * @param bxdfType
                 * @return
                 */
                bool allIncludeOf(const BXDFType bxdfType);

                /**
                 * 判断自身是否包含 bxdfType
                 * @param bxdfType
                 * @return
                 */
                bool hasAllOf(const BXDFType bxdfType);

                /**
                 * 判断是否包含 bxdfType 中任一类型
                 * @param bxdfType
                 * @return
                 */
                bool hasAnyOf(const BXDFType bxdfType);

            public:
                const BXDFType type;
            };
        }
    }
}

#endif //KAGUYA_BXDF_H
