//
// Created by Storm Phoenix on 2021/1/10.
//

#ifndef KAGUYA_SEPARABLEBSSRDF_H
#define KAGUYA_SEPARABLEBSSRDF_H

#include <kaguya/core/bssrdf/BSSRDF.h>
#include <kaguya/core/bssrdf/SeparableBSSRDFAdapter.h>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace bssrdf {

            // TODO 多项式拟合积分，尚未推导，现在这里记录下来
            Float fresnelMoment1(Float eta);

            Float fresnelMoment2(Float eta);

            /**
             * SeparableBSSRDF
             * S(p_o, p_i, w_o, w_i) = (1 - Fr(w_o)) * S_p(p_o, p_i) * S_w(w_i)
             */
            class SeparableBSSRDF : public BSSRDF {
                friend class SeparableBSSRDFAdapter;

            public:
                SeparableBSSRDF(const SurfaceInteraction &po, Material *material, Float theta);

                /**
                 * S(p_o, p_i, w_o, w_i) = (1 - Fr(w_o)) * S_p(p_o, p_i) * S_w(w_i)
                 */
                Spectrum S(const SurfaceInteraction &si, const Vector3F &wi) override;

                /**
                 * Sample S(p_o, p_i, w_o, w_i) = (1 - Fr(w_o)) * S_p(p_o, p_i) * S_w(w_i)
                 */
                virtual Spectrum
                sampleS(std::shared_ptr<Scene> scene, SurfaceInteraction *pi, Float *pdf,
                        MemoryAllocator &allocator, Sampler *sampler) override;

            protected:
                /**
                 * Sample S_p(p_o, p_i)
                 * @return
                 */
                virtual Spectrum sampleSp(std::shared_ptr<Scene> scene, SurfaceInteraction *pi, Float *pdf,
                                          MemoryAllocator &allocator, Sampler *sampler);

                /**
                 * S_(p_o, p_i)
                 * @param si
                 * @return
                 */
                virtual Spectrum Sp(const SurfaceInteraction &si) const;

                /**
                 * pdf of S_p(p_o, p_i)
                 * @param pi
                 * @return
                 */
                virtual Float SpPdf(SurfaceInteraction &pi) const;

                /**
                 * S_p(p_o, p_i) = S_r(|p_o - p_i|)
                 * Sample radius for the channel @param spectrumChannel
                 * @return
                 */
                virtual Float sampleSr(int spectrumChannel, Float sample) const = 0;

                /**
                 * Get spectrum value for the @param radius
                 * S_p(p_o, p_i) = S_r(|p_o - p_i|)
                 * @return
                 */
                virtual Spectrum Sr(Float radius) const = 0;

                /**
                 * Get pdf of the @param radius for channel @param ch
                 * Pdf of S_p(p_o, p_i) = S_r(|p_o - p_i|)
                 * @return
                 */
                virtual Float SrPdf(int ch, Float radius) const = 0;

                /**
                 * S_w(w_i)
                 * @param wi
                 * @return
                 */
                virtual Spectrum subsurfaceWi(const Vector3F &wi) const;

            private:
                Vector3F _tanY;
                Vector3F _tanX;
                Vector3F _tanZ;

                Material *_material;
                Float _theta;
            };
        }
    }
}

#endif //KAGUYA_SEPARABLEBSSRDF_H
