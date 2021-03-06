


#include "a_star.h"


static double test_data_times[] = {
  1.360252,
  1.360252,
  1.360252,
  1.360252,
  1.360252
} ;

static int test_data_positions[][2][2] = {
  {{0, 0}, {3248, 2222}},
  {{0, 0}, {-3248, -2222}},
  {{0, 0}, {3248, -2222}},
  {{0, 0}, {-3248, 2222}},
  {{2954, 1542}, {2954, 1862}}
} ;

static int test_data_steps[][2] = {
  {5, 5},
  {5, 5},
  {5, 5},
  {5, 5},
  {5, 5}
} ;

static int test_data_hitbox[][4][2] = {
  {{283, 196}, {229, 171}, {172, 221}, {261, 246}},
  {{447, 304}, {393, 279}, {336, 329}, {425, 354}},
  {{571, 208}, {517, 183}, {460, 233}, {549, 258}},
  {{739, 252}, {685, 227}, {628, 277}, {717, 302}},
  {{1047, 168}, {993, 143}, {936, 193}, {1025, 218}},
  {{1383, 16}, {1329, -9}, {1272, 41}, {1361, 66}},
  {{1655, 92}, {1601, 67}, {1544, 117}, {1633, 142}},
  {{1967, 72}, {1913, 47}, {1856, 97}, {1945, 122}},
  {{2303, 60}, {2249, 35}, {2192, 85}, {2281, 110}},
  {{2547, 52}, {2493, 27}, {2436, 77}, {2525, 102}},
  {{2503, 124}, {2449, 99}, {2392, 149}, {2481, 174}},
  {{2847, 56}, {2793, 31}, {2736, 81}, {2825, 106}},
  {{3107, -40}, {3053, -65}, {2996, -15}, {3085, 10}},
  {{3023, 132}, {2969, 107}, {2912, 157}, {3001, 182}},
  {{2867, 148}, {2813, 123}, {2756, 173}, {2845, 198}},
  {{2651, 192}, {2597, 167}, {2540, 217}, {2629, 242}},
  {{2383, 136}, {2329, 111}, {2272, 161}, {2361, 186}},
  {{2135, 156}, {2081, 131}, {2024, 181}, {2113, 206}},
  {{1879, 204}, {1825, 179}, {1768, 229}, {1857, 254}},
  {{1675, 228}, {1621, 203}, {1564, 253}, {1653, 278}},
  {{1675, 228}, {1621, 203}, {1564, 253}, {1653, 278}},
  {{1411, 140}, {1357, 115}, {1300, 165}, {1389, 190}},
  {{1199, 224}, {1145, 199}, {1088, 249}, {1177, 274}},
  {{963, 272}, {909, 247}, {852, 297}, {941, 322}},
  {{807, 348}, {753, 323}, {696, 373}, {785, 398}},
  {{599, 388}, {545, 363}, {488, 413}, {577, 438}},
  {{1355, 240}, {1301, 215}, {1244, 265}, {1333, 290}},
  {{1515, 268}, {1461, 243}, {1404, 293}, {1493, 318}},
  {{1799, 304}, {1745, 279}, {1688, 329}, {1777, 354}},
  {{2035, 348}, {1981, 323}, {1924, 373}, {2013, 398}},
  {{2251, 760}, {2197, 735}, {2140, 785}, {2229, 810}},
  {{2251, 1154}, {2197, 1129}, {2140, 1179}, {2229, 1204}},
  {{2251, 1548}, {2197, 1523}, {2140, 1573}, {2229, 1598}},
  {{2251, 1942}, {2197, 1917}, {2140, 1967}, {2229, 1992}},
  {{2645, 760}, {2591, 735}, {2534, 785}, {2623, 810}},
  {{2645, 1154}, {2591, 1129}, {2534, 1179}, {2623, 1204}},
  {{2645, 1548}, {2591, 1523}, {2534, 1573}, {2623, 1598}},
  {{2645, 1942}, {2591, 1917}, {2534, 1967}, {2623, 1992}},
  {{3039, 760}, {2985, 735}, {2928, 785}, {3017, 810}},
  {{3039, 1154}, {2985, 1129}, {2928, 1179}, {3017, 1204}},
  {{3039, 1548}, {2985, 1523}, {2928, 1573}, {3017, 1598}},
  {{3039, 1942}, {2985, 1917}, {2928, 1967}, {3017, 1992}},
  {{2335, 784}, {2281, 759}, {2224, 809}, {2313, 834}},
  {{2335, 1178}, {2281, 1153}, {2224, 1203}, {2313, 1228}},
  {{2335, 1572}, {2281, 1547}, {2224, 1597}, {2313, 1622}},
  {{2335, 1966}, {2281, 1941}, {2224, 1991}, {2313, 2016}},
  {{2729, 784}, {2675, 759}, {2618, 809}, {2707, 834}},
  {{2729, 1178}, {2675, 1153}, {2618, 1203}, {2707, 1228}},
  {{2729, 1572}, {2675, 1547}, {2618, 1597}, {2707, 1622}},
  {{2729, 1966}, {2675, 1941}, {2618, 1991}, {2707, 2016}},
  {{3123, 784}, {3069, 759}, {3012, 809}, {3101, 834}},
  {{3123, 1178}, {3069, 1153}, {3012, 1203}, {3101, 1228}},
  {{3123, 1572}, {3069, 1547}, {3012, 1597}, {3101, 1622}},
  {{3123, 1966}, {3069, 1941}, {3012, 1991}, {3101, 2016}},
  {{2371, 840}, {2317, 815}, {2260, 865}, {2349, 890}},
  {{2371, 1234}, {2317, 1209}, {2260, 1259}, {2349, 1284}},
  {{2371, 1628}, {2317, 1603}, {2260, 1653}, {2349, 1678}},
  {{2371, 2022}, {2317, 1997}, {2260, 2047}, {2349, 2072}},
  {{2765, 840}, {2711, 815}, {2654, 865}, {2743, 890}},
  {{2765, 1234}, {2711, 1209}, {2654, 1259}, {2743, 1284}},
  {{2765, 1628}, {2711, 1603}, {2654, 1653}, {2743, 1678}},
  {{2765, 2022}, {2711, 1997}, {2654, 2047}, {2743, 2072}},
  {{3159, 840}, {3105, 815}, {3048, 865}, {3137, 890}},
  {{3159, 1234}, {3105, 1209}, {3048, 1259}, {3137, 1284}},
  {{3159, 1628}, {3105, 1603}, {3048, 1653}, {3137, 1678}},
  {{3159, 2022}, {3105, 1997}, {3048, 2047}, {3137, 2072}},
  {{2455, 868}, {2401, 843}, {2344, 893}, {2433, 918}},
  {{2455, 1262}, {2401, 1237}, {2344, 1287}, {2433, 1312}},
  {{2455, 1656}, {2401, 1631}, {2344, 1681}, {2433, 1706}},
  {{2455, 2050}, {2401, 2025}, {2344, 2075}, {2433, 2100}},
  {{2849, 868}, {2795, 843}, {2738, 893}, {2827, 918}},
  {{2849, 1262}, {2795, 1237}, {2738, 1287}, {2827, 1312}},
  {{2849, 1656}, {2795, 1631}, {2738, 1681}, {2827, 1706}},
  {{2849, 2050}, {2795, 2025}, {2738, 2075}, {2827, 2100}},
  {{3243, 868}, {3189, 843}, {3132, 893}, {3221, 918}},
  {{3243, 1262}, {3189, 1237}, {3132, 1287}, {3221, 1312}},
  {{3243, 1656}, {3189, 1631}, {3132, 1681}, {3221, 1706}},
  {{3243, 2050}, {3189, 2025}, {3132, 2075}, {3221, 2100}},
  {{2507, 932}, {2453, 907}, {2396, 957}, {2485, 982}},
  {{2507, 1326}, {2453, 1301}, {2396, 1351}, {2485, 1376}},
  {{2507, 1720}, {2453, 1695}, {2396, 1745}, {2485, 1770}},
  {{2507, 2114}, {2453, 2089}, {2396, 2139}, {2485, 2164}},
  {{2901, 932}, {2847, 907}, {2790, 957}, {2879, 982}},
  {{2901, 1326}, {2847, 1301}, {2790, 1351}, {2879, 1376}},
  {{2901, 1720}, {2847, 1695}, {2790, 1745}, {2879, 1770}},
  {{2901, 2114}, {2847, 2089}, {2790, 2139}, {2879, 2164}},
  {{3295, 932}, {3241, 907}, {3184, 957}, {3273, 982}},
  {{3295, 1326}, {3241, 1301}, {3184, 1351}, {3273, 1376}},
  {{3295, 1720}, {3241, 1695}, {3184, 1745}, {3273, 1770}},
  {{3295, 2114}, {3241, 2089}, {3184, 2139}, {3273, 2164}},
  {{2463, 976}, {2409, 951}, {2352, 1001}, {2441, 1026}},
  {{2463, 1370}, {2409, 1345}, {2352, 1395}, {2441, 1420}},
  {{2463, 1764}, {2409, 1739}, {2352, 1789}, {2441, 1814}},
  {{2463, 2158}, {2409, 2133}, {2352, 2183}, {2441, 2208}},
  {{2857, 976}, {2803, 951}, {2746, 1001}, {2835, 1026}},
  {{2857, 1370}, {2803, 1345}, {2746, 1395}, {2835, 1420}},
  {{2857, 1764}, {2803, 1739}, {2746, 1789}, {2835, 1814}},
  {{2857, 2158}, {2803, 2133}, {2746, 2183}, {2835, 2208}},
  {{3251, 976}, {3197, 951}, {3140, 1001}, {3229, 1026}},
  {{3251, 1370}, {3197, 1345}, {3140, 1395}, {3229, 1420}},
  {{3251, 1764}, {3197, 1739}, {3140, 1789}, {3229, 1814}},
  {{3251, 2158}, {3197, 2133}, {3140, 2183}, {3229, 2208}},
  {{2407, 1016}, {2353, 991}, {2296, 1041}, {2385, 1066}},
  {{2407, 1410}, {2353, 1385}, {2296, 1435}, {2385, 1460}},
  {{2407, 1804}, {2353, 1779}, {2296, 1829}, {2385, 1854}},
  {{2407, 2198}, {2353, 2173}, {2296, 2223}, {2385, 2248}},
  {{2801, 1016}, {2747, 991}, {2690, 1041}, {2779, 1066}},
  {{2801, 1410}, {2747, 1385}, {2690, 1435}, {2779, 1460}},
  {{2801, 1804}, {2747, 1779}, {2690, 1829}, {2779, 1854}},
  {{2801, 2198}, {2747, 2173}, {2690, 2223}, {2779, 2248}},
  {{3195, 1016}, {3141, 991}, {3084, 1041}, {3173, 1066}},
  {{3195, 1410}, {3141, 1385}, {3084, 1435}, {3173, 1460}},
  {{3195, 1804}, {3141, 1779}, {3084, 1829}, {3173, 1854}},
  {{3195, 2198}, {3141, 2173}, {3084, 2223}, {3173, 2248}},
  {{2383, 1100}, {2329, 1075}, {2272, 1125}, {2361, 1150}},
  {{2383, 1494}, {2329, 1469}, {2272, 1519}, {2361, 1544}},
  {{2383, 1888}, {2329, 1863}, {2272, 1913}, {2361, 1938}},
  {{2383, 2282}, {2329, 2257}, {2272, 2307}, {2361, 2332}},
  {{2777, 1100}, {2723, 1075}, {2666, 1125}, {2755, 1150}},
  {{2777, 1494}, {2723, 1469}, {2666, 1519}, {2755, 1544}},
  {{2777, 1888}, {2723, 1863}, {2666, 1913}, {2755, 1938}},
  {{2777, 2282}, {2723, 2257}, {2666, 2307}, {2755, 2332}},
  {{3171, 1100}, {3117, 1075}, {3060, 1125}, {3149, 1150}},
  {{3171, 1494}, {3117, 1469}, {3060, 1519}, {3149, 1544}},
  {{3171, 1888}, {3117, 1863}, {3060, 1913}, {3149, 1938}},
  {{3171, 2282}, {3117, 2257}, {3060, 2307}, {3149, 2332}},
  {{2335, 1128}, {2281, 1103}, {2224, 1153}, {2313, 1178}},
  {{2335, 1522}, {2281, 1497}, {2224, 1547}, {2313, 1572}},
  {{2335, 1916}, {2281, 1891}, {2224, 1941}, {2313, 1966}},
  {{2335, 2310}, {2281, 2285}, {2224, 2335}, {2313, 2360}},
  {{2729, 1128}, {2675, 1103}, {2618, 1153}, {2707, 1178}},
  {{2729, 1522}, {2675, 1497}, {2618, 1547}, {2707, 1572}},
  {{2729, 1916}, {2675, 1891}, {2618, 1941}, {2707, 1966}},
  {{2729, 2310}, {2675, 2285}, {2618, 2335}, {2707, 2360}},
  {{3123, 1128}, {3069, 1103}, {3012, 1153}, {3101, 1178}},
  {{3123, 1522}, {3069, 1497}, {3012, 1547}, {3101, 1572}},
  {{3123, 1916}, {3069, 1891}, {3012, 1941}, {3101, 1966}},
  {{3123, 2310}, {3069, 2285}, {3012, 2335}, {3101, 2360}}
} ;

