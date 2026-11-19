window.BENCHMARK_DATA = {
  "lastUpdate": 1795086711037,
  "repoUrl": "https://github.com/wstux/thread_safe_lru_cache",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "wstux1@gmail.com",
            "name": "wstux",
            "username": "wstux"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "7c7375d251fe101665014f0cf7b793b69d292250",
          "message": "* all: #40 Merge remote-tracking branch 'wstux/feature/update' into master;",
          "timestamp": "2026-11-19T14:07:35+03:00",
          "tree_id": "dd2fab3af84df4b541f5c76c2109b6ce8ccbe9ae",
          "url": "https://github.com/wstux/thread_safe_lru_cache/commit/7c7375d251fe101665014f0cf7b793b69d292250"
        },
        "date": 1795086710588,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "insert<fifo_cache_intrusive>",
            "value": 3.292640435953777,
            "unit": "ns/iter",
            "extra": "iterations: 212150569\ncpu: 3.292385565084202 ns\nthreads: 1"
          },
          {
            "name": "emplace<fifo_cache_intrusive>",
            "value": 3.2826617143709718,
            "unit": "ns/iter",
            "extra": "iterations: 211585648\ncpu: 3.282347430294516 ns\nthreads: 1"
          },
          {
            "name": "update_insert<fifo_cache_intrusive>",
            "value": 3.28079509489239,
            "unit": "ns/iter",
            "extra": "iterations: 212652454\ncpu: 3.280479486025588 ns\nthreads: 1"
          },
          {
            "name": "update<fifo_cache_intrusive>",
            "value": 3.2723463773163095,
            "unit": "ns/iter",
            "extra": "iterations: 214174995\ncpu: 3.272070971683692 ns\nthreads: 1"
          },
          {
            "name": "find<fifo_cache_intrusive>",
            "value": 3.323559995061193,
            "unit": "ns/iter",
            "extra": "iterations: 214164387\ncpu: 3.3232368881199683 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<fifo_cache_intrusive>",
            "value": 3.8574711593987288,
            "unit": "ns/iter",
            "extra": "iterations: 181815904\ncpu: 3.857060958759692 ns\nthreads: 1"
          },
          {
            "name": "insert<lfu_cache_intrusive>",
            "value": 165.7922318052589,
            "unit": "ns/iter",
            "extra": "iterations: 4210914\ncpu: 165.78278160038417 ns\nthreads: 1"
          },
          {
            "name": "emplace<lfu_cache_intrusive>",
            "value": 168.86305492924933,
            "unit": "ns/iter",
            "extra": "iterations: 4157039\ncpu: 168.85028670647546 ns\nthreads: 1"
          },
          {
            "name": "update_insert<lfu_cache_intrusive>",
            "value": 169.50907417333534,
            "unit": "ns/iter",
            "extra": "iterations: 4210907\ncpu: 169.49459795716226 ns\nthreads: 1"
          },
          {
            "name": "update<lfu_cache_intrusive>",
            "value": 168.863180172513,
            "unit": "ns/iter",
            "extra": "iterations: 4175981\ncpu: 168.85833460449152 ns\nthreads: 1"
          },
          {
            "name": "find<lfu_cache_intrusive>",
            "value": 3.3766122524286235,
            "unit": "ns/iter",
            "extra": "iterations: 203349903\ncpu: 3.3764122769215206 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<lfu_cache_intrusive>",
            "value": 143.47358678172944,
            "unit": "ns/iter",
            "extra": "iterations: 5135194\ncpu: 143.45980794493855 ns\nthreads: 1"
          },
          {
            "name": "insert<lru_cache_intrusive>",
            "value": 3.3227802060599574,
            "unit": "ns/iter",
            "extra": "iterations: 210302527\ncpu: 3.3224244471394333 ns\nthreads: 1"
          },
          {
            "name": "emplace<lru_cache_intrusive>",
            "value": 3.3224009434957895,
            "unit": "ns/iter",
            "extra": "iterations: 209505122\ncpu: 3.3222004137922734 ns\nthreads: 1"
          },
          {
            "name": "update_insert<lru_cache_intrusive>",
            "value": 3.298620827716305,
            "unit": "ns/iter",
            "extra": "iterations: 210331518\ncpu: 3.2984474347777004 ns\nthreads: 1"
          },
          {
            "name": "update<lru_cache_intrusive>",
            "value": 3.2936725020031177,
            "unit": "ns/iter",
            "extra": "iterations: 212705417\ncpu: 3.2934060865972214 ns\nthreads: 1"
          },
          {
            "name": "find<lru_cache_intrusive>",
            "value": 3.2729175936746557,
            "unit": "ns/iter",
            "extra": "iterations: 213956767\ncpu: 3.272723853599832 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<lru_cache_intrusive>",
            "value": 3.868980339075679,
            "unit": "ns/iter",
            "extra": "iterations: 181267062\ncpu: 3.8688087193689924 ns\nthreads: 1"
          },
          {
            "name": "insert<naive_rr_cache_intrusive>",
            "value": 3.273334409830884,
            "unit": "ns/iter",
            "extra": "iterations: 206038530\ncpu: 3.2731651259596943 ns\nthreads: 1"
          },
          {
            "name": "emplace<naive_rr_cache_intrusive>",
            "value": 3.276432816417669,
            "unit": "ns/iter",
            "extra": "iterations: 213347517\ncpu: 3.2763053858273916 ns\nthreads: 1"
          },
          {
            "name": "update_insert<naive_rr_cache_intrusive>",
            "value": 3.2740746796903535,
            "unit": "ns/iter",
            "extra": "iterations: 213568802\ncpu: 3.2739041538473486 ns\nthreads: 1"
          },
          {
            "name": "update<naive_rr_cache_intrusive>",
            "value": 3.3016555962042657,
            "unit": "ns/iter",
            "extra": "iterations: 214059261\ncpu: 3.3014065950643374 ns\nthreads: 1"
          },
          {
            "name": "find<naive_rr_cache_intrusive>",
            "value": 9.299999987888441e-10,
            "unit": "ns/iter",
            "extra": "iterations: 1000000000000\ncpu: 5.640000004802914e-10 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<naive_rr_cache_intrusive>",
            "value": 3.265989338517828,
            "unit": "ns/iter",
            "extra": "iterations: 214393078\ncpu: 3.2658309565386263 ns\nthreads: 1"
          },
          {
            "name": "insert<rr_cache_intrusive>",
            "value": 3.272533746833811,
            "unit": "ns/iter",
            "extra": "iterations: 212969387\ncpu: 3.272286373252335 ns\nthreads: 1"
          },
          {
            "name": "emplace<rr_cache_intrusive>",
            "value": 3.2752295710689534,
            "unit": "ns/iter",
            "extra": "iterations: 213236255\ncpu: 3.2750297832795976 ns\nthreads: 1"
          },
          {
            "name": "update_insert<rr_cache_intrusive>",
            "value": 3.340031920982416,
            "unit": "ns/iter",
            "extra": "iterations: 208702850\ncpu: 3.3396781165182885 ns\nthreads: 1"
          },
          {
            "name": "update<rr_cache_intrusive>",
            "value": 3.403431345443164,
            "unit": "ns/iter",
            "extra": "iterations: 209978917\ncpu: 3.4030292050701423 ns\nthreads: 1"
          },
          {
            "name": "find<rr_cache_intrusive>",
            "value": 1.4540000279339438e-9,
            "unit": "ns/iter",
            "extra": "iterations: 1000000000000\ncpu: 6.959999971911657e-10 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<rr_cache_intrusive>",
            "value": 56.82850135982182,
            "unit": "ns/iter",
            "extra": "iterations: 13516113\ncpu: 56.822735131024565 ns\nthreads: 1"
          },
          {
            "name": "insert<ttl_cache_intrusive>",
            "value": 35.38150319982125,
            "unit": "ns/iter",
            "extra": "iterations: 20142219\ncpu: 35.378001053409314 ns\nthreads: 1"
          },
          {
            "name": "emplace<ttl_cache_intrusive>",
            "value": 35.08806606509443,
            "unit": "ns/iter",
            "extra": "iterations: 19970848\ncpu: 35.08484301718191 ns\nthreads: 1"
          },
          {
            "name": "update_insert<ttl_cache_intrusive>",
            "value": 3.4674752289278454,
            "unit": "ns/iter",
            "extra": "iterations: 208133302\ncpu: 3.4672582141612023 ns\nthreads: 1"
          },
          {
            "name": "update<ttl_cache_intrusive>",
            "value": 3.340327630092105,
            "unit": "ns/iter",
            "extra": "iterations: 211881392\ncpu: 3.3402019512879195 ns\nthreads: 1"
          },
          {
            "name": "find<ttl_cache_intrusive>",
            "value": 3.292020305651635,
            "unit": "ns/iter",
            "extra": "iterations: 210551235\ncpu: 3.2918213137054004 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<ttl_cache_intrusive>",
            "value": 4.431687964066773,
            "unit": "ns/iter",
            "extra": "iterations: 159873818\ncpu: 4.431512913515348 ns\nthreads: 1"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_intrusive>/insert/threads:4",
            "value": 112.27383747575266,
            "unit": "ns/iter",
            "extra": "iterations: 4800760\ncpu: 111.91382135328573 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_intrusive>/emplace/threads:4",
            "value": 111.69946674916265,
            "unit": "ns/iter",
            "extra": "iterations: 5534356\ncpu: 110.89015415705097 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_intrusive>/update_insert/threads:4",
            "value": 114.4786563198397,
            "unit": "ns/iter",
            "extra": "iterations: 5952488\ncpu: 114.22535013930312 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_intrusive>/update/threads:4",
            "value": 113.34936700769737,
            "unit": "ns/iter",
            "extra": "iterations: 5569736\ncpu: 113.24137104523449 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_intrusive>/find/threads:4",
            "value": 52.648435410614965,
            "unit": "ns/iter",
            "extra": "iterations: 13416044\ncpu: 52.43497166526885 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_intrusive>/insert_overflow/threads:4",
            "value": 146.54750958452175,
            "unit": "ns/iter",
            "extra": "iterations: 4101404\ncpu: 146.3335925941458 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_intrusive>/request/threads:4",
            "value": 142.91914713181836,
            "unit": "ns/iter",
            "extra": "iterations: 4506816\ncpu: 142.7967185702722 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_intrusive>/request_hot/threads:4",
            "value": 145.4492354050088,
            "unit": "ns/iter",
            "extra": "iterations: 4547244\ncpu: 127.28691136873239 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_intrusive>/many_shards/threads:4",
            "value": 85.73882881997413,
            "unit": "ns/iter",
            "extra": "iterations: 7815468\ncpu: 85.21461069253947 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_intrusive>/insert/threads:4",
            "value": 552.7008971313275,
            "unit": "ns/iter",
            "extra": "iterations: 1884228\ncpu: 551.3629735891836 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_intrusive>/emplace/threads:4",
            "value": 529.6698891264521,
            "unit": "ns/iter",
            "extra": "iterations: 1805660\ncpu: 529.2508539813698 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_intrusive>/update_insert/threads:4",
            "value": 575.0040211702116,
            "unit": "ns/iter",
            "extra": "iterations: 1748496\ncpu: 568.3154511076948 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_intrusive>/update/threads:4",
            "value": 514.9824184418569,
            "unit": "ns/iter",
            "extra": "iterations: 1529216\ncpu: 514.3244662624508 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_intrusive>/find/threads:4",
            "value": 111.32198621151166,
            "unit": "ns/iter",
            "extra": "iterations: 6234180\ncpu: 111.27405560955881 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_intrusive>/insert_overflow/threads:4",
            "value": 445.46453156208975,
            "unit": "ns/iter",
            "extra": "iterations: 1838536\ncpu: 445.1809891130769 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_intrusive>/request/threads:4",
            "value": 553.1205021433309,
            "unit": "ns/iter",
            "extra": "iterations: 1647896\ncpu: 549.6322595600694 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_intrusive>/request_hot/threads:4",
            "value": 552.8276656410894,
            "unit": "ns/iter",
            "extra": "iterations: 1553660\ncpu: 548.8470991079128 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_intrusive>/many_shards/threads:4",
            "value": 464.43159226411365,
            "unit": "ns/iter",
            "extra": "iterations: 1907372\ncpu: 463.8502043649585 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_intrusive>/insert/threads:4",
            "value": 130.21447450000778,
            "unit": "ns/iter",
            "extra": "iterations: 4000000\ncpu: 129.77621349999998 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_intrusive>/emplace/threads:4",
            "value": 127.35118957240837,
            "unit": "ns/iter",
            "extra": "iterations: 5585032\ncpu: 126.568384388845 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_intrusive>/update_insert/threads:4",
            "value": 128.651636105825,
            "unit": "ns/iter",
            "extra": "iterations: 5441824\ncpu: 128.46403264787682 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_intrusive>/update/threads:4",
            "value": 128.17513213986132,
            "unit": "ns/iter",
            "extra": "iterations: 5123360\ncpu: 127.25713613722247 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_intrusive>/find/threads:4",
            "value": 56.45158049267538,
            "unit": "ns/iter",
            "extra": "iterations: 12278956\ncpu: 56.32703488798231 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_intrusive>/insert_overflow/threads:4",
            "value": 152.9774074780089,
            "unit": "ns/iter",
            "extra": "iterations: 4154472\ncpu: 152.79009847701468 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_intrusive>/request/threads:4",
            "value": 157.72740404599537,
            "unit": "ns/iter",
            "extra": "iterations: 4359276\ncpu: 157.61109069487694 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_intrusive>/request_hot/threads:4",
            "value": 156.1589289220367,
            "unit": "ns/iter",
            "extra": "iterations: 4339292\ncpu: 156.0814655939264 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_intrusive>/many_shards/threads:4",
            "value": 106.33303332560368,
            "unit": "ns/iter",
            "extra": "iterations: 6466500\ncpu: 106.18272249284786 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_intrusive>/insert/threads:4",
            "value": 77.72510391536623,
            "unit": "ns/iter",
            "extra": "iterations: 8966912\ncpu: 77.6140452811403 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_intrusive>/emplace/threads:4",
            "value": 77.73018257224626,
            "unit": "ns/iter",
            "extra": "iterations: 8907816\ncpu: 77.59686257551787 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_intrusive>/update_insert/threads:4",
            "value": 78.15635787164884,
            "unit": "ns/iter",
            "extra": "iterations: 8858908\ncpu: 77.97542733257855 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_intrusive>/update/threads:4",
            "value": 79.62014205267528,
            "unit": "ns/iter",
            "extra": "iterations: 8859812\ncpu: 79.23276193670922 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_intrusive>/find/threads:4",
            "value": 48.342313601509844,
            "unit": "ns/iter",
            "extra": "iterations: 14517124\ncpu: 48.32835642927625 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_intrusive>/insert_overflow/threads:4",
            "value": 62.44056749813341,
            "unit": "ns/iter",
            "extra": "iterations: 11160072\ncpu: 62.373473665761296 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_intrusive>/request/threads:4",
            "value": 88.88287697363948,
            "unit": "ns/iter",
            "extra": "iterations: 7863680\ncpu: 88.36977814458385 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_intrusive>/request_hot/threads:4",
            "value": 88.62570391875428,
            "unit": "ns/iter",
            "extra": "iterations: 7896508\ncpu: 88.50837534768527 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_intrusive>/many_shards/threads:4",
            "value": 71.62942131565798,
            "unit": "ns/iter",
            "extra": "iterations: 9495816\ncpu: 71.61388120831323 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_intrusive>/insert/threads:4",
            "value": 95.9372861903073,
            "unit": "ns/iter",
            "extra": "iterations: 7118480\ncpu: 95.67664627841896 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_intrusive>/emplace/threads:4",
            "value": 94.96025957172398,
            "unit": "ns/iter",
            "extra": "iterations: 7009084\ncpu: 94.75980770097779 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_intrusive>/update_insert/threads:4",
            "value": 98.59580917787856,
            "unit": "ns/iter",
            "extra": "iterations: 6812124\ncpu: 97.60861149914476 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_intrusive>/update/threads:4",
            "value": 96.33085541024778,
            "unit": "ns/iter",
            "extra": "iterations: 7061828\ncpu: 96.08341522903125 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_intrusive>/find/threads:4",
            "value": 51.30758887300605,
            "unit": "ns/iter",
            "extra": "iterations: 13724752\ncpu: 51.25505684911469 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_intrusive>/insert_overflow/threads:4",
            "value": 134.10612434725851,
            "unit": "ns/iter",
            "extra": "iterations: 4154108\ncpu: 133.02123873524747 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_intrusive>/request/threads:4",
            "value": 126.82132131682965,
            "unit": "ns/iter",
            "extra": "iterations: 4681784\ncpu: 126.66000140117558 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_intrusive>/request_hot/threads:4",
            "value": 126.37373054507188,
            "unit": "ns/iter",
            "extra": "iterations: 4780792\ncpu: 126.24618619676433 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_intrusive>/many_shards/threads:4",
            "value": 77.56891822027376,
            "unit": "ns/iter",
            "extra": "iterations: 8681028\ncpu: 77.46027083428363 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_intrusive>/insert/threads:4",
            "value": 251.9410708611686,
            "unit": "ns/iter",
            "extra": "iterations: 3562516\ncpu: 250.5042380160536 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_intrusive>/emplace/threads:4",
            "value": 218.81335097191547,
            "unit": "ns/iter",
            "extra": "iterations: 3564160\ncpu: 218.56323200978622 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_intrusive>/update_insert/threads:4",
            "value": 149.07818497228428,
            "unit": "ns/iter",
            "extra": "iterations: 4187352\ncpu: 148.89792737749272 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_intrusive>/update/threads:4",
            "value": 145.13410527846946,
            "unit": "ns/iter",
            "extra": "iterations: 4264120\ncpu: 144.92377864600445 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_intrusive>/find/threads:4",
            "value": 76.71608415612455,
            "unit": "ns/iter",
            "extra": "iterations: 9182552\ncpu: 76.431667035482 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_intrusive>/insert_overflow/threads:4",
            "value": 250.8526432318919,
            "unit": "ns/iter",
            "extra": "iterations: 3050508\ncpu: 250.43434175553696 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_intrusive>/request/threads:4",
            "value": 248.70666111724438,
            "unit": "ns/iter",
            "extra": "iterations: 3178716\ncpu: 248.50354010864734 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_intrusive>/request_hot/threads:4",
            "value": 258.1797219893976,
            "unit": "ns/iter",
            "extra": "iterations: 2820612\ncpu: 256.86108085762925 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_intrusive>/many_shards/threads:4",
            "value": 195.31045720293253,
            "unit": "ns/iter",
            "extra": "iterations: 3607676\ncpu: 194.15492328024985 ns\nthreads: 4"
          },
          {
            "name": "insert<fifo_cache_std>",
            "value": 3.316528559952148,
            "unit": "ns/iter",
            "extra": "iterations: 210207496\ncpu: 3.316385996054109 ns\nthreads: 1"
          },
          {
            "name": "emplace<fifo_cache_std>",
            "value": 3.3116748249255235,
            "unit": "ns/iter",
            "extra": "iterations: 209935525\ncpu: 3.3116262004727406 ns\nthreads: 1"
          },
          {
            "name": "update_insert<fifo_cache_std>",
            "value": 3.394783022076363,
            "unit": "ns/iter",
            "extra": "iterations: 207615638\ncpu: 3.3946729195803647 ns\nthreads: 1"
          },
          {
            "name": "update<fifo_cache_std>",
            "value": 3.3244081894816304,
            "unit": "ns/iter",
            "extra": "iterations: 210593207\ncpu: 3.3241904189245783 ns\nthreads: 1"
          },
          {
            "name": "find<fifo_cache_std>",
            "value": 1.8129999830307498e-9,
            "unit": "ns/iter",
            "extra": "iterations: 1000000000000\ncpu: 6.930000004956583e-10 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<fifo_cache_std>",
            "value": 20.005510422662447,
            "unit": "ns/iter",
            "extra": "iterations: 35046132\ncpu: 20.00424092450487 ns\nthreads: 1"
          },
          {
            "name": "insert<lfu_cache_std>",
            "value": 34.59992623589629,
            "unit": "ns/iter",
            "extra": "iterations: 20042269\ncpu: 34.59754042818207 ns\nthreads: 1"
          },
          {
            "name": "emplace<lfu_cache_std>",
            "value": 34.463461878736545,
            "unit": "ns/iter",
            "extra": "iterations: 20034815\ncpu: 34.4608684931705 ns\nthreads: 1"
          },
          {
            "name": "update_insert<lfu_cache_std>",
            "value": 34.37819041208042,
            "unit": "ns/iter",
            "extra": "iterations: 20131412\ncpu: 34.374768347098566 ns\nthreads: 1"
          },
          {
            "name": "update<lfu_cache_std>",
            "value": 33.88567599915451,
            "unit": "ns/iter",
            "extra": "iterations: 20522681\ncpu: 33.883311980535055 ns\nthreads: 1"
          },
          {
            "name": "find<lfu_cache_std>",
            "value": 4.914823383498761,
            "unit": "ns/iter",
            "extra": "iterations: 135336052\ncpu: 4.914382931755686 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<lfu_cache_std>",
            "value": 59.62296370757524,
            "unit": "ns/iter",
            "extra": "iterations: 11631188\ncpu: 59.619830493669255 ns\nthreads: 1"
          },
          {
            "name": "insert<lru_cache_std>",
            "value": 8.115881715291676,
            "unit": "ns/iter",
            "extra": "iterations: 130932123\ncpu: 8.115373963652909 ns\nthreads: 1"
          },
          {
            "name": "emplace<lru_cache_std>",
            "value": 7.251481025450496,
            "unit": "ns/iter",
            "extra": "iterations: 121920761\ncpu: 7.251225105132015 ns\nthreads: 1"
          },
          {
            "name": "update_insert<lru_cache_std>",
            "value": 5.3636532468212375,
            "unit": "ns/iter",
            "extra": "iterations: 124696420\ncpu: 5.363219890354516 ns\nthreads: 1"
          },
          {
            "name": "update<lru_cache_std>",
            "value": 5.323256857128785,
            "unit": "ns/iter",
            "extra": "iterations: 131189347\ncpu: 5.3230341789871085 ns\nthreads: 1"
          },
          {
            "name": "find<lru_cache_std>",
            "value": 4.888087278670225,
            "unit": "ns/iter",
            "extra": "iterations: 143082840\ncpu: 4.887712411914661 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<lru_cache_std>",
            "value": 19.985091655339513,
            "unit": "ns/iter",
            "extra": "iterations: 34607531\ncpu: 19.984066040423432 ns\nthreads: 1"
          },
          {
            "name": "insert<naive_rr_cache_std>",
            "value": 1.3240163725318028,
            "unit": "ns/iter",
            "extra": "iterations: 526002213\ncpu: 1.3239740000865794 ns\nthreads: 1"
          },
          {
            "name": "emplace<naive_rr_cache_std>",
            "value": 1.349198112731963,
            "unit": "ns/iter",
            "extra": "iterations: 520825017\ncpu: 1.3491319139149565 ns\nthreads: 1"
          },
          {
            "name": "update_insert<naive_rr_cache_std>",
            "value": 1.3920866980106161,
            "unit": "ns/iter",
            "extra": "iterations: 499691558\ncpu: 1.3920220141081532 ns\nthreads: 1"
          },
          {
            "name": "update<naive_rr_cache_std>",
            "value": 1.4052400139944177,
            "unit": "ns/iter",
            "extra": "iterations: 501047784\ncpu: 1.4051756369009292 ns\nthreads: 1"
          },
          {
            "name": "find<naive_rr_cache_std>",
            "value": 8.590000106778462e-10,
            "unit": "ns/iter",
            "extra": "iterations: 1000000000000\ncpu: 4.3699999707769165e-10 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<naive_rr_cache_std>",
            "value": 1.4069152376631935,
            "unit": "ns/iter",
            "extra": "iterations: 502275145\ncpu: 1.4069063222310147 ns\nthreads: 1"
          },
          {
            "name": "insert<rr_cache_std>",
            "value": 3.2774098046831632,
            "unit": "ns/iter",
            "extra": "iterations: 212565360\ncpu: 3.2772888630584034 ns\nthreads: 1"
          },
          {
            "name": "emplace<rr_cache_std>",
            "value": 3.2800890547514205,
            "unit": "ns/iter",
            "extra": "iterations: 213265656\ncpu: 3.279985451572197 ns\nthreads: 1"
          },
          {
            "name": "update_insert<rr_cache_std>",
            "value": 3.3451988438778373,
            "unit": "ns/iter",
            "extra": "iterations: 210526522\ncpu: 3.3450591322645833 ns\nthreads: 1"
          },
          {
            "name": "update<rr_cache_std>",
            "value": 3.3105806545825356,
            "unit": "ns/iter",
            "extra": "iterations: 209928146\ncpu: 3.3104521153633266 ns\nthreads: 1"
          },
          {
            "name": "find<rr_cache_std>",
            "value": 1.0150000093744893e-9,
            "unit": "ns/iter",
            "extra": "iterations: 1000000000000\ncpu: 6.399999996631322e-10 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<rr_cache_std>",
            "value": 20.308766763703872,
            "unit": "ns/iter",
            "extra": "iterations: 34535401\ncpu: 20.307431959455098 ns\nthreads: 1"
          },
          {
            "name": "insert<ttl_cache_std>",
            "value": 36.50763637928814,
            "unit": "ns/iter",
            "extra": "iterations: 19146181\ncpu: 36.50564538170826 ns\nthreads: 1"
          },
          {
            "name": "emplace<ttl_cache_std>",
            "value": 37.014448319295234,
            "unit": "ns/iter",
            "extra": "iterations: 19133921\ncpu: 37.013222120024494 ns\nthreads: 1"
          },
          {
            "name": "update_insert<ttl_cache_std>",
            "value": 5.437245291457618,
            "unit": "ns/iter",
            "extra": "iterations: 116994751\ncpu: 5.437033683673529 ns\nthreads: 1"
          },
          {
            "name": "update<ttl_cache_std>",
            "value": 5.523140078433837,
            "unit": "ns/iter",
            "extra": "iterations: 129268771\ncpu: 5.52276045078204 ns\nthreads: 1"
          },
          {
            "name": "find<ttl_cache_std>",
            "value": 4.922109584338237,
            "unit": "ns/iter",
            "extra": "iterations: 135102463\ncpu: 4.921884170239006 ns\nthreads: 1"
          },
          {
            "name": "insert_overflow<ttl_cache_std>",
            "value": 20.92003869358332,
            "unit": "ns/iter",
            "extra": "iterations: 33182762\ncpu: 20.918409594716625 ns\nthreads: 1"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_std>/insert/threads:4",
            "value": 117.93452093753555,
            "unit": "ns/iter",
            "extra": "iterations: 6134816\ncpu: 117.83614667497773 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_std>/emplace/threads:4",
            "value": 115.49191227268237,
            "unit": "ns/iter",
            "extra": "iterations: 5083752\ncpu: 115.38475873724761 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_std>/update_insert/threads:4",
            "value": 165.8563751664744,
            "unit": "ns/iter",
            "extra": "iterations: 4619048\ncpu: 156.96332231230335 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_std>/update/threads:4",
            "value": 149.20623441609254,
            "unit": "ns/iter",
            "extra": "iterations: 4692340\ncpu: 140.06893682043503 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_std>/find/threads:4",
            "value": 39.45797066588513,
            "unit": "ns/iter",
            "extra": "iterations: 17843252\ncpu: 39.42827731178151 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_std>/insert_overflow/threads:4",
            "value": 111.60234328933863,
            "unit": "ns/iter",
            "extra": "iterations: 4687684\ncpu: 111.4750887645157 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_std>/request/threads:4",
            "value": 123.48831061160323,
            "unit": "ns/iter",
            "extra": "iterations: 4836780\ncpu: 122.83403338584763 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_std>/request_hot/threads:4",
            "value": 126.4607782963383,
            "unit": "ns/iter",
            "extra": "iterations: 5050724\ncpu: 126.32002501027576 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_fifo_cache_std>/many_shards/threads:4",
            "value": 133.14460702587232,
            "unit": "ns/iter",
            "extra": "iterations: 4510424\ncpu: 133.04252504864291 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_std>/insert/threads:4",
            "value": 766.6093931547106,
            "unit": "ns/iter",
            "extra": "iterations: 1745420\ncpu: 674.420017531597 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_std>/emplace/threads:4",
            "value": 657.1200976714264,
            "unit": "ns/iter",
            "extra": "iterations: 1706128\ncpu: 656.7054980634514 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_std>/update_insert/threads:4",
            "value": 660.5910735245127,
            "unit": "ns/iter",
            "extra": "iterations: 1765288\ncpu: 659.0684188642304 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_std>/update/threads:4",
            "value": 676.4819779494629,
            "unit": "ns/iter",
            "extra": "iterations: 1273440\ncpu: 672.3834126460611 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_std>/find/threads:4",
            "value": 185.63607900170456,
            "unit": "ns/iter",
            "extra": "iterations: 4234136\ncpu: 185.60539245787103 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_std>/insert_overflow/threads:4",
            "value": 607.0563369076539,
            "unit": "ns/iter",
            "extra": "iterations: 1659232\ncpu: 601.1564404495575 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_std>/request/threads:4",
            "value": 632.1218681306577,
            "unit": "ns/iter",
            "extra": "iterations: 1728680\ncpu: 630.8966448388366 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_std>/request_hot/threads:4",
            "value": 654.7717018811053,
            "unit": "ns/iter",
            "extra": "iterations: 1446188\ncpu: 654.1522616699901 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lfu_cache_std>/many_shards/threads:4",
            "value": 699.2594476340475,
            "unit": "ns/iter",
            "extra": "iterations: 1581560\ncpu: 697.7547902071373 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_std>/insert/threads:4",
            "value": 230.84451207655303,
            "unit": "ns/iter",
            "extra": "iterations: 4113760\ncpu: 229.72196384815837 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_std>/emplace/threads:4",
            "value": 218.68872533061256,
            "unit": "ns/iter",
            "extra": "iterations: 4178056\ncpu: 218.4033359055026 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_std>/update_insert/threads:4",
            "value": 225.65875944697584,
            "unit": "ns/iter",
            "extra": "iterations: 4453272\ncpu: 225.36440936012897 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_std>/update/threads:4",
            "value": 200.52049978087453,
            "unit": "ns/iter",
            "extra": "iterations: 4070824\ncpu: 200.22893989030248 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_std>/find/threads:4",
            "value": 50.364662353375536,
            "unit": "ns/iter",
            "extra": "iterations: 14247440\ncpu: 50.35689099234668 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_std>/insert_overflow/threads:4",
            "value": 207.21464018886394,
            "unit": "ns/iter",
            "extra": "iterations: 3856412\ncpu: 206.75378823631908 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_std>/request/threads:4",
            "value": 212.0833851989698,
            "unit": "ns/iter",
            "extra": "iterations: 3727580\ncpu: 210.8864397276516 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_std>/request_hot/threads:4",
            "value": 188.1434098411376,
            "unit": "ns/iter",
            "extra": "iterations: 4138140\ncpu: 187.67611753106485 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_lru_cache_std>/many_shards/threads:4",
            "value": 204.1707344707199,
            "unit": "ns/iter",
            "extra": "iterations: 4104180\ncpu: 203.7393462275045 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_std>/insert/threads:4",
            "value": 77.96032283663111,
            "unit": "ns/iter",
            "extra": "iterations: 8972340\ncpu: 77.47367019083096 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_std>/emplace/threads:4",
            "value": 78.26831903674382,
            "unit": "ns/iter",
            "extra": "iterations: 8896756\ncpu: 78.21055865756009 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_std>/update_insert/threads:4",
            "value": 80.29268985218829,
            "unit": "ns/iter",
            "extra": "iterations: 8720152\ncpu: 80.14553198155248 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_std>/update/threads:4",
            "value": 80.54762813503983,
            "unit": "ns/iter",
            "extra": "iterations: 8688100\ncpu: 77.06992069612463 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_std>/find/threads:4",
            "value": 49.78434448308645,
            "unit": "ns/iter",
            "extra": "iterations: 14225720\ncpu: 49.7048938120531 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_std>/insert_overflow/threads:4",
            "value": 63.62557308194214,
            "unit": "ns/iter",
            "extra": "iterations: 11058628\ncpu: 63.25097923539881 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_std>/request/threads:4",
            "value": 96.49358403611888,
            "unit": "ns/iter",
            "extra": "iterations: 7246456\ncpu: 96.28636494860379 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_std>/request_hot/threads:4",
            "value": 95.97336608293409,
            "unit": "ns/iter",
            "extra": "iterations: 7287212\ncpu: 95.37308191390616 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_naive_rr_cache_std>/many_shards/threads:4",
            "value": 73.52722905072409,
            "unit": "ns/iter",
            "extra": "iterations: 9155736\ncpu: 73.44647115207336 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_std>/insert/threads:4",
            "value": 92.29194779135251,
            "unit": "ns/iter",
            "extra": "iterations: 7487188\ncpu: 92.06781771741281 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_std>/emplace/threads:4",
            "value": 88.58680356405597,
            "unit": "ns/iter",
            "extra": "iterations: 7378568\ncpu: 88.26965706624924 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_std>/update_insert/threads:4",
            "value": 97.59461325032728,
            "unit": "ns/iter",
            "extra": "iterations: 7105992\ncpu: 97.48836953376814 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_std>/update/threads:4",
            "value": 95.33902447304078,
            "unit": "ns/iter",
            "extra": "iterations: 7304688\ncpu: 95.21458137568636 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_std>/find/threads:4",
            "value": 38.2274912352694,
            "unit": "ns/iter",
            "extra": "iterations: 17690220\ncpu: 38.19101950117069 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_std>/insert_overflow/threads:4",
            "value": 103.20114367915613,
            "unit": "ns/iter",
            "extra": "iterations: 5050892\ncpu: 103.07879063737643 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_std>/request/threads:4",
            "value": 88.76775653313342,
            "unit": "ns/iter",
            "extra": "iterations: 7708712\ncpu: 88.65085762705885 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_std>/request_hot/threads:4",
            "value": 87.82726369585485,
            "unit": "ns/iter",
            "extra": "iterations: 7692332\ncpu: 87.61360768099985 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_rr_cache_std>/many_shards/threads:4",
            "value": 91.6169173638869,
            "unit": "ns/iter",
            "extra": "iterations: 7448560\ncpu: 91.56837724338659 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_std>/insert/threads:4",
            "value": 288.94675240672234,
            "unit": "ns/iter",
            "extra": "iterations: 3206436\ncpu: 288.5731996521994 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_std>/emplace/threads:4",
            "value": 283.33198787189605,
            "unit": "ns/iter",
            "extra": "iterations: 3322528\ncpu: 282.7604357284574 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_std>/update_insert/threads:4",
            "value": 205.96672847505002,
            "unit": "ns/iter",
            "extra": "iterations: 3790208\ncpu: 204.0256943682248 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_std>/update/threads:4",
            "value": 221.5759041232132,
            "unit": "ns/iter",
            "extra": "iterations: 3590108\ncpu: 221.3980554345437 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_std>/find/threads:4",
            "value": 84.73703269192794,
            "unit": "ns/iter",
            "extra": "iterations: 9192116\ncpu: 84.7027792077472 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_std>/insert_overflow/threads:4",
            "value": 277.4408853819832,
            "unit": "ns/iter",
            "extra": "iterations: 2852780\ncpu: 275.7134812358464 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_std>/request/threads:4",
            "value": 279.5153230514078,
            "unit": "ns/iter",
            "extra": "iterations: 3015000\ncpu: 279.1250749585402 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_std>/request_hot/threads:4",
            "value": 284.29155018679415,
            "unit": "ns/iter",
            "extra": "iterations: 2851708\ncpu: 283.8346012284572 ns\nthreads: 4"
          },
          {
            "name": "cache_fixture<thread_safe_ttl_cache_std>/many_shards/threads:4",
            "value": 277.418300722842,
            "unit": "ns/iter",
            "extra": "iterations: 2926256\ncpu: 277.0968524284955 ns\nthreads: 4"
          }
        ]
      }
    ]
  }
}