// ms14_engine.cpp — an INDEPENDENT implementation of the capped AC
// quotient-component search whose semantics are documented in the ms14 artifact
// (github.com/alejandrozarco/andrews-curtis-ms14, README "Search model"):
//
//   States: unordered pairs {u, v} of nonempty cyclically reduced words over
//   x, X=x^-1, y, Y=y^-1, modulo rotation and inversion of either relator,
//   swapping the two, and the 8 signed letter permutations phi (x<->y,
//   x->x^-1, y->y^-1).  A move replaces u by the cyclic reduction of
//        rot_i(u) * c * rot_j(v^s) * c^-1        (s = +-1)
//   with c a reduced word; for c = 1 all i,j are used, for |c| >= 1 only
//   junction-free c (every other c reduces to a shorter one with other i,j).
//   Only states with total relator length <= cap are kept.
//
// This file was written in this run. It shares the *semantics* (as published)
// but not the code of the ms14 acsearch.cpp; reproducing its published counts
// is a cross-implementation check. Letter codes match the published dumps:
// 0=x, 1=X, 2=y, 3=Y (inverse = code XOR 1).
//
// Usage:
//   ms14_engine --start "XyyxYYY|XXXYxxy" --cap 29 --conj 8 [--threads 8]
//               [--maxstates 150000000] [--nosym] [--target T]... [--dump F]
//               [--goal F]
//   --dump F   write sorted 128-bit keys of all states (binary) to F
//   --goal F   read a sorted key file and report any intersection hit
//
// Key packing (matches the published convention so key files are comparable):
//   after sorting the pair by (length, packed word), with (na,a) first:
//   key = na<<122 | nb<<116 | a << (2*nb) | b      (fits u128 for total <= 58)
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

using namespace std;
typedef unsigned __int128 u128;
typedef uint64_t u64;
typedef uint32_t u32;

static int CAP = 28;
static int CONJ = 0;
static int THREADS = 4;
static bool NOSYM = false;
static u64 MAXSTATES = 150000000;
static const int MAXW = 58;

static chrono::steady_clock::time_point T0;
static double elapsed() {
    return chrono::duration<double>(chrono::steady_clock::now() - T0).count();
}

struct Wd {
    int n = 0;
    uint8_t c[64];
};

static inline int invl(int a) { return a ^ 1; }

static inline u128 maskn(int n) {
    if (n >= 64) return ~(u128)0;
    return (((u128)1) << (2 * n)) - 1;
}

static inline u128 packw(const uint8_t* c, int n) {
    u128 w = 0;
    for (int i = 0; i < n; i++) w = (w << 2) | c[i];
    return w;
}

static inline void unpackw(u128 w, int n, uint8_t* c) {
    for (int i = 0; i < n; i++) c[i] = (uint8_t)((w >> (2 * (n - 1 - i))) & 3);
}

// inverse of a packed word (reverse letters, flip the sign bit of each)
static inline u128 invw(u128 w, int n) {
    u128 r = 0;
    for (int i = 0; i < n; i++) r = (r << 2) | (uint8_t)((((u128)w >> (2 * i)) & 3) ^ 1);
    return r;
}

// phi transform on a packed word: p bit0 = invert x, bit1 = invert y,
// bit2 = swap x<->y (X<->Y); composition in that order.
static inline u128 phiw(u128 w, int n, int p) {
    u128 m = maskn(n);
    u128 g = (w >> 1) & (u128)0x5555555555555555ULL;   // high bit of each 2-bit letter
    u128 v = w & m;
    if (p & 1) v ^= (~g) & (u128)0x5555555555555555ULL & m;  // flip x<->X
    if (p & 2) v ^= g & m;                                   // flip y<->Y
    if (p & 4) v ^= ((u128)0xAAAAAAAAAAAAAAAAULL) & m;       // flip bit1: swap
    return v;
}

// min over rotations of w and of inverse(w), as a packed word
static inline u128 canonW(u128 w, int n) {
    u128 m = maskn(n);
    u128 iw = invw(w, n);
    u128 best = w < iw ? w : iw;
    for (int k = 1; k < n; k++) {
        u128 a = ((w << (2 * k)) | (w >> (2 * (n - k)))) & m;
        u128 b = ((iw << (2 * k)) | (iw >> (2 * (n - k)))) & m;
        if (a < best) best = a;
        if (b < best) best = b;
    }
    return best;
}

// canonical key of the class of the pair (u, v)
static inline u128 classKey(u128 wu, int nu, u128 wv, int nv) {
    u128 best = ~(u128)0;
    int np = NOSYM ? 1 : 8;
    for (int p = 0; p < np; p++) {
        u128 a = canonW(phiw(wu, nu, p), nu);
        u128 b = canonW(phiw(wv, nv, p), nv);
        u128 k;
        if (nv < nu || (nv == nu && b < a)) {
            // shorter first: (nv,b) then (nu,a)
            k = ((u128)nv << 122) | ((u128)nu << 116) | (b << (2 * nu)) | a;
        } else {
            k = ((u128)nu << 122) | ((u128)nv << 116) | (a << (2 * nv)) | b;
        }
        if (k < best) best = k;
    }
    return best;
}

