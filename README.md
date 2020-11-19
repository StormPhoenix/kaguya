## 已解决问题

- front 墙壁材质是 Lambertian，使用 area light 的时候，表面的光照位置光斑会非常亮（BDPT）
    - 将 BDPT 的 connectPath() 中的 t - s 路径分拆成几个子路径，分别输出结果。
    - 光斑是 t = 1，s = 2 情况下出现的。推测是 light path 连接 camera vertex 时，beta 计算错误。
    - 经过检查，发现 Camera 计算成像平面面积时少算了 3/4 的面积，修改过后，结果正确。

- front 墙壁渲染结果会有一圈曲线（BDPT）
    - 去掉 openMP，结果正常
    - 加上 openMP，对访问数据上锁，不正常
    - 加上 openMP，FilmPlane 的写入函数添加 lock，结果不正常
    - 调整 openMP 运行线程数，线程数目小，结果会相对正常
    - 编写多线程并行，不使用 OpenMP。依然出现白色曲线
    - 移动墙壁位置，白色曲线位置变化，但形状不变
    - 移动点光源位置，白色曲线位置变化，但形状不变
    - 猜测是点光源采样函数有问题，偏向某个方向采样。修改 0-1 均匀采样函数，
        白色曲线不仅位置变化，形状也变化。
    - 结果显示 c++ 随机数生成器 std::uniform_distribution 生成点随机数并不随机，只在单线程条件下随机
        无法在多线程环境下被共享。
    - 让每个线程使用单独的随机数生成器，结果终于正常。

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

- ShapeSampler 和 Shape 的功能应该合并，不然现在只能让 class Wall 成为 AreaLight。 

- randomInt 会出现随机到 randomValue = 1.0 的地方，得出的 int 值会让调用者越界

- 删除 isFrontFace

- 烟雾体积渲染

- 多光源问题

- 去掉 Interaction 里面的 frontFace

- bunny 透明材质出现黑块，且整体偏暗；光源面积缩小一倍，提高光源亮度，整个场景变成暗色，出现大量白色燥点；
    参考 [知乎-文刀秋二](https://www.zhihu.com/question/48961286/answer/113580178)

- 电解质折射率问题
    无法在光线传输过程中确定碰撞位置两端的折射率。程序采用的是将外界材质的折射率固定设置为空气折射率（1.0）
    
- 只考虑了单个光源情况

## 空间变换

## 背景贴图

## 一些想法
- 新的光线追踪想法：不需要像 Path Tracing 一次性把 shaderOfRecursion color 求解出来，而是多次迭代。每次迭代只让光线反射一次，迭代 N 轮让
    结果收敛。 
    
- 编译 PBRT，对 SamplerIntegrator::Render() 做 debug

## 效果图

#### path tracing

<img src="https://github.com/StormPhoenix/render-repository-data/blob/master/path-tracing/bunny_lambertian.png" alt="bunny" width="50%">
<img src="https://github.com/StormPhoenix/render-repository-data/blob/master/path-tracing/two-spheres-with-area-light_ssp300_depth15_pt_1000x1000.png" alt="bunny" width="50%">
<img src="https://github.com/StormPhoenix/render-repository-data/blob/master/path-tracing/two-spheres-with-point-light_ssp300_depth%3D15_pt_1000x1000.png" alt="bunny" width="50%">

#### bidirectional path tracing