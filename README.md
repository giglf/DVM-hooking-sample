# DVM hooking sample
This project is modify from this article. https://bbs.pediy.com/thread-192803.htm
Targeting to hook a java method from jni interface.
Mainly work as a idea verification for my graduate project.

* Testing pass for XiaoMi3 td, android 4.4.4. Dalvik virtual machine.
* Haven't tested for ART.


# How to try it?
Under this project. There is a android studio project name TestHookWifiInfo.
This app just has one activity, one button and one textview. When you click the button, the screen will show the mac address of your phone.
This operation is supported by getMacAddress().
``` java
public void onClick(View view) {

    TextView textView = findViewById(R.id.textView);

    WifiManager manager = (WifiManager)getApplicationContext().getSystemService(Context.WIFI_SERVICE);

    WifiInfo info = manager.getConnectionInfo();

    textView.setText(info.getMacAddress());

}
```
You can just get the release apk from the project directory.

Then, my goal is hooking getMacAddress, let it shows the custom string instead of mac address.
Using adb to push `InjectModule/libs/armeabi-v7a/libinjectModule.so` and `InjectProgram/libs/armeabi-v7a/inject` into `/data/tmp/jni_hook`.
**Notification!! You have to push to the directory above. Because the .so file location have been written in source file. Or you can change it then using ndk-build to compile it manually.**

After preparation, you should let TestHookWifiInfo launch and then run `/data/tmp/jni_hook/inject` in adb shell.
Then press then button, you can see the screen printing `ahahahahah have been hooked!`.
At the same time, you can see some log from android device monitor.