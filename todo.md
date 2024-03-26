# 3/24 17:43 在调试时，发现下列问题：

## 1、

camera.shootRaysOut()的所有射出射线的Origin似乎全是错的，计算错误了应该是，怎么x和y轴坐标在同时增长啊

## 2（已解决）、

在对camera调用其射出光线的ray.scatter()后，其反射射线的intensity[]全部为0了；反而折射射线的intensity也有问题，
五根散射射线，怎么除以4了，具体来说是在100减去反射射线的0后除以4，平等复制给了五根散射射线。
注：此时的反射率0.2，SCATTERED_RAYS设为了5，也许与floor(0.2*5)有关。

## 3、

在还是camera那部分，运行过程中，内存咋没了！！！？！ray怎么就指向一片无效内存了？？
问题发生 在第30根散射射线加入后（即rayIndex仍为0、ray.size()为480030），此时faceIdx为683171，参数设置同问题2。
太奇怪了，what the duck？我甚至没有什么头绪，主要是我不知道我该上哪儿去改，也许就是上面俩问题引起的吧。
以上。

# 3/24 19:13调试：

将分辨率改成了8x6，在第41根时出了问题，此时rays长度已达1560。
发生SIGSEGV的函数是判断光线与面元相交函数使用到的Ray与Vec叉积：Vec pvec = this->crossVec(edge2);
crossVec的这一行：this->direction.tail.y * other.tail.z - this->direction.tail.z * other.tail.y,
调试观察发现，this所指向的内存无法被读。，而 other 的内存是可读的。this的类型是 const Ray* const，other的类型是const Vec&
对了，17:43提出的问题就解决了一个。

JB的AI给出了建议：
    
If this is broken, it means that the Ray instance on which you're calling the crossVec function is not properly
instantiated or has been deleted or gone out of scope.
Considering this new information, the problem likely roots from the misuse of a Ray object rather than the crossVec
method itself. It's important to make sure that a Ray instance is correctly initialized and still in scope when you are
calling the crossVec method on it.
A careful tracking down of all the Ray instances and how they have been used would be necessary in this case.
You may want to look out for the following scenarios in your code:
If you created a Ray object dynamically (with new) and you deleted it (with delete), the memory block for this object is
deallocated. Accessing this 'dead' object would cause such an error.
If you created a Ray object within a local scope (for example, within a function or a block), and you're trying to
access it out of this local scope, this could also cause a segmentation fault.
Please search through your code where the Ray object which causes the segmentation fault is created, and used, and
ensure it is correctly initialized and not deleted or out of scope when crossVec is called on it.

