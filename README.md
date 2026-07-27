## ZemonScript
一个简单、轻量、快速的解释器，用于 `Zemon` `(*.zns)` 语言。设计用于快速脚本编写和教育目的，具有直观简洁的语法。

[![GitHub release](https://img.shields.io/github/release/3-1415f/ZemonScript.svg)](https://github.com/3-1415f/ZemonScript/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

### 特性
- **轻量快速**: 基于栈式虚拟机的高效解释器
- **直观语法**: 类C风格语法，易于学习
- **REPL支持**: 交互式编程环境
- **内置函数**: 丰富的内置函数库

### 快速开始
```sh
zn -h       # 获取帮助

zn          # 启动交互式REPL

zn file.zns # 运行脚本文件
```

> ZemonScript 在线体验
> - https://3-1415f.github.io/zemon-web
> - https://zemon.znstd.dpdns.org
> - https://zemon.netlify.app

### 示例

```javascript
//hello world
outln("hello, world!");

// 数据类型与变量
eaten = true;
x = 42;
age = x > 0 ? x : null;
pi = 3.14159;
name = "Hello";
arr = [3.14159, pi + pi, pi * 3, pi * (2 + 2),
  "fifth", [[], "sixth-second"]]; // 列表
first = arr[0];

// 函数
x = float(input("enter x: "));
outln("sin(x) = ", sin(x));

// 条件语句
if x > 10 {
    outln("x is greater than 10");
} elif x < 5 {
    outln("x is less than 5");
} else {
    outln("x is between 5 and 10");
}

// 循环
i = 0;
while i < 10 {
    outln(i);
    i = i + 1;
}
```
> [查看更多示例](examples)

<details>
<summary><b>数据类型</b></summary>

<table>
  <tr><td><code>null</code></td><td>空值</td></tr>
  <tr><td><code>int</code></td><td>64位整数</td></tr>
  <tr><td><code>float</code></td><td>64位浮点数</td></tr>
  <tr><td><code>str</code></td><td>字符串</td></tr>
  <tr><td><code>list</code></td><td>列表</td></tr>
  <tr><td><code>function</code></td><td>函数</td></tr>
</table>

> 使用 `type(val)` 函数获取变量类型

</details>

<details>
<summary><b>内置函数</b></summary>

| 函数 | 说明 |
|--|--|
| `bool(val)` | 转换为布尔值 |
| `int(val)` | 转换为整数 |
| `str(val)` | 转换为字符串 |
| `type(val)` | 获取变量类型 |
| `repl(val)` | 格式化为字符串 |
| `out(...)` | 输出内容 |
| `outln(...)` | 输出内容并换行 |
| `input(prompt)` | 读取用户输入 |
| `exit()` | 退出程序 |
| `clock()` | 获取当前时间 |
| `sleep(ms)` | 延迟指定毫秒数 |
| `sin(x)` | 正弦函数 |
| `cos(x)` | 余弦函数 |
| `tan(x)` | 正切函数 |
| `asin(x)` | 反正弦函数 |
| `acos(x)` | 反余弦函数 |
| `atan(x)` | 反正切函数 |
</details>

<details>
<summary><b>常量</b></summary>

| 常量 | 值 |
|--|--|
| `pi` | 3.1415926 |
| `e` | 2.7182818 |
</details>

<details>
<summary><b>运算符</b></summary>

| 运算符 | 说明 |
|--|--|
| `+` | 加法，字符串拼接，列表拼接 |
| `-` | 减法 |
| `*` | 乘法，字符串重复 |
| `/` | 除法 |
| `%` | 取模 |
| `**` | 幂运算 |
|  |  |
| `<` | 小于 |
| `>` | 大于 |
| `<=` | 小于等于 |
| `>=` | 大于等于 |
| `==` | 等于 |
| `!=` | 不等于 |
|  |  |
| `&&` | 逻辑与 |
| `\|\|` | 逻辑或 |
| `!` | 逻辑非 |
| `&` | 按位与，列表追加 |
| `\|` | 按位或 |
| `^` | 按位异或 |
| `~` | 按位取反 |
| `<<` | 左移 |
| `>>` | 右移 |
</details>

### 许可证
本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情