// parse a word string with letters x,X,y,Y; freely + cyclically reduce
static bool parseWord(const string& s, Wd& w) {
    w.n = 0;
    for (char ch : s) {
        int l = ch == 'x' ? 0 : ch == 'X' ? 1 : ch == 'y' ? 2 : ch == 'Y' ? 3 : -1;
        if (l < 0) return false;
        if (w.n && (w.c[w.n - 1] ^ 1) == l) w.n--;
        else w.c[w.n++] = (uint8_t)l;
    }
    int lo = 0, hi = w.n - 1;
    while (lo < hi && (w.c[lo] ^ 1) == w.c[hi]) { lo++; hi--; }
    int m = hi - lo + 1;
    if (m <= 0) return false;
    if (lo) memmove(w.c, w.c + lo, m);
    w.n = m;
    return true;
}

static string wordStr(const Wd& w) {
    static const char* C = "xXyY";
    string s;
    for (int i = 0; i < w.n; i++) s += C[w.c[i]];
    return s;
}

static u128 keyOfSpec(const string& spec) {
    size_t bar = spec.find('|');
    if (bar == string::npos) { fprintf(stderr, "bad presentation %s\n", spec.c_str()); exit(2); }
    Wd a, b;
    if (!parseWord(spec.substr(0, bar), a) || !parseWord(spec.substr(bar + 1), b)) {
        fprintf(stderr, "bad word in %s\n", spec.c_str()); exit(2);
    }
    return classKey(packw(a.c, a.n), a.n, packw(b.c, b.n), b.n);
}

static string keyStr(u128 k) {
    int na = (int)(k >> 122);
    int nb = (int)((k >> 116) & 63);
    u128 low = k;
    uint8_t bc[64], ac[64];
    for (int i = nb - 1; i >= 0; i--) { bc[i] = (uint8_t)(low & 3); low >>= 2; }
    for (int i = na - 1; i >= 0; i--) { ac[i] = (uint8_t)(low & 3); low >>= 2; }
    Wd A; A.n = na; memcpy(A.c, ac, na);
    Wd B; B.n = nb; memcpy(B.c, bc, nb);
    return wordStr(A) + "|" + wordStr(B);
}

// ---------------------------------------------------------------- hash set ---
struct Table {
    vector<u128> keys;
    vector<u32> slot;
    u64 nslots = 0;
    void init(u64 maxstates) {
        nslots = maxstates * 5 / 4 + 1024;
        slot.assign(nslots, 0);
        keys.reserve(maxstates);
    }
    static inline u64 h(u128 k) {
        u64 x = (u64)k ^ (u64)(k >> 64) * 0x9E3779B97F4A7C15ULL;
        x ^= x >> 31; x *= 0xBF58476D1CE4E5B9ULL; x ^= x >> 29;
        x *= 0x94D049BB133111EBULL; x ^= x >> 32;
        return x;
    }
    inline bool contains(u128 k) const {
        for (u64 i = h(k) % nslots;; i = (i + 1 == nslots) ? 0 : i + 1) {
            u32 s = slot[i];
            if (!s) return false;
            if (keys[s - 1] == k) return true;
        }
    }
    // returns true if newly inserted
    inline bool insert(u128 k) {
        for (u64 i = h(k) % nslots;; i = (i + 1 == nslots) ? 0 : i + 1) {
            u32 s = slot[i];
            if (!s) { keys.push_back(k); slot[i] = (u32)keys.size(); return true; }
            if (keys[s - 1] == k) return false;
        }
    }
};

// ---------------------------------------------------------------- neighbours --
static void cyclicReduce(Wd& w) {
    int lo = 0, hi = w.n - 1;
    while (lo < hi && (w.c[lo] ^ 1) == w.c[hi]) { lo++; hi--; }
    int m = hi - lo + 1;
    if (m <= 0) { w.n = 0; return; }
    if (lo) memmove(w.c, w.c + lo, m);
    w.n = m;
}

static inline void pushClass(vector<u128>& out, u128 wu, int nu, u128 wv, int nv) {
    out.push_back(classKey(wu, nu, wv, nv));
}

