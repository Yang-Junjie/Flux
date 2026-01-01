# 使用 Flux 创建安卓应用

使用 Android Studio 创建一个 Native C++ 项目  
将 Flux 放在 Android Studio 生成的 app/src/main/cpp/ 下
将 example/AndroidApplication 中的模板样例文件拷贝到 Android Studio 生成的 app/src/main/cpp/ 下  

修改 AndroidManifest.xml
```xml
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    xmlns:tools="http://schemas.android.com/tools">

    <uses-feature android:glEsVersion="0x00030000" android:required="true"/>

    <application
        android:allowBackup="true"
        android:icon="@mipmap/ic_launcher"
        android:label="@string/app_name"
        android:roundIcon="@mipmap/ic_launcher_round"
        android:supportsRtl="true"
        android:theme="@style/Theme.example">

        <!-- 使用 NativeActivity -->
        <activity
            android:name="android.app.NativeActivity"
            android:label="@string/app_name"
            android:exported="true"
            android:configChanges="keyboardHidden|orientation|screenSize"
            android:theme="@android:style/Theme.NoTitleBar.Fullscreen">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER"/>
            </intent-filter>

            <!-- 告诉 NativeActivity 需要加载的 so -->
            <meta-data android:name="android.app.lib_name" android:value="example"/>
        </activity>
    </application>

</manifest>

```