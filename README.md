## 修改成 pbrt 架构
-   不传输 Material 了，改为传输 BSDF，光源的判断 / 光源的发光 / 光源的采样
-   pbrt 中好像是不和光源做求交判断的


## 空间变换

## 散射 PDF 与采样 PDF
    Pdf 采样过程中应该返回一个 pdf
    
## 对光源采样

## 背景贴图

## 已解决问题
- 黑斑
    玻璃球、金属球表面的反射会出现黑斑
    
    Debug：把金属球和玻璃球交换位置，看是否还有黑斑出来 （金属球在在光源边界依然出现黑斑，但玻璃球没有出现黑斑）
    Debug：背景颜色修改成白色，黑斑点变成白色
    Debug：对 (459, 451) (459, 556) (516, 498) 两处坐标打断点 （依据PDF采样时，切线空间的计算没有单位化）

- Cornel Box 的 Front 墙面中心总是黑的，看起来就像磨砂金属
    Debug：不对光源采样，采用随机采样，对中心点打断点 (漫反射物体的PDF函数在计算切线空间时没有单位化)

- 射线从内部击中金属球
    Debug：击中金属球的射线，其 step 非常小，且起始位置就在金属球表面，击中点也是次位置附近。（float 类型导致精度损失，用 double 即可）

- Triangle 内部的法线变换是不对的 (采用 inverse transpose 对法线做变换)

## TODO
- Triangle Intersection 的计算方法，两种：1 对矩阵求逆 2 PBRT 中的方法

## 未解决的问题
- class Light 应该允许 Intersection 操作，这样修改的话需要仔细考虑 class Light 的设计 

- 去掉 Interaction 里面的 frontFace

- *动态内存分配*

- RGB 扩展为 SpectrumTemplate

- bunny 金属材质下非常不平滑，但是按道理讲，Normal 是进行插值过了的
    > DEBUG：设置成 Lambertian 表面做插值

- bunny 透明材质出现黑块，且整体偏暗；光源面积缩小一倍，提高光源亮度，整个场景变成暗色，出现大量白色燥点；
    参考 (知乎-文刀秋二)[https://www.zhihu.com/question/48961286/answer/113580178]

- Triangle 优化 Triangle 内部的矩阵变换效率（提前做变换）
    > Triangle 中的 Vertex / Normal 坐标向 World Space 的变换提前做好。不同于其他 Shape 将 Ray 转化到 Object Space
    > 的方法，Triangle 需要做大量 insect 操作，频繁对 Ray 进行变换会损失效率
    > 这也提醒自己最好将其他 Shape 的 Transformation 修改成对 Ray 的变换
- Bitmap 的写入策略替换

- OpenMP 到底是什么？应该如何配置

- 条带
    图像中心出现十字条带(修改切线空间后消失，不清楚是不是这个原因)

- Shadow Ray 
   - 先阅读 PBRT 中的 Path Tracing 部分再说
   - Dielectric 材质，采用自带 PDF 散射
   - 非 Dielectric
        - 单光源
            - 采样 Shadow Ray，Shadow Ray 判定是否可以对光源采样（无遮挡/击中正光面），是：依概率选择是否对光源采样，计算混合 PDF；否：计算自带 PDF
        - 多光源
            - 只采样单个光源，则可以随机选取一个光源，按照上述步骤进行即可

- 电解质折射率问题
    无法在光线传输过程中确定碰撞位置两端的折射率。程序采用的是将外界材质的折射率固定设置为空气折射率（1.0）
    
- 只考虑了单个光源情况

- 添加多种光的类型

- 俄罗斯轮盘赌不只适用于判断何时处理不继续递归问题。还可以根据 BRDF 函数，取消掉一部分 Shadow Ray

- RGB 类型 -> SpectrumTemplate 类型
    评估 Light Unit 的指标
    
## 一些想法
- 新的光线追踪想法：不需要像 Path Tracing 一次性把 shaderOfRecursion color 求解出来，而是多次迭代。每次迭代只让光线反射一次，迭代 N 轮让
    结果收敛。 
    
- 编译 PBRT，对 SamplerIntegrator::Render() 做 debug

## 效果图

![image](https://github.com/StormPhoenix/kaguya/blob/master/image/bunny_lambertian.png)