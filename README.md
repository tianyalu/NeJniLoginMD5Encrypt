# NeJniLoginMD5Encrypt JNI语法规范以及登录在JNI层用MD5加密
## 一、JNI语法规范 
### 1.1 JNI数据类型
* JNI系统类型：JNIEnv(线程上下文)  
* 基本数据类型  
* 引用类型   
* 数组  
### 1.2 数据类型映射关系
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

### 1.3 JNI中使用jstring
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

### 1.4 JNI中使用jobject
* 对象的创建  
* 字段的读写  
    - 静态字段  
    - 实例字段  
* 方法的调用  
    - 静态方法  
    - 实例方法  

#### 1.4.1 JNI内部描述
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

#### 1.4.2 在JNI中对字段进行读写的步骤
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

#### 1.4.3 在JNI中对调用方法的步骤
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/invoke_method_process.png)  
例子：  
```c++
//invoke Message.obtain()
jclass cls = (*env)->FindClass(env, "android/os/Message");
jmethodID obtainMethod = (*env)->GetStaticMethodID(env, cls, "obtain", "()Landroid/os/Message;");
return (*env)->CallStaticObjectMethod(env, cls, obtainMethod);
```

#### 1.4.4 在JNI中创建对象的步骤  
![image](https://github.com/tianyalu/NeJniLoginMD5Encrypt/blob/master/show/create_object_process.png)  
例子：  
```c++
//new Message()
jclass cls = (*env)->FindClass(env, "android/os/Message");
jmethodID constructMethod = (*env)->GetMethodID(env, cls, "<init>", "()V");
return (*env)->NewObject(env, cls, constructMethod);
```

#### 1.4.5 在JNI中对数组操作的步骤 
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