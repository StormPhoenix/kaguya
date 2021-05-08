//
// Created by Storm Phoenix on 2020/10/16.
//

#include <kaguya/core/bsdf/BSDF.h>

namespace kaguya {
	namespace core {
        namespace bsdf {
            BSDF::BSDF(const Interaction &interaction) {
                // Build tangent space
                _tanY = NORMALIZE(interaction.rendering.normal);
                _tanZ = NORMALIZE(CROSS(interaction.direction, _tanY));
                if (!math::isValid(_tanZ)) {
                    math::tangentSpace(_tanY, &_tanX, &_tanZ);
                } else {
                    _tanX = NORMALIZE(CROSS(_tanY, _tanZ));
                }
                _ng = interaction.normal;
            }

            Vector3F BSDF::toObjectSpace(const Vector3F &v) const {
                return Vector3F(DOT(_tanX, v),
                                DOT(_tanY, v),
                                DOT(_tanZ, v));
            }

            Vector3F BSDF::toWorldSpace(const Vector3F &v) const {
                return Vector3F(_tanX.x * v.x + _tanY.x * v.y + _tanZ.x * v.z,
                                _tanX.y * v.x + _tanY.y * v.y + _tanZ.y * v.z,
                                _tanX.z * v.x + _tanY.z * v.y + _tanZ.z * v.z);
            }

            void BSDF::addBXDF(BXDF *bxdf) {
                assert(_bxdfCount < MAX_BXDF_NUM && bxdf != nullptr);
                _bxdfs[_bxdfCount] = bxdf;
                _bxdfCount++;
            }

            Spectrum BSDF::f(const Vector3F &worldWo, const Vector3F &worldWi, BXDFType type) const {
                Vector3F wo = toObjectSpace(worldWo);
                Vector3F wi = toObjectSpace(worldWi);

                // 局部空间法向可以用 rendering.normal，检查反射 / 折射方向用 _ng
                bool reflect = DOT(worldWo, _ng) * DOT(worldWi, _ng) > 0;
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

            Spectrum BSDF::sampleF(const Vector3F &worldWo, Vector3F *worldWi, Float *pdf,
                                   sampler::Sampler *const sampler,
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
                    int bxdfOrder = math::randomInt(1, matchedCount, sampler);
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
                    Vector3F wo = toObjectSpace(worldWo);
                    Vector3F wi = Vector3F(0.0f);
                    Float samplePdf;
                    Spectrum f = bxdf->sampleF(wo, &wi, &samplePdf, sampler, sampleType);
                    // 一般来说 surfacePointPdf = 0 的情况不会发生
                    if (samplePdf == 0) {
                        if (sampleType != nullptr) {
                            *sampleType = BXDFType(0);
                        }
                        return Spectrum(0.0);
                    }

                    // 计算最终 sample pdf
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

                    (*worldWi) = toWorldSpace(wi);

                    // 计算 f 的总和
                    /* 计算 f 的总和一直觉得有问题有问题，如果处理不当 f 的加和是会大于 1 的，
                    * 参考 pbrt 中 MixMaterial 中的方法，当多个 BXDF 加在一起是会进行加权的，其加权和为 1 */
                    if (!bxdf->hasAllOf(BSDF_SPECULAR)) {
                        for (int i = 0; i < _bxdfCount; i++) {
                            bool reflect = DOT(worldWo, _ng) * DOT(*worldWi, _ng) > 0;
                            if (_bxdfs[i] != nullptr && _bxdfs[i] != bxdf) {
                                if ((reflect && _bxdfs[i]->hasAllOf(BSDF_REFLECTION)) ||
                                    (!reflect && _bxdfs[i]->hasAllOf(BSDF_TRANSMISSION))) {
                                    f += _bxdfs[i]->f(wo, wi);
                                }
                            }
                        }
                    }
                    (*pdf) = samplePdf;
                    return f;
                }
            }

            Float BSDF::samplePdf(const Vector3F &worldWo, const Vector3F &worldWi, BXDFType type) const {
                if (_bxdfCount == 0) {
                    return 0.f;
                }
                Vector3F wo = toObjectSpace(worldWo);
                Vector3F wi = toObjectSpace(worldWi);
                if (std::abs(wo.y - 0) < math::EPSILON) {
                    return 0.0;
                }
                Float pdf = 0.0f;
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
}