#pragma warning(disable:4996)

#include <windows.h>
#include <bgfx/c99/bgfx.h>
#include <bgfx/c99/platform.h>
#include <stdlib.h>
#include <stdio.h>

//=========================================================================
//常量
//=========================================================================

//窗口大小
#define					WND_W							800
#define					WND_H							600

//默认渲染器
#define					RENDER_TYPE						BGFX_RENDERER_TYPE_OPENGL

//=========================================================================
//全局变量
//=========================================================================
HINSTANCE							g_hInstance;
HWND								g_hMainWnd;

bgfx_program_handle_t				g_Program;
bgfx_shader_handle_t				g_VertexShader;
bgfx_shader_handle_t				g_FragShader;

bgfx_vertex_decl_t					g_VertexDecl;

bgfx_vertex_buffer_handle_t			g_VertexBuffer;
bgfx_index_buffer_handle_t			g_IndexBuffer;

//Debug
char								g_strtmp[256];


//=========================================================================
//子程式声明
//=========================================================================

void								InitBgfx();//初始化Bgfx
void								Update();//渲染函数

bgfx_shader_handle_t				LoadShader(const char *strFile);

LRESULT APIENTRY					MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool								RegClass(WNDCLASS &wc);

//窗口消息处理
void								OnCreate();

//Debug
void								OutputBreakLine(const char *str);

//=========================================================================
//数据定义
//=========================================================================
struct PosColorVertex
{
	float x, y, z;
	uint32_t rgba;
};

static const PosColorVertex triangles[] = {
	{ 0.0f,0.0f,1.0f,0xff0000ff },
	{ 1.0f,0.0f,1.0f,0xff0000ff },
	{ 1.0f,1.0f,1.0f,0xff0000ff },
};

static const uint16_t indexs[] = {
	0,1,2
};

//=========================================================================
//函数名称：WinMain
//说明：主函数
//输入：
//输出：
//返回：
//备注：
//日期：2018-9-13
//修改记录：
//=========================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	HWND hWnd;
	MSG Msg;
	int x, y;
	
	g_hInstance = hInstance;

	ZeroMemory(&wc, sizeof(wc));
	if (!RegClass(wc))
	{
		MessageBox(NULL, "注册窗口类失败！", "BgfxTest", MB_OK);
		return 1;
	}

	x = (GetSystemMetrics(SM_CXSCREEN) - WND_W) / 2;
	y = (GetSystemMetrics(SM_CYSCREEN) - WND_H) / 2;

	hWnd = CreateWindow("BgfxTestWnd", "BgfxTest", WS_OVERLAPPEDWINDOW,
							x, y,
							WND_W, WND_H,
							NULL,
							NULL,
							hInstance,
							NULL);

	ShowWindow(hWnd, nCmdShow);
	g_hMainWnd = hWnd;
	InitBgfx();

	while (GetMessage(&Msg, NULL, NULL, NULL))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);

		Update();
	}

	return (int)Msg.wParam;
}

//=========================================================================
//函数名称：Update
//说明：渲染函数
//输入：
//输出：
//返回：
//备注：
//日期：2018-9-13
//修改记录：
//=========================================================================
void Update()
{
	bgfx_set_vertex_buffer(0, g_VertexBuffer, 0, 3);
	bgfx_set_index_buffer(g_IndexBuffer, 0, 3);
	bgfx_set_state(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A | BGFX_STATE_PT_TRISTRIP, 0);
	bgfx_submit(0, g_Program, 0, true);

	bgfx_frame(false);
}


//=========================================================================
//函数名称：RegClass
//说明：注册窗口类
//输入：
//输出：
//返回：
//备注：
//日期：2018-9-13
//修改记录：
//=========================================================================
bool RegClass(WNDCLASS &wc)
{
	wc.lpszClassName = "BgfxTestWnd";
	wc.lpszMenuName = NULL;
	wc.hInstance = g_hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = MainWndProc;
	wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;

	if (RegisterClass(&wc)) return true;
	else return false;
}

