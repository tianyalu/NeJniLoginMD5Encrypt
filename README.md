# NeJniLoginMD5Encrypt JNI语法规范以及登录在JNI层用MD5加密
## 一、JNI语法基础规范
可以参考：[JNI基础](https://www.jianshu.com/p/e3bcff7e3b24)
**JNI数据类型**  
* JNI系统类型：JNIEnv(线程上下文)  
* 基本数据类型  
* 引用类型   
* 数组  
### 1.1 数据类型映射关系
基本数据类型： 

java类型 | Jni.h    | 在C中的定义
------  | -------- | ----------
boolean | jboolean | typedef unsigned char 
byte    | jbyte    | typedef signed char
char    | jchar    | typedef unsigned short
short   | jshort   | typedef short
int     | jint     | typedef int
long    | jlong    | typedef long long
float   | jfloat   | typedef float
double  | jdouble  | typedef double
String  | jstring  | typedef jobject
Object  | jobject  | typedef void*
Class   | jclass   | --

数组：  

Java类型  | Jni.h
--------- | -----
boolean[] | jbooleanArray
byte[]    | jbyteArray
char[]    | jcharArray
short[]   | jshortArray
int[]     | jintArray
long[]    | jlongArray
float[]   | jfloatArray
double[]  | jdoubleArray
String[]  | jstringArray
Object[]  | jobjectArray
Class[]   | jclassArray

### 1.2 JNI中使用jstring
`jstring->char[]`:  
```c++
const char* charStr = (*env)->GetStringUTFChars(env, str, NULL);
printf("%s", charStr);
```  
`char[]->jstring`:  
```c++
const char* jniChars = "I am jni chars";
jstring str = (*env)->NewStringUTF(env, jniChars);
return str;
```

`Unicode`编码：  
```c++
const char* unicodeCharStr = (*env)->GetStringChars(env, str, NULL):
```  

### 1.3 JNI中使用jobject
* 对象的创建  
* 字段的读写  
    - 静态字段  
    - 实例字段  
* 方法的调用  
    - 静态方法  
    - 实例方法  

#### 1.3.1 JNI内部描述
Java方法：  
```java
public native void printStr(String str);
```
jni方法：  
```c++
/*
 * Class: com_sty_ne_jnilogin_md5_encrypt_MainActivity
 * Method: printStr
 * Signature: (Ljava/lang/Strig;)V  <-- 方法名+返回值
 */
JNIEXPORT jstring JNICALL
Java_com_sty_ne_jnilogin_md5_encrypt_MainActivity_printStr(
        JNIEnv *env,
        jobject,
        jstring);
```  
JNI内部描述与Java对照表：  

java类型 | JNI描述    
------  | -------- 
boolean | Z
byte    | B
char    | C
short   | S
int     | I
long    | L
float   | F
double  | D
Object  | L全路径名称;
type[]  | [Type
method  | (参数类型)返回值  

JNI描述实例：  
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/jni_description_instance.png)  

#### 1.3.2 在JNI中对字段进行读写的步骤
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/invoke_field_process.png)  
例子：    
```c++
//将message.what的值修改为-1
jclass cls = (*env)->GetObjectClass(env, msg);
jfieldID whatField = (*env)->GetFieldID(env, cls, "what", "I");
(*env)->SetIntField(env, msg, whatField, -1);

//得到message.what的值
int iWhat = (*env)->GetIntField(env, msg, whatField);
```

#### 1.3.3 在JNI中对调用方法的步骤
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/invoke_method_process.png)  
例子：  
```c++
//invoke Message.obtain()
jclass cls = (*env)->FindClass(env, "android/os/Message");
jmethodID obtainMethod = (*env)->GetStaticMethodID(env, cls, "obtain", "()Landroid/os/Message;");
return (*env)->CallStaticObjectMethod(env, cls, obtainMethod);
```

#### 1.3.4 在JNI中创建对象的步骤  
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/create_object_process.png)  
例子：  
```c++
//new Message()
jclass cls = (*env)->FindClass(env, "android/os/Message");
jmethodID constructMethod = (*env)->GetMethodID(env, cls, "<init>", "()V");
return (*env)->NewObject(env, cls, constructMethod);
```

#### 1.3.5 在JNI中对数组操作的步骤 
```java
Message[] messages;
for(int index = 0; i < messages.length; index++) {
    messages[i].what = -2;
}
```
```c++
jsize length = (*env)->GetArrayLength(env, objArray);
int index = 0;
for(; index < length; index++) {
    jobject element = (*env)->GetObjectArrayElement(env, objArray, index);
    (*env)->SetIntField(env, element, whatField, -2);
}
```

### 1.4 `JNI Reference`
Jni.h         | 在C中的定义
------------- | ----------
jobject       | typedef void*
jclass        | typedef jobject
jstring       | typedef jobject
jarray        | typedef jobject
jobjectArray  | typedef jarray
jbooleanArray | typedef jarray

各种引用的区别：  

JNI Reference           | 特点
----------------------- | -----------------------
局部引用(LocalRef)       | 本地方法栈内有效
全局引用(GlobalRef)      | 虚拟机全局生效，不会被GC回收
弱全局引用(WeakGlobalRef) | 虚拟机全局生效，GC时被回收

