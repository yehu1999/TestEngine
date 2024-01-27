# TestEngine
An exploratory attempt at a game engine with OpenGL/GLFW/C++/imGui

在学了图形学和图形渲染管线的一些知识后，我迫不及待地想做一个我自己的游戏引擎。
在开始这个小项目时，我并没有期望真的可以做出一个成品引擎，
我的最终目的是综合回顾前面学到的渲染管线相关知识(如果可以实现一些成果当然更好)，
并且可以为以后真正实现一个像模像样的游戏引擎铺路。
于是我按照自己对游戏引擎的简单想法一点一点地拼凑实现，
最终得到了这个，testEngine。

它有一个天空盒，
有Phong和PBR两种光照模式，
可以添加不同的光源，渲染基本的几何体，特定的模型，并可以自定义它们的材质，位置，旋转，颜色等属性。
你可以在这个世界中搭积木，
现在去看它的界面，将其中折叠的选项展开之后，
天哪！各种属性参数，密密麻麻的，简直有模有样了！
只是。。。越是实现了一些小功能，越是能明白游戏引擎制作的困难。
物体选中该怎么实现？testEngine用的是简单的物体列表索引选中，
可是我希望用鼠标选中它们，该如何实现呢？
或许可以从摄像机Z轴发射射线去选中？
可是每次实现一个新功能，testEngine都会因为整体架构的某处不适配而罢工，
每次罢工都要花费我不少时间去调整整个引擎的各处结构，
这些都是可以避免的问题，而问题的根源就在于一开始引擎的架构就是不合理的。
在多次为了解决新功能的不适配问题，引擎的架构已经被更改得更加不合理。
我已经可以预见后续的功能的添加或许需要将整个架构推翻重做。
更别说这个testEngine还没能实现的物理引擎，引擎剥离等等更为重要的功能了。
我觉得，是时候去学习一下大佬们是如何实现一个游戏引擎了(已经在学了)。

我将带着一个全新的，拥有完整功能的游戏引擎归来。

(Tranlated by Youdao)
After learning a bit about graphics and graphics rendering pipelines, I couldn't wait to make my own game engine.
When I started this little project, I didn't expect to actually build a finished engine,
My ultimate goal is to review everything I've learned about rendering pipelines (it would be nice if I could implement some of them),
And it can pave the way for a real game engine in the future.
So I built it bit by bit from my simple idea of a game engine,
And you end up with this, testEngine.

It has a sky box,
There are two illumination modes: Phong and PBR,
You can add different light sources, render basic geometry, specific models, and customize their material, position, rotation, color, and other properties.
You can build blocks in this world,
Now, if you look at the screen, if you expand the accordion option,
Oh my God! All kinds of attribute parameters, dense, simply have a shape!
It's just... The more the implementation of some small functions, the more can understand the difficulties of game engine production.
How to implement object selection? testEngine uses a simple object list index selection,
But I want to select them with the mouse. How do I do that?
Maybe we could shoot a ray from the z-axis of the camera?
But every time we implemented a new feature, testEngine would grind to a halt because something didn't fit in the overall architecture.
Every strike took me a long time to adjust the whole engine structure,
These are all avoidable problems, and the root of the problem is that the engine's architecture was wrong in the first place.
On several occasions, the engine's architecture has been changed to make more sense in order to solve the problem of new feature incompatibility.
I can already foresee that subsequent feature additions may require the entire architecture to be redone from the ground up.
Not to mention the physics engine, engine stripping, and more important features that testEngine does not yet implement.
I thought it was time to learn how the big guys implemented a game engine (I'm already learning).

I'm coming back with a new, fully functional game engine.

2024.1.27

下面是testEngine的运行截图
Below is a screenshot of testEngine in action

![image](https://github.com/yehu1999/TestEngine/assets/114279940/e9b8c5c4-8d52-46c7-8516-271e181a3afd)

![image](https://github.com/yehu1999/TestEngine/assets/114279940/624d0e48-23d5-4610-80fb-defc8e910ab8)

![image](https://github.com/yehu1999/TestEngine/assets/114279940/0028b778-e80c-49c6-83ba-3991d2953999)
