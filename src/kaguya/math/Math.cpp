//
// Created by Storm Phoenix on 2020/12/28.
//

#include <kaguya/math/Math.h>
#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace math {

        double randomDouble(double min, double max, Sampler *const sampler1D) {
            return min + (max - min) * sampler1D->sample1D();
        }

        int randomInt(int min, int max, Sampler *const sampler1D) {
            int ret = std::min(static_cast<int>(randomDouble(min, max + 1, sampler1D)), max);
            if (ret <= max) {
                return ret;
            } else {
                return randomInt(min, max, sampler1D);
            }
        }

        namespace sampling {

            typedef struct DRand48 {
                template<typename Integer>
                Integer operator()(const Integer n) const {
                    return static_cast<Integer>(drand48() * n);
                }
            } DRand48;

            Vector3d hemiCosineSampling(Sampler *const sampler) {
                Vector2d sample = sampler->sample2D();
                // fi = 2 * Pi * sampleU
                double sampleU = sample.x;
                // sampleV = sin^2(theta)
                double sampleV = sample.y;
                // x = sin(theta) * cos(fi)
                double x = sqrt(sampleV) * cos(2 * PI * sampleU);
                // y = cos(theta)
                double y = sqrt(1 - sampleV);
                // z = sin(theta) * sin(fi)
                double z = sqrt(sampleV) * sin(2 * PI * sampleU);
                return NORMALIZE(Vector3d(x, y, z));
            }

            Vector3d sphereUniformSampling(Sampler *sampler) {
                Vector2d sample = sampler->sample2D();
                // fi = 2 * Pi * sampleU
                double sampleU = sample.x;
                // 2 * sampleV = 1 - cos(theta)
                double sampleV = sample.y;

                // y = 1 - 2 * sampleV
                double y = 1 - 2 * sampleV;
                // x = sin(theta) * cos(phi)
                double x = std::sqrt(std::max(0.0, (1 - y * y))) * std::cos(2 * PI * sampleU);
                // z = sin(theta) * sin(phi)
                double z = std::sqrt(std::max(0.0, (1 - y * y))) * std::sin(2 * PI * sampleU);

                return NORMALIZE(Vector3d(x, y, z));
            }

            Vector2d diskUniformSampling(Sampler *const sampler1D, double radius) {
                // sampleY = r / Radius
                // sampleX = theta / (2 * PI)
                double sampleY = sampler1D->sample1D();
                double sampleX = sampler1D->sample1D();

                double theta = 2 * PI * sampleX;
                double r = sampleY * radius;

                return Vector2d(r * std::cos(theta), r * std::sin(theta));
            }

            Vector2d triangleUniformSampling(Sampler *sampler) {
                Vector2d sample = sampler->sample2D();
                double sampleU = sample.x;
                double sampleV = sample.y;

                double u = 1 - std::sqrt(sampleU);
                double v = sampleV * sqrt(sampleU);
                return Vector2d(u, v);
            }

            Vector3d coneUniformSampling(double cosThetaMax, Sampler *sampler) {
                Vector2d sample = sampler->sample2D();
                // phi = 2 * PI * sampleU
                double sampleU = sample.x;
                // sampleV = (1 - cos(theta)) / (1 - cos(thetaMax))
                double sampleV = sample.y;

                // 计算 cos(theta) sin(theta)
                double cosTheta = 1.0 - sampleV + sampleV * cosThetaMax;
                double sinTheta = std::sqrt(std::max(0.0, 1 - cosTheta * cosTheta));
                // 计算 cos(phi) sin(phi)
                double cosPhi = std::cos(2 * PI * sampleU);
                double sinPhi = std::sin(2 * PI * sampleU);

                // y = cos(theta)
                double y = cosTheta;
                // x = sin(theta) * cos(phi)
                double x = sinTheta * cosPhi;
                // z = sin(theta) * sin(phi)
                double z = sinTheta * sinPhi;

                return NORMALIZE(Vector3d(x, y, z));
            }

            namespace low_discrepancy {
                // low discrepancy primes
                const uint16_t primes[primeArraySize] = {
                        2, 3, 5, 7, 11,
                        // Subsequent prime numbers
                        13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89,
                        97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167,
                        173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251,
                        257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347,
                        349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433,
                        439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523,
                        541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619,
                        631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727,
                        733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827,
                        829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937,
                        941, 947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 1019, 1021, 1031,
                        1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097, 1103,
                        1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193, 1201,
                        1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289,
                        1291, 1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381,
                        1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, 1471,
                        1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531, 1543, 1549, 1553,
                        1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607, 1609, 1613, 1619,};

                /*
                1621,
                1627, 1637, 1657, 1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723,
                1733, 1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823,
                1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901, 1907, 1913,
                1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993, 1997, 1999, 2003, 2011,
                2017, 2027, 2029, 2039, 2053, 2063, 2069, 2081, 2083, 2087, 2089, 2099,
                2111, 2113, 2129, 2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207,
                2213, 2221, 2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287, 2293,
                2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357, 2371, 2377, 2381,
                2383, 2389, 2393, 2399, 2411, 2417, 2423, 2437, 2441, 2447, 2459, 2467,
                2473, 2477, 2503, 2521, 2531, 2539, 2543, 2549, 2551, 2557, 2579, 2591,
                2593, 2609, 2617, 2621, 2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683,
                2687, 2689, 2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741, 2749,
                2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819, 2833, 2837, 2843,
                2851, 2857, 2861, 2879, 2887, 2897, 2903, 2909, 2917, 2927, 2939, 2953,
                2957, 2963, 2969, 2971, 2999, 3001, 3011, 3019, 3023, 3037, 3041, 3049,
                3061, 3067, 3079, 3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169,
                3181, 3187, 3191, 3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257, 3259,
                3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329, 3331, 3343, 3347, 3359,
                3361, 3371, 3373, 3389, 3391, 3407, 3413, 3433, 3449, 3457, 3461, 3463,
                3467, 3469, 3491, 3499, 3511, 3517, 3527, 3529, 3533, 3539, 3541, 3547,
                3557, 3559, 3571, 3581, 3583, 3593, 3607, 3613, 3617, 3623, 3631, 3637,
                3643, 3659, 3671, 3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727, 3733,
                3739, 3761, 3767, 3769, 3779, 3793, 3797, 3803, 3821, 3823, 3833, 3847,
                3851, 3853, 3863, 3877, 3881, 3889, 3907, 3911, 3917, 3919, 3923, 3929,
                3931, 3943, 3947, 3967, 3989, 4001, 4003, 4007, 4013, 4019, 4021, 4027,
                4049, 4051, 4057, 4073, 4079, 4091, 4093, 4099, 4111, 4127, 4129, 4133,
                4139, 4153, 4157, 4159, 4177, 4201, 4211, 4217, 4219, 4229, 4231, 4241,
                4243, 4253, 4259, 4261, 4271, 4273, 4283, 4289, 4297, 4327, 4337, 4339,
                4349, 4357, 4363, 4373, 4391, 4397, 4409, 4421, 4423, 4441, 4447, 4451,
                4457, 4463, 4481, 4483, 4493, 4507, 4513, 4517, 4519, 4523, 4547, 4549,
                4561, 4567, 4583, 4591, 4597, 4603, 4621, 4637, 4639, 4643, 4649, 4651,
                4657, 4663, 4673, 4679, 4691, 4703, 4721, 4723, 4729, 4733, 4751, 4759,
                4783, 4787, 4789, 4793, 4799, 4801, 4813, 4817, 4831, 4861, 4871, 4877,
                4889, 4903, 4909, 4919, 4931, 4933, 4937, 4943, 4951, 4957, 4967, 4969,
                4973, 4987, 4993, 4999, 5003, 5009, 5011, 5021, 5023, 5039, 5051, 5059,
                5077, 5081, 5087, 5099, 5101, 5107, 5113, 5119, 5147, 5153, 5167, 5171,
                5179, 5189, 5197, 5209, 5227, 5231, 5233, 5237, 5261, 5273, 5279, 5281,
                5297, 5303, 5309, 5323, 5333, 5347, 5351, 5381, 5387, 5393, 5399, 5407,
                5413, 5417, 5419, 5431, 5437, 5441, 5443, 5449, 5471, 5477, 5479, 5483,
                5501, 5503, 5507, 5519, 5521, 5527, 5531, 5557, 5563, 5569, 5573, 5581,
                5591, 5623, 5639, 5641, 5647, 5651, 5653, 5657, 5659, 5669, 5683, 5689,
                5693, 5701, 5711, 5717, 5737, 5741, 5743, 5749, 5779, 5783, 5791, 5801,
                5807, 5813, 5821, 5827, 5839, 5843, 5849, 5851, 5857, 5861, 5867, 5869,
                5879, 5881, 5897, 5903, 5923, 5927, 5939, 5953, 5981, 5987, 6007, 6011,
                6029, 6037, 6043, 6047, 6053, 6067, 6073, 6079, 6089, 6091, 6101, 6113,
                6121, 6131, 6133, 6143, 6151, 6163, 6173, 6197, 6199, 6203, 6211, 6217,
                6221, 6229, 6247, 6257, 6263, 6269, 6271, 6277, 6287, 6299, 6301, 6311,
                6317, 6323, 6329, 6337, 6343, 6353, 6359, 6361, 6367, 6373, 6379, 6389,
                6397, 6421, 6427, 6449, 6451, 6469, 6473, 6481, 6491, 6521, 6529, 6547,
                6551, 6553, 6563, 6569, 6571, 6577, 6581, 6599, 6607, 6619, 6637, 6653,
                6659, 6661, 6673, 6679, 6689, 6691, 6701, 6703, 6709, 6719, 6733, 6737,
                6761, 6763, 6779, 6781, 6791, 6793, 6803, 6823, 6827, 6829, 6833, 6841,
                6857, 6863, 6869, 6871, 6883, 6899, 6907, 6911, 6917, 6947, 6949, 6959,
                6961, 6967, 6971, 6977, 6983, 6991, 6997, 7001, 7013, 7019, 7027, 7039,
                7043, 7057, 7069, 7079, 7103, 7109, 7121, 7127, 7129, 7151, 7159, 7177,
                7187, 7193, 7207, 7211, 7213, 7219, 7229, 7237, 7243, 7247, 7253, 7283,
                7297, 7307, 7309, 7321, 7331, 7333, 7349, 7351, 7369, 7393, 7411, 7417,
                7433, 7451, 7457, 7459, 7477, 7481, 7487, 7489, 7499, 7507, 7517, 7523,
                7529, 7537, 7541, 7547, 7549, 7559, 7561, 7573, 7577, 7583, 7589, 7591,
                7603, 7607, 7621, 7639, 7643, 7649, 7669, 7673, 7681, 7687, 7691, 7699,
                7703, 7717, 7723, 7727, 7741, 7753, 7757, 7759, 7789, 7793, 7817, 7823,
                7829, 7841, 7853, 7867, 7873, 7877, 7879, 7883, 7901, 7907, 7919};
                 */

                const int primeSums[primeArraySize] = {
                        0, 2, 5, 10, 17,
                        // Subsequent prime sums
                        28, 41, 58, 77, 100, 129, 160, 197, 238, 281, 328, 381, 440, 501, 568, 639,
                        712, 791, 874, 963, 1060, 1161, 1264, 1371, 1480, 1593, 1720, 1851, 1988,
                        2127, 2276, 2427, 2584, 2747, 2914, 3087, 3266, 3447, 3638, 3831, 4028,
                        4227, 4438, 4661, 4888, 5117, 5350, 5589, 5830, 6081, 6338, 6601, 6870,
                        7141, 7418, 7699, 7982, 8275, 8582, 8893, 9206, 9523, 9854, 10191, 10538,
                        10887, 11240, 11599, 11966, 12339, 12718, 13101, 13490, 13887, 14288, 14697,
                        15116, 15537, 15968, 16401, 16840, 17283, 17732, 18189, 18650, 19113, 19580,
                        20059, 20546, 21037, 21536, 22039, 22548, 23069, 23592, 24133, 24680, 25237,
                        25800, 26369, 26940, 27517, 28104, 28697, 29296, 29897, 30504, 31117, 31734,
                        32353, 32984, 33625, 34268, 34915, 35568, 36227, 36888, 37561, 38238, 38921,
                        39612, 40313, 41022, 41741, 42468, 43201, 43940, 44683, 45434, 46191, 46952,
                        47721, 48494, 49281, 50078, 50887, 51698, 52519, 53342, 54169, 54998, 55837,
                        56690, 57547, 58406, 59269, 60146, 61027, 61910, 62797, 63704, 64615, 65534,
                        66463, 67400, 68341, 69288, 70241, 71208, 72179, 73156, 74139, 75130, 76127,
                        77136, 78149, 79168, 80189, 81220, 82253, 83292, 84341, 85392, 86453, 87516,
                        88585, 89672, 90763, 91856, 92953, 94056, 95165, 96282, 97405, 98534, 99685,
                        100838, 102001, 103172, 104353, 105540, 106733, 107934, 109147, 110364,
                        111587, 112816, 114047, 115284, 116533, 117792, 119069, 120348, 121631,
                        122920, 124211, 125508, 126809, 128112, 129419, 130738, 132059, 133386,
                        134747, 136114, 137487, 138868, 140267, 141676, 143099, 144526, 145955,
                        147388, 148827, 150274, 151725, 153178, 154637, 156108, 157589, 159072,
                        160559, 162048, 163541, 165040, 166551, 168074, 169605, 171148, 172697,
                        174250, 175809, 177376, 178947, 180526, 182109, 183706, 185307, 186914,
                        188523, 190136,};

                /*
                191755, 193376, 195003, 196640, 198297, 199960, 201627,
                203296, 204989, 206686, 208385, 210094, 211815, 213538, 215271, 217012,
                218759, 220512, 222271, 224048, 225831, 227618, 229407, 231208, 233019,
                234842, 236673, 238520, 240381, 242248, 244119, 245992, 247869, 249748,
                251637, 253538, 255445, 257358, 259289, 261222, 263171, 265122, 267095,
                269074, 271061, 273054, 275051, 277050, 279053, 281064, 283081, 285108,
                287137, 289176, 291229, 293292, 295361, 297442, 299525, 301612, 303701,
                305800, 307911, 310024, 312153, 314284, 316421, 318562, 320705, 322858,
                325019, 327198, 329401, 331608, 333821, 336042, 338279, 340518, 342761,
                345012, 347279, 349548, 351821, 354102, 356389, 358682, 360979, 363288,
                365599, 367932, 370271, 372612, 374959, 377310, 379667, 382038, 384415,
                386796, 389179, 391568, 393961, 396360, 398771, 401188, 403611, 406048,
                408489, 410936, 413395, 415862, 418335, 420812, 423315, 425836, 428367,
                430906, 433449, 435998, 438549, 441106, 443685, 446276, 448869, 451478,
                454095, 456716, 459349, 461996, 464653, 467312, 469975, 472646, 475323,
                478006, 480693, 483382, 486075, 488774, 491481, 494192, 496905, 499624,
                502353, 505084, 507825, 510574, 513327, 516094, 518871, 521660, 524451,
                527248, 530049, 532852, 535671, 538504, 541341, 544184, 547035, 549892,
                552753, 555632, 558519, 561416, 564319, 567228, 570145, 573072, 576011,
                578964, 581921, 584884, 587853, 590824, 593823, 596824, 599835, 602854,
                605877, 608914, 611955, 615004, 618065, 621132, 624211, 627294, 630383,
                633492, 636611, 639732, 642869, 646032, 649199, 652368, 655549, 658736,
                661927, 665130, 668339, 671556, 674777, 678006, 681257, 684510, 687767,
                691026, 694297, 697596, 700897, 704204, 707517, 710836, 714159, 717488,
                720819, 724162, 727509, 730868, 734229, 737600, 740973, 744362, 747753,
                751160, 754573, 758006, 761455, 764912, 768373, 771836, 775303, 778772,
                782263, 785762, 789273, 792790, 796317, 799846, 803379, 806918, 810459,
                814006, 817563, 821122, 824693, 828274, 831857, 835450, 839057, 842670,
                846287, 849910, 853541, 857178, 860821, 864480, 868151, 871824, 875501,
                879192, 882889, 886590, 890299, 894018, 897745, 901478, 905217, 908978,
                912745, 916514, 920293, 924086, 927883, 931686, 935507, 939330, 943163,
                947010, 950861, 954714, 958577, 962454, 966335, 970224, 974131, 978042,
                981959, 985878, 989801, 993730, 997661, 1001604, 1005551, 1009518, 1013507,
                1017508, 1021511, 1025518, 1029531, 1033550, 1037571, 1041598, 1045647,
                1049698, 1053755, 1057828, 1061907, 1065998, 1070091, 1074190, 1078301,
                1082428, 1086557, 1090690, 1094829, 1098982, 1103139, 1107298, 1111475,
                1115676, 1119887, 1124104, 1128323, 1132552, 1136783, 1141024, 1145267,
                1149520, 1153779, 1158040, 1162311, 1166584, 1170867, 1175156, 1179453,
                1183780, 1188117, 1192456, 1196805, 1201162, 1205525, 1209898, 1214289,
                1218686, 1223095, 1227516, 1231939, 1236380, 1240827, 1245278, 1249735,
                1254198, 1258679, 1263162, 1267655, 1272162, 1276675, 1281192, 1285711,
                1290234, 1294781, 1299330, 1303891, 1308458, 1313041, 1317632, 1322229,
                1326832, 1331453, 1336090, 1340729, 1345372, 1350021, 1354672, 1359329,
                1363992, 1368665, 1373344, 1378035, 1382738, 1387459, 1392182, 1396911,
                1401644, 1406395, 1411154, 1415937, 1420724, 1425513, 1430306, 1435105,
                1439906, 1444719, 1449536, 1454367, 1459228, 1464099, 1468976, 1473865,
                1478768, 1483677, 1488596, 1493527, 1498460, 1503397, 1508340, 1513291,
                1518248, 1523215, 1528184, 1533157, 1538144, 1543137, 1548136, 1553139,
                1558148, 1563159, 1568180, 1573203, 1578242, 1583293, 1588352, 1593429,
                1598510, 1603597, 1608696, 1613797, 1618904, 1624017, 1629136, 1634283,
                1639436, 1644603, 1649774, 1654953, 1660142, 1665339, 1670548, 1675775,
                1681006, 1686239, 1691476, 1696737, 1702010, 1707289, 1712570, 1717867,
                1723170, 1728479, 1733802, 1739135, 1744482, 1749833, 1755214, 1760601,
                1765994, 1771393, 1776800, 1782213, 1787630, 1793049, 1798480, 1803917,
                1809358, 1814801, 1820250, 1825721, 1831198, 1836677, 1842160, 1847661,
                1853164, 1858671, 1864190, 1869711, 1875238, 1880769, 1886326, 1891889,
                1897458, 1903031, 1908612, 1914203, 1919826, 1925465, 1931106, 1936753,
                1942404, 1948057, 1953714, 1959373, 1965042, 1970725, 1976414, 1982107,
                1987808, 1993519, 1999236, 2004973, 2010714, 2016457, 2022206, 2027985,
                2033768, 2039559, 2045360, 2051167, 2056980, 2062801, 2068628, 2074467,
                2080310, 2086159, 2092010, 2097867, 2103728, 2109595, 2115464, 2121343,
                2127224, 2133121, 2139024, 2144947, 2150874, 2156813, 2162766, 2168747,
                2174734, 2180741, 2186752, 2192781, 2198818, 2204861, 2210908, 2216961,
                2223028, 2229101, 2235180, 2241269, 2247360, 2253461, 2259574, 2265695,
                2271826, 2277959, 2284102, 2290253, 2296416, 2302589, 2308786, 2314985,
                2321188, 2327399, 2333616, 2339837, 2346066, 2352313, 2358570, 2364833,
                2371102, 2377373, 2383650, 2389937, 2396236, 2402537, 2408848, 2415165,
                2421488, 2427817, 2434154, 2440497, 2446850, 2453209, 2459570, 2465937,
                2472310, 2478689, 2485078, 2491475, 2497896, 2504323, 2510772, 2517223,
                2523692, 2530165, 2536646, 2543137, 2549658, 2556187, 2562734, 2569285,
                2575838, 2582401, 2588970, 2595541, 2602118, 2608699, 2615298, 2621905,
                2628524, 2635161, 2641814, 2648473, 2655134, 2661807, 2668486, 2675175,
                2681866, 2688567, 2695270, 2701979, 2708698, 2715431, 2722168, 2728929,
                2735692, 2742471, 2749252, 2756043, 2762836, 2769639, 2776462, 2783289,
                2790118, 2796951, 2803792, 2810649, 2817512, 2824381, 2831252, 2838135,
                2845034, 2851941, 2858852, 2865769, 2872716, 2879665, 2886624, 2893585,
                2900552, 2907523, 2914500, 2921483, 2928474, 2935471, 2942472, 2949485,
                2956504, 2963531, 2970570, 2977613, 2984670, 2991739, 2998818, 3005921,
                3013030, 3020151, 3027278, 3034407, 3041558, 3048717, 3055894, 3063081,
                3070274, 3077481, 3084692, 3091905, 3099124, 3106353, 3113590, 3120833,
                3128080, 3135333, 3142616, 3149913, 3157220, 3164529, 3171850, 3179181,
                3186514, 3193863, 3201214, 3208583, 3215976, 3223387, 3230804, 3238237,
                3245688, 3253145, 3260604, 3268081, 3275562, 3283049, 3290538, 3298037,
                3305544, 3313061, 3320584, 3328113, 3335650, 3343191, 3350738, 3358287,
                3365846, 3373407, 3380980, 3388557, 3396140, 3403729, 3411320, 3418923,
                3426530, 3434151, 3441790, 3449433, 3457082, 3464751, 3472424, 3480105,
                3487792, 3495483, 3503182, 3510885, 3518602, 3526325, 3534052, 3541793,
                3549546, 3557303, 3565062, 3572851, 3580644, 3588461, 3596284, 3604113,
                3611954, 3619807, 3627674, 3635547, 3643424, 3651303, 3659186, 3667087,
                3674994,
        };
                 */

                unsigned short shuffle(const unsigned short base, const unsigned short digits,
                                       unsigned short index, const std::vector<unsigned short> &perm) {
                    unsigned short ret = 0;
                    for (unsigned short i = 0; i < digits; i++) {
                        ret = ret * base + perm[index % base];
                        index /= base;
                    }
                    return ret;
                }

                void randomPermutation(std::vector<std::vector<unsigned short>> &perms) {
                    const unsigned short maxBase = 1619u;
                    perms.resize(maxBase + 1);
                    DRand48 rand48;

                    for (unsigned int i = 1; i <= 3; i++) {
                        perms[i].resize(i);
                        for (unsigned int j = 0; i < j; j++) {
                            perms[i][j] = j;
                        }
                    }
                    for (unsigned int base = 4; base <= maxBase; base++) {
                        perms[base].resize(base);
                        for (unsigned i = 0; i < base; i++) {
                            perms[base][i] = i;
                        }
                        // TODO deprecated
                        std::random_shuffle(perms[base].begin(), perms[base].end(), rand48);
                    }
                }

                void faurePermutation(std::vector<std::vector<unsigned short>> &perms) {
                    const unsigned int maxBase = 1619u;
                    perms.resize(maxBase + 1);

                    // keep identity
                    for (unsigned int i = 1; i <= 3; i++) {
                        perms[i].resize(i);
                        for (unsigned int j = 0; j < i; j++) {
                            perms[i][j] = j;
                        }
                    }

                    for (unsigned int base = 4; base <= maxBase; base++) {
                        perms[base].resize(base);
                        const unsigned int mid = base / 2;
                        if (base & 1) {
                            // odd
                            for (unsigned int i = 0; i < base - 1; i++) {
                                perms[base][i + (i >= mid)] = perms[base - 1][i] + (perms[base - 1][i] >= mid);
                            }
                            perms[base][mid] = mid;
                        } else {
                            // even
                            for (unsigned int i = 0; i < mid; i++) {
                                perms[base][i] = 2 * perms[mid][i];
                                perms[base][mid + i] = 2 * perms[mid][i] + 1;
                            }
                        }
                    }
                }

                std::vector<uint16_t> computePermutationArray(bool initFaure) {
                    std::vector<std::vector<unsigned short>> perms;
                    if (initFaure) {
                        faurePermutation(perms);
                    } else {
                        randomPermutation(perms);
                    }

                    // resize permutation array size
                    int permArraySize = 0;
                    for (int i = 0; i < primeArraySize; i++) {
                        permArraySize += primeSums[i];
                    }
                    std::vector<uint16_t> ret;
                    ret.resize(permArraySize);

                    uint16_t *p = &ret[0];
                    for (int i = 0; i < primeArraySize; i++) {
                        const uint16_t base = primes[i];
                        for (uint16_t j = 0; j < base; ++j) {
                            p[j] = shuffle(base, 1, j, perms[base]);
                        }
                        p += primes[i];
                    }
                    return ret;
                };

                template<int base>
                double scrambledRadicalReverseSpecialized(uint64_t n, uint16_t *perm) {
                    const double invBase = 1.0 / base;
                    uint64_t reverse = 0;
                    double inv = 1;
                    while (n != 0) {
                        uint64_t next = n / base;
                        uint64_t rest = n - next * base;
                        reverse = reverse * base + perm[rest];
                        n = next;
                        inv *= invBase;
                    }
                    return reverse * inv;
                }

                double scrambledRadicalReverse(const int dimension, uint64_t n, uint16_t *perm) {
                    if (dimension == 0) {
                        return radicalReverse(n);
                    } else {
                        switch (dimension) {
                            case 0:
                                return scrambledRadicalReverseSpecialized<2>(n, perm);
                            case 1:
                                return scrambledRadicalReverseSpecialized<3>(n, perm);
                            case 2:
                                return scrambledRadicalReverseSpecialized<5>(n, perm);
                            case 3:
                                return scrambledRadicalReverseSpecialized<7>(n, perm);
                                // Remainder of cases for _ScrambledRadicalInverse()_
                            case 4:
                                return scrambledRadicalReverseSpecialized<11>(n, perm);
                            case 5:
                                return scrambledRadicalReverseSpecialized<13>(n, perm);
                            case 6:
                                return scrambledRadicalReverseSpecialized<17>(n, perm);
                            case 7:
                                return scrambledRadicalReverseSpecialized<19>(n, perm);
                            case 8:
                                return scrambledRadicalReverseSpecialized<23>(n, perm);
                            case 9:
                                return scrambledRadicalReverseSpecialized<29>(n, perm);
                            case 10:
                                return scrambledRadicalReverseSpecialized<31>(n, perm);
                            case 11:
                                return scrambledRadicalReverseSpecialized<37>(n, perm);
                            case 12:
                                return scrambledRadicalReverseSpecialized<41>(n, perm);
                            case 13:
                                return scrambledRadicalReverseSpecialized<43>(n, perm);
                            case 14:
                                return scrambledRadicalReverseSpecialized<47>(n, perm);
                            case 15:
                                return scrambledRadicalReverseSpecialized<53>(n, perm);
                            case 16:
                                return scrambledRadicalReverseSpecialized<59>(n, perm);
                            case 17:
                                return scrambledRadicalReverseSpecialized<61>(n, perm);
                            case 18:
                                return scrambledRadicalReverseSpecialized<67>(n, perm);
                            case 19:
                                return scrambledRadicalReverseSpecialized<71>(n, perm);
                            case 20:
                                return scrambledRadicalReverseSpecialized<73>(n, perm);
                            case 21:
                                return scrambledRadicalReverseSpecialized<79>(n, perm);
                            case 22:
                                return scrambledRadicalReverseSpecialized<83>(n, perm);
                            case 23:
                                return scrambledRadicalReverseSpecialized<89>(n, perm);
                            case 24:
                                return scrambledRadicalReverseSpecialized<97>(n, perm);
                            case 25:
                                return scrambledRadicalReverseSpecialized<101>(n, perm);
                            case 26:
                                return scrambledRadicalReverseSpecialized<103>(n, perm);
                            case 27:
                                return scrambledRadicalReverseSpecialized<107>(n, perm);
                            case 28:
                                return scrambledRadicalReverseSpecialized<109>(n, perm);
                            case 29:
                                return scrambledRadicalReverseSpecialized<113>(n, perm);
                            case 30:
                                return scrambledRadicalReverseSpecialized<127>(n, perm);
                            case 31:
                                return scrambledRadicalReverseSpecialized<131>(n, perm);
                            case 32:
                                return scrambledRadicalReverseSpecialized<137>(n, perm);
                            case 33:
                                return scrambledRadicalReverseSpecialized<139>(n, perm);
                            case 34:
                                return scrambledRadicalReverseSpecialized<149>(n, perm);
                            case 35:
                                return scrambledRadicalReverseSpecialized<151>(n, perm);
                            case 36:
                                return scrambledRadicalReverseSpecialized<157>(n, perm);
                            case 37:
                                return scrambledRadicalReverseSpecialized<163>(n, perm);
                            case 38:
                                return scrambledRadicalReverseSpecialized<167>(n, perm);
                            case 39:
                                return scrambledRadicalReverseSpecialized<173>(n, perm);
                            case 40:
                                return scrambledRadicalReverseSpecialized<179>(n, perm);
                            case 41:
                                return scrambledRadicalReverseSpecialized<181>(n, perm);
                            case 42:
                                return scrambledRadicalReverseSpecialized<191>(n, perm);
                            case 43:
                                return scrambledRadicalReverseSpecialized<193>(n, perm);
                            case 44:
                                return scrambledRadicalReverseSpecialized<197>(n, perm);
                            case 45:
                                return scrambledRadicalReverseSpecialized<199>(n, perm);
                            case 46:
                                return scrambledRadicalReverseSpecialized<211>(n, perm);
                            case 47:
                                return scrambledRadicalReverseSpecialized<223>(n, perm);
                            case 48:
                                return scrambledRadicalReverseSpecialized<227>(n, perm);
                            case 49:
                                return scrambledRadicalReverseSpecialized<229>(n, perm);
                            case 50:
                                return scrambledRadicalReverseSpecialized<233>(n, perm);
                            case 51:
                                return scrambledRadicalReverseSpecialized<239>(n, perm);
                            case 52:
                                return scrambledRadicalReverseSpecialized<241>(n, perm);
                            case 53:
                                return scrambledRadicalReverseSpecialized<251>(n, perm);
                            case 54:
                                return scrambledRadicalReverseSpecialized<257>(n, perm);
                            case 55:
                                return scrambledRadicalReverseSpecialized<263>(n, perm);
                            case 56:
                                return scrambledRadicalReverseSpecialized<269>(n, perm);
                            case 57:
                                return scrambledRadicalReverseSpecialized<271>(n, perm);
                            case 58:
                                return scrambledRadicalReverseSpecialized<277>(n, perm);
                            case 59:
                                return scrambledRadicalReverseSpecialized<281>(n, perm);
                            case 60:
                                return scrambledRadicalReverseSpecialized<283>(n, perm);
                            case 61:
                                return scrambledRadicalReverseSpecialized<293>(n, perm);
                            case 62:
                                return scrambledRadicalReverseSpecialized<307>(n, perm);
                            case 63:
                                return scrambledRadicalReverseSpecialized<311>(n, perm);
                            case 64:
                                return scrambledRadicalReverseSpecialized<313>(n, perm);
                            case 65:
                                return scrambledRadicalReverseSpecialized<317>(n, perm);
                            case 66:
                                return scrambledRadicalReverseSpecialized<331>(n, perm);
                            case 67:
                                return scrambledRadicalReverseSpecialized<337>(n, perm);
                            case 68:
                                return scrambledRadicalReverseSpecialized<347>(n, perm);
                            case 69:
                                return scrambledRadicalReverseSpecialized<349>(n, perm);
                            case 70:
                                return scrambledRadicalReverseSpecialized<353>(n, perm);
                            case 71:
                                return scrambledRadicalReverseSpecialized<359>(n, perm);
                            case 72:
                                return scrambledRadicalReverseSpecialized<367>(n, perm);
                            case 73:
                                return scrambledRadicalReverseSpecialized<373>(n, perm);
                            case 74:
                                return scrambledRadicalReverseSpecialized<379>(n, perm);
                            case 75:
                                return scrambledRadicalReverseSpecialized<383>(n, perm);
                            case 76:
                                return scrambledRadicalReverseSpecialized<389>(n, perm);
                            case 77:
                                return scrambledRadicalReverseSpecialized<397>(n, perm);
                            case 78:
                                return scrambledRadicalReverseSpecialized<401>(n, perm);
                            case 79:
                                return scrambledRadicalReverseSpecialized<409>(n, perm);
                            case 80:
                                return scrambledRadicalReverseSpecialized<419>(n, perm);
                            case 81:
                                return scrambledRadicalReverseSpecialized<421>(n, perm);
                            case 82:
                                return scrambledRadicalReverseSpecialized<431>(n, perm);
                            case 83:
                                return scrambledRadicalReverseSpecialized<433>(n, perm);
                            case 84:
                                return scrambledRadicalReverseSpecialized<439>(n, perm);
                            case 85:
                                return scrambledRadicalReverseSpecialized<443>(n, perm);
                            case 86:
                                return scrambledRadicalReverseSpecialized<449>(n, perm);
                            case 87:
                                return scrambledRadicalReverseSpecialized<457>(n, perm);
                            case 88:
                                return scrambledRadicalReverseSpecialized<461>(n, perm);
                            case 89:
                                return scrambledRadicalReverseSpecialized<463>(n, perm);
                            case 90:
                                return scrambledRadicalReverseSpecialized<467>(n, perm);
                            case 91:
                                return scrambledRadicalReverseSpecialized<479>(n, perm);
                            case 92:
                                return scrambledRadicalReverseSpecialized<487>(n, perm);
                            case 93:
                                return scrambledRadicalReverseSpecialized<491>(n, perm);
                            case 94:
                                return scrambledRadicalReverseSpecialized<499>(n, perm);
                            case 95:
                                return scrambledRadicalReverseSpecialized<503>(n, perm);
                            case 96:
                                return scrambledRadicalReverseSpecialized<509>(n, perm);
                            case 97:
                                return scrambledRadicalReverseSpecialized<521>(n, perm);
                            case 98:
                                return scrambledRadicalReverseSpecialized<523>(n, perm);
                            case 99:
                                return scrambledRadicalReverseSpecialized<541>(n, perm);
                            case 100:
                                return scrambledRadicalReverseSpecialized<547>(n, perm);
                            case 101:
                                return scrambledRadicalReverseSpecialized<557>(n, perm);
                            case 102:
                                return scrambledRadicalReverseSpecialized<563>(n, perm);
                            case 103:
                                return scrambledRadicalReverseSpecialized<569>(n, perm);
                            case 104:
                                return scrambledRadicalReverseSpecialized<571>(n, perm);
                            case 105:
                                return scrambledRadicalReverseSpecialized<577>(n, perm);
                            case 106:
                                return scrambledRadicalReverseSpecialized<587>(n, perm);
                            case 107:
                                return scrambledRadicalReverseSpecialized<593>(n, perm);
                            case 108:
                                return scrambledRadicalReverseSpecialized<599>(n, perm);
                            case 109:
                                return scrambledRadicalReverseSpecialized<601>(n, perm);
                            case 110:
                                return scrambledRadicalReverseSpecialized<607>(n, perm);
                            case 111:
                                return scrambledRadicalReverseSpecialized<613>(n, perm);
                            case 112:
                                return scrambledRadicalReverseSpecialized<617>(n, perm);
                            case 113:
                                return scrambledRadicalReverseSpecialized<619>(n, perm);
                            case 114:
                                return scrambledRadicalReverseSpecialized<631>(n, perm);
                            case 115:
                                return scrambledRadicalReverseSpecialized<641>(n, perm);
                            case 116:
                                return scrambledRadicalReverseSpecialized<643>(n, perm);
                            case 117:
                                return scrambledRadicalReverseSpecialized<647>(n, perm);
                            case 118:
                                return scrambledRadicalReverseSpecialized<653>(n, perm);
                            case 119:
                                return scrambledRadicalReverseSpecialized<659>(n, perm);
                            case 120:
                                return scrambledRadicalReverseSpecialized<661>(n, perm);
                            case 121:
                                return scrambledRadicalReverseSpecialized<673>(n, perm);
                            case 122:
                                return scrambledRadicalReverseSpecialized<677>(n, perm);
                            case 123:
                                return scrambledRadicalReverseSpecialized<683>(n, perm);
                            case 124:
                                return scrambledRadicalReverseSpecialized<691>(n, perm);
                            case 125:
                                return scrambledRadicalReverseSpecialized<701>(n, perm);
                            case 126:
                                return scrambledRadicalReverseSpecialized<709>(n, perm);
                            case 127:
                                return scrambledRadicalReverseSpecialized<719>(n, perm);
                            case 128:
                                return scrambledRadicalReverseSpecialized<727>(n, perm);
                            case 129:
                                return scrambledRadicalReverseSpecialized<733>(n, perm);
                            case 130:
                                return scrambledRadicalReverseSpecialized<739>(n, perm);
                            case 131:
                                return scrambledRadicalReverseSpecialized<743>(n, perm);
                            case 132:
                                return scrambledRadicalReverseSpecialized<751>(n, perm);
                            case 133:
                                return scrambledRadicalReverseSpecialized<757>(n, perm);
                            case 134:
                                return scrambledRadicalReverseSpecialized<761>(n, perm);
                            case 135:
                                return scrambledRadicalReverseSpecialized<769>(n, perm);
                            case 136:
                                return scrambledRadicalReverseSpecialized<773>(n, perm);
                            case 137:
                                return scrambledRadicalReverseSpecialized<787>(n, perm);
                            case 138:
                                return scrambledRadicalReverseSpecialized<797>(n, perm);
                            case 139:
                                return scrambledRadicalReverseSpecialized<809>(n, perm);
                            case 140:
                                return scrambledRadicalReverseSpecialized<811>(n, perm);
                            case 141:
                                return scrambledRadicalReverseSpecialized<821>(n, perm);
                            case 142:
                                return scrambledRadicalReverseSpecialized<823>(n, perm);
                            case 143:
                                return scrambledRadicalReverseSpecialized<827>(n, perm);
                            case 144:
                                return scrambledRadicalReverseSpecialized<829>(n, perm);
                            case 145:
                                return scrambledRadicalReverseSpecialized<839>(n, perm);
                            case 146:
                                return scrambledRadicalReverseSpecialized<853>(n, perm);
                            case 147:
                                return scrambledRadicalReverseSpecialized<857>(n, perm);
                            case 148:
                                return scrambledRadicalReverseSpecialized<859>(n, perm);
                            case 149:
                                return scrambledRadicalReverseSpecialized<863>(n, perm);
                            case 150:
                                return scrambledRadicalReverseSpecialized<877>(n, perm);
                            case 151:
                                return scrambledRadicalReverseSpecialized<881>(n, perm);
                            case 152:
                                return scrambledRadicalReverseSpecialized<883>(n, perm);
                            case 153:
                                return scrambledRadicalReverseSpecialized<887>(n, perm);
                            case 154:
                                return scrambledRadicalReverseSpecialized<907>(n, perm);
                            case 155:
                                return scrambledRadicalReverseSpecialized<911>(n, perm);
                            case 156:
                                return scrambledRadicalReverseSpecialized<919>(n, perm);
                            case 157:
                                return scrambledRadicalReverseSpecialized<929>(n, perm);
                            case 158:
                                return scrambledRadicalReverseSpecialized<937>(n, perm);
                            case 159:
                                return scrambledRadicalReverseSpecialized<941>(n, perm);
                            case 160:
                                return scrambledRadicalReverseSpecialized<947>(n, perm);
                            case 161:
                                return scrambledRadicalReverseSpecialized<953>(n, perm);
                            case 162:
                                return scrambledRadicalReverseSpecialized<967>(n, perm);
                            case 163:
                                return scrambledRadicalReverseSpecialized<971>(n, perm);
                            case 164:
                                return scrambledRadicalReverseSpecialized<977>(n, perm);
                            case 165:
                                return scrambledRadicalReverseSpecialized<983>(n, perm);
                            case 166:
                                return scrambledRadicalReverseSpecialized<991>(n, perm);
                            case 167:
                                return scrambledRadicalReverseSpecialized<997>(n, perm);
                            case 168:
                                return scrambledRadicalReverseSpecialized<1009>(n, perm);
                            case 169:
                                return scrambledRadicalReverseSpecialized<1013>(n, perm);
                            case 170:
                                return scrambledRadicalReverseSpecialized<1019>(n, perm);
                            case 171:
                                return scrambledRadicalReverseSpecialized<1021>(n, perm);
                            case 172:
                                return scrambledRadicalReverseSpecialized<1031>(n, perm);
                            case 173:
                                return scrambledRadicalReverseSpecialized<1033>(n, perm);
                            case 174:
                                return scrambledRadicalReverseSpecialized<1039>(n, perm);
                            case 175:
                                return scrambledRadicalReverseSpecialized<1049>(n, perm);
                            case 176:
                                return scrambledRadicalReverseSpecialized<1051>(n, perm);
                            case 177:
                                return scrambledRadicalReverseSpecialized<1061>(n, perm);
                            case 178:
                                return scrambledRadicalReverseSpecialized<1063>(n, perm);
                            case 179:
                                return scrambledRadicalReverseSpecialized<1069>(n, perm);
                            case 180:
                                return scrambledRadicalReverseSpecialized<1087>(n, perm);
                            case 181:
                                return scrambledRadicalReverseSpecialized<1091>(n, perm);
                            case 182:
                                return scrambledRadicalReverseSpecialized<1093>(n, perm);
                            case 183:
                                return scrambledRadicalReverseSpecialized<1097>(n, perm);
                            case 184:
                                return scrambledRadicalReverseSpecialized<1103>(n, perm);
                            case 185:
                                return scrambledRadicalReverseSpecialized<1109>(n, perm);
                            case 186:
                                return scrambledRadicalReverseSpecialized<1117>(n, perm);
                            case 187:
                                return scrambledRadicalReverseSpecialized<1123>(n, perm);
                            case 188:
                                return scrambledRadicalReverseSpecialized<1129>(n, perm);
                            case 189:
                                return scrambledRadicalReverseSpecialized<1151>(n, perm);
                            case 190:
                                return scrambledRadicalReverseSpecialized<1153>(n, perm);
                            case 191:
                                return scrambledRadicalReverseSpecialized<1163>(n, perm);
                            case 192:
                                return scrambledRadicalReverseSpecialized<1171>(n, perm);
                            case 193:
                                return scrambledRadicalReverseSpecialized<1181>(n, perm);
                            case 194:
                                return scrambledRadicalReverseSpecialized<1187>(n, perm);
                            case 195:
                                return scrambledRadicalReverseSpecialized<1193>(n, perm);
                            case 196:
                                return scrambledRadicalReverseSpecialized<1201>(n, perm);
                            case 197:
                                return scrambledRadicalReverseSpecialized<1213>(n, perm);
                            case 198:
                                return scrambledRadicalReverseSpecialized<1217>(n, perm);
                            case 199:
                                return scrambledRadicalReverseSpecialized<1223>(n, perm);
                            case 200:
                                return scrambledRadicalReverseSpecialized<1229>(n, perm);
                            case 201:
                                return scrambledRadicalReverseSpecialized<1231>(n, perm);
                            case 202:
                                return scrambledRadicalReverseSpecialized<1237>(n, perm);
                            case 203:
                                return scrambledRadicalReverseSpecialized<1249>(n, perm);
                            case 204:
                                return scrambledRadicalReverseSpecialized<1259>(n, perm);
                            case 205:
                                return scrambledRadicalReverseSpecialized<1277>(n, perm);
                            case 206:
                                return scrambledRadicalReverseSpecialized<1279>(n, perm);
                            case 207:
                                return scrambledRadicalReverseSpecialized<1283>(n, perm);
                            case 208:
                                return scrambledRadicalReverseSpecialized<1289>(n, perm);
                            case 209:
                                return scrambledRadicalReverseSpecialized<1291>(n, perm);
                            case 210:
                                return scrambledRadicalReverseSpecialized<1297>(n, perm);
                            case 211:
                                return scrambledRadicalReverseSpecialized<1301>(n, perm);
                            case 212:
                                return scrambledRadicalReverseSpecialized<1303>(n, perm);
                            case 213:
                                return scrambledRadicalReverseSpecialized<1307>(n, perm);
                            case 214:
                                return scrambledRadicalReverseSpecialized<1319>(n, perm);
                            case 215:
                                return scrambledRadicalReverseSpecialized<1321>(n, perm);
                            case 216:
                                return scrambledRadicalReverseSpecialized<1327>(n, perm);
                            case 217:
                                return scrambledRadicalReverseSpecialized<1361>(n, perm);
                            case 218:
                                return scrambledRadicalReverseSpecialized<1367>(n, perm);
                            case 219:
                                return scrambledRadicalReverseSpecialized<1373>(n, perm);
                            case 220:
                                return scrambledRadicalReverseSpecialized<1381>(n, perm);
                            case 221:
                                return scrambledRadicalReverseSpecialized<1399>(n, perm);
                            case 222:
                                return scrambledRadicalReverseSpecialized<1409>(n, perm);
                            case 223:
                                return scrambledRadicalReverseSpecialized<1423>(n, perm);
                            case 224:
                                return scrambledRadicalReverseSpecialized<1427>(n, perm);
                            case 225:
                                return scrambledRadicalReverseSpecialized<1429>(n, perm);
                            case 226:
                                return scrambledRadicalReverseSpecialized<1433>(n, perm);
                            case 227:
                                return scrambledRadicalReverseSpecialized<1439>(n, perm);
                            case 228:
                                return scrambledRadicalReverseSpecialized<1447>(n, perm);
                            case 229:
                                return scrambledRadicalReverseSpecialized<1451>(n, perm);
                            case 230:
                                return scrambledRadicalReverseSpecialized<1453>(n, perm);
                            case 231:
                                return scrambledRadicalReverseSpecialized<1459>(n, perm);
                            case 232:
                                return scrambledRadicalReverseSpecialized<1471>(n, perm);
                            case 233:
                                return scrambledRadicalReverseSpecialized<1481>(n, perm);
                            case 234:
                                return scrambledRadicalReverseSpecialized<1483>(n, perm);
                            case 235:
                                return scrambledRadicalReverseSpecialized<1487>(n, perm);
                            case 236:
                                return scrambledRadicalReverseSpecialized<1489>(n, perm);
                            case 237:
                                return scrambledRadicalReverseSpecialized<1493>(n, perm);
                            case 238:
                                return scrambledRadicalReverseSpecialized<1499>(n, perm);
                            case 239:
                                return scrambledRadicalReverseSpecialized<1511>(n, perm);
                            case 240:
                                return scrambledRadicalReverseSpecialized<1523>(n, perm);
                            case 241:
                                return scrambledRadicalReverseSpecialized<1531>(n, perm);
                            case 242:
                                return scrambledRadicalReverseSpecialized<1543>(n, perm);
                            case 243:
                                return scrambledRadicalReverseSpecialized<1549>(n, perm);
                            case 244:
                                return scrambledRadicalReverseSpecialized<1553>(n, perm);
                            case 245:
                                return scrambledRadicalReverseSpecialized<1559>(n, perm);
                            case 246:
                                return scrambledRadicalReverseSpecialized<1567>(n, perm);
                            case 247:
                                return scrambledRadicalReverseSpecialized<1571>(n, perm);
                            case 248:
                                return scrambledRadicalReverseSpecialized<1579>(n, perm);
                            case 249:
                                return scrambledRadicalReverseSpecialized<1583>(n, perm);
                            case 250:
                                return scrambledRadicalReverseSpecialized<1597>(n, perm);
                            case 251:
                                return scrambledRadicalReverseSpecialized<1601>(n, perm);
                            case 252:
                                return scrambledRadicalReverseSpecialized<1607>(n, perm);
                            case 253:
                                return scrambledRadicalReverseSpecialized<1609>(n, perm);
                            case 254:
                                return scrambledRadicalReverseSpecialized<1613>(n, perm);
                            case 255:
                                return scrambledRadicalReverseSpecialized<1619>(n, perm);
                            default:
                                // Not support
                                assert(false);
                                return 0;
                        }
                    }
                }

                // TODO delete template specialized
                double radicalReverse(const int dimension, uint64_t n) {
                    if (dimension == 0) {
                        return radicalReverse(n);
                    } else {
                        const int base = primes[dimension];
                        const double invBase = 1.0 / base;
                        uint64_t reverse = 0;
                        double inv = 1;
                        while (n != 0) {
                            uint64_t next = n / base;
                            uint64_t rest = n - next * base;
                            reverse = reverse * base + rest;
                            n = next;
                            inv *= invBase;
                        }
                        return reverse * inv;
                    }
                }

                uint32_t reverseBit32(uint32_t n) {
                    n = (n << 16) | (n >> 16);
                    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
                    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
                    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
                    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
                    return n;
                }

                uint64_t reverseBit64(uint64_t n) {
                    uint64_t left = reverseBit32(uint32_t(n));
                    uint64_t right = reverseBit32(uint32_t(n >> 32));
                    return (left << 32) | right;
                }

                double radicalReverse(uint64_t n) {
                    uint64_t rev64 = reverseBit64(n);
                    return rev64 * 0x1p-64;
                }
            }
        }
    }
}