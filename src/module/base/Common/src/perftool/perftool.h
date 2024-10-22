#ifndef PERF_TOOL_H
#define PERF_TOOL_H
#include <string>
#include "Base.h"

class PerfTool
{
public:

    // 开始性能检测
    static std::string ABSTRACT_EXPORT startCPUProfiler();

    // 停止性能检测
    static std::string ABSTRACT_EXPORT stopCPUProfiler();

    // 开始分析堆栈
    static std::string ABSTRACT_EXPORT startHeapProfiler();

    // 停止堆栈分析
    static std::string ABSTRACT_EXPORT stopHeapProfiler();

private:
    static std::string  mClassName;
    static bool         mCpuProfiler;
    static bool         mHeapProfiler;
    static std::string  mCpuProfilerFile;
    static std::string  mHeapProfilerFile;
};


#endif
