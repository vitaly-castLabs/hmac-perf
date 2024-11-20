# hmac-perf
HMAC-SHA256 performance tests: one 30k block of data vs thirty 1k blocks.

## Build and run on macOS/Linux
```bash
cmake -S . -B _build && \
cmake --build _build && \
./_build/hmac-perf
```

macOS 15.1, OpenSSL 3.4.0, MacBook Air M1:
```
=== Naive HMAC =============================================
Result: 1a275a1ffb0ffbe37ed949dbbf806b61777de4be7c5cf470e7c1e468e130d873
Done in 1311 ms
============================================================

=== Reuse HMAC ctx =========================================
Result: 1a275a1ffb0ffbe37ed949dbbf806b61777de4be7c5cf470e7c1e468e130d873
Done in 1232 ms
============================================================

=== Reuse chunked ==========================================
Result: b33ec3f1ef463463a40375aaf2fed46ee6ad0a9fa10b20c8766f63ea7ecb3304
Done in 1643 ms
============================================================
```

## Build and run on Windows
Git Bash or such:
```bash
cmake -S . -B _build && \
cmake --build _build --config Release && \
./_build/Release/hmac-perf.exe
```

Win 11 24H2 Build 26100.2314, OpenSSL 3.3.2, Intel Core Ultra 7 258V:
```
=== Naive HMAC =============================================
Result: 1a275a1ffb0ffbe37ed949dbbf806b61777de4be7c5cf470e7c1e468e130d873
Done in 772 ms
============================================================

=== Reuse HMAC ctx =========================================
Result: 1a275a1ffb0ffbe37ed949dbbf806b61777de4be7c5cf470e7c1e468e130d873
Done in 692 ms
============================================================

=== Reuse chunked ==========================================
Result: b33ec3f1ef463463a40375aaf2fed46ee6ad0a9fa10b20c8766f63ea7ecb3304
Done in 1128 ms
============================================================
```