//=========================================================================
//函数名称：MainWndProc
//说明：消息处理函数
//输入：
//输出：
//返回：
//备注：
//日期：2018-9-13
//修改记录：
//=========================================================================
LRESULT WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmID;
	int wmEvent;
	
	switch (uMsg)
	{
	case WM_CREATE:
		OnCreate();
		break;
	case WM_COMMAND:
		break;
	case WM_LBUTTONDOWN:
		
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_KEYDOWN:
		break;
	case WM_PAINT:
		break;
	case WM_SIZE:
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=========================================================================
//函数名称：InitBgfx
//说明：初始化Bgfx
//输入：
//输出：
//返回：
//备注：
//日期：2018-9-13
//修改记录：
//=========================================================================
void InitBgfx()
{
	//设置平台相关信息
	
	bgfx_platform_data_t platdata;
	
	ZeroMemory(&platdata, sizeof(platdata));
	platdata.nwh = g_hMainWnd;
	bgfx_set_platform_data(&platdata);

	bgfx_init_t init;

	bgfx_init_ctor(&init);

	init.type = RENDER_TYPE;

	bgfx_init(&init);

	bgfx_reset(WND_W, WND_H, BGFX_RESET_VSYNC, init.resolution.format);

	bgfx_set_debug(BGFX_DEBUG_TEXT);
	bgfx_set_view_clear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
		0xaaaaaaff, 1.0f, 0);
	bgfx_set_view_rect(0,0,0,WND_W, WND_H);
	
	

	OutputBreakLine("start");

	g_VertexShader = LoadShader("vs_testgl.bin");
	
	OutputBreakLine("end");

	g_FragShader = LoadShader("fs_testgl.bin");

	g_Program = bgfx_create_program(g_VertexShader, g_FragShader,true);

	bgfx_vertex_decl_begin(&g_VertexDecl, RENDER_TYPE);
	bgfx_vertex_decl_add(&g_VertexDecl, BGFX_ATTRIB_POSITION, 3, BGFX_ATTRIB_TYPE_FLOAT, false, false);
	bgfx_vertex_decl_add(&g_VertexDecl, BGFX_ATTRIB_COLOR0, 4, BGFX_ATTRIB_TYPE_UINT8, false, false);
	bgfx_vertex_decl_end(&g_VertexDecl);

	g_VertexBuffer=bgfx_create_vertex_buffer(bgfx_make_ref(triangles,sizeof(triangles)),&g_VertexDecl,BGFX_BUFFER_NONE);
	g_IndexBuffer=bgfx_create_index_buffer(bgfx_make_ref(indexs, sizeof(indexs)), BGFX_BUFFER_NONE);
}

//=========================================================================
//函数名称：OnCreate
//说明：窗口创建消息处理
//输入：
//输出：
//返回：
//备注：
//日期：2018-9-13
//修改记录：
//=========================================================================
void OnCreate()
{

}

//=========================================================================
//函数名称：LoadShader
//说明：
//输入：
//输出：
//返回：
//备注：
//日期：2018-9-13
//修改记录：
//=========================================================================
bgfx_shader_handle_t LoadShader(const char *strFile)
{
	bgfx_shader_handle_t shader;
	FILE *fp = fopen(strFile,"rb");

	if (!fp)
	{
		MessageBox(NULL, "Load Shader File Fail!", "Error", MB_OK);
		exit(1);
	}

	fseek(fp, 0,SEEK_END);

	unsigned int len = ftell(fp);

	unsigned char *pMem = (unsigned char *)malloc(len);

	fseek(fp, 0,SEEK_SET);

	fread(pMem, len, 1, fp);

	shader=bgfx_create_shader(bgfx_make_ref(pMem, len));

	fclose(fp);

	return shader;
}

//=========================================================================
//函数名称：OutputBreakLine
//说明：
//输入：
//输出：
//返回：
//备注：
//日期：2018-9-19
//修改记录：
//=========================================================================
void OutputBreakLine(const char *str)
{
	OutputDebugString("\n==================================================================\n");
	OutputDebugString(str);
	OutputDebugString("\n==================================================================\n");
}