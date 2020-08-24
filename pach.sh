
#!/bin/sh  
exe="adc_radar_box_GUI" #你需要发布的程序名称
des="/home/nvidia/Desktop/radar_bag" #创建文件夹的位置
deplist=$(ldd $exe | awk '{if (match($3,"/")){ printf("%s "),$3 } }')  
cp $deplist $des

