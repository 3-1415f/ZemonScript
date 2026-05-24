# ZemonScript
一个简单、轻量、快速的解释器，用于 Zemon (Zn) (*.zns) 语言。设计用于快速脚本编写和教育目的，具有直观简洁的语法。

[![GitHub release](https://img.shields.io/github/release/3-1415f/ZemonScript.svg)](https://github.com/3-1415f/ZemonScript/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## 特性
- **轻量快速**: 基于栈式虚拟机的高效解释器
- **直观语法**: 类C风格语法，易于学习
- **REPL支持**: 交互式编程环境
- **内置函数**: 丰富的内置函数库

## 快速开始
```bash
# 启动交互式REPL
zn -r

# 运行脚本文件
zn script.zns

# 带颜色输出的REPL
zn -r -c
```

| 参数 | 说明 |
|------|------|
| `-h` | 显示帮助信息 |
| `-r` | 启动REPL模式 |
| `-c` | 启用颜色输出 |
| `-f file.zns` | 运行指定脚本文件 |

## 语法内容

```javascript
// 变量声明与赋值
x = 42
name = "Hello"
pi = 3.14159

// 数学运算
result = (1 + 2) * 3 / 4

// 条件语句
if x > 10 {
    outln("x is greater than 10")
} elif x < 5 {
    outln("x is less than 5")
} else {
    outln("x is between 5 and 10")
}

// 循环
i = 0
while i < 10 {
    out(i)
    i = i + 1
}

// 列表
arr = [1, 2, 3, 4, 5]
outln(arr[0])

// 内置函数
outln("Hello, World!")
user_input = input("Enter something: ")
outln("You entered:", user_input)
```

### 内置函数
| 函数 | 说明 |
|------|------|
| `out(...)` | 输出内容 |
| `outln(...)` | 输出内容并换行 |
| `input(prompt)` | 读取用户输入 |
| `exit()` | 退出程序 |
| `clock()` | 获取当前时间 |
| `sleep(ms)` | 延迟指定毫秒数 |
| `bool(x)` | 转换为布尔值 |
| `int(x)` | 转换为整数 |
| `str(x)` | 转换为字符串 |
| `sin(x)` | 正弦函数 |
| `cos(x)` | 余弦函数 |
| `tan(x)` | 正切函数 |
| `asin(x)` | 反正弦函数 |
| `acos(x)` | 反余弦函数 |
| `atan(x)` | 反正切函数 |

### 常量
| 常量 | 值 |
|------|-----|
| `pi` | 3.1415926 |
| `e` | 2.7182818 |

### 算术运算符
| 运算符 | 说明 |
|--------|------|
| `+` | 加法 |
| `-` | 减法 |
| `*` | 乘法 |
| `/` | 除法 |
| `%` | 取模 |
| `**` | 幂运算 |

### 比较运算符
| 运算符 | 说明 |
|--------|------|
| `<` | 小于 |
| `>` | 大于 |
| `<=` | 小于等于 |
| `>=` | 大于等于 |
| `==` | 等于 |
| `!=` | 不等于 |

### 逻辑运算符
| 运算符 | 说明 |
|--------|------|
| `&&` | 逻辑与 |
| `\|\|` | 逻辑或 |
| `!` | 逻辑非 |

### 位运算符
| 运算符 | 说明 |
|--------|------|
| `&` | 按位与 |
| `\|` | 按位或 |
| `^` | 按位异或 |
| `~` | 按位取反 |
| `<<` | 左移 |
| `>>` | 右移 |

### 数据类型
- **I64**: 64位整数
- **F64**: 64位浮点数
- **Str**: 字符串
- **List**: 列表
- **Null**: 空值

## 许可证
本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情
