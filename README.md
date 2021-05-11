# Kaguya Render

A simple physically based render.
   
## Processing
- [ ] 场景导入
    - [ ] XML
    - [ ] GLM 的矩阵变换好像和 kaguya 坐标系有些不匹配，需要自己改矩阵
        - [ ] LookAt 用 macro 实现
    - [ ] Camera 内部变换修改成矩阵形式
    - [x] 删除 struct Vertex
    - [x] 删除 Assimp
    - [x] 重构 TriangleMesh
    - [x] Intersectable 和 Shape 重构 (删除 Aggagrete)
    - [x] 测试 bunny 焦散，以及为什么 water-caustic 无法成像。
        - [x] photon 数太少，radius 初始太大，收敛太慢。这个配置下 CPU 跑的太慢了，是时候升级到 GPU 版本了 
    
- [ ] 添加环境光贴图
    - [ ] 考虑 EnvironmentLight(EL) 在 PT \ BDPT \ SPPM 三种情况下如何处理
        - [ ] BDPT
            - [ ] connectible() cameraPath 最后一个 Vertex 是 EL
            - [ ] connectPath() cameraPath 最后一个 Vertex 是 EL，则调过 connect 步骤
            - [ ] randomWalk() for cameraPath
        - [x] PT
            - [x] Path tracing 未击中
        - [ ] SPPM
            - [ ] Visible point 未击中
    - [ ] Light::Le() 



- [ ] FilmPlane 添加 Filter

- [ ] 构建不依赖 C++ std 的随机数生成器    

- [ ] Torrance-Sparrow Model 公式推导 - 疑惑？
    - [ ] 课上讲解的有问题，重新阅读 PBRT radiance 章节，公主 basics
    - [ ] 推导 Torrance-Sparrow Model formulation
    
- [ ] BDPT RandomWalk 添加 Russian Roullete

- [ ] BSDF 局部坐标系代码重构，不然可能会影响各向异性材质

- Sampling
- [ ] Sampler 和 sampling 方法单独放在不同文件，不要挤在 math.h 里面

- Scene
- [ ] Material-testball 场景渲染错误
    - [x] 环境光照没有生效，mirror 反射全是黑色
    - [ ] roughconductor 材质没有生效
    - [ ] 重构 Path Tracer 中环境光照的代码（不要用 background 这个东西代替

- Material
- [x] CoatingMaterial 重构，添加 Refraction 参数
    - [x] mitsuba 和 pbrt 中的 CoatingMaterial 种类要区分开 （不需要额外修改了，PatinaMaterial 属于内部 diffuse，外部 glossy 材质）
    
- [ ] Mitsuba 中的 Coating 材质研究一下

- [ ] Microfacet Material
    - [ ] 进一步理解 Mask and Shadowing 里面的 AlphaX and AlphaY (和粗糙度有关)
    - [ ] 进一步理解 Normal Distribution 里面的 sigma
    
- [ ] 重新利用 Fresnel
    - [ ] FresnelConductor 中的 K 做记录
    
- [ ] Material 结构整理 
    - [ ] Texture Texture Mapping package 结构整理
    - [ ] Medium phase function 
    
## TODO
- [ ] 修正 C++ template 特化语法
    
- [ ] HaltonSampler 代码写的可能有问题
- [ ] Triangle Intersection 的计算方法，两种：1 对矩阵求逆 2 PBRT 中的方法，写成文档记录下来
- [ ] Triangle sampling strategy 计算方法记录。

            
- [ ] HaltonSampler 代码写的可能有问题

- [ ] 添加 Stratified Sampler、Sobel Sampler

- [ ] 自己实现 Transformation，加上 Error 管理

## Unsolved
- [ ] SubsurfaceMaterial 渲染错误
    - [x] 材质的 bsdf 有没有错误（SubsurfaceMaterial 仅仅使用 bsdf，省略 bssrdf）
    - [x] sampleS()，检查 BSSRDFTable 数据、插值结果、采样的 pi (经过检查，catmull-rom、TabulatedBSSRDF、Table 赋值都没有错误)
    - [x] 采样得到的 pi.bsdf->sampleF() 经常返回 0.3 左右的值。检查是因为场景的 MODEL_SCALE 参数和 sigma_t 参数不匹配，导致射线击不中场景
    - [x] 修改 sigma_t 参数，射线依然无法击中（实际上是能击中的，但是 PT 里面的 uniformSampleLight 用的不是 rendering.normal，导致采样的光线为 0）
    - [x] Subsurface Bunny 下面会透光，这是不允许的
        - [x] 降低兔子靠近下平面，光线越来越明显(模型下面破了一个洞)
    - [x] Bunny 表面呈现血管纹路，猜测是 found、额外的 pdf 造成的 (已解决：去掉额外的 x \ z 轴的采样)
    
    - [ ] Bunny 耳朵边缘部分存在噪点，应该在采样 Sr 半径时考虑射线与 Surface 的夹角，判断使用哪一条轴做采样

