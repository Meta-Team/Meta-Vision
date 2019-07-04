# SVM 训练图片生成程序使用方法

`Armor_Sample -E 敌方颜色 -O 输出目录 -P 输出图片前缀 输入视频`

相对路径以 Armor_Sample 可执行文件所在位置为准

例如：

```bash
cd build
./Armor_Sample -E blue -O ../training/small_3_1 -P small_3_1 ../test_clips/armors/small_3_1.MOV
./Armor_Sample -E blue -O ../training/small_3_1 -P big_3_1 ../test_clips/armors/big_3_1.MOV --big
```

完整用法使用 `Armor_Sample --help` 查看。

建议在文件前缀中包含大小装甲标示和装甲数字，便于数据集维护。

## 快捷脚本
```bash
./generate.sh small_3_1.MOV blue
./generate.sh big_3_1.MOV blue --big
```

包含创建目录