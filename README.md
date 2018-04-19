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
Following the command below~
``` shell
adb push InjectModule/libs/armeabi-v7a/libinjectModule.so /data/tmp/jni_hook
adb push InjectProgram/libs/armeabi-v7a/inject /data/tmp/jni_hook
adb shell chmod 777 /data/tmp/jni_hook/inject
adb shell /data/tmp/jni_hook/inject /data/tmp/jni_hook/libinjectModule.so HookEntry tk.gifish.testhookwifiinfo
```

Then press then button, you can see the screen printing `ahahahahah have been hooked!`.
At the same time, you can see some log from android device monitor.