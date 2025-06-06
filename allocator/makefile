.PHONY: all clean

# 编译器和编译选项
CXX = g++
CXXFLAGS = -ggdb -Wall -Wextra -pedantic -MMD -MP # -MMD 和 -MP 实现自动化的依赖管理

# 源文件
SRCS = alloc.cpp
# 目标文件
OBJS = $(SRCS:.cpp=.o) # 将 SRCS 中的所有 .cpp 文件替换为 .o 文件
# 依赖文件 (自动生成)
DEPS = $(SRCS:.cpp=.d)

# 最终可执行文件
TARGET = alloc

# 默认目标：编译所有
all: $(TARGET)

# 链接规则：从 .o 文件生成可执行文件
# $@ 代表规则的目标的完整名称
# 这里 $@ 替换为 TARGET
# %^  代表规则的所有先决条件（依赖项）的列表，并且不包含重复项。各个依赖项之间用空格分隔
# 这里 $^ 替换为 OBJS
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) 


# 编译规则：从 .cpp 文件生成 .o 文件
# -MMD 和 -MP 标志已包含在 CXXFLAGS 中，用于自动生成依赖文件
# $< 代表规则的第一个先决条件（依赖项）的名称
# 这里 $< 替换为 %.cpp
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# 包含自动生成的依赖文件
# 如果 .d 文件不存在，不会报错
-include $(DEPS)

# 清理规则：删除所有生成的文件
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET) *.stackdump