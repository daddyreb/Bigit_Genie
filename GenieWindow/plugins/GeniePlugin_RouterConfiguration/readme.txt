断网时的操作
	停止所有soap的响应
	断网时所有的ui，除了 back按钮外，其它统统变灰色
	
重连的操作：
	info和gacc的刷新按钮变亮
	刷新info页面，这时info的刷新按钮变灰
	
刷mac的逻辑
	没有刷到mac			有seting pending
	第一次mac			有gacc的pending
	相同的mac			没有pending
	不相同的mac
	
	
登录流程
	1.是不是netgear路由器，卡线程刷新
	2.parental control接口判断用户名密码
	3. 刷新加密方式
	4. 刷新密码
		（刷新过程同时刷新了setting页面，刷新过程中modify按钮变灰色，即无法进入setting页面）
		刷新完成之后modify按钮和export按钮变亮
	5. gacc在登录之后第一次进入的时候会自动refresh，之后就不会了，所以登录后需要清理gacc的这个标志位
		登陆后gacc需要检查是否需要禁用gacc（从ini文件中进行判断）
	
modify按钮流程
	检查是不是netgear路由（快速返回的接口）
	进入setting模式
	
info中refresh按钮流程
	是不是netgear路由器(快速返回型)
		不是，则提示错误信息，同时modify和export两个按钮变灰
		是，则开始刷新流程
		
gacc refresh按钮流程
	所有编辑按钮变灰
	是不是netgea路由器
		是，则继续流程
		不是，则提示错误信息，编辑类按钮保持灰色
	获得正确信息
		是，编辑类按钮变亮
		不是，编辑类按钮保持灰色
		
gacc 启用和禁止gacc流程
	切换radiobutton的时候
		弹出对话框确认断网，
		开启pi，功能按钮变灰，等待重连，等待mac改变的信号
		(断网和重连的操作见本文件上面)
		
所有操作的超时一律跳到login
		
		
	
		
	