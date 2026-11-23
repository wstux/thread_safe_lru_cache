window.ADAPTER_CONFIG = {
  // Path by default data.js relatively index.html
  dataUrl: "./data.js",

  // Project metadata for ASV head
  project: "thread_safe_lru_cache",
  branch: "master",
  machine: "github-runner",

  // Slowdown threshold for the Regressions tab (1.10 = warn if the slowdown exceeds 10%)
  regressionThreshold: 1.10,

  // ==========================================================================
  // 1. Filtering (works based on the original test names from data.js)
  // Can mix exact strings ("insert<lru_cache>") and regular expressions (/naive_rr/)
  filters: {
    include: [/.*/],
    exclude: [
      // Example:
      // /naive_rr_cache/,
      // "cache_fixture<thread_safe_dm_cache_std>/many_shards/threads:4"
    ]
  },

  // ==========================================================================
  // 2. Renaming (applied sequentially from top to bottom)
  // Important: avoid using parentheses (...) in new names — within ASV, they
  // are reserved for parameter lists; use square brackets [...] instead.
  renames: [
    { match: /naive_rr/g,     replace: "dm" },
    { match: /_intrusive\b/g, replace: " [intrusive]" },
    { match: /_std\b/g,       replace: " [std]" },
    { match: /\b(dm|fifo|lfu|lru|naive_rr|rr|ttl)_cache/g, replace: (_, t) => t.toUpperCase() }
  ],

  // ==========================================================================
  // 3. Grouping andD parameter axes (based on the renamed names)
  // Checked from top to bottom until the first match is found.
  // - section: a major section heading on the main page (Benchmark Grid)
  // - chart: the name of an individual chart within a section
  // - params: a dictionary of parameter axes that can be toggled within a chart
  groups: [
    {
      // "cache_fixture<thread_safe_LRU [intrusive]>/insert/threads:4"
      match: /^cache_fixture<([^>]+)>\/(?!threads:)([^/]+)(?:\/threads:(\d+))?$/,
      section: "Thread safe",
      chart: "$2",
      params: {
        cache_type: "$1",
        threads: (m) => `threads:${m.at(3) || 1}`
      },
      //metrics: [
      //  {
      //    section: "Thread safe (Wall Time)",
      //    unit: (b) => b.unit || "ns/iter",
      //    getValue: (b) => Number(b.value)
      //  },
      //  {
      //    section: "Thread safe (CPU Time)",
      //    unit: "ns",
      //    getValue: (b, extra) => extra.cpu ?? null
      //  }
      //]
    },
    {
      // "insert<FIFO [intrusive]>"
      match: /^([^<]+)<([^>]+)>$/,
      section: "Thread unsafe",
      chart: "$1",
      params: {
        cache_type: "$2",
        threads: "threads:1"
      },
      //metrics: [
      //  {
      //    section: "Thread unsafe (Wall Time)",
      //    unit: (b) => b.unit || "ns/iter",
      //    getValue: (b) => Number(b.value)
      //  },
      //  {
      //    section: "Thread unsafe (CPU Time)",
      //    unit: "ns",
      //    getValue: (b, extra) => extra.cpu ?? null
      //  }
      //]
    },
    {
      // Default rule
      match: /^(.*)$/,
      section: "Other",
      chart: "$1",
      params: {
        variant: "default"
      }
    }
  ]
};
