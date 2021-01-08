//
// Created by Storm Phoenix on 2020/10/16.
//

#include <kaguya/core/bsdf/BSDF.h>

namespace kaguya {
    namespace core {

        BSDF::BSDF(const Interaction &insect) {
            // 构造切线空间
            /* 此处的实现和 pbrt 中的实现不同，pbrt 用的是 dudp 和纹理相关，但目前没有实现纹理部分
             * 暂时用入射光线和法线来构造切线空间*/
            _tanY = NORMALIZE(insect.getNormal());
            _tanZ = NORMALIZE(CROSS(insect.getDirection(), _tanY));
            if (!math::isValid(_tanZ)) {
                math::tangentSpace(_tanY, &_tanX, &_tanZ);
            } else {
                _tanX = NORMALIZE(CROSS(_tanY, _tanZ));
            }
        }

        Vector3d BSDF::toObjectSpace(const Vector3d &v) const {
            return Vector3d(DOT(_tanX, v),
                            DOT(_tanY, v),
                            DOT(_tanZ, v));
        }

        Vector3d BSDF::toWorldSpace(const Vector3d &v) const {
            return Vector3d(_tanX.x * v.x + _tanY.x * v.y + _tanZ.x * v.z,
                           _tanX.y * v.x + _tanY.y * v.y + _tanZ.y * v.z,
                           _tanX.z * v.x + _tanY.z * v.y + _tanZ.z * v.z);
        }

        void BSDF::addBXDF(BXDF *bxdf) {
            assert(_bxdfCount < MAX_BXDF_NUM && bxdf != nullptr);
            _bxdfs[_bxdfCount] = bxdf;
            _bxdfCount++;
        }

        Spectrum BSDF::f(const Vector3d &worldWo, const Vector3d &worldWi, BXDFType type) const {
            Vector3d wo = toObjectSpace(worldWo);
            Vector3d wi = toObjectSpace(worldWi);

            bool reflect = wo.y * wi.y > 0;
            Spectrum f = 0;
            for (int i = 0; i < _bxdfCount; i++) {
                if (_bxdfs[i]->allIncludeOf(type)) {
                    if ((reflect && _bxdfs[i]->hasAllOf(BSDF_REFLECTION)) ||
                        (!reflect && _bxdfs[i]->hasAllOf(BSDF_TRANSMISSION))) {
                        f += _bxdfs[i]->f(wo, wi);
                    }
                }
            }
            return f;
        }

        Spectrum BSDF::sampleF(const Vector3d &worldWo, Vector3d *worldWi, double *pdf,
                               math::random::Sampler *const sampler1D,
                               BXDFType type, BXDFType *sampleType) const {
            // 找到符合类型的 BXDF，并随机选择一个做 sampleF
            int matchedCount = 0;
            for (int i = 0; i < _bxdfCount; i++) {
                if (_bxdfs[i] != nullptr && _bxdfs[i]->allIncludeOf(type)) {
                    matchedCount++;
                }
            }

            if (matchedCount == 0) {
                // 没有类型被匹配上
                if (sampleType != nullptr) {
                    *sampleType = BXDFType(0);
                }
                return Spectrum(0.0);
            } else {
                BXDF *bxdf = nullptr;
                // 随机选取 bxdf
                int bxdfOrder = math::randomInt(1, matchedCount, sampler1D);
                int order = 0;
                for (int i = 0; i < _bxdfCount; i++) {
                    if (_bxdfs[i] != nullptr && _bxdfs[i]->allIncludeOf(type)) {
                        order++;
                        if (order == bxdfOrder) {
                            // 找到选中的 bxdf
                            bxdf = _bxdfs[i];
                            break;
                        }
                    }
                }
                assert(bxdf != nullptr);
                if (sampleType != nullptr) {
                    *sampleType = bxdf->type;
                }

                // 匹配成功，开始采样 bxdf 值
                Vector3d wo = toObjectSpace(worldWo);
                Vector3d wi = Vector3d(0.0f);
                double samplePdf;
                Spectrum f = bxdf->sampleF(wo, &wi, &samplePdf, sampler1D);
                // 一般来说 surfacePointPdf = 0 的情况不会发生
                if (samplePdf == 0) {
                    return Spectrum(0.0);
                }

                // 计算最终 samplePdf
                if (!bxdf->hasAllOf(BSDF_SPECULAR) && matchedCount > 1) {
                    // 如果是 specular 类型，则可以不用进行额外计算
                    for (int i = 0; i < _bxdfCount; i++) {
                        if (_bxdfs[i] != nullptr && _bxdfs[i] != bxdf && _bxdfs[i]->allIncludeOf(type)) {
                            samplePdf += _bxdfs[i]->samplePdf(wo, wi);
                        }
                    }
                }

                // 计算最终 samplePdf 采用的办法是均匀在多个 BXDF 中采样，更好的办法是根据 BXDF 的分布去按照概率采样
                if (matchedCount > 1) {
                    samplePdf /= matchedCount;
                }

                // 计算 f 的总和
                /* 计算 f 的总和一直觉得有问题有问题，如果处理不当 f 的加和是会大于 1 的，
                 * 参考 pbrt 中 MixMaterial 中的方法，当多个 BXDF 加在一起是会进行加权的，其加权和为 1 */
                if (!bxdf->hasAllOf(BSDF_SPECULAR)) {
                    for (int i = 0; i < _bxdfCount; i++) {
                        bool reflect = wi.y * wo.y > 0 ? true : false;
                        if (_bxdfs[i] != nullptr && _bxdfs[i] != bxdf) {
                            if ((reflect && _bxdfs[i]->hasAllOf(BSDF_REFLECTION)) ||
                                (!reflect && _bxdfs[i]->hasAllOf(BSDF_TRANSMISSION))) {
                                f += _bxdfs[i]->f(wo, wi);
                            }
                        }
                    }
                }
                (*worldWi) = toWorldSpace(wi);
                (*pdf) = samplePdf;
                return f;
            }
        }

        double BSDF::samplePdf(const Vector3d &worldWo, const Vector3d &worldWi, BXDFType type) const {
            if (_bxdfCount == 0) {
                return 0.f;
            }
            Vector3d wo = toObjectSpace(worldWo);
            Vector3d wi = toObjectSpace(worldWi);
            if (std::abs(wo.y - 0) < math::EPSILON) {
                return 0.0;
            }
            double pdf = 0.0f;
            int matchCount = 0;
            for (int i = 0; i < _bxdfCount; ++i)
                if (_bxdfs[i]->allIncludeOf(type)) {
                    matchCount++;
                    pdf += _bxdfs[i]->samplePdf(wo, wi);
                }
            pdf = matchCount > 0 ? pdf / matchCount : 0.f;
            return pdf;
        }

        int BSDF::allIncludeOf(BXDFType bxdfType) {
            int ans = 0;
            for (int i = 0; i < _bxdfCount; i++) {
                if (_bxdfs[i]->allIncludeOf(bxdfType)) {
                    ans++;
                }
            }
            return ans;
        }

        int BSDF::hasAllOf(BXDFType bxdfType) {
            int ret = 0;
            for (int i = 0; i < _bxdfCount; i++) {
                if (_bxdfs[i]->hasAllOf(bxdfType)) {
                    ret++;
                }
            }
            return ret;
        }

        int BSDF::hasAnyOf(const BXDFType bxdfType) const {
            int ret = 0;
            for (int i = 0; i < _bxdfCount; i++) {
                if (_bxdfs[i]->hasAnyOf(bxdfType)) {
                    ret++;
                }
            }
            return ret;
        }
    }
}