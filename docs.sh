#!/bin/bash

# Skynet 文档快速启动脚本

echo "=== Skynet 源码解析文档 ==="
echo ""

# 检查依赖
if ! command -v pnpm >/dev/null 2>&1; then
    echo "错误: 未找到 pnpm。请按 https://pnpm.io/zh/installation 安装；Node.js 22.12–24 可用 corepack enable pnpm，Node.js 25+ 不再内置 Corepack。"
    exit 1
fi
if [ ! -d "node_modules" ]; then
    echo "正在安装依赖..."
    pnpm install --frozen-lockfile
fi

# 显示菜单
echo "请选择操作:"
echo "1) 启动开发服务器"
echo "2) 构建生产版本"
echo "3) 预览生产版本"
echo "4) 退出"
echo ""
read -p "请输入选项 (1-4): " choice

case $choice in
    1)
        echo "启动开发服务器..."
        pnpm run docs:dev
        ;;
    2)
        echo "构建生产版本..."
        pnpm run docs:build
        ;;
    3)
        echo "预览生产版本..."
        pnpm run docs:preview
        ;;
    4)
        echo "退出"
        exit 0
        ;;
    *)
        echo "无效选项"
        exit 1
        ;;
esac
