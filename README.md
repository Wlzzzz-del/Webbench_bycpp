# Webbench
Web Bench is very simple tool for benchmarking WWW or proxy servers. Uses fork() for simulating multiple clients and can use HTTP/0.9-HTTP/1.1 requests. This benchmark is not very realistic, but it can test if your HTTPD can realy handle that many clients at once (try to run some CGIs) without taking your machine down. Displays pages/min and bytes/sec. Can be used in more aggressive mode with -f switch.

# Webbench_bycpp
Using C++ implement webbench, based on multi-thread、lock、Socket.

# how to compile
```bash
g++ webbench.cpp -o webbench -std=c++11 -pthread
```

# how to run
```bash
./webbench
```

# contact me
vx: xxscoder
email: wu_lizhao@yeah.net
