## 修改成 pbrt 架构
-   不传输 Material 了，改为传输 BSDF，光源的判断 / 光源的发光 / 光源的采样
-   pbrt 中好像是不和光源做求交判断的


## 空间变换

## 散射 PDF 与采样 PDF
    Pdf 采样过程中应该返回一个 surfacePointPdf
    
## 对光源采样

## 背景贴图

## 已解决问题

- bunny 金属材质下非常不平滑，但是按道理讲，Normal 是进行插值过了的
    - 切线空间基坐标忘记 normalize，导致计算错误。
    
- BSDF.cpp 的构造函数，计算切线空间有误，direction 和 _tanY 如果平行，则 CROSS 计算得到 nan

- 玻璃材质球下面本应该是阴影的地方渲染成了亮白色
    Interaction 中添加了 areaLight 变量，Interaction 在场景求交的过程中传递时恰好和光源相交，areaLight 变量被赋值。但后续发现这个
    光源被遮挡，因此 Interaction 变量需要被修改，而 areaLight 变量是新添加的，应该也被修改成 nullptr，这部分代码没有添加进去导致地板
    看不到光的位置也被渲染成白色。
    
- 实现的 BDPT 算法渲染 lambertian 球时，出现棱一条明显的边界，实验情况如下：
    - 替换成 PT，没有出现上述情况
    - 移动光源位置、扩展 box 边界：不影响棱边位置
    - depth = 2：出现棱边
    - depth = 2，s = 0：无棱边
    - depth = 2，s = 1：有棱边
    - depth = 2，s = 1，MIS 默认返回 1：无棱边
        在 MIS = 1 的情况下，除了球体外的场景在视觉上看不出什么区别，唯独是边界位置，使用 MIS 明显很暗淡
    - depth = 2, s = 1，有 MIS，改变光源位置、大小，相机位置，都不会影响边界，但移动球的位置会影响边界
    - 将 s = 1 作为限制条件，看是否出现 "边界"。s = 1 的情况相当于 PT 了，
        如果继续出现边界说明 randomWalk 有问题了。
    - 已解决：cameraSubPath 起始点 PathVertex 是 Camera 类型，但在创建 camera path vertex 时没有给 PathVertex
        的 point 属性赋值。
        
- 内存溢出
    MemoryArena::clean() 将未保存的 block 指针赋值为 nullptr，造成内存溢出
    Metal::bsdf() 生成的 Fresnel * 复制给了智能指针 std::shared_ptr<Fresnel> 导致内存溢出，暂时不清楚原理。
    
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

- 条带
    图像中心出现十字条带(修改切线空间后消失，不清楚是不是这个原因)
    Debug：不知道修改了什么，这个现象消失了
    
## TODO
- Triangle Intersection 的计算方法，两种：1 对矩阵求逆 2 PBRT 中的方法

## 未解决的问题
- 玻璃球黑的不正常

- ShapeSampler 和 Shape 的功能应该合并，不然现在只能让 class Wall 成为 AreaLight。 

- BDPT 中的点光源，在采样发射光线时没有考虑 distance，那为什么地板下面出现的颜色会暗淡，靠近地板的颜色会很亮呢？
    
- randomInt 会出现随机到 randomValue = 1.0 的地方，得出的 int 值会让调用者越界

- 删除 isFrontFace

- 烟雾体积渲染

- 多光源问题

- class Light 应该允许 Intersection 操作，这样修改的话需要仔细考虑 class Light 的设计 

- 去掉 Interaction 里面的 frontFace

- bunny 透明材质出现黑块，且整体偏暗；光源面积缩小一倍，提高光源亮度，整个场景变成暗色，出现大量白色燥点；
    参考 [知乎-文刀秋二](https://www.zhihu.com/question/48961286/answer/113580178)

- Triangle 优化 Triangle 内部的矩阵变换效率（提前做变换）
    > Triangle 中的 Vertex / Normal 坐标向 World Space 的变换提前做好。不同于其他 Shape 将 Ray 转化到 Object Space
    > 的方法，Triangle 需要做大量 insect 操作，频繁对 Ray 进行变换会损失效率
    > 这也提醒自己最好将其他 Shape 的 Transformation 修改成对 Ray 的变换
- Bitmap 的写入策略替换

- OpenMP 到底是什么？应该如何配置

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

## 一些想法
- 新的光线追踪想法：不需要像 Path Tracing 一次性把 shaderOfRecursion color 求解出来，而是多次迭代。每次迭代只让光线反射一次，迭代 N 轮让
    结果收敛。 
    
- 编译 PBRT，对 SamplerIntegrator::Render() 做 debug

## 效果图

![image](https://github.com/StormPhoenix/kaguya/blob/master/image/bunny_lambertian.png)