**注意：**  
如果没有对jobject对象做全局引用或弱全局引用的转换，则默认为局部引用！！  

#### 1.4.1 局部引用
如果循环次数过多，可能报“局部引用表溢出”的异常，从而导致崩溃。需要及时释放不用的局部引用。  
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/local_reference_exception.png)  
删除一个局部变量的引用方法如下：  
```c++
(*env)->DeleteLocalRef(env, element);
```
删除多个局部变量的引用方法如下： 
```c++
(*env)->PushLocalRef(env, 3); # 预测的帧数（可以不等于实际帧数）
//中间要删除的局部引用帧
(*env)->PopLocalRef(env, 3);
```
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/clear_multi_reference.png)  
可以通过如下方法来判断是否还有足够可用的引用帧：  
```c++
if((*env)->EnsureLocalCapacity(env, 3) < 0) {
    //TODO 异常处理
}
```
#### 1.4.2 全局引用
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/global_reference.png)  
声明为全局引用需要调用如下方法：  
```c++
jclass globalCls;
jclass cls = (*env)->GetObjectClass(env, msg);
globalCls = (*env)->NewGlobalRef(env, cls);
```
删除全局引用需要调用如下方法：  
```c++
(*env)->DeleteGlobalRef(env, globalCls);
```

**例外：**  
jmethodID 和 jfieldID 不是 jobject，采用如下方式声明为全局引用：  
```c++
jfieldID whatField;
jmethodID obtainMethod;

if(whatField == NULL) {
    whatField = (*env)->GetFieldID(env, cls, "what", "I");
}
if(obtainMethod == NULL) {
    obtainMethod = (*env)->GetStaticMethodID(env, cls, "obtain", "()Landroid/os/Message;");
}
```

#### 1.4.2 弱全局引用
创建和删除方式如下：  
```c++
//创建
jclass globalWeakCls = (*env)->NewWeakGlobalRef(env, cls);
//删除
(*env)->DeleteWeakGlobalRef(env, globalWeakCls);
```

### 1.4.3 懒加载与预加载(JNI_ONLoad, JNI_ONUnLoad)
懒加载：  
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/pre_lazy_init.png)  
预加载与卸载（so加载卸载的时候调用）：  
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/pre_init.png)  

### 1.4.4 关联JNI方法与native方法
native方法命名必须遵循一定的规则才能被解析，`registerNative`则可以将两者关联起来而不必遵循命名规则。  
该方法除了在`JNI_ONLoad`方法中调用外，还可以在任意地方调用，动态关联。  
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/register_native.png)  



## 二、实操
**注意：**  
上面讲的规范内容比较陈旧，仅可做思路上的参考，实操部分会有一些写法上的改变。  
### 2.1 `native-lib.cpp`文件
```c++
#include <jni.h>
#include <string>

/**
 * Class:  com_sty_ne_jnilogin_md5_encrypt_UserInfo
 * Method: nativeEncrypt
 * Signature: ()V
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_sty_ne_jnilogin_md5_encrypt_UserInfo_nativeEncrypt(JNIEnv *env, jobject instance) {
    jclass cls = env->GetObjectClass(instance);
    jfieldID pwdField = env->GetFieldID(cls, "userPwd", "Ljava/lang/String;");
    jstring strPwd = static_cast<jstring>(env->GetObjectField(instance, pwdField));

    jclass clsMd5 = env->FindClass("com/sty/ne/jnilogin/md5/encrypt/MD5Helper");
    jmethodID md5Method = env->GetStaticMethodID(clsMd5, "getMD5", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring strNewPwd = static_cast<jstring>(env->CallStaticObjectMethod(clsMd5, md5Method,
                                                                         strPwd));
    env->SetObjectField(instance, pwdField, strNewPwd);
}
```

### 2.2 `LoginActivity.java`文件
```java
public class LoginActivity extends AppCompatActivity {
    private EditText edtUser;
    private EditText edtPwd;
    private Button btnLogin;

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        initView();
    }

    private void initView() {
        edtUser = findViewById(R.id.edt_user);
        edtPwd = findViewById(R.id.edt_pwd);
        btnLogin = findViewById(R.id.btn_login);

        btnLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                login();
            }
        });
    }

    private void login() {
        String strUser = edtUser.getText().toString().trim();
        String strPwd = edtPwd.getText().toString();
        if(TextUtils.isEmpty(strUser) || TextUtils.isEmpty(strPwd)) {
            Toast.makeText(this, "用户名或密码为空", Toast.LENGTH_SHORT).show();
        } else {
            UserInfo userInfo = new UserInfo(strUser, strPwd);
            userInfo.nativeEncrypt();
            Toast.makeText(this, "密码: " + userInfo.getUserPwd(), Toast.LENGTH_SHORT).show();

            doMockLogin(strUser, userInfo.getUserPwd());
            saveUserAndPwd(this, strUser, userInfo.getUserPwd());

//            doMockLogin(strUser, MD5Helper.getMD5(strPwd));
//            saveUserAndPwd(this, strUser, MD5Helper.getMD5(strPwd));
        }
    }

    private void saveUserAndPwd(Context context, String strUser, String pwd) {
        //todo 保存用户名和密码
    }

    private void doMockLogin(String userName, String pwd) {
        //todo 登录
    }
}
```