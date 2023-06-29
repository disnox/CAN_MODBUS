# STM32CubeMX工程的建立

**作者**：

**文档版本**：版本1.0 (2023-5-28)

## 1. STM32CubeMX主界面

![image-20230601095531754](D:\Git\note\GPIO\PNG\image-20230601095531754.png)

## 2. CubeMX主界面功能

### 2.1 版本更新以及芯片包的下载

![image-20230601095936517](D:\Git\note\GPIO\PNG\image-20230601095936517.png)

<div style="page-break-after:always"></div>

#### 2.1.1 CubeMX版本的更新选择

![image-20230601100046079](D:\Git\note\GPIO\PNG\image-20230601100046079.png)

**Tips：记得要更新之前以管理员身份运行STM32CubeMX软件，低版本是打不开高版本配置的工程的。**

#### 2.1.2 芯片包的选择与下载

![image-20230601100638053](D:\Git\note\GPIO\PNG\image-20230601100638053.png)

**Tips：按需求下载自己所需要的芯片包**

### 2.2 新工程的建立

![image-20230601095722332](D:\Git\note\GPIO\PNG\image-20230601095722332.png)

### 2.3 新工程芯片的查找

![image-20230601101633266](D:\Git\note\GPIO\PNG\image-20230601101633266.png)

### 2.4 新工程的芯片选择

![image-20230601103452203](D:\Git\note\GPIO\PNG\image-20230601103452203.png)

## 3. 工程配置

### 3.1 主工程界面

![image-20230601110656327](D:\Git\note\GPIO\PNG\image-20230601110656327.png)

### 3.2 核心功能配置

**Tips：每次配置新工程时候都需要先进行这些配置。**

#### 3.2.1 系统下载口配置

![image-20230601111625174](D:\Git\note\GPIO\PNG\image-20230601111625174.png)

### 3.2.2 系统时钟配置

+ 时钟源与时钟引脚配置

![image-20230601112552079](D:\Git\note\GPIO\PNG\image-20230601112552079.png)

<div style="page-break-after:always"></div>

+ 时钟源频率配置以及使用外部或者内部时钟

![image-20230601113529720](D:\Git\note\GPIO\PNG\image-20230601113529720.png)

## 4. 工程生成

### 4.1 工程管理界面

![image-20230601114824327](D:\Git\note\GPIO\PNG\image-20230601114824327.png)

### 4.2 工程文件的导入选项

![image-20230601115144512](D:\Git\note\GPIO\PNG\image-20230601115144512.png)

### 4.3 生成工程

![image-20230601115509491](D:\Git\note\GPIO\PNG\image-20230601115509491.png)

## 5. 工程目录

![image-20230601130911473](D:\Git\note\GPIO\PNG\image-20230601130911473.png)

## 6. keil界面 

![image-20230601131147309](D:\Git\note\GPIO\PNG\image-20230601131147309.png)

**Tips：一定要将你的代码写在 USER CODE BEGIN 和 USER CODE END之间，否则CubeMX重新配置会覆盖你的代码。**