- [x] Bunny 透明材质出现黑块，且整体偏暗；光源面积缩小一倍，提高光源亮度，整个场景变成暗色，出现大量白色燥点；
    参考 [知乎-文刀秋二](https://www.zhihu.com/question/48961286/answer/113580178)

- [ ] 电解质折射率问题
    无法在光线传输过程中确定碰撞位置两端的折射率。程序采用的是将外界材质的折射率固定设置为空气折射率（1.0）
    
## Solved
- [x] 添加纹理
    - [x] 添加 ImageTexture
    - [x] 简单 2D 线性映射纹理
    - [x] ImageTexture 细节检查
    - [x] 多通道纹理
    - [x] Sphere2D 映射

- [x] Photon Mapping 开发
    - [x] 研究 Photon Mapping、Progressive Photon Mapping 和 Stochastic Progressive Photon Mapping
    - [x] 开发 SPPM Tracer
    - [x] bugfix: 增加 Iteration，画面会越来越亮（FilmPlane::addSpectrum() 改成 setSpectrum()）
    - [x] bugfix: 画面出现和 tileSize 大小一致的分界线 (photon tracing pass 在不同 tile 下的 photon 分布不一致导致)

- [x] 添加 low Discrepancy 序列
    - [x] 添加 halton 序列
        - [x] 添加 halton sampling 渲染的效果反而不如 c++ 自带的随机数生成程序
        - [x] 对照 PBRT 的 HaltonSampler 、检查 halton + Faure 都没有错误 
        - [x] 使用 PT 的情况下，halton 和 random 的效果没有什么差别，仅仅只是 halton 的噪点看起来更均匀
    - [x] 添加 Faure permutation

- [x] Add multiple light source
    - [x] Delete Wall.h
    
- [x] Better space transformation
    - [x] Transform class
    - [x] Replace all occurrence.
    
- [x] Volume (smoke etc) rendering
    - [x] add medium boundary to path tracing
    - [x] add medium boundary to bi-directional path tracing
    
- [x] Shape 文件位置重构，Material \ AreaLight 属性拆解到 Geometry
    - [x] *Geometry* response for set *Material* and *MediumBound*
    - [x] merge *ShapeSampler* and *Shape*
    - [x] delete pdf-method in Shape
    - [x] when sampling the surface of shape, *Geometry* will set other attrs
    
- [x] Math.hpp function 加入 namespace，否则会与 glm 冲突

- [x] 考虑该如何设计 Ray 重的 minStep。当前问题是 Ray 的 minStep 不能为 0。如果设置成 0，就会遇到浮点误差问题。
    例如：在表面 A 向外发射射线 x，由于浮点误差 x 不在 A 的表面，而是在稍微下面一点的地方，这样导致 x 又会和表面 A 相交。
    - [x] modify the method which applied for triangle intersection, and calculate the error bound. 
    
- [x] randomInt 会出现随机到 randomValue = 1.0 的地方，得出的 int 值会让调用者越界

- [x] Smoke 的渲染改进
    - [x] Smoke 会在地板下面透射一个方形的影子，这个很不正常
    - [x] Smoke 的体积坐标变换有 bug，和场景的围墙不匹配
    - [x] 创建 Box Shape，代替围墙 Wall
    
- front 墙壁材质是 Lambertian，使用 area light 的时候，表面的光照位置光斑会非常亮（BDPT）
    - 将 BDPT 的 connectPath() 中的 t - s 路径分拆成几个子路径，分别输出结果。
    - 光斑是 t = 1，s = 2 情况下出现的。推测是 light path 连接 camera vertex 时，beta 计算错误。
    - 经过检查，发现 Camera 计算成像平面面积时少算了 3/4 的面积，修改过后，结果正确。

- [x] SPPM 渲染耗费时间太长太长了，改进 SPPM 的写法   
    - [x] Camera pass 在每个 iteration 需要并行生成 rays (按照块并行，采用 HaltonSampler，利用 seed 和 pixel 不同确定种子序列)
    - [x] Photon pass 也要并行，单枪的 photon pass 每个线程都要全部处理一次，太慢了
    
- [x] front 墙壁渲染结果会有一圈曲线（BDPT）
    - [x] 去掉 openMP，结果正常
    - [x] 加上 openMP，对访问数据上锁，不正常
    - [x] 加上 openMP，FilmPlane 的写入函数添加 lock，结果不正常
    - [x] 调整 openMP 运行线程数，线程数目小，结果会相对正常
    - [x] 编写多线程并行，不使用 OpenMP。依然出现白色曲线
    - [x] 移动墙壁位置，白色曲线位置变化，但形状不变
    - [x] 移动点光源位置，白色曲线位置变化，但形状不变
    - [x] 猜测是点光源采样函数有问题，偏向某个方向采样。修改 0-1 均匀采样函数，
        白色曲线不仅位置变化，形状也变化。
    - [x] 结果显示 c++ 随机数生成器 std::uniform_distribution 生成点随机数并不随机，只在单线程条件下随机
        无法在多线程环境下被共享。
    - [x] Solved, 让每个线程使用单独的随机数生成器，结果终于正常。

- bunny 金属材质下非常不平滑，但是按道理讲，Normal 是进行插值过了的
    - 切线空间基坐标忘记 normalize，导致计算错误。
    
- BSDF.cpp 的构造函数，计算切线空间有误，direction 和 _tanY 如果平行，则 CROSS 计算得到 nan

- 玻璃材质球下面本应该是阴影的地方渲染成了亮白色
    Interaction 中添加了 areaLight 变量，Interaction 在场景求交的过程中传递时恰好和光源相交，areaLight 变量被赋值。但后续发现这个
    光源被遮挡，因此 Interaction 变量需要被修改，而 areaLight 变量是新添加的，应该也被修改成 nullptr，这部分代码没有添加进去导致地板
    看不到光的位置也被渲染成白色。
    
- [x] 实现的 BDPT 算法渲染 lambertian 球时，出现棱一条明显的边界，实验情况如下：
    - [x] 替换成 PT，没有出现上述情况
    - [x] 移动光源位置、扩展 box 边界：不影响棱边位置
    - [x] depth = 2：出现棱边
    - [x] depth = 2，s = 0：无棱边
    - [x] depth = 2，s = 1：有棱边
    - [x] depth = 2，s = 1，MIS 默认返回 1：无棱边
        在 MIS = 1 的情况下，除了球体外的场景在视觉上看不出什么区别，唯独是边界位置，使用 MIS 明显很暗淡
    - [x] depth = 2, s = 1，有 MIS，改变光源位置、大小，相机位置，都不会影响边界，但移动球的位置会影响边界
    - [x] 将 s = 1 作为限制条件，看是否出现 "边界"。s = 1 的情况相当于 PT 了，
        如果继续出现边界说明 randomWalk 有问题了。
    - [x] Solved：cameraSubPath 起始点 PathVertex 是 Camera 类型，但在创建 camera path vertex 时没有给 PathVertex
        的 point 属性赋值。
        
- [x] Memory leak
    - [x] MemoryArena::clean() 将未保存的 block 指针赋值为 nullptr，造成内存溢出
    - [x] Metal::bsdf() 生成的 Fresnel * 复制给了智能指针 std::shared_ptr<Fresnel> 导致内存溢出，暂时不清楚原理。
    
- [x] Black spot 玻璃球、金属球表面的反射会出现黑斑
    - [x] Debug：把金属球和玻璃球交换位置，看是否还有黑斑出来 （金属球在在光源边界依然出现黑斑，但玻璃球没有出现黑斑）
    - [x] Debug：背景颜色修改成白色，黑斑点变成白色
    - [x] Debug：对 (459, 451) (459, 556) (516, 498) 两处坐标打断点 （依据PDF采样时，切线空间的计算没有单位化）

- [x] Cornel Box 的 Front 墙面中心总是黑的，看起来就像磨砂金属
    - [x] Debug：不对光源采样，采用随机采样，对中心点打断点 (漫反射物体的PDF函数在计算切线空间时没有单位化)

- [x] 射线从内部击中金属球
    - [x] Debug：击中金属球的射线，其 step 非常小，且起始位置就在金属球表面，击中点也是次位置附近。（float 类型导致精度损失，用 double 即可）

- [x] Triangle 内部的法线变换是不对的 (采用 inverse transpose 对法线做变换)

- [x] 条带
    图像中心出现十字条带(修改切线空间后消失，不清楚是不是这个原因)
    Debug：不知道修改了什么，这个现象消失了

## Taking Notes
- [ ] Triangle Intersection 的计算方法，两种：1 对矩阵求逆 2 PBRT 中的方法，写成文档记录下来
- [ ] Medium sampling method.
- [ ] Regular tracing, ray marching, delta tracking, ratio tracking article
    - [ ] Delta tracing - reject sampling, Delta tracing's sampling strategy is a little different 
            from reject sampling.
- [ ] AABB intersection.
- [ ] Jacobi matrix

## 一些想法
- 新的光线追踪想法：不需要像 Path Tracing 一次性把 shaderOfRecursion color 求解出来，而是多次迭代。每次迭代只让光线反射一次，迭代 N 轮让
    结果收敛。 
    
- Compile PBRT，and debug SamplerIntegrator::Render()

## Example

<img src="https://github.com/StormPhoenix/render-repository-data/blob/master/thumbnail/scene_bunny-with-point-light2_ssp=300_max-depth=15_render-type=bdpt_1500x1500.jpg" alt="bunny-glass-point-light" width="50%">
<img src="https://github.com/StormPhoenix/render-repository-data/blob/master/thumbnail/scene_light_through-air_ssp=200_max-depth=12_render-type=bdpt_1200x1200.jpg" alt="light-through-air" width="50%">
<img src="https://github.com/StormPhoenix/render-repository-data/blob/master/thumbnail/scene_smoke-with-area-light_ssp=200_max-depth=12_render-type=bdpt_1000x1000.jpg" alt="smoke" width="50%">