//
// Created by Storm Phoenix on 2020/12/28.
//

#include <kaguya/Common.h>
#include <kaguya/math/Math.h>
#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace math {
        Float randomDouble(Float min, Float max, Sampler *const sampler) {
            return min + (max - min) * sampler->sample1D();
        }

        int randomInt(int min, int max, Sampler *const sampler) {
            int ret = std::min(static_cast<int>(randomDouble(min, max + 1, sampler)), max);
            if (ret <= max) {
                return ret;
            } else {
                return randomInt(min, max, sampler);
            }
        }

        namespace sampling {

            typedef struct DRand48 {
				const long long m = 0x100000000LL;
				const long long c = 0xb16;
				const long long a = 0x5deece66dLL;
				unsigned long long seed = 1;

                template<typename Integer>
                Integer operator()(const Integer n) {
					seed = (a * seed + c) & 0xffffffffffffLL;
					unsigned int x = seed >> 16;
					double randSeed = (double(x) / double(m));
                    return static_cast<Integer>(randSeed * n);
                }
            } DRand48;

            Vector3F hemiCosineSampling(Sampler *const sampler) {
                Vector2F sample = sampler->sample2D();
                // fi = 2 * Pi * sampleU
                Float sampleU = sample.x;
                // sampleV = sin^2(theta)
                Float sampleV = sample.y;
                // x = sin(theta) * cos(fi)
                Float x = sqrt(sampleV) * cos(2 * PI * sampleU);
                // y = cos(theta)
                Float y = sqrt(1 - sampleV);
                // z = sin(theta) * sin(fi)
                Float z = sqrt(sampleV) * sin(2 * PI * sampleU);
                return NORMALIZE(Vector3F(x, y, z));
            }

            Vector3F sphereUniformSampling(Sampler *sampler) {
                Vector2F sample = sampler->sample2D();
                // fi = 2 * Pi * sampleU
                Float sampleU = sample.x;
                // 2 * sampleV = 1 - cos(theta)
                Float sampleV = sample.y;

                // y = 1 - 2 * sampleV
                Float y = 1 - 2 * sampleV;
                // x = sin(theta) * cos(phi)
                Float x = std::sqrt(std::max(Float(0.), (1 - y * y))) * std::cos(2 * PI * sampleU);
                // z = sin(theta) * sin(phi)
                Float z = std::sqrt(std::max(Float(0.), (1 - y * y))) * std::sin(2 * PI * sampleU);

                return NORMALIZE(Vector3F(x, y, z));
            }

            Vector2F diskUniformSampling(Sampler *const sampler, Float radius) {
                // sampleY = r / Radius
                // sampleX = theta / (2 * PI)
                Float sampleY = sampler->sample1D();
                Float sampleX = sampler->sample1D();

                Float theta = 2 * PI * sampleX;
                Float r = sampleY * radius;

                return Vector2F(r * std::cos(theta), r * std::sin(theta));
            }

            Vector2F triangleUniformSampling(Sampler *sampler) {
                Vector2F sample = sampler->sample2D();
                Float sampleU = sample.x;
                Float sampleV = sample.y;

                Float u = 1 - std::sqrt(sampleU);
                Float v = sampleV * sqrt(sampleU);
                return Vector2F(u, v);
            }

            Vector3F coneUniformSampling(Float cosThetaMax, Sampler *sampler) {
                Vector2F sample = sampler->sample2D();
                // phi = 2 * PI * sampleU
                Float sampleU = sample.x;
                // sampleV = (1 - cos(theta)) / (1 - cos(thetaMax))
                Float sampleV = sample.y;

                // 计算 cos(theta) sin(theta)
                Float cosTheta = 1.0 - sampleV + sampleV * cosThetaMax;
                Float sinTheta = std::sqrt(std::max(Float(0.), 1 - cosTheta * cosTheta));
                // 计算 cos(phi) sin(phi)
                Float cosPhi = std::cos(2 * PI * sampleU);
                Float sinPhi = std::sin(2 * PI * sampleU);

                // y = cos(theta)
                Float y = cosTheta;
                // x = sin(theta) * cos(phi)
                Float x = sinTheta * cosPhi;
                // z = sin(theta) * sin(phi)
                Float z = sinTheta * sinPhi;

                return NORMALIZE(Vector3F(x, y, z));
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
                        std::random_shuffle(perms[base].begin(), perms[base].end(), rand48);
//                        std::shuffle(perms[base].begin(), perms[base].end(), rand48);
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
                Float scrambledRadicalReverseSpecialized(uint64_t n, uint16_t *perm) {
                    const Float invBase = 1.0 / base;
                    uint64_t reverse = 0;
                    Float inv = 1;
                    while (n != 0) {
                        uint64_t next = n / base;
                        uint64_t rest = n - next * base;
                        reverse = reverse * base + perm[rest];
                        n = next;
                        inv *= invBase;
                    }
                    return reverse * inv;
                }

                Float scrambledRadicalReverse(const int dimension, uint64_t n, uint16_t *perm) {
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

                template<int base>
                Float radicalReverseSpecialized(uint64_t n) {
                    const Float invBase = (Float) 1 / (Float) base;
                    uint64_t reversedDigits = 0;
                    Float invBaseN = 1;
                    while (n) {
                        uint64_t next = n / base;
                        uint64_t digit = n - next * base;
                        reversedDigits = reversedDigits * base + digit;
                        invBaseN *= invBase;
                        n = next;
                    }
                    return std::min(reversedDigits * invBaseN, ONE_MINUS_EPSILON);
                }

                // TODO delete template specialized
                Float radicalReverse(const int dimension, uint64_t a) {
                    if (dimension == 0) {
                        return radicalReverse(a);
                    } else {
                        switch (dimension) {
                            case 1:
                                return radicalReverseSpecialized<3>(a);
                            case 2:
                                return radicalReverseSpecialized<5>(a);
                            case 3:
                                return radicalReverseSpecialized<7>(a);
                                // Remainder of cases for _RadicalInverse()_
                            case 4:
                                return radicalReverseSpecialized<11>(a);
                            case 5:
                                return radicalReverseSpecialized<13>(a);
                            case 6:
                                return radicalReverseSpecialized<17>(a);
                            case 7:
                                return radicalReverseSpecialized<19>(a);
                            case 8:
                                return radicalReverseSpecialized<23>(a);
                            case 9:
                                return radicalReverseSpecialized<29>(a);
                            case 10:
                                return radicalReverseSpecialized<31>(a);
                            case 11:
                                return radicalReverseSpecialized<37>(a);
                            case 12:
                                return radicalReverseSpecialized<41>(a);
                            case 13:
                                return radicalReverseSpecialized<43>(a);
                            case 14:
                                return radicalReverseSpecialized<47>(a);
                            case 15:
                                return radicalReverseSpecialized<53>(a);
                            case 16:
                                return radicalReverseSpecialized<59>(a);
                            case 17:
                                return radicalReverseSpecialized<61>(a);
                            case 18:
                                return radicalReverseSpecialized<67>(a);
                            case 19:
                                return radicalReverseSpecialized<71>(a);
                            case 20:
                                return radicalReverseSpecialized<73>(a);
                            case 21:
                                return radicalReverseSpecialized<79>(a);
                            case 22:
                                return radicalReverseSpecialized<83>(a);
                            case 23:
                                return radicalReverseSpecialized<89>(a);
                            case 24:
                                return radicalReverseSpecialized<97>(a);
                            case 25:
                                return radicalReverseSpecialized<101>(a);
                            case 26:
                                return radicalReverseSpecialized<103>(a);
                            case 27:
                                return radicalReverseSpecialized<107>(a);
                            case 28:
                                return radicalReverseSpecialized<109>(a);
                            case 29:
                                return radicalReverseSpecialized<113>(a);
                            case 30:
                                return radicalReverseSpecialized<127>(a);
                            case 31:
                                return radicalReverseSpecialized<131>(a);
                            case 32:
                                return radicalReverseSpecialized<137>(a);
                            case 33:
                                return radicalReverseSpecialized<139>(a);
                            case 34:
                                return radicalReverseSpecialized<149>(a);
                            case 35:
                                return radicalReverseSpecialized<151>(a);
                            case 36:
                                return radicalReverseSpecialized<157>(a);
                            case 37:
                                return radicalReverseSpecialized<163>(a);
                            case 38:
                                return radicalReverseSpecialized<167>(a);
                            case 39:
                                return radicalReverseSpecialized<173>(a);
                            case 40:
                                return radicalReverseSpecialized<179>(a);
                            case 41:
                                return radicalReverseSpecialized<181>(a);
                            case 42:
                                return radicalReverseSpecialized<191>(a);
                            case 43:
                                return radicalReverseSpecialized<193>(a);
                            case 44:
                                return radicalReverseSpecialized<197>(a);
                            case 45:
                                return radicalReverseSpecialized<199>(a);
                            case 46:
                                return radicalReverseSpecialized<211>(a);
                            case 47:
                                return radicalReverseSpecialized<223>(a);
                            case 48:
                                return radicalReverseSpecialized<227>(a);
                            case 49:
                                return radicalReverseSpecialized<229>(a);
                            case 50:
                                return radicalReverseSpecialized<233>(a);
                            case 51:
                                return radicalReverseSpecialized<239>(a);
                            case 52:
                                return radicalReverseSpecialized<241>(a);
                            case 53:
                                return radicalReverseSpecialized<251>(a);
                            case 54:
                                return radicalReverseSpecialized<257>(a);
                            case 55:
                                return radicalReverseSpecialized<263>(a);
                            case 56:
                                return radicalReverseSpecialized<269>(a);
                            case 57:
                                return radicalReverseSpecialized<271>(a);
                            case 58:
                                return radicalReverseSpecialized<277>(a);
                            case 59:
                                return radicalReverseSpecialized<281>(a);
                            case 60:
                                return radicalReverseSpecialized<283>(a);
                            case 61:
                                return radicalReverseSpecialized<293>(a);
                            case 62:
                                return radicalReverseSpecialized<307>(a);
                            case 63:
                                return radicalReverseSpecialized<311>(a);
                            case 64:
                                return radicalReverseSpecialized<313>(a);
                            case 65:
                                return radicalReverseSpecialized<317>(a);
                            case 66:
                                return radicalReverseSpecialized<331>(a);
                            case 67:
                                return radicalReverseSpecialized<337>(a);
                            case 68:
                                return radicalReverseSpecialized<347>(a);
                            case 69:
                                return radicalReverseSpecialized<349>(a);
                            case 70:
                                return radicalReverseSpecialized<353>(a);
                            case 71:
                                return radicalReverseSpecialized<359>(a);
                            case 72:
                                return radicalReverseSpecialized<367>(a);
                            case 73:
                                return radicalReverseSpecialized<373>(a);
                            case 74:
                                return radicalReverseSpecialized<379>(a);
                            case 75:
                                return radicalReverseSpecialized<383>(a);
                            case 76:
                                return radicalReverseSpecialized<389>(a);
                            case 77:
                                return radicalReverseSpecialized<397>(a);
                            case 78:
                                return radicalReverseSpecialized<401>(a);
                            case 79:
                                return radicalReverseSpecialized<409>(a);
                            case 80:
                                return radicalReverseSpecialized<419>(a);
                            case 81:
                                return radicalReverseSpecialized<421>(a);
                            case 82:
                                return radicalReverseSpecialized<431>(a);
                            case 83:
                                return radicalReverseSpecialized<433>(a);
                            case 84:
                                return radicalReverseSpecialized<439>(a);
                            case 85:
                                return radicalReverseSpecialized<443>(a);
                            case 86:
                                return radicalReverseSpecialized<449>(a);
                            case 87:
                                return radicalReverseSpecialized<457>(a);
                            case 88:
                                return radicalReverseSpecialized<461>(a);
                            case 89:
                                return radicalReverseSpecialized<463>(a);
                            case 90:
                                return radicalReverseSpecialized<467>(a);
                            case 91:
                                return radicalReverseSpecialized<479>(a);
                            case 92:
                                return radicalReverseSpecialized<487>(a);
                            case 93:
                                return radicalReverseSpecialized<491>(a);
                            case 94:
                                return radicalReverseSpecialized<499>(a);
                            case 95:
                                return radicalReverseSpecialized<503>(a);
                            case 96:
                                return radicalReverseSpecialized<509>(a);
                            case 97:
                                return radicalReverseSpecialized<521>(a);
                            case 98:
                                return radicalReverseSpecialized<523>(a);
                            case 99:
                                return radicalReverseSpecialized<541>(a);
                            case 100:
                                return radicalReverseSpecialized<547>(a);
                            case 101:
                                return radicalReverseSpecialized<557>(a);
                            case 102:
                                return radicalReverseSpecialized<563>(a);
                            case 103:
                                return radicalReverseSpecialized<569>(a);
                            case 104:
                                return radicalReverseSpecialized<571>(a);
                            case 105:
                                return radicalReverseSpecialized<577>(a);
                            case 106:
                                return radicalReverseSpecialized<587>(a);
                            case 107:
                                return radicalReverseSpecialized<593>(a);
                            case 108:
                                return radicalReverseSpecialized<599>(a);
                            case 109:
                                return radicalReverseSpecialized<601>(a);
                            case 110:
                                return radicalReverseSpecialized<607>(a);
                            case 111:
                                return radicalReverseSpecialized<613>(a);
                            case 112:
                                return radicalReverseSpecialized<617>(a);
                            case 113:
                                return radicalReverseSpecialized<619>(a);
                            case 114:
                                return radicalReverseSpecialized<631>(a);
                            case 115:
                                return radicalReverseSpecialized<641>(a);
                            case 116:
                                return radicalReverseSpecialized<643>(a);
                            case 117:
                                return radicalReverseSpecialized<647>(a);
                            case 118:
                                return radicalReverseSpecialized<653>(a);
                            case 119:
                                return radicalReverseSpecialized<659>(a);
                            case 120:
                                return radicalReverseSpecialized<661>(a);
                            case 121:
                                return radicalReverseSpecialized<673>(a);
                            case 122:
                                return radicalReverseSpecialized<677>(a);
                            case 123:
                                return radicalReverseSpecialized<683>(a);
                            case 124:
                                return radicalReverseSpecialized<691>(a);
                            case 125:
                                return radicalReverseSpecialized<701>(a);
                            case 126:
                                return radicalReverseSpecialized<709>(a);
                            case 127:
                                return radicalReverseSpecialized<719>(a);
                            case 128:
                                return radicalReverseSpecialized<727>(a);
                            case 129:
                                return radicalReverseSpecialized<733>(a);
                            case 130:
                                return radicalReverseSpecialized<739>(a);
                            case 131:
                                return radicalReverseSpecialized<743>(a);
                            case 132:
                                return radicalReverseSpecialized<751>(a);
                            case 133:
                                return radicalReverseSpecialized<757>(a);
                            case 134:
                                return radicalReverseSpecialized<761>(a);
                            case 135:
                                return radicalReverseSpecialized<769>(a);
                            case 136:
                                return radicalReverseSpecialized<773>(a);
                            case 137:
                                return radicalReverseSpecialized<787>(a);
                            case 138:
                                return radicalReverseSpecialized<797>(a);
                            case 139:
                                return radicalReverseSpecialized<809>(a);
                            case 140:
                                return radicalReverseSpecialized<811>(a);
                            case 141:
                                return radicalReverseSpecialized<821>(a);
                            case 142:
                                return radicalReverseSpecialized<823>(a);
                            case 143:
                                return radicalReverseSpecialized<827>(a);
                            case 144:
                                return radicalReverseSpecialized<829>(a);
                            case 145:
                                return radicalReverseSpecialized<839>(a);
                            case 146:
                                return radicalReverseSpecialized<853>(a);
                            case 147:
                                return radicalReverseSpecialized<857>(a);
                            case 148:
                                return radicalReverseSpecialized<859>(a);
                            case 149:
                                return radicalReverseSpecialized<863>(a);
                            case 150:
                                return radicalReverseSpecialized<877>(a);
                            case 151:
                                return radicalReverseSpecialized<881>(a);
                            case 152:
                                return radicalReverseSpecialized<883>(a);
                            case 153:
                                return radicalReverseSpecialized<887>(a);
                            case 154:
                                return radicalReverseSpecialized<907>(a);
                            case 155:
                                return radicalReverseSpecialized<911>(a);
                            case 156:
                                return radicalReverseSpecialized<919>(a);
                            case 157:
                                return radicalReverseSpecialized<929>(a);
                            case 158:
                                return radicalReverseSpecialized<937>(a);
                            case 159:
                                return radicalReverseSpecialized<941>(a);
                            case 160:
                                return radicalReverseSpecialized<947>(a);
                            case 161:
                                return radicalReverseSpecialized<953>(a);
                            case 162:
                                return radicalReverseSpecialized<967>(a);
                            case 163:
                                return radicalReverseSpecialized<971>(a);
                            case 164:
                                return radicalReverseSpecialized<977>(a);
                            case 165:
                                return radicalReverseSpecialized<983>(a);
                            case 166:
                                return radicalReverseSpecialized<991>(a);
                            case 167:
                                return radicalReverseSpecialized<997>(a);
                            case 168:
                                return radicalReverseSpecialized<1009>(a);
                            case 169:
                                return radicalReverseSpecialized<1013>(a);
                            case 170:
                                return radicalReverseSpecialized<1019>(a);
                            case 171:
                                return radicalReverseSpecialized<1021>(a);
                            case 172:
                                return radicalReverseSpecialized<1031>(a);
                            case 173:
                                return radicalReverseSpecialized<1033>(a);
                            case 174:
                                return radicalReverseSpecialized<1039>(a);
                            case 175:
                                return radicalReverseSpecialized<1049>(a);
                            case 176:
                                return radicalReverseSpecialized<1051>(a);
                            case 177:
                                return radicalReverseSpecialized<1061>(a);
                            case 178:
                                return radicalReverseSpecialized<1063>(a);
                            case 179:
                                return radicalReverseSpecialized<1069>(a);
                            case 180:
                                return radicalReverseSpecialized<1087>(a);
                            case 181:
                                return radicalReverseSpecialized<1091>(a);
                            case 182:
                                return radicalReverseSpecialized<1093>(a);
                            case 183:
                                return radicalReverseSpecialized<1097>(a);
                            case 184:
                                return radicalReverseSpecialized<1103>(a);
                            case 185:
                                return radicalReverseSpecialized<1109>(a);
                            case 186:
                                return radicalReverseSpecialized<1117>(a);
                            case 187:
                                return radicalReverseSpecialized<1123>(a);
                            case 188:
                                return radicalReverseSpecialized<1129>(a);
                            case 189:
                                return radicalReverseSpecialized<1151>(a);
                            case 190:
                                return radicalReverseSpecialized<1153>(a);
                            case 191:
                                return radicalReverseSpecialized<1163>(a);
                            case 192:
                                return radicalReverseSpecialized<1171>(a);
                            case 193:
                                return radicalReverseSpecialized<1181>(a);
                            case 194:
                                return radicalReverseSpecialized<1187>(a);
                            case 195:
                                return radicalReverseSpecialized<1193>(a);
                            case 196:
                                return radicalReverseSpecialized<1201>(a);
                            case 197:
                                return radicalReverseSpecialized<1213>(a);
                            case 198:
                                return radicalReverseSpecialized<1217>(a);
                            case 199:
                                return radicalReverseSpecialized<1223>(a);
                            case 200:
                                return radicalReverseSpecialized<1229>(a);
                            case 201:
                                return radicalReverseSpecialized<1231>(a);
                            case 202:
                                return radicalReverseSpecialized<1237>(a);
                            case 203:
                                return radicalReverseSpecialized<1249>(a);
                            case 204:
                                return radicalReverseSpecialized<1259>(a);
                            case 205:
                                return radicalReverseSpecialized<1277>(a);
                            case 206:
                                return radicalReverseSpecialized<1279>(a);
                            case 207:
                                return radicalReverseSpecialized<1283>(a);
                            case 208:
                                return radicalReverseSpecialized<1289>(a);
                            case 209:
                                return radicalReverseSpecialized<1291>(a);
                            case 210:
                                return radicalReverseSpecialized<1297>(a);
                            case 211:
                                return radicalReverseSpecialized<1301>(a);
                            case 212:
                                return radicalReverseSpecialized<1303>(a);
                            case 213:
                                return radicalReverseSpecialized<1307>(a);
                            case 214:
                                return radicalReverseSpecialized<1319>(a);
                            case 215:
                                return radicalReverseSpecialized<1321>(a);
                            case 216:
                                return radicalReverseSpecialized<1327>(a);
                            case 217:
                                return radicalReverseSpecialized<1361>(a);
                            case 218:
                                return radicalReverseSpecialized<1367>(a);
                            case 219:
                                return radicalReverseSpecialized<1373>(a);
                            case 220:
                                return radicalReverseSpecialized<1381>(a);
                            case 221:
                                return radicalReverseSpecialized<1399>(a);
                            case 222:
                                return radicalReverseSpecialized<1409>(a);
                            case 223:
                                return radicalReverseSpecialized<1423>(a);
                            case 224:
                                return radicalReverseSpecialized<1427>(a);
                            case 225:
                                return radicalReverseSpecialized<1429>(a);
                            case 226:
                                return radicalReverseSpecialized<1433>(a);
                            case 227:
                                return radicalReverseSpecialized<1439>(a);
                            case 228:
                                return radicalReverseSpecialized<1447>(a);
                            case 229:
                                return radicalReverseSpecialized<1451>(a);
                            case 230:
                                return radicalReverseSpecialized<1453>(a);
                            case 231:
                                return radicalReverseSpecialized<1459>(a);
                            case 232:
                                return radicalReverseSpecialized<1471>(a);
                            case 233:
                                return radicalReverseSpecialized<1481>(a);
                            case 234:
                                return radicalReverseSpecialized<1483>(a);
                            case 235:
                                return radicalReverseSpecialized<1487>(a);
                            case 236:
                                return radicalReverseSpecialized<1489>(a);
                            case 237:
                                return radicalReverseSpecialized<1493>(a);
                            case 238:
                                return radicalReverseSpecialized<1499>(a);
                            case 239:
                                return radicalReverseSpecialized<1511>(a);
                            case 240:
                                return radicalReverseSpecialized<1523>(a);
                            case 241:
                                return radicalReverseSpecialized<1531>(a);
                            case 242:
                                return radicalReverseSpecialized<1543>(a);
                            case 243:
                                return radicalReverseSpecialized<1549>(a);
                            case 244:
                                return radicalReverseSpecialized<1553>(a);
                            case 245:
                                return radicalReverseSpecialized<1559>(a);
                            case 246:
                                return radicalReverseSpecialized<1567>(a);
                            case 247:
                                return radicalReverseSpecialized<1571>(a);
                            case 248:
                                return radicalReverseSpecialized<1579>(a);
                            case 249:
                                return radicalReverseSpecialized<1583>(a);
                            case 250:
                                return radicalReverseSpecialized<1597>(a);
                            case 251:
                                return radicalReverseSpecialized<1601>(a);
                            case 252:
                                return radicalReverseSpecialized<1607>(a);
                            case 253:
                                return radicalReverseSpecialized<1609>(a);
                            case 254:
                                return radicalReverseSpecialized<1613>(a);
                            case 255:
                                return radicalReverseSpecialized<1619>(a);
                            case 256:
                                return radicalReverseSpecialized<1621>(a);
                            case 257:
                                return radicalReverseSpecialized<1627>(a);
                            case 258:
                                return radicalReverseSpecialized<1637>(a);
                            case 259:
                                return radicalReverseSpecialized<1657>(a);
                            case 260:
                                return radicalReverseSpecialized<1663>(a);
                            case 261:
                                return radicalReverseSpecialized<1667>(a);
                            case 262:
                                return radicalReverseSpecialized<1669>(a);
                            case 263:
                                return radicalReverseSpecialized<1693>(a);
                            case 264:
                                return radicalReverseSpecialized<1697>(a);
                            case 265:
                                return radicalReverseSpecialized<1699>(a);
                            case 266:
                                return radicalReverseSpecialized<1709>(a);
                            case 267:
                                return radicalReverseSpecialized<1721>(a);
                            case 268:
                                return radicalReverseSpecialized<1723>(a);
                            case 269:
                                return radicalReverseSpecialized<1733>(a);
                            case 270:
                                return radicalReverseSpecialized<1741>(a);
                            case 271:
                                return radicalReverseSpecialized<1747>(a);
                            case 272:
                                return radicalReverseSpecialized<1753>(a);
                            case 273:
                                return radicalReverseSpecialized<1759>(a);
                            case 274:
                                return radicalReverseSpecialized<1777>(a);
                            case 275:
                                return radicalReverseSpecialized<1783>(a);
                            case 276:
                                return radicalReverseSpecialized<1787>(a);
                            case 277:
                                return radicalReverseSpecialized<1789>(a);
                            case 278:
                                return radicalReverseSpecialized<1801>(a);
                            case 279:
                                return radicalReverseSpecialized<1811>(a);
                            case 280:
                                return radicalReverseSpecialized<1823>(a);
                            case 281:
                                return radicalReverseSpecialized<1831>(a);
                            case 282:
                                return radicalReverseSpecialized<1847>(a);
                            case 283:
                                return radicalReverseSpecialized<1861>(a);
                            case 284:
                                return radicalReverseSpecialized<1867>(a);
                            case 285:
                                return radicalReverseSpecialized<1871>(a);
                            case 286:
                                return radicalReverseSpecialized<1873>(a);
                            case 287:
                                return radicalReverseSpecialized<1877>(a);
                            case 288:
                                return radicalReverseSpecialized<1879>(a);
                            case 289:
                                return radicalReverseSpecialized<1889>(a);
                            case 290:
                                return radicalReverseSpecialized<1901>(a);
                            case 291:
                                return radicalReverseSpecialized<1907>(a);
                            case 292:
                                return radicalReverseSpecialized<1913>(a);
                            case 293:
                                return radicalReverseSpecialized<1931>(a);
                            case 294:
                                return radicalReverseSpecialized<1933>(a);
                            case 295:
                                return radicalReverseSpecialized<1949>(a);
                            case 296:
                                return radicalReverseSpecialized<1951>(a);
                            case 297:
                                return radicalReverseSpecialized<1973>(a);
                            case 298:
                                return radicalReverseSpecialized<1979>(a);
                            case 299:
                                return radicalReverseSpecialized<1987>(a);
                            case 300:
                                return radicalReverseSpecialized<1993>(a);
                            case 301:
                                return radicalReverseSpecialized<1997>(a);
                            case 302:
                                return radicalReverseSpecialized<1999>(a);
                            case 303:
                                return radicalReverseSpecialized<2003>(a);
                            case 304:
                                return radicalReverseSpecialized<2011>(a);
                            case 305:
                                return radicalReverseSpecialized<2017>(a);
                            case 306:
                                return radicalReverseSpecialized<2027>(a);
                            case 307:
                                return radicalReverseSpecialized<2029>(a);
                            case 308:
                                return radicalReverseSpecialized<2039>(a);
                            case 309:
                                return radicalReverseSpecialized<2053>(a);
                            case 310:
                                return radicalReverseSpecialized<2063>(a);
                            case 311:
                                return radicalReverseSpecialized<2069>(a);
                            case 312:
                                return radicalReverseSpecialized<2081>(a);
                            case 313:
                                return radicalReverseSpecialized<2083>(a);
                            case 314:
                                return radicalReverseSpecialized<2087>(a);
                            case 315:
                                return radicalReverseSpecialized<2089>(a);
                            case 316:
                                return radicalReverseSpecialized<2099>(a);
                            case 317:
                                return radicalReverseSpecialized<2111>(a);
                            case 318:
                                return radicalReverseSpecialized<2113>(a);
                            case 319:
                                return radicalReverseSpecialized<2129>(a);
                            case 320:
                                return radicalReverseSpecialized<2131>(a);
                            case 321:
                                return radicalReverseSpecialized<2137>(a);
                            case 322:
                                return radicalReverseSpecialized<2141>(a);
                            case 323:
                                return radicalReverseSpecialized<2143>(a);
                            case 324:
                                return radicalReverseSpecialized<2153>(a);
                            case 325:
                                return radicalReverseSpecialized<2161>(a);
                            case 326:
                                return radicalReverseSpecialized<2179>(a);
                            case 327:
                                return radicalReverseSpecialized<2203>(a);
                            case 328:
                                return radicalReverseSpecialized<2207>(a);
                            case 329:
                                return radicalReverseSpecialized<2213>(a);
                            case 330:
                                return radicalReverseSpecialized<2221>(a);
                            case 331:
                                return radicalReverseSpecialized<2237>(a);
                            case 332:
                                return radicalReverseSpecialized<2239>(a);
                            case 333:
                                return radicalReverseSpecialized<2243>(a);
                            case 334:
                                return radicalReverseSpecialized<2251>(a);
                            case 335:
                                return radicalReverseSpecialized<2267>(a);
                            case 336:
                                return radicalReverseSpecialized<2269>(a);
                            case 337:
                                return radicalReverseSpecialized<2273>(a);
                            case 338:
                                return radicalReverseSpecialized<2281>(a);
                            case 339:
                                return radicalReverseSpecialized<2287>(a);
                            case 340:
                                return radicalReverseSpecialized<2293>(a);
                            case 341:
                                return radicalReverseSpecialized<2297>(a);
                            case 342:
                                return radicalReverseSpecialized<2309>(a);
                            case 343:
                                return radicalReverseSpecialized<2311>(a);
                            case 344:
                                return radicalReverseSpecialized<2333>(a);
                            case 345:
                                return radicalReverseSpecialized<2339>(a);
                            case 346:
                                return radicalReverseSpecialized<2341>(a);
                            case 347:
                                return radicalReverseSpecialized<2347>(a);
                            case 348:
                                return radicalReverseSpecialized<2351>(a);
                            case 349:
                                return radicalReverseSpecialized<2357>(a);
                            case 350:
                                return radicalReverseSpecialized<2371>(a);
                            case 351:
                                return radicalReverseSpecialized<2377>(a);
                            case 352:
                                return radicalReverseSpecialized<2381>(a);
                            case 353:
                                return radicalReverseSpecialized<2383>(a);
                            case 354:
                                return radicalReverseSpecialized<2389>(a);
                            case 355:
                                return radicalReverseSpecialized<2393>(a);
                            case 356:
                                return radicalReverseSpecialized<2399>(a);
                            case 357:
                                return radicalReverseSpecialized<2411>(a);
                            case 358:
                                return radicalReverseSpecialized<2417>(a);
                            case 359:
                                return radicalReverseSpecialized<2423>(a);
                            case 360:
                                return radicalReverseSpecialized<2437>(a);
                            case 361:
                                return radicalReverseSpecialized<2441>(a);
                            case 362:
                                return radicalReverseSpecialized<2447>(a);
                            case 363:
                                return radicalReverseSpecialized<2459>(a);
                            case 364:
                                return radicalReverseSpecialized<2467>(a);
                            case 365:
                                return radicalReverseSpecialized<2473>(a);
                            case 366:
                                return radicalReverseSpecialized<2477>(a);
                            case 367:
                                return radicalReverseSpecialized<2503>(a);
                            case 368:
                                return radicalReverseSpecialized<2521>(a);
                            case 369:
                                return radicalReverseSpecialized<2531>(a);
                            case 370:
                                return radicalReverseSpecialized<2539>(a);
                            case 371:
                                return radicalReverseSpecialized<2543>(a);
                            case 372:
                                return radicalReverseSpecialized<2549>(a);
                            case 373:
                                return radicalReverseSpecialized<2551>(a);
                            case 374:
                                return radicalReverseSpecialized<2557>(a);
                            case 375:
                                return radicalReverseSpecialized<2579>(a);
                            case 376:
                                return radicalReverseSpecialized<2591>(a);
                            case 377:
                                return radicalReverseSpecialized<2593>(a);
                            case 378:
                                return radicalReverseSpecialized<2609>(a);
                            case 379:
                                return radicalReverseSpecialized<2617>(a);
                            case 380:
                                return radicalReverseSpecialized<2621>(a);
                            case 381:
                                return radicalReverseSpecialized<2633>(a);
                            case 382:
                                return radicalReverseSpecialized<2647>(a);
                            case 383:
                                return radicalReverseSpecialized<2657>(a);
                            case 384:
                                return radicalReverseSpecialized<2659>(a);
                            case 385:
                                return radicalReverseSpecialized<2663>(a);
                            case 386:
                                return radicalReverseSpecialized<2671>(a);
                            case 387:
                                return radicalReverseSpecialized<2677>(a);
                            case 388:
                                return radicalReverseSpecialized<2683>(a);
                            case 389:
                                return radicalReverseSpecialized<2687>(a);
                            case 390:
                                return radicalReverseSpecialized<2689>(a);
                            case 391:
                                return radicalReverseSpecialized<2693>(a);
                            case 392:
                                return radicalReverseSpecialized<2699>(a);
                            case 393:
                                return radicalReverseSpecialized<2707>(a);
                            case 394:
                                return radicalReverseSpecialized<2711>(a);
                            case 395:
                                return radicalReverseSpecialized<2713>(a);
                            case 396:
                                return radicalReverseSpecialized<2719>(a);
                            case 397:
                                return radicalReverseSpecialized<2729>(a);
                            case 398:
                                return radicalReverseSpecialized<2731>(a);
                            case 399:
                                return radicalReverseSpecialized<2741>(a);
                            case 400:
                                return radicalReverseSpecialized<2749>(a);
                            case 401:
                                return radicalReverseSpecialized<2753>(a);
                            case 402:
                                return radicalReverseSpecialized<2767>(a);
                            case 403:
                                return radicalReverseSpecialized<2777>(a);
                            case 404:
                                return radicalReverseSpecialized<2789>(a);
                            case 405:
                                return radicalReverseSpecialized<2791>(a);
                            case 406:
                                return radicalReverseSpecialized<2797>(a);
                            case 407:
                                return radicalReverseSpecialized<2801>(a);
                            case 408:
                                return radicalReverseSpecialized<2803>(a);
                            case 409:
                                return radicalReverseSpecialized<2819>(a);
                            case 410:
                                return radicalReverseSpecialized<2833>(a);
                            case 411:
                                return radicalReverseSpecialized<2837>(a);
                            case 412:
                                return radicalReverseSpecialized<2843>(a);
                            case 413:
                                return radicalReverseSpecialized<2851>(a);
                            case 414:
                                return radicalReverseSpecialized<2857>(a);
                            case 415:
                                return radicalReverseSpecialized<2861>(a);
                            case 416:
                                return radicalReverseSpecialized<2879>(a);
                            case 417:
                                return radicalReverseSpecialized<2887>(a);
                            case 418:
                                return radicalReverseSpecialized<2897>(a);
                            case 419:
                                return radicalReverseSpecialized<2903>(a);
                            case 420:
                                return radicalReverseSpecialized<2909>(a);
                            case 421:
                                return radicalReverseSpecialized<2917>(a);
                            case 422:
                                return radicalReverseSpecialized<2927>(a);
                            case 423:
                                return radicalReverseSpecialized<2939>(a);
                            case 424:
                                return radicalReverseSpecialized<2953>(a);
                            case 425:
                                return radicalReverseSpecialized<2957>(a);
                            case 426:
                                return radicalReverseSpecialized<2963>(a);
                            case 427:
                                return radicalReverseSpecialized<2969>(a);
                            case 428:
                                return radicalReverseSpecialized<2971>(a);
                            case 429:
                                return radicalReverseSpecialized<2999>(a);
                            case 430:
                                return radicalReverseSpecialized<3001>(a);
                            case 431:
                                return radicalReverseSpecialized<3011>(a);
                            case 432:
                                return radicalReverseSpecialized<3019>(a);
                            case 433:
                                return radicalReverseSpecialized<3023>(a);
                            case 434:
                                return radicalReverseSpecialized<3037>(a);
                            case 435:
                                return radicalReverseSpecialized<3041>(a);
                            case 436:
                                return radicalReverseSpecialized<3049>(a);
                            case 437:
                                return radicalReverseSpecialized<3061>(a);
                            case 438:
                                return radicalReverseSpecialized<3067>(a);
                            case 439:
                                return radicalReverseSpecialized<3079>(a);
                            case 440:
                                return radicalReverseSpecialized<3083>(a);
                            case 441:
                                return radicalReverseSpecialized<3089>(a);
                            case 442:
                                return radicalReverseSpecialized<3109>(a);
                            case 443:
                                return radicalReverseSpecialized<3119>(a);
                            case 444:
                                return radicalReverseSpecialized<3121>(a);
                            case 445:
                                return radicalReverseSpecialized<3137>(a);
                            case 446:
                                return radicalReverseSpecialized<3163>(a);
                            case 447:
                                return radicalReverseSpecialized<3167>(a);
                            case 448:
                                return radicalReverseSpecialized<3169>(a);
                            case 449:
                                return radicalReverseSpecialized<3181>(a);
                            case 450:
                                return radicalReverseSpecialized<3187>(a);
                            case 451:
                                return radicalReverseSpecialized<3191>(a);
                            case 452:
                                return radicalReverseSpecialized<3203>(a);
                            case 453:
                                return radicalReverseSpecialized<3209>(a);
                            case 454:
                                return radicalReverseSpecialized<3217>(a);
                            case 455:
                                return radicalReverseSpecialized<3221>(a);
                            case 456:
                                return radicalReverseSpecialized<3229>(a);
                            case 457:
                                return radicalReverseSpecialized<3251>(a);
                            case 458:
                                return radicalReverseSpecialized<3253>(a);
                            case 459:
                                return radicalReverseSpecialized<3257>(a);
                            case 460:
                                return radicalReverseSpecialized<3259>(a);
                            case 461:
                                return radicalReverseSpecialized<3271>(a);
                            case 462:
                                return radicalReverseSpecialized<3299>(a);
                            case 463:
                                return radicalReverseSpecialized<3301>(a);
                            case 464:
                                return radicalReverseSpecialized<3307>(a);
                            case 465:
                                return radicalReverseSpecialized<3313>(a);
                            case 466:
                                return radicalReverseSpecialized<3319>(a);
                            case 467:
                                return radicalReverseSpecialized<3323>(a);
                            case 468:
                                return radicalReverseSpecialized<3329>(a);
                            case 469:
                                return radicalReverseSpecialized<3331>(a);
                            case 470:
                                return radicalReverseSpecialized<3343>(a);
                            case 471:
                                return radicalReverseSpecialized<3347>(a);
                            case 472:
                                return radicalReverseSpecialized<3359>(a);
                            case 473:
                                return radicalReverseSpecialized<3361>(a);
                            case 474:
                                return radicalReverseSpecialized<3371>(a);
                            case 475:
                                return radicalReverseSpecialized<3373>(a);
                            case 476:
                                return radicalReverseSpecialized<3389>(a);
                            case 477:
                                return radicalReverseSpecialized<3391>(a);
                            case 478:
                                return radicalReverseSpecialized<3407>(a);
                            case 479:
                                return radicalReverseSpecialized<3413>(a);
                            case 480:
                                return radicalReverseSpecialized<3433>(a);
                            case 481:
                                return radicalReverseSpecialized<3449>(a);
                            case 482:
                                return radicalReverseSpecialized<3457>(a);
                            case 483:
                                return radicalReverseSpecialized<3461>(a);
                            case 484:
                                return radicalReverseSpecialized<3463>(a);
                            case 485:
                                return radicalReverseSpecialized<3467>(a);
                            case 486:
                                return radicalReverseSpecialized<3469>(a);
                            case 487:
                                return radicalReverseSpecialized<3491>(a);
                            case 488:
                                return radicalReverseSpecialized<3499>(a);
                            case 489:
                                return radicalReverseSpecialized<3511>(a);
                            case 490:
                                return radicalReverseSpecialized<3517>(a);
                            case 491:
                                return radicalReverseSpecialized<3527>(a);
                            case 492:
                                return radicalReverseSpecialized<3529>(a);
                            case 493:
                                return radicalReverseSpecialized<3533>(a);
                            case 494:
                                return radicalReverseSpecialized<3539>(a);
                            case 495:
                                return radicalReverseSpecialized<3541>(a);
                            case 496:
                                return radicalReverseSpecialized<3547>(a);
                            case 497:
                                return radicalReverseSpecialized<3557>(a);
                            case 498:
                                return radicalReverseSpecialized<3559>(a);
                            case 499:
                                return radicalReverseSpecialized<3571>(a);
                            case 500:
                                return radicalReverseSpecialized<3581>(a);
                            case 501:
                                return radicalReverseSpecialized<3583>(a);
                            case 502:
                                return radicalReverseSpecialized<3593>(a);
                            case 503:
                                return radicalReverseSpecialized<3607>(a);
                            case 504:
                                return radicalReverseSpecialized<3613>(a);
                            case 505:
                                return radicalReverseSpecialized<3617>(a);
                            case 506:
                                return radicalReverseSpecialized<3623>(a);
                            case 507:
                                return radicalReverseSpecialized<3631>(a);
                            case 508:
                                return radicalReverseSpecialized<3637>(a);
                            case 509:
                                return radicalReverseSpecialized<3643>(a);
                            case 510:
                                return radicalReverseSpecialized<3659>(a);
                            case 511:
                                return radicalReverseSpecialized<3671>(a);
                            case 512:
                                return radicalReverseSpecialized<3673>(a);
                            case 513:
                                return radicalReverseSpecialized<3677>(a);
                            case 514:
                                return radicalReverseSpecialized<3691>(a);
                            case 515:
                                return radicalReverseSpecialized<3697>(a);
                            case 516:
                                return radicalReverseSpecialized<3701>(a);
                            case 517:
                                return radicalReverseSpecialized<3709>(a);
                            case 518:
                                return radicalReverseSpecialized<3719>(a);
                            case 519:
                                return radicalReverseSpecialized<3727>(a);
                            case 520:
                                return radicalReverseSpecialized<3733>(a);
                            case 521:
                                return radicalReverseSpecialized<3739>(a);
                            case 522:
                                return radicalReverseSpecialized<3761>(a);
                            case 523:
                                return radicalReverseSpecialized<3767>(a);
                            case 524:
                                return radicalReverseSpecialized<3769>(a);
                            case 525:
                                return radicalReverseSpecialized<3779>(a);
                            case 526:
                                return radicalReverseSpecialized<3793>(a);
                            case 527:
                                return radicalReverseSpecialized<3797>(a);
                            case 528:
                                return radicalReverseSpecialized<3803>(a);
                            case 529:
                                return radicalReverseSpecialized<3821>(a);
                            case 530:
                                return radicalReverseSpecialized<3823>(a);
                            case 531:
                                return radicalReverseSpecialized<3833>(a);
                            case 532:
                                return radicalReverseSpecialized<3847>(a);
                            case 533:
                                return radicalReverseSpecialized<3851>(a);
                            case 534:
                                return radicalReverseSpecialized<3853>(a);
                            case 535:
                                return radicalReverseSpecialized<3863>(a);
                            case 536:
                                return radicalReverseSpecialized<3877>(a);
                            case 537:
                                return radicalReverseSpecialized<3881>(a);
                            case 538:
                                return radicalReverseSpecialized<3889>(a);
                            case 539:
                                return radicalReverseSpecialized<3907>(a);
                            case 540:
                                return radicalReverseSpecialized<3911>(a);
                            case 541:
                                return radicalReverseSpecialized<3917>(a);
                            case 542:
                                return radicalReverseSpecialized<3919>(a);
                            case 543:
                                return radicalReverseSpecialized<3923>(a);
                            case 544:
                                return radicalReverseSpecialized<3929>(a);
                            case 545:
                                return radicalReverseSpecialized<3931>(a);
                            case 546:
                                return radicalReverseSpecialized<3943>(a);
                            case 547:
                                return radicalReverseSpecialized<3947>(a);
                            case 548:
                                return radicalReverseSpecialized<3967>(a);
                            case 549:
                                return radicalReverseSpecialized<3989>(a);
                            case 550:
                                return radicalReverseSpecialized<4001>(a);
                            case 551:
                                return radicalReverseSpecialized<4003>(a);
                            case 552:
                                return radicalReverseSpecialized<4007>(a);
                            case 553:
                                return radicalReverseSpecialized<4013>(a);
                            case 554:
                                return radicalReverseSpecialized<4019>(a);
                            case 555:
                                return radicalReverseSpecialized<4021>(a);
                            case 556:
                                return radicalReverseSpecialized<4027>(a);
                            case 557:
                                return radicalReverseSpecialized<4049>(a);
                            case 558:
                                return radicalReverseSpecialized<4051>(a);
                            case 559:
                                return radicalReverseSpecialized<4057>(a);
                            case 560:
                                return radicalReverseSpecialized<4073>(a);
                            case 561:
                                return radicalReverseSpecialized<4079>(a);
                            case 562:
                                return radicalReverseSpecialized<4091>(a);
                            case 563:
                                return radicalReverseSpecialized<4093>(a);
                            case 564:
                                return radicalReverseSpecialized<4099>(a);
                            case 565:
                                return radicalReverseSpecialized<4111>(a);
                            case 566:
                                return radicalReverseSpecialized<4127>(a);
                            case 567:
                                return radicalReverseSpecialized<4129>(a);
                            case 568:
                                return radicalReverseSpecialized<4133>(a);
                            case 569:
                                return radicalReverseSpecialized<4139>(a);
                            case 570:
                                return radicalReverseSpecialized<4153>(a);
                            case 571:
                                return radicalReverseSpecialized<4157>(a);
                            case 572:
                                return radicalReverseSpecialized<4159>(a);
                            case 573:
                                return radicalReverseSpecialized<4177>(a);
                            case 574:
                                return radicalReverseSpecialized<4201>(a);
                            case 575:
                                return radicalReverseSpecialized<4211>(a);
                            case 576:
                                return radicalReverseSpecialized<4217>(a);
                            case 577:
                                return radicalReverseSpecialized<4219>(a);
                            case 578:
                                return radicalReverseSpecialized<4229>(a);
                            case 579:
                                return radicalReverseSpecialized<4231>(a);
                            case 580:
                                return radicalReverseSpecialized<4241>(a);
                            case 581:
                                return radicalReverseSpecialized<4243>(a);
                            case 582:
                                return radicalReverseSpecialized<4253>(a);
                            case 583:
                                return radicalReverseSpecialized<4259>(a);
                            case 584:
                                return radicalReverseSpecialized<4261>(a);
                            case 585:
                                return radicalReverseSpecialized<4271>(a);
                            case 586:
                                return radicalReverseSpecialized<4273>(a);
                            case 587:
                                return radicalReverseSpecialized<4283>(a);
                            case 588:
                                return radicalReverseSpecialized<4289>(a);
                            case 589:
                                return radicalReverseSpecialized<4297>(a);
                            case 590:
                                return radicalReverseSpecialized<4327>(a);
                            case 591:
                                return radicalReverseSpecialized<4337>(a);
                            case 592:
                                return radicalReverseSpecialized<4339>(a);
                            case 593:
                                return radicalReverseSpecialized<4349>(a);
                            case 594:
                                return radicalReverseSpecialized<4357>(a);
                            case 595:
                                return radicalReverseSpecialized<4363>(a);
                            case 596:
                                return radicalReverseSpecialized<4373>(a);
                            case 597:
                                return radicalReverseSpecialized<4391>(a);
                            case 598:
                                return radicalReverseSpecialized<4397>(a);
                            case 599:
                                return radicalReverseSpecialized<4409>(a);
                            case 600:
                                return radicalReverseSpecialized<4421>(a);
                            case 601:
                                return radicalReverseSpecialized<4423>(a);
                            case 602:
                                return radicalReverseSpecialized<4441>(a);
                            case 603:
                                return radicalReverseSpecialized<4447>(a);
                            case 604:
                                return radicalReverseSpecialized<4451>(a);
                            case 605:
                                return radicalReverseSpecialized<4457>(a);
                            case 606:
                                return radicalReverseSpecialized<4463>(a);
                            case 607:
                                return radicalReverseSpecialized<4481>(a);
                            case 608:
                                return radicalReverseSpecialized<4483>(a);
                            case 609:
                                return radicalReverseSpecialized<4493>(a);
                            case 610:
                                return radicalReverseSpecialized<4507>(a);
                            case 611:
                                return radicalReverseSpecialized<4513>(a);
                            case 612:
                                return radicalReverseSpecialized<4517>(a);
                            case 613:
                                return radicalReverseSpecialized<4519>(a);
                            case 614:
                                return radicalReverseSpecialized<4523>(a);
                            case 615:
                                return radicalReverseSpecialized<4547>(a);
                            case 616:
                                return radicalReverseSpecialized<4549>(a);
                            case 617:
                                return radicalReverseSpecialized<4561>(a);
                            case 618:
                                return radicalReverseSpecialized<4567>(a);
                            case 619:
                                return radicalReverseSpecialized<4583>(a);
                            case 620:
                                return radicalReverseSpecialized<4591>(a);
                            case 621:
                                return radicalReverseSpecialized<4597>(a);
                            case 622:
                                return radicalReverseSpecialized<4603>(a);
                            case 623:
                                return radicalReverseSpecialized<4621>(a);
                            case 624:
                                return radicalReverseSpecialized<4637>(a);
                            case 625:
                                return radicalReverseSpecialized<4639>(a);
                            case 626:
                                return radicalReverseSpecialized<4643>(a);
                            case 627:
                                return radicalReverseSpecialized<4649>(a);
                            case 628:
                                return radicalReverseSpecialized<4651>(a);
                            case 629:
                                return radicalReverseSpecialized<4657>(a);
                            case 630:
                                return radicalReverseSpecialized<4663>(a);
                            case 631:
                                return radicalReverseSpecialized<4673>(a);
                            case 632:
                                return radicalReverseSpecialized<4679>(a);
                            case 633:
                                return radicalReverseSpecialized<4691>(a);
                            case 634:
                                return radicalReverseSpecialized<4703>(a);
                            case 635:
                                return radicalReverseSpecialized<4721>(a);
                            case 636:
                                return radicalReverseSpecialized<4723>(a);
                            case 637:
                                return radicalReverseSpecialized<4729>(a);
                            case 638:
                                return radicalReverseSpecialized<4733>(a);
                            case 639:
                                return radicalReverseSpecialized<4751>(a);
                            case 640:
                                return radicalReverseSpecialized<4759>(a);
                            case 641:
                                return radicalReverseSpecialized<4783>(a);
                            case 642:
                                return radicalReverseSpecialized<4787>(a);
                            case 643:
                                return radicalReverseSpecialized<4789>(a);
                            case 644:
                                return radicalReverseSpecialized<4793>(a);
                            case 645:
                                return radicalReverseSpecialized<4799>(a);
                            case 646:
                                return radicalReverseSpecialized<4801>(a);
                            case 647:
                                return radicalReverseSpecialized<4813>(a);
                            case 648:
                                return radicalReverseSpecialized<4817>(a);
                            case 649:
                                return radicalReverseSpecialized<4831>(a);
                            case 650:
                                return radicalReverseSpecialized<4861>(a);
                            case 651:
                                return radicalReverseSpecialized<4871>(a);
                            case 652:
                                return radicalReverseSpecialized<4877>(a);
                            case 653:
                                return radicalReverseSpecialized<4889>(a);
                            case 654:
                                return radicalReverseSpecialized<4903>(a);
                            case 655:
                                return radicalReverseSpecialized<4909>(a);
                            case 656:
                                return radicalReverseSpecialized<4919>(a);
                            case 657:
                                return radicalReverseSpecialized<4931>(a);
                            case 658:
                                return radicalReverseSpecialized<4933>(a);
                            case 659:
                                return radicalReverseSpecialized<4937>(a);
                            case 660:
                                return radicalReverseSpecialized<4943>(a);
                            case 661:
                                return radicalReverseSpecialized<4951>(a);
                            case 662:
                                return radicalReverseSpecialized<4957>(a);
                            case 663:
                                return radicalReverseSpecialized<4967>(a);
                            case 664:
                                return radicalReverseSpecialized<4969>(a);
                            case 665:
                                return radicalReverseSpecialized<4973>(a);
                            case 666:
                                return radicalReverseSpecialized<4987>(a);
                            case 667:
                                return radicalReverseSpecialized<4993>(a);
                            case 668:
                                return radicalReverseSpecialized<4999>(a);
                            case 669:
                                return radicalReverseSpecialized<5003>(a);
                            case 670:
                                return radicalReverseSpecialized<5009>(a);
                            case 671:
                                return radicalReverseSpecialized<5011>(a);
                            case 672:
                                return radicalReverseSpecialized<5021>(a);
                            case 673:
                                return radicalReverseSpecialized<5023>(a);
                            case 674:
                                return radicalReverseSpecialized<5039>(a);
                            case 675:
                                return radicalReverseSpecialized<5051>(a);
                            case 676:
                                return radicalReverseSpecialized<5059>(a);
                            case 677:
                                return radicalReverseSpecialized<5077>(a);
                            case 678:
                                return radicalReverseSpecialized<5081>(a);
                            case 679:
                                return radicalReverseSpecialized<5087>(a);
                            case 680:
                                return radicalReverseSpecialized<5099>(a);
                            case 681:
                                return radicalReverseSpecialized<5101>(a);
                            case 682:
                                return radicalReverseSpecialized<5107>(a);
                            case 683:
                                return radicalReverseSpecialized<5113>(a);
                            case 684:
                                return radicalReverseSpecialized<5119>(a);
                            case 685:
                                return radicalReverseSpecialized<5147>(a);
                            case 686:
                                return radicalReverseSpecialized<5153>(a);
                            case 687:
                                return radicalReverseSpecialized<5167>(a);
                            case 688:
                                return radicalReverseSpecialized<5171>(a);
                            case 689:
                                return radicalReverseSpecialized<5179>(a);
                            case 690:
                                return radicalReverseSpecialized<5189>(a);
                            case 691:
                                return radicalReverseSpecialized<5197>(a);
                            case 692:
                                return radicalReverseSpecialized<5209>(a);
                            case 693:
                                return radicalReverseSpecialized<5227>(a);
                            case 694:
                                return radicalReverseSpecialized<5231>(a);
                            case 695:
                                return radicalReverseSpecialized<5233>(a);
                            case 696:
                                return radicalReverseSpecialized<5237>(a);
                            case 697:
                                return radicalReverseSpecialized<5261>(a);
                            case 698:
                                return radicalReverseSpecialized<5273>(a);
                            case 699:
                                return radicalReverseSpecialized<5279>(a);
                            case 700:
                                return radicalReverseSpecialized<5281>(a);
                            case 701:
                                return radicalReverseSpecialized<5297>(a);
                            case 702:
                                return radicalReverseSpecialized<5303>(a);
                            case 703:
                                return radicalReverseSpecialized<5309>(a);
                            case 704:
                                return radicalReverseSpecialized<5323>(a);
                            case 705:
                                return radicalReverseSpecialized<5333>(a);
                            case 706:
                                return radicalReverseSpecialized<5347>(a);
                            case 707:
                                return radicalReverseSpecialized<5351>(a);
                            case 708:
                                return radicalReverseSpecialized<5381>(a);
                            case 709:
                                return radicalReverseSpecialized<5387>(a);
                            case 710:
                                return radicalReverseSpecialized<5393>(a);
                            case 711:
                                return radicalReverseSpecialized<5399>(a);
                            case 712:
                                return radicalReverseSpecialized<5407>(a);
                            case 713:
                                return radicalReverseSpecialized<5413>(a);
                            case 714:
                                return radicalReverseSpecialized<5417>(a);
                            case 715:
                                return radicalReverseSpecialized<5419>(a);
                            case 716:
                                return radicalReverseSpecialized<5431>(a);
                            case 717:
                                return radicalReverseSpecialized<5437>(a);
                            case 718:
                                return radicalReverseSpecialized<5441>(a);
                            case 719:
                                return radicalReverseSpecialized<5443>(a);
                            case 720:
                                return radicalReverseSpecialized<5449>(a);
                            case 721:
                                return radicalReverseSpecialized<5471>(a);
                            case 722:
                                return radicalReverseSpecialized<5477>(a);
                            case 723:
                                return radicalReverseSpecialized<5479>(a);
                            case 724:
                                return radicalReverseSpecialized<5483>(a);
                            case 725:
                                return radicalReverseSpecialized<5501>(a);
                            case 726:
                                return radicalReverseSpecialized<5503>(a);
                            case 727:
                                return radicalReverseSpecialized<5507>(a);
                            case 728:
                                return radicalReverseSpecialized<5519>(a);
                            case 729:
                                return radicalReverseSpecialized<5521>(a);
                            case 730:
                                return radicalReverseSpecialized<5527>(a);
                            case 731:
                                return radicalReverseSpecialized<5531>(a);
                            case 732:
                                return radicalReverseSpecialized<5557>(a);
                            case 733:
                                return radicalReverseSpecialized<5563>(a);
                            case 734:
                                return radicalReverseSpecialized<5569>(a);
                            case 735:
                                return radicalReverseSpecialized<5573>(a);
                            case 736:
                                return radicalReverseSpecialized<5581>(a);
                            case 737:
                                return radicalReverseSpecialized<5591>(a);
                            case 738:
                                return radicalReverseSpecialized<5623>(a);
                            case 739:
                                return radicalReverseSpecialized<5639>(a);
                            case 740:
                                return radicalReverseSpecialized<5641>(a);
                            case 741:
                                return radicalReverseSpecialized<5647>(a);
                            case 742:
                                return radicalReverseSpecialized<5651>(a);
                            case 743:
                                return radicalReverseSpecialized<5653>(a);
                            case 744:
                                return radicalReverseSpecialized<5657>(a);
                            case 745:
                                return radicalReverseSpecialized<5659>(a);
                            case 746:
                                return radicalReverseSpecialized<5669>(a);
                            case 747:
                                return radicalReverseSpecialized<5683>(a);
                            case 748:
                                return radicalReverseSpecialized<5689>(a);
                            case 749:
                                return radicalReverseSpecialized<5693>(a);
                            case 750:
                                return radicalReverseSpecialized<5701>(a);
                            case 751:
                                return radicalReverseSpecialized<5711>(a);
                            case 752:
                                return radicalReverseSpecialized<5717>(a);
                            case 753:
                                return radicalReverseSpecialized<5737>(a);
                            case 754:
                                return radicalReverseSpecialized<5741>(a);
                            case 755:
                                return radicalReverseSpecialized<5743>(a);
                            case 756:
                                return radicalReverseSpecialized<5749>(a);
                            case 757:
                                return radicalReverseSpecialized<5779>(a);
                            case 758:
                                return radicalReverseSpecialized<5783>(a);
                            case 759:
                                return radicalReverseSpecialized<5791>(a);
                            case 760:
                                return radicalReverseSpecialized<5801>(a);
                            case 761:
                                return radicalReverseSpecialized<5807>(a);
                            case 762:
                                return radicalReverseSpecialized<5813>(a);
                            case 763:
                                return radicalReverseSpecialized<5821>(a);
                            case 764:
                                return radicalReverseSpecialized<5827>(a);
                            case 765:
                                return radicalReverseSpecialized<5839>(a);
                            case 766:
                                return radicalReverseSpecialized<5843>(a);
                            case 767:
                                return radicalReverseSpecialized<5849>(a);
                            case 768:
                                return radicalReverseSpecialized<5851>(a);
                            case 769:
                                return radicalReverseSpecialized<5857>(a);
                            case 770:
                                return radicalReverseSpecialized<5861>(a);
                            case 771:
                                return radicalReverseSpecialized<5867>(a);
                            case 772:
                                return radicalReverseSpecialized<5869>(a);
                            case 773:
                                return radicalReverseSpecialized<5879>(a);
                            case 774:
                                return radicalReverseSpecialized<5881>(a);
                            case 775:
                                return radicalReverseSpecialized<5897>(a);
                            case 776:
                                return radicalReverseSpecialized<5903>(a);
                            case 777:
                                return radicalReverseSpecialized<5923>(a);
                            case 778:
                                return radicalReverseSpecialized<5927>(a);
                            case 779:
                                return radicalReverseSpecialized<5939>(a);
                            case 780:
                                return radicalReverseSpecialized<5953>(a);
                            case 781:
                                return radicalReverseSpecialized<5981>(a);
                            case 782:
                                return radicalReverseSpecialized<5987>(a);
                            case 783:
                                return radicalReverseSpecialized<6007>(a);
                            case 784:
                                return radicalReverseSpecialized<6011>(a);
                            case 785:
                                return radicalReverseSpecialized<6029>(a);
                            case 786:
                                return radicalReverseSpecialized<6037>(a);
                            case 787:
                                return radicalReverseSpecialized<6043>(a);
                            case 788:
                                return radicalReverseSpecialized<6047>(a);
                            case 789:
                                return radicalReverseSpecialized<6053>(a);
                            case 790:
                                return radicalReverseSpecialized<6067>(a);
                            case 791:
                                return radicalReverseSpecialized<6073>(a);
                            case 792:
                                return radicalReverseSpecialized<6079>(a);
                            case 793:
                                return radicalReverseSpecialized<6089>(a);
                            case 794:
                                return radicalReverseSpecialized<6091>(a);
                            case 795:
                                return radicalReverseSpecialized<6101>(a);
                            case 796:
                                return radicalReverseSpecialized<6113>(a);
                            case 797:
                                return radicalReverseSpecialized<6121>(a);
                            case 798:
                                return radicalReverseSpecialized<6131>(a);
                            case 799:
                                return radicalReverseSpecialized<6133>(a);
                            case 800:
                                return radicalReverseSpecialized<6143>(a);
                            case 801:
                                return radicalReverseSpecialized<6151>(a);
                            case 802:
                                return radicalReverseSpecialized<6163>(a);
                            case 803:
                                return radicalReverseSpecialized<6173>(a);
                            case 804:
                                return radicalReverseSpecialized<6197>(a);
                            case 805:
                                return radicalReverseSpecialized<6199>(a);
                            case 806:
                                return radicalReverseSpecialized<6203>(a);
                            case 807:
                                return radicalReverseSpecialized<6211>(a);
                            case 808:
                                return radicalReverseSpecialized<6217>(a);
                            case 809:
                                return radicalReverseSpecialized<6221>(a);
                            case 810:
                                return radicalReverseSpecialized<6229>(a);
                            case 811:
                                return radicalReverseSpecialized<6247>(a);
                            case 812:
                                return radicalReverseSpecialized<6257>(a);
                            case 813:
                                return radicalReverseSpecialized<6263>(a);
                            case 814:
                                return radicalReverseSpecialized<6269>(a);
                            case 815:
                                return radicalReverseSpecialized<6271>(a);
                            case 816:
                                return radicalReverseSpecialized<6277>(a);
                            case 817:
                                return radicalReverseSpecialized<6287>(a);
                            case 818:
                                return radicalReverseSpecialized<6299>(a);
                            case 819:
                                return radicalReverseSpecialized<6301>(a);
                            case 820:
                                return radicalReverseSpecialized<6311>(a);
                            case 821:
                                return radicalReverseSpecialized<6317>(a);
                            case 822:
                                return radicalReverseSpecialized<6323>(a);
                            case 823:
                                return radicalReverseSpecialized<6329>(a);
                            case 824:
                                return radicalReverseSpecialized<6337>(a);
                            case 825:
                                return radicalReverseSpecialized<6343>(a);
                            case 826:
                                return radicalReverseSpecialized<6353>(a);
                            case 827:
                                return radicalReverseSpecialized<6359>(a);
                            case 828:
                                return radicalReverseSpecialized<6361>(a);
                            case 829:
                                return radicalReverseSpecialized<6367>(a);
                            case 830:
                                return radicalReverseSpecialized<6373>(a);
                            case 831:
                                return radicalReverseSpecialized<6379>(a);
                            case 832:
                                return radicalReverseSpecialized<6389>(a);
                            case 833:
                                return radicalReverseSpecialized<6397>(a);
                            case 834:
                                return radicalReverseSpecialized<6421>(a);
                            case 835:
                                return radicalReverseSpecialized<6427>(a);
                            case 836:
                                return radicalReverseSpecialized<6449>(a);
                            case 837:
                                return radicalReverseSpecialized<6451>(a);
                            case 838:
                                return radicalReverseSpecialized<6469>(a);
                            case 839:
                                return radicalReverseSpecialized<6473>(a);
                            case 840:
                                return radicalReverseSpecialized<6481>(a);
                            case 841:
                                return radicalReverseSpecialized<6491>(a);
                            case 842:
                                return radicalReverseSpecialized<6521>(a);
                            case 843:
                                return radicalReverseSpecialized<6529>(a);
                            case 844:
                                return radicalReverseSpecialized<6547>(a);
                            case 845:
                                return radicalReverseSpecialized<6551>(a);
                            case 846:
                                return radicalReverseSpecialized<6553>(a);
                            case 847:
                                return radicalReverseSpecialized<6563>(a);
                            case 848:
                                return radicalReverseSpecialized<6569>(a);
                            case 849:
                                return radicalReverseSpecialized<6571>(a);
                            case 850:
                                return radicalReverseSpecialized<6577>(a);
                            case 851:
                                return radicalReverseSpecialized<6581>(a);
                            case 852:
                                return radicalReverseSpecialized<6599>(a);
                            case 853:
                                return radicalReverseSpecialized<6607>(a);
                            case 854:
                                return radicalReverseSpecialized<6619>(a);
                            case 855:
                                return radicalReverseSpecialized<6637>(a);
                            case 856:
                                return radicalReverseSpecialized<6653>(a);
                            case 857:
                                return radicalReverseSpecialized<6659>(a);
                            case 858:
                                return radicalReverseSpecialized<6661>(a);
                            case 859:
                                return radicalReverseSpecialized<6673>(a);
                            case 860:
                                return radicalReverseSpecialized<6679>(a);
                            case 861:
                                return radicalReverseSpecialized<6689>(a);
                            case 862:
                                return radicalReverseSpecialized<6691>(a);
                            case 863:
                                return radicalReverseSpecialized<6701>(a);
                            case 864:
                                return radicalReverseSpecialized<6703>(a);
                            case 865:
                                return radicalReverseSpecialized<6709>(a);
                            case 866:
                                return radicalReverseSpecialized<6719>(a);
                            case 867:
                                return radicalReverseSpecialized<6733>(a);
                            case 868:
                                return radicalReverseSpecialized<6737>(a);
                            case 869:
                                return radicalReverseSpecialized<6761>(a);
                            case 870:
                                return radicalReverseSpecialized<6763>(a);
                            case 871:
                                return radicalReverseSpecialized<6779>(a);
                            case 872:
                                return radicalReverseSpecialized<6781>(a);
                            case 873:
                                return radicalReverseSpecialized<6791>(a);
                            case 874:
                                return radicalReverseSpecialized<6793>(a);
                            case 875:
                                return radicalReverseSpecialized<6803>(a);
                            case 876:
                                return radicalReverseSpecialized<6823>(a);
                            case 877:
                                return radicalReverseSpecialized<6827>(a);
                            case 878:
                                return radicalReverseSpecialized<6829>(a);
                            case 879:
                                return radicalReverseSpecialized<6833>(a);
                            case 880:
                                return radicalReverseSpecialized<6841>(a);
                            case 881:
                                return radicalReverseSpecialized<6857>(a);
                            case 882:
                                return radicalReverseSpecialized<6863>(a);
                            case 883:
                                return radicalReverseSpecialized<6869>(a);
                            case 884:
                                return radicalReverseSpecialized<6871>(a);
                            case 885:
                                return radicalReverseSpecialized<6883>(a);
                            case 886:
                                return radicalReverseSpecialized<6899>(a);
                            case 887:
                                return radicalReverseSpecialized<6907>(a);
                            case 888:
                                return radicalReverseSpecialized<6911>(a);
                            case 889:
                                return radicalReverseSpecialized<6917>(a);
                            case 890:
                                return radicalReverseSpecialized<6947>(a);
                            case 891:
                                return radicalReverseSpecialized<6949>(a);
                            case 892:
                                return radicalReverseSpecialized<6959>(a);
                            case 893:
                                return radicalReverseSpecialized<6961>(a);
                            case 894:
                                return radicalReverseSpecialized<6967>(a);
                            case 895:
                                return radicalReverseSpecialized<6971>(a);
                            case 896:
                                return radicalReverseSpecialized<6977>(a);
                            case 897:
                                return radicalReverseSpecialized<6983>(a);
                            case 898:
                                return radicalReverseSpecialized<6991>(a);
                            case 899:
                                return radicalReverseSpecialized<6997>(a);
                            case 900:
                                return radicalReverseSpecialized<7001>(a);
                            case 901:
                                return radicalReverseSpecialized<7013>(a);
                            case 902:
                                return radicalReverseSpecialized<7019>(a);
                            case 903:
                                return radicalReverseSpecialized<7027>(a);
                            case 904:
                                return radicalReverseSpecialized<7039>(a);
                            case 905:
                                return radicalReverseSpecialized<7043>(a);
                            case 906:
                                return radicalReverseSpecialized<7057>(a);
                            case 907:
                                return radicalReverseSpecialized<7069>(a);
                            case 908:
                                return radicalReverseSpecialized<7079>(a);
                            case 909:
                                return radicalReverseSpecialized<7103>(a);
                            case 910:
                                return radicalReverseSpecialized<7109>(a);
                            case 911:
                                return radicalReverseSpecialized<7121>(a);
                            case 912:
                                return radicalReverseSpecialized<7127>(a);
                            case 913:
                                return radicalReverseSpecialized<7129>(a);
                            case 914:
                                return radicalReverseSpecialized<7151>(a);
                            case 915:
                                return radicalReverseSpecialized<7159>(a);
                            case 916:
                                return radicalReverseSpecialized<7177>(a);
                            case 917:
                                return radicalReverseSpecialized<7187>(a);
                            case 918:
                                return radicalReverseSpecialized<7193>(a);
                            case 919:
                                return radicalReverseSpecialized<7207>(a);
                            case 920:
                                return radicalReverseSpecialized<7211>(a);
                            case 921:
                                return radicalReverseSpecialized<7213>(a);
                            case 922:
                                return radicalReverseSpecialized<7219>(a);
                            case 923:
                                return radicalReverseSpecialized<7229>(a);
                            case 924:
                                return radicalReverseSpecialized<7237>(a);
                            case 925:
                                return radicalReverseSpecialized<7243>(a);
                            case 926:
                                return radicalReverseSpecialized<7247>(a);
                            case 927:
                                return radicalReverseSpecialized<7253>(a);
                            case 928:
                                return radicalReverseSpecialized<7283>(a);
                            case 929:
                                return radicalReverseSpecialized<7297>(a);
                            case 930:
                                return radicalReverseSpecialized<7307>(a);
                            case 931:
                                return radicalReverseSpecialized<7309>(a);
                            case 932:
                                return radicalReverseSpecialized<7321>(a);
                            case 933:
                                return radicalReverseSpecialized<7331>(a);
                            case 934:
                                return radicalReverseSpecialized<7333>(a);
                            case 935:
                                return radicalReverseSpecialized<7349>(a);
                            case 936:
                                return radicalReverseSpecialized<7351>(a);
                            case 937:
                                return radicalReverseSpecialized<7369>(a);
                            case 938:
                                return radicalReverseSpecialized<7393>(a);
                            case 939:
                                return radicalReverseSpecialized<7411>(a);
                            case 940:
                                return radicalReverseSpecialized<7417>(a);
                            case 941:
                                return radicalReverseSpecialized<7433>(a);
                            case 942:
                                return radicalReverseSpecialized<7451>(a);
                            case 943:
                                return radicalReverseSpecialized<7457>(a);
                            case 944:
                                return radicalReverseSpecialized<7459>(a);
                            case 945:
                                return radicalReverseSpecialized<7477>(a);
                            case 946:
                                return radicalReverseSpecialized<7481>(a);
                            case 947:
                                return radicalReverseSpecialized<7487>(a);
                            case 948:
                                return radicalReverseSpecialized<7489>(a);
                            case 949:
                                return radicalReverseSpecialized<7499>(a);
                            case 950:
                                return radicalReverseSpecialized<7507>(a);
                            case 951:
                                return radicalReverseSpecialized<7517>(a);
                            case 952:
                                return radicalReverseSpecialized<7523>(a);
                            case 953:
                                return radicalReverseSpecialized<7529>(a);
                            case 954:
                                return radicalReverseSpecialized<7537>(a);
                            case 955:
                                return radicalReverseSpecialized<7541>(a);
                            case 956:
                                return radicalReverseSpecialized<7547>(a);
                            case 957:
                                return radicalReverseSpecialized<7549>(a);
                            case 958:
                                return radicalReverseSpecialized<7559>(a);
                            case 959:
                                return radicalReverseSpecialized<7561>(a);
                            case 960:
                                return radicalReverseSpecialized<7573>(a);
                            case 961:
                                return radicalReverseSpecialized<7577>(a);
                            case 962:
                                return radicalReverseSpecialized<7583>(a);
                            case 963:
                                return radicalReverseSpecialized<7589>(a);
                            case 964:
                                return radicalReverseSpecialized<7591>(a);
                            case 965:
                                return radicalReverseSpecialized<7603>(a);
                            case 966:
                                return radicalReverseSpecialized<7607>(a);
                            case 967:
                                return radicalReverseSpecialized<7621>(a);
                            case 968:
                                return radicalReverseSpecialized<7639>(a);
                            case 969:
                                return radicalReverseSpecialized<7643>(a);
                            case 970:
                                return radicalReverseSpecialized<7649>(a);
                            case 971:
                                return radicalReverseSpecialized<7669>(a);
                            case 972:
                                return radicalReverseSpecialized<7673>(a);
                            case 973:
                                return radicalReverseSpecialized<7681>(a);
                            case 974:
                                return radicalReverseSpecialized<7687>(a);
                            case 975:
                                return radicalReverseSpecialized<7691>(a);
                            case 976:
                                return radicalReverseSpecialized<7699>(a);
                            case 977:
                                return radicalReverseSpecialized<7703>(a);
                            case 978:
                                return radicalReverseSpecialized<7717>(a);
                            case 979:
                                return radicalReverseSpecialized<7723>(a);
                            case 980:
                                return radicalReverseSpecialized<7727>(a);
                            case 981:
                                return radicalReverseSpecialized<7741>(a);
                            case 982:
                                return radicalReverseSpecialized<7753>(a);
                            case 983:
                                return radicalReverseSpecialized<7757>(a);
                            case 984:
                                return radicalReverseSpecialized<7759>(a);
                            case 985:
                                return radicalReverseSpecialized<7789>(a);
                            case 986:
                                return radicalReverseSpecialized<7793>(a);
                            case 987:
                                return radicalReverseSpecialized<7817>(a);
                            case 988:
                                return radicalReverseSpecialized<7823>(a);
                            case 989:
                                return radicalReverseSpecialized<7829>(a);
                            case 990:
                                return radicalReverseSpecialized<7841>(a);
                            case 991:
                                return radicalReverseSpecialized<7853>(a);
                            case 992:
                                return radicalReverseSpecialized<7867>(a);
                            case 993:
                                return radicalReverseSpecialized<7873>(a);
                            case 994:
                                return radicalReverseSpecialized<7877>(a);
                            case 995:
                                return radicalReverseSpecialized<7879>(a);
                            case 996:
                                return radicalReverseSpecialized<7883>(a);
                            case 997:
                                return radicalReverseSpecialized<7901>(a);
                            case 998:
                                return radicalReverseSpecialized<7907>(a);
                            case 999:
                                return radicalReverseSpecialized<7919>(a);
                            case 1000:
                                return radicalReverseSpecialized<7927>(a);
                            case 1001:
                                return radicalReverseSpecialized<7933>(a);
                            case 1002:
                                return radicalReverseSpecialized<7937>(a);
                            case 1003:
                                return radicalReverseSpecialized<7949>(a);
                            case 1004:
                                return radicalReverseSpecialized<7951>(a);
                            case 1005:
                                return radicalReverseSpecialized<7963>(a);
                            case 1006:
                                return radicalReverseSpecialized<7993>(a);
                            case 1007:
                                return radicalReverseSpecialized<8009>(a);
                            case 1008:
                                return radicalReverseSpecialized<8011>(a);
                            case 1009:
                                return radicalReverseSpecialized<8017>(a);
                            case 1010:
                                return radicalReverseSpecialized<8039>(a);
                            case 1011:
                                return radicalReverseSpecialized<8053>(a);
                            case 1012:
                                return radicalReverseSpecialized<8059>(a);
                            case 1013:
                                return radicalReverseSpecialized<8069>(a);
                            case 1014:
                                return radicalReverseSpecialized<8081>(a);
                            case 1015:
                                return radicalReverseSpecialized<8087>(a);
                            case 1016:
                                return radicalReverseSpecialized<8089>(a);
                            case 1017:
                                return radicalReverseSpecialized<8093>(a);
                            case 1018:
                                return radicalReverseSpecialized<8101>(a);
                            case 1019:
                                return radicalReverseSpecialized<8111>(a);
                            case 1020:
                                return radicalReverseSpecialized<8117>(a);
                            case 1021:
                                return radicalReverseSpecialized<8123>(a);
                            case 1022:
                                return radicalReverseSpecialized<8147>(a);
                            case 1023:
                                return radicalReverseSpecialized<8161>(a);
                            default:
                                assert(false);
                                return 0;
                        }
                        /*
                        const int base = primes[dimension];
                        const Float invBase = 1.0 / base;
                        uint64_t reverse = 0;
                        Float inv = 1;
                        while (a != 0) {
                            uint64_t next = a / base;
                            uint64_t rest = a - next * base;
                            reverse = reverse * base + rest;
                            a = next;
                            inv *= invBase;
                        }
                        return reverse * inv;
                         */
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

                Float radicalReverse(uint64_t n) {
                    uint64_t rev64 = reverseBit64(n);
#ifdef WINDOWS
					return rev64 * 5.4210108624275222e-20;
#else
					return rev64 * 0x1p-64;
#endif
                }
            }
        }

        namespace spline_curve {

            /**
             * Copy from pbr-book
             * @tparam Predicate
             * @param size
             * @param pred
             * @return
             */
            template<typename Predicate>
            int intervalSearch(int size, const Predicate &pred) {
                int first = 0, len = size;
                while (len > 0) {
                    int half = len >> 1, middle = first + half;
                    // Bisect range based on value of _pred_ at _middle_
                    if (pred(middle)) {
                        first = middle + 1;
                        len -= half + 1;
                    } else
                        len = half;
                }
                return clamp(first - 1, 0, size - 2);
            }

            int intervalSearch(int size, const Float *array, Float x) {
                int begin = 0;
                int end = size - 1;
                while ((end - begin) > 1) {
                    int middle = (begin + end) / 2;
                    if (x > array[middle]) {
                        begin = middle;
                    } else {
                        end = middle;
                    }
                }
                if (begin < 0) {
                    return 0;
                } else if (begin > (size - 2)) {
                    return size - 2;
                } else {
                    return begin;
                }
            }

            Float integrateCatmullRom(int n, const Float *x, const Float *values,
                                      Float *cdf) {
                Float sum = 0;
                cdf[0] = 0;
                for (int i = 0; i < n - 1; ++i) {
                    // Look up $x_i$ and function values of spline segment _i_
                    Float x0 = x[i], x1 = x[i + 1];
                    Float f0 = values[i], f1 = values[i + 1];
                    Float width = x1 - x0;

                    // Approximate derivatives using finite differences
                    Float d0, d1;
                    if (i > 0)
                        d0 = width * (f1 - values[i - 1]) / (x1 - x[i - 1]);
                    else
                        d0 = f1 - f0;
                    if (i + 2 < n)
                        d1 = width * (values[i + 2] - f0) / (x[i + 2] - x0);
                    else
                        d1 = f1 - f0;

                    // Keep a running sum and build a cumulative distribution function
                    sum += ((d0 - d1) * (1.f / 12.f) + (f0 + f1) * .5f) * width;
                    cdf[i + 1] = sum;
                }
                return sum;
            }

            bool catmullRomWeights(int size, const Float *nodes, Float x, int *offset,
                                   Float *weights) {
                // Return _false_ if _x_ is out of bounds
                if (!(x >= nodes[0] && x <= nodes[size - 1])) return false;

                // Search for the interval _idx_ containing _x_
                int idx = intervalSearch(size, [&](int i) { return nodes[i] <= x; });
                *offset = idx - 1;
                Float x0 = nodes[idx], x1 = nodes[idx + 1];

                // Compute the $t$ parameter and powers
                Float t = (x - x0) / (x1 - x0), t2 = t * t, t3 = t2 * t;

                // Compute initial node weights $w_1$ and $w_2$
                weights[1] = 2 * t3 - 3 * t2 + 1;
                weights[2] = -2 * t3 + 3 * t2;

                // Compute first node weight $w_0$
                if (idx > 0) {
                    Float w0 = (t3 - 2 * t2 + t) * (x1 - x0) / (x1 - nodes[idx - 1]);
                    weights[0] = -w0;
                    weights[2] += w0;
                } else {
                    Float w0 = t3 - 2 * t2 + t;
                    weights[0] = 0;
                    weights[1] -= w0;
                    weights[2] += w0;
                }

                // Compute last node weight $w_3$
                if (idx + 2 < size) {
                    Float w3 = (t3 - t2) * (x1 - x0) / (nodes[idx + 2] - x0);
                    weights[1] -= w3;
                    weights[3] = w3;
                } else {
                    Float w3 = t3 - t2;
                    weights[1] -= w3;
                    weights[2] += w3;
                    weights[3] = 0;
                }
                return true;
            }

            Float sampleCatmullRom(int n, const Float *x, const Float *f, const Float *F,
                                   Float u, Float *fval, Float *pdf) {
                // Map _u_ to a spline interval by inverting _F_
                u *= F[n - 1];
                int i = intervalSearch(n, [&](int i) { return F[i] <= u; });

                // Look up $x_i$ and function values of spline segment _i_
                Float x0 = x[i], x1 = x[i + 1];
                Float f0 = f[i], f1 = f[i + 1];
                Float width = x1 - x0;

                // Approximate derivatives using finite differences
                Float d0, d1;
                if (i > 0)
                    d0 = width * (f1 - f[i - 1]) / (x1 - x[i - 1]);
                else
                    d0 = f1 - f0;
                if (i + 2 < n)
                    d1 = width * (f[i + 2] - f0) / (x[i + 2] - x0);
                else
                    d1 = f1 - f0;

                // Re-scale _u_ for continous spline sampling step
                u = (u - F[i]) / width;

                // Invert definite integral over spline segment and return solution

                // Set initial guess for $t$ by importance sampling a linear interpolant
                Float t;
                if (f0 != f1)
                    t = (f0 - std::sqrt(std::max((Float) 0, f0 * f0 + 2 * u * (f1 - f0)))) /
                        (f0 - f1);
                else
                    t = u / f0;
                Float a = 0, b = 1, Fhat, fhat;
                while (true) {
                    // Fall back to a bisection step when _t_ is out of bounds
                    if (!(t > a && t < b)) t = 0.5f * (a + b);

                    // Evaluate target function and its derivative in Horner form
                    Fhat = t * (f0 +
                                t * (.5f * d0 +
                                     t * ((1.f / 3.f) * (-2 * d0 - d1) + f1 - f0 +
                                          t * (.25f * (d0 + d1) + .5f * (f0 - f1)))));
                    fhat = f0 +
                           t * (d0 +
                                t * (-2 * d0 - d1 + 3 * (f1 - f0) +
                                     t * (d0 + d1 + 2 * (f0 - f1))));

                    // Stop the iteration if converged
                    if (std::abs(Fhat - u) < 1e-6f || b - a < 1e-6f) break;

                    // Update bisection bounds using updated _t_
                    if (Fhat - u < 0)
                        a = t;
                    else
                        b = t;

                    // Perform a Newton step
                    t -= (Fhat - u) / fhat;
                }

                // Return the sample position and function value
                if (fval) *fval = fhat;
                if (pdf) *pdf = fhat / F[n - 1];
                return x0 + width * t;
            }

            Float sampleCatmullRom2D(int size1, int size2, const Float *nodes1,
                                     const Float *nodes2, const Float *values,
                                     const Float *cdf, Float alpha, Float u, Float *fval,
                                     Float *pdf) {
                // Determine offset and coefficients for the _alpha_ parameter
                int offset;
                Float weights[4];
                if (!catmullRomWeights(size1, nodes1, alpha, &offset, weights)) return 0;

                // Define a lambda function to interpolate table entries
                auto interpolate = [&](const Float *array, int idx) {
                    Float value = 0;
                    for (int i = 0; i < 4; ++i)
                        if (weights[i] != 0)
                            value += array[(offset + i) * size2 + idx] * weights[i];
                    return value;
                };

                // Map _u_ to a spline interval by inverting the interpolated _cdf_
                Float maximum = interpolate(cdf, size2 - 1);
                u *= maximum;
                int idx =
                        intervalSearch(size2, [&](int i) { return interpolate(cdf, i) <= u; });

                // Look up node positions and interpolated function values
                Float f0 = interpolate(values, idx), f1 = interpolate(values, idx + 1);
                Float x0 = nodes2[idx], x1 = nodes2[idx + 1];
                Float width = x1 - x0;
                Float d0, d1;

                // Re-scale _u_ using the interpolated _cdf_
                u = (u - interpolate(cdf, idx)) / width;

                // Approximate derivatives using finite differences of the interpolant
                if (idx > 0)
                    d0 = width * (f1 - interpolate(values, idx - 1)) /
                         (x1 - nodes2[idx - 1]);
                else
                    d0 = f1 - f0;
                if (idx + 2 < size2)
                    d1 = width * (interpolate(values, idx + 2) - f0) /
                         (nodes2[idx + 2] - x0);
                else
                    d1 = f1 - f0;

                // Invert definite integral over spline segment and return solution

                // Set initial guess for $t$ by importance sampling a linear interpolant
                Float t;
                if (f0 != f1)
                    t = (f0 - std::sqrt(std::max((Float) 0, f0 * f0 + 2 * u * (f1 - f0)))) /
                        (f0 - f1);
                else
                    t = u / f0;
                Float a = 0, b = 1, Fhat, fhat;
                while (true) {
                    // Fall back to a bisection step when _t_ is out of bounds
                    if (!(t >= a && t <= b)) t = 0.5f * (a + b);

                    // Evaluate target function and its derivative in Horner form
                    Fhat = t * (f0 +
                                t * (.5f * d0 +
                                     t * ((1.f / 3.f) * (-2 * d0 - d1) + f1 - f0 +
                                          t * (.25f * (d0 + d1) + .5f * (f0 - f1)))));
                    fhat = f0 +
                           t * (d0 +
                                t * (-2 * d0 - d1 + 3 * (f1 - f0) +
                                     t * (d0 + d1 + 2 * (f0 - f1))));

                    // Stop the iteration if converged
                    if (std::abs(Fhat - u) < 1e-6f || b - a < 1e-6f) break;

                    // Update bisection bounds using updated _t_
                    if (Fhat - u < 0)
                        a = t;
                    else
                        b = t;

                    // Perform a Newton step
                    t -= (Fhat - u) / fhat;
                }

                // Return the sample position and function value
                if (fval) *fval = fhat;
                if (pdf) *pdf = fhat / maximum;
                return x0 + width * t;
            }

            Float invertCatmullRom(int n, const Float *x, const Float *values, Float u) {
                if (!(u > values[0])) {
                    return x[0];
                } else if (!(u < values[n - 1])) {
                    return x[n - 1];
                }

                // Map _u_ to a spline interval by inverting _values_
                int i = intervalSearch(n, [&](int i) { return values[i] <= u; });

                // Look up $x_i$ and function values of spline segment _i_
                Float x0 = x[i], x1 = x[i + 1];
                Float f0 = values[i], f1 = values[i + 1];
                Float width = x1 - x0;

                // Approximate derivatives using finite differences
                Float d0, d1;
                if (i > 0)
                    d0 = width * (f1 - values[i - 1]) / (x1 - x[i - 1]);
                else
                    d0 = f1 - f0;
                if (i + 2 < n)
                    d1 = width * (values[i + 2] - f0) / (x[i + 2] - x0);
                else
                    d1 = f1 - f0;

                // Invert the spline interpolant using Newton-Bisection
                Float a = 0, b = 1, t = .5f;
                Float Fhat, fhat;
                while (true) {
                    // Fall back to a bisection step when _t_ is out of bounds
                    if (!(t > a && t < b)) t = 0.5f * (a + b);

                    // Compute powers of _t_
                    Float t2 = t * t, t3 = t2 * t;

                    // Set _Fhat_ using Equation (8.27)
                    Fhat = (2 * t3 - 3 * t2 + 1) * f0 + (-2 * t3 + 3 * t2) * f1 +
                           (t3 - 2 * t2 + t) * d0 + (t3 - t2) * d1;

                    // Set _fhat_ using Equation (not present)
                    fhat = (6 * t2 - 6 * t) * f0 + (-6 * t2 + 6 * t) * f1 +
                           (3 * t2 - 4 * t + 1) * d0 + (3 * t2 - 2 * t) * d1;

                    // Stop the iteration if converged
                    if (std::abs(Fhat - u) < 1e-6f || b - a < 1e-6f) break;

                    // Update bisection bounds using updated _t_
                    if (Fhat - u < 0)
                        a = t;
                    else
                        b = t;

                    // Perform a Newton step
                    t -= (Fhat - u) / fhat;
                }
                return x0 + t * width;
            }
        }
    }
}