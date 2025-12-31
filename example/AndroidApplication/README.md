# 使用 Flux 创建安卓应用

使用 Android Studio 创建一个 Native C++ 项目  
将 Flux 放在 Android Studio 生成的 app/src/main/cpp/ 下
将 example/AndroidApplication 中的模板样例文件拷贝到 Android Studio 生成的 app/src/main/cpp/ 下  

修改 AndroidManifest.xml
```xml

<manifest xmlns:android="http://schemas.android.com/apk/res/android"
          xmlns:tools="http://schemas.android.com/tools">

    <!-- 添加这一行在 AndroidManifest.xml -->
    <uses-feature android:glEsVersion="0x00030000" android:required="true"/>

    <application
            android:allowBackup="true"
            ....
</application>
...</manifest>
```