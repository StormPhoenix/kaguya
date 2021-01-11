//
// Created by Storm Phoenix on 2021/1/10.
//

#ifndef KAGUYA_SEPARABLEBSSRDF_H
#define KAGUYA_SEPARABLEBSSRDF_H

#include <kaguya/core/bssrdf/BSSRDF.h>
#include <kaguya/core/bssrdf/SeparableBSSRDFAdapter.h>

namespace kaguya {
    namespace core {

        /**
         * SeparableBSSRDF
         * S(p_o, p_i, w_o, w_i) = (1 - Fr(w_o)) * S_p(p_o, p_i) * S_w(w_i)
         */
        class SeparableBSSRDF : public BSSRDF {
            friend class SeparableBSSRDFAdapter;

        public:
            SeparableBSSRDF();

            /**
             * S(p_o, p_i, w_o, w_i) = (1 - Fr(w_o)) * S_p(p_o, p_i) * S_w(w_i)
             */
            Spectrum s(const SurfaceInteraction &si, const Vector3d &wi) override;

            /**
             * Sample S(p_o, p_i, w_o, w_i) = (1 - Fr(w_o)) * S_p(p_o, p_i) * S_w(w_i)
             */
            virtual Spectrum sampleS(const Scene &scene, SurfaceInteraction *si, double *pdf, MemoryArena &memoryArena,
                                     Sampler *sampler) override;

        protected:
            /**
             * Sample S_p(p_o, p_i)
             * @return
             */
            virtual Spectrum sampleSubsurfacePoint(const Scene &scene, SurfaceInteraction *si, double *pdf,
                                                   MemoryArena &memoryArena, Sampler *sampler);

            /**
             * S_(p_o, p_i)
             * @param si
             * @return
             */
            virtual Spectrum subsurfacePoint(SurfaceInteraction &si) const;

            /**
             * pdf of S_p(p_o, p_i)
             * @param si
             * @return
             */
            virtual double subsurfacePointPdf(SurfaceInteraction &si) const;

            /**
             * S_p(p_o, p_i) = S_r(|p_o - p_i|)
             * Sample radius for the channel @param spectrumChannel
             * @return
             */
            virtual double sampleSubsurfaceRadius(int spectrumChannel, double sample) const = 0;

            /**
             * Get spectrum value for the @param radius
             * S_p(p_o, p_i) = S_r(|p_o - p_i|)
             * @return
             */
            virtual Spectrum subsurfaceRadius(double radius) const = 0;

            /**
             * Get pdf of the @param radius for channel @param spectrumChannel
             * Pdf of S_p(p_o, p_i) = S_r(|p_o - p_i|)
             * @return
             */
            virtual double subsurfaceRadiusPdf(int spectrumChannel, double radius) const = 0;

            /**
             * S_w(w_i)
             * @param wi
             * @return
             */
            virtual Spectrum subsurfaceWi(const Vector3d &wi) const;

        private:
            Vector3d _tanY;
            Vector3d _tanX;
            Vector3d _tanZ;

            Material *_material;
        };

    }
}

#endif //KAGUYA_SEPARABLEBSSRDF_H
