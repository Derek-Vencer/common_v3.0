#!/bin/bash

# 设置目标目录
TARGET_DIR="lib/bes"

# 检查目录是否存在
if [ ! -d "$TARGET_DIR" ]; then
    echo "错误：目录 $TARGET_DIR 不存在！"
    exit 1
fi

# 显示将要保留的文件列表
echo "以下文件将被保留："
find "$TARGET_DIR" -type f -name "*mulitimedia*" -o -name "*multimedia*"

# 显示将要删除的文件列表
echo -e "\n以下文件将被删除："
find "$TARGET_DIR" -type f ! -name "*mulitimedia*" ! -name "*multimedia*"

# 确认删除
read -p "\n确认要删除这些文件吗？(y/n): " CONFIRM
if [ "$CONFIRM" != "y" ] && [ "$CONFIRM" != "Y" ]; then
    echo "操作已取消。"
    exit 0
fi

# 执行删除操作（删除除包含mulitimedia或multimedia的文件之外的所有文件）
find "$TARGET_DIR" -type f ! -name "*mulitimedia*" ! -name "*multimedia*" -exec rm -f {} \;

# 显示删除完成信息
echo "\n已完成删除操作。"

# 再次查找以确认结果
echo -e "\n当前保留的文件："
find "$TARGET_DIR" -type f -name "*mulitimedia*" -o -name "*multimedia*"

# 统计保留的文件数量
REMAINING_FILES=$(find "$TARGET_DIR" -type f -name "*mulitimedia*" -o -name "*multimedia*" | wc -l)
echo "\n共保留了 $REMAINING_FILES 个文件。"