# 功能
- 双击关闭标签页
- 右键关闭标签页（按住 Shift 弹出原有菜单）
- 保留最后标签页（防止关闭最后一个标签页时关闭浏览器，点X不行）
- 鼠标悬停标签栏滚动
- 按住右键时滚轮滚动标签栏
- 新建前台标签页打开地址栏输入的内容
- 新建前台标签页打开书签
- 当前为新标签页时，可以禁用上面两个功能
- 使用快捷键快速隐藏浏览器窗口（老板键）
- 便携设计，程序放在App目录，数据放在Data目录（不兼容原版数据，可以重装系统换电脑不丢数据）
- 可以使用 ini 文件配置功能
- 可以自定义启动参数
# 自定义
本dll提供有限的自定义选项（dll版本需要为1.5.7.0 +），新建一个名为config.ini的文件到dll同目录即可。可以使用%app%来表示dll所在目录
```
[dir_setting]
data=%app%\..\Data
cache=%app%\..\Cache

; double_click_close_tab       -- Double left click to close the TAB
; right_click_close_tab        -- Right click to close the TAB (pop up the original menu when the Shift key is pressed)
; keep_last_tab                -- Keep the last TAB from closing the window (except for manually clicking the close button)
; wheel_tab                    -- Use the mouse wheel to switch TAB when the mouse is on the TAB bar
; wheel_tab_when_press_rbutton -- Use the mouse wheel to switch TAB when the right mouse button is pressed anywhere (if you use mouse gesture software, it will not take effect)
; open_url_new_tab             -- New foreground tab opens the contents entered in address bar
; open_bookmark_new_tab        -- New foreground tab opens the bookmark
; new_tab_disable              -- If the current TAB is a new TAB, disable the above two options

; double_click_close_tab       -- 双击左键关闭标签页
; right_click_close_tab        -- 右键关闭标签页（按 Shift 键时弹出原有菜单）
; keep_last_tab                -- 保留最后一个标签页不关闭窗口（手动点击关闭按钮除外）
; wheel_tab                    -- 鼠标在标签栏时使用鼠标滚轮切换标签页
; wheel_tab_when_press_rbutton -- 在任何位置按住右键时使用鼠标滚轮切换标签页（若使用了鼠标手势软件则不生效）
; open_url_new_tab             -- 新建前台标签页打开地址栏输入的内容
; open_bookmark_new_tab        -- 新建前台标签页打开书签
; new_tab_disable              -- 如果当前标签页是新标签页，则禁用上面两个选项
[features]
double_click_close_tab=1
right_click_close_tab=0
keep_last_tab=1
wheel_tab=1
wheel_tab_when_press_rbutton=1
open_url_new_tab=0
open_bookmark_new_tab=0
new_tab_disable=1
[General]
; Appends Chromium startup parameters
; You can imitate the form below to write, pay attention to the space, do not wrap
; CommandLine=--no-first-run --disable-logging --no-report-upload

; 追加 Chromium 启动参数
; 可以模仿下面的形式来写，注意空格，不要换行
; CommandLine=--no-first-run --disable-logging --no-report-upload
CommandLine=


; Customize a shortcut key that can hide all Chrome windows and restore them when pressed again
; Available keys are: Ctrl, Alt, Shift, Win, F1-F12, numeric keys, letter keys, symbol keys
; For example: Ctrl+Alt+B

; 自定义一个快捷键，按下后可以隐藏所有 Chrome 窗口，再次按下恢复
; 可以使用的键位有：Ctrl、Alt、Shift、Win、F1-F12、数字键、字母键、符号键
; 例如：Ctrl+Alt+B
Bosskey=

```
# 获取
采用GitHub Actions自动编译发布，下载地址：[Powered by nightly.link](https://nightly.link/avatartw/vivaldi_plus/workflows/build/main)

[![build status](https://github.com/czyt/vivaldi_plus/actions/workflows/build.yml/badge.svg)](https://github.com/czyt/vivaldi_plus/actions/workflows/build.yml)
# 安装
dll放入解压版Vivaldi目录(vivaldi.exe同目录)即可
## 源项目
[chromePlus](https://github.com/icy37785/chrome_plus)

> 修复 Chrome 118+ 的代码参考了 [Bush2021的项目](https://github.com/Bush2021/chrome_plus)，在此表示感谢。我不会cpp，所以代码写的很烂。版本号更新频繁，看不惯的可以不看。

> 右键关闭标签页、老板键、自定义启动参数、新建前台标签页打开地址栏输入的内容、新建前台标签页打开书签 的代码源自 [Bush2021的项目](https://github.com/Bush2021/chrome_plus)，在此表示感谢。