static int test_data_hitbox_nb = 135 ;


void prepare_set_no (int no, p_context context) {
  int hitbox_no = 0 ;
  int vertex_no ;

  context->start.x = test_data_positions[no][0][0] ;
  context->start.y = test_data_positions[no][0][1] ;
  context->end.x = test_data_positions[no][1][0] ;
  context->end.y = test_data_positions[no][1][1] ;
  context->step_x = test_data_steps[no][0] ;
  context->step_y = test_data_steps[no][1] ;
  context->hitbox_nb = test_data_hitbox_nb ;
  context->hitbox_list = calloc (context->hitbox_nb, sizeof (t_hitbox)) ;
  while (hitbox_no < context->hitbox_nb) {
    context->hitbox_list[hitbox_no].size = 4 ;
    context->hitbox_list[hitbox_no].vertex = calloc (4, sizeof (t_position)) ;
    for (vertex_no=0 ; vertex_no < 4 ; vertex_no += 1) {
      context->hitbox_list[hitbox_no].vertex[vertex_no].x = test_data_hitbox[hitbox_no][vertex_no][0] ;
      context->hitbox_list[hitbox_no].vertex[vertex_no].y = test_data_hitbox[hitbox_no][vertex_no][1] ;
    }
    hitbox_no += 1 ;
  }
}

int main (int argc, char *argv[]) {
  t_context context ;
  int i ;

  Py_SetProgramName (argv[0]) ;
  Py_Initialize () ;
  argc++ ;

  bzero (&context, sizeof (t_context)) ;

  //int j ;
  //for (j=0 ; j<4;j+=1)
  for (i=0 ; i<5 ; i+=1) {
    prepare_set_no (i, &context) ;
    printf ("From: [%d;%d]\nTo: [%d;%d]\ntime expected: %f\n",
      context.start.x, 
      context.start.y, 
      context.end.x, 
      context.end.y, 
      test_data_times[i]) ;
    shortcut_static_find (&context) ;
    shortcut_free_hitbox_list (&context) ;
    Py_DECREF (context.result) ;
  }

  Py_Finalize () ;

  return 0 ;
}