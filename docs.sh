#!/bin/bash

# Skynet 文档快速启动脚本

echo "=== Skynet 源码解析文档 ==="
echo ""

# 检查依赖
if [ ! -d "node_modules" ]; then
    echo "正在安装依赖..."
    npm install
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
        npm run docs:dev
        ;;
    2)
        echo "构建生产版本..."
        npm run docs:build
        ;;
    3)
        echo "预览生产版本..."
        npm run docs:preview
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
