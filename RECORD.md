# 问题解决方案

#### Error bound 错误

- [x] 顶部区域光设置只朝下投射光线，但却把光投射到上面天花板了 - 对 Triangle 表面采样 point 时没有计算 error bound。
- [x] 金属球用的是全反射材质，但会渲染出穿透金属球的光线 - Sphere 求交线时 error bound 计算错误。
- [x] 墙壁边缘会出现莫名其妙对暗色条纹 - 计算 Ray 到 Triangle 的 step 时，没有考虑 error bound.

#### 墙壁白色曲线错误

- [x] 墙壁上会出现一条规则的白色，只在多线程条件下出现 - 多个线程共用了同一个 Sampler。