// all neighbours of the state (u, v) inside the cap; emits class keys
static void neighbours(const Wd& u, const Wd& v, vector<u128>& out) {
    for (int which = 0; which < 2; which++) {
        const Wd& A = which ? v : u;
        const Wd& B0 = which ? u : v;
        int budget = CAP - B0.n;
        if (budget <= 0) continue;
        u128 pkA = packw(A.c, A.n);
        u128 pkB0 = packw(B0.c, B0.n);
        for (int s = 0; s < 2; s++) {
            Wd B;
            B.n = B0.n;
            if (s == 0) memcpy(B.c, B0.c, B0.n);
            else for (int t = 0; t < B0.n; t++) B.c[t] = (uint8_t)(B0.c[B0.n - 1 - t] ^ 1);
            u128 pkB = packw(B.c, B.n);
            for (int i = 0; i < A.n; i++) {
                uint8_t Afirst = A.c[i];
                uint8_t Alast = A.c[(i + A.n - 1) % A.n];
                for (int j = 0; j < B.n; j++) {
                    uint8_t Bfirst = B.c[j];
                    uint8_t Blast = B.c[(j + B.n - 1) % B.n];
                    // ---- c = 1: product of the two rotations, free+cyclic reduce
                    {
                        Wd w;
                        w.n = 0;
                        for (int t = 0; t < A.n; t++) w.c[w.n++] = A.c[(i + t) % A.n];
                        for (int t = 0; t < B.n; t++) {
                            uint8_t l = B.c[(j + t) % B.n];
                            if (w.n && (w.c[w.n - 1] ^ 1) == l) w.n--;
                            else w.c[w.n++] = l;
                        }
                        cyclicReduce(w);
                        if (w.n > 0 && w.n <= budget) {
                            pushClass(out, packw(w.c, w.n), w.n, pkB0, B0.n);
                        }
                    }
                    // ---- junction-free conjugators c, 1 <= |c| <= maxc
                    int maxc = min(CONJ, (budget - A.n - B.n) / 2);
                    if (maxc < 1) continue;
                    uint8_t c[32];
                    for (int k = 1; k <= maxc; k++) {
                        // enumerate reduced words of length k (no adjacent inverse)
                        int idx[32];
                        for (int t = 0; t < k; t++) idx[t] = 0;
                        while (true) {
                            bool ok = true;
                            for (int t = 0; t < k && ok; t++) {
                                c[t] = (uint8_t)idx[t];
                                if (t && (c[t - 1] ^ 1) == c[t]) ok = false;
                            }
                            if (ok && (c[0] == (Alast ^ 1) || c[0] == Afirst)) ok = false;
                            if (ok && (c[k - 1] == (Bfirst ^ 1) || c[k - 1] == Blast)) ok = false;
                            if (ok) {
                                Wd w;
                                w.n = 0;
                                for (int t = 0; t < A.n; t++) w.c[w.n++] = A.c[(i + t) % A.n];
                                for (int t = 0; t < k; t++) w.c[w.n++] = c[t];
                                for (int t = 0; t < B.n; t++) w.c[w.n++] = B.c[(j + t) % B.n];
                                for (int t = k - 1; t >= 0; t--) w.c[w.n++] = (uint8_t)(c[t] ^ 1);
                                pushClass(out, packw(w.c, w.n), w.n, pkB0, B0.n);
                            }
                            int t = k - 1;
                            while (t >= 0 && idx[t] == 3) idx[t--] = 0;
                            if (t < 0) break;
                            idx[t]++;
                        }
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------- goal file --
static vector<u128> GOAL;
static inline bool inGoal(u128 k) {
    return !GOAL.empty() && binary_search(GOAL.begin(), GOAL.end(), k);
}
static void loadGoal(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "cannot open goal %s\n", path); exit(2); }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    GOAL.resize(sz / sizeof(u128));
    if (fread(GOAL.data(), sizeof(u128), GOAL.size(), f) != GOAL.size()) exit(2);
    fclose(f);
    sort(GOAL.begin(), GOAL.end());
    fprintf(stderr, "goal set: %zu keys\n", GOAL.size());
}

// ---------------------------------------------------------------- main ------
int main(int argc, char** argv) {
    T0 = chrono::steady_clock::now();
    string startSpec, dumpPath, goalPath;
    vector<string> targetSpecs;
    for (int i = 1; i < argc; i++) {
        string a = argv[i];
        auto nx = [&]() { if (i + 1 >= argc) { fprintf(stderr, "missing arg\n"); exit(2); } return string(argv[++i]); };
        if (a == "--start") startSpec = nx();
        else if (a == "--cap") CAP = stoi(nx());
        else if (a == "--conj") CONJ = stoi(nx());
        else if (a == "--threads") THREADS = stoi(nx());
        else if (a == "--maxstates") MAXSTATES = stoull(nx());
        else if (a == "--nosym") NOSYM = true;
        else if (a == "--target") targetSpecs.push_back(nx());
        else if (a == "--dump") dumpPath = nx();
        else if (a == "--goal") goalPath = nx();
        else { fprintf(stderr, "unknown arg %s\n", a.c_str()); return 2; }
    }
    if (CAP > MAXW) { fprintf(stderr, "cap too large\n"); return 2; }
    if (!goalPath.empty()) loadGoal(goalPath.c_str());

    u128 k0 = keyOfSpec(startSpec);
    vector<u128> targets;
    for (auto& t : targetSpecs) targets.push_back(keyOfSpec(t));
    Table T;
    T.init(MAXSTATES);
    T.insert(k0);
    fprintf(stderr, "start %s cap=%d conj=%d nosym=%d threads=%d maxstates=%llu\n",
            keyStr(k0).c_str(), CAP, CONJ, (int)NOSYM, THREADS, (unsigned long long)MAXSTATES);
    for (size_t t = 0; t < targets.size(); t++) {
        fprintf(stderr, "target %zu: %s\n", t, keyStr(targets[t]).c_str());
        if (targets[t] == k0) fprintf(stderr, "start equals target %zu\n", t);
    }
    if (inGoal(k0)) fprintf(stderr, "start is in goal set\n");

    bool complete = false, overflow = false;
    int level = 0;
    vector<u64> bylen(MAXW + 2, 0);
    bylen[((int)(k0 >> 122)) + ((int)((k0 >> 116) & 63))]++;
    u64 lo = 0, hi = 1;
    const u64 BATCH = 1 << 15;
    vector<thread> pool;
    while (true) {
        if (lo >= hi) { complete = true; break; }
        u64 lvlNew = 0;
        for (u64 b0 = lo; b0 < hi && !overflow; b0 += BATCH) {
            u64 b1 = min(hi, b0 + BATCH);
            vector<vector<u128>> bufs(THREADS);
            atomic<u64> next(b0);
            pool.clear();
            for (int t = 0; t < THREADS; t++) {
                pool.emplace_back([&, t]() {
                    auto& buf = bufs[t];
                    Wd u, v;
                    while (true) {
                        u64 s = next.fetch_add(128);
                        if (s >= b1) break;
                        u64 e = min(b1, s + 128);
                        for (u64 x = s; x < e; x++) {
                            u128 k = T.keys[x];
                            int nu = (int)(k >> 122);
                            int nv = (int)((k >> 116) & 63);
                            u128 low = k;
                            uint8_t bc[64], ac[64];
                            for (int q = nv - 1; q >= 0; q--) { bc[q] = (uint8_t)(low & 3); low >>= 2; }
                            for (int q = nu - 1; q >= 0; q--) { ac[q] = (uint8_t)(low & 3); low >>= 2; }
                            u.n = nu; memcpy(u.c, ac, nu);
                            v.n = nv; memcpy(v.c, bc, nv);
                            neighbours(u, v, buf);
                        }
                    }
                });
            }
            for (auto& th : pool) th.join();
            for (auto& buf : bufs) {
                for (u128 k : buf) {
                    if (!T.insert(k)) continue;
                    lvlNew++;
                    int tot = (int)(k >> 122) + (int)((k >> 116) & 63);
                    bylen[tot]++;
                    for (size_t ti = 0; ti < targets.size(); ti++) {
                        if (targets[ti] == k)
                            fprintf(stderr, "HIT target %zu (%s) total %d level %d\n",
                                    ti, targetSpecs[ti].c_str(), tot, level + 1);
                    }
                    if (inGoal(k))
                        fprintf(stderr, "GOALHIT target-state %s total %d level %d\n",
                                keyStr(k).c_str(), tot, level + 1);
                    if (T.keys.size() >= MAXSTATES) { overflow = true; break; }
                }
                if (overflow) break;
            }
        }
        fprintf(stderr, "level %d: +%llu new (total %zu) %.0fs\n", level + 1,
                (unsigned long long)lvlNew, T.keys.size(), elapsed());
        lo = hi; hi = T.keys.size(); level++;
        if (overflow) break;
    }
    fprintf(stderr, "RESULT start=%s cap=%d conj=%d nosym=%d states=%zu levels=%d "
                    "complete=%d overflow=%d time=%.0fs\n",
            startSpec.c_str(), CAP, CONJ, (int)NOSYM, T.keys.size(), level,
            (int)complete, (int)overflow, elapsed());
    fprintf(stderr, "states by total length:");
    for (int L = 0; L <= CAP; L++) if (bylen[L]) fprintf(stderr, " %d:%llu", L, (unsigned long long)bylen[L]);
    fprintf(stderr, "\n");
    if (!dumpPath.empty()) {
        vector<u128> ks = T.keys;
        sort(ks.begin(), ks.end());
        FILE* f = fopen(dumpPath.c_str(), "wb");
        fwrite(ks.data(), sizeof(u128), ks.size(), f);
        fclose(f);
        fprintf(stderr, "dumped %zu keys to %s\n", ks.size(), dumpPath.c_str());
    }
    return 0;
}
