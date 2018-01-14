#include "stdafx.h"

#pragma comment(lib,"SDL2.lib")  
#pragma comment(lib,"SDL2_image.lib")  
#pragma comment(lib,"SDL2_ttf.lib")

#include <iostream> 
#include<SDL.h>
#include<SDL_image.h>
#include<time.h>
#include<Windows.h>
#include<math.h>
#include<SDL_ttf.h>
#include<..\shape.h>
#include <stdlib.h>
#include<iomanip>
#include<string>

using namespace std;

// 解决乱码问题
#pragma once
#pragma execution_character_set("utf-8")

// 必须申明才能使用加载图片函数
#ifdef LOAD_JPG
#include <jpeglib.h>
#endif
// bool类型
#define _Bool bool
// 窗口标题
#define WINDOW_TITLE "俄罗斯方块"
// 窗口图标位置
#define WINDOW_ICON "image/head.jpg"
// 字体
#define FONT_ "font/cute.ttf"
// 图片资源
#define T_IMAGE_0 "image/tetrics_0.png"
#define T_IMAGE_1 "image/tetrics_1.png"
#define T_IMAGE_2 "image/tetrics_2.png"
#define T_IMAGE_3 "image/tetrics_3.png"
#define T_IMAGE_4 "image/tetrics_4.png"
#define T_IMAGE_5 "image/tetrics_5.png"
#define T_IMAGE_6 "image/tetrics_6.png"


class Tetric
{
private:
	// 一个方块的颜色
	int index;
	// 方块的位置
	int posX, posY;
	// 是否下落中
	bool droping;
	// 方块的类型,等于 类型*4 + 方向
	int type;
public:
	Tetric()
	{
		build();
	}

	// 初始化
	void build()
	{
		// 随机创建一个颜色,使用时间作为种子产生随机数
		srand((unsigned)time(nullptr));
		index = rand() % 6;
		// 高度默认为 -4
		posX = 3;
		posY = -3;
		// 初始化下落状态
		droping = true;
		// 初始化类型(28种)
		srand((unsigned)time(nullptr));
		type = rand() % 28;
	}

	void reBuild()
	{
		build();
	}

	// 将下一个形状复制过来
	void clone(Tetric t)
	{
		this->droping = t.isDroping();
		this->index = t.getIndex();
		this->posX = t.getX();
		this->posY = t.getY();
		this->type = t.getType();
	}

	// 获取真实的第一行高度
	int getRealY() {
		// 先找到对应的形状
		int m = type / 4;
		int n = type % 4;
		int i = 0;
		for (; i < 4; i++) {
			if (shapes[m][n][i][0] != 0 ||
				shapes[m][n][i][1] != 0 ||
				shapes[m][n][i][2] != 0 ||
				shapes[m][n][i][3] != 0)
			{
				break;
			}
		}
		return this->posY + i;
	}

	void setX(int x)
	{
		this->posX = x;
	}

	int getX()
	{
		return this->posX;
	}

	void setY(int y)
	{
		this->posY = y;
	}

	int getY()
	{
		return this->posY;
	}

	bool isDroping()
	{
		return this->droping;
	}

	int getType()
	{
		return this->type;
	}

	int getIndex()
	{
		return this->index;
	}

	void setType(int t)
	{
		this->type = t;
	}
};

// 窗口引用
SDL_Window* window = nullptr;
// 各种事件
SDL_Event event;
// 渲染器
SDL_Renderer* render = nullptr;
// 图标
SDL_Surface *icon = nullptr;
// 游戏循环控制逻辑
_Bool running = true;
// 时间种子
time_t timeStamp = time(nullptr);
// 每一帧的时间
const long FPS = 10;

// 游戏地图方格大小
const int MAP_UNIT = 28;
// 游戏地图行、列数
const int MAP_ROWS = 21;
const int MAP_COLUMES = 21;
// 游戏区域行、列数
const int GAME_ROWS = 20;
const int GAME_COLUMES = 10;
// 窗口大小
const int WINDOW_WIDTH = MAP_UNIT * MAP_COLUMES;
const int WINDOW_HEIGHT = MAP_UNIT * MAP_ROWS;
// 绘制文字标题部分的x坐标
const int TITLE_X = 12;
// 绘制内容部分的X坐标
const int CONTENT_X = 13;
// 字体高度
const int TEXT_HEGHT = (int)(MAP_UNIT * 0.8f);
const int TEXT_WIDTH = (int)(MAP_UNIT * 0.7f);
const int NUMBER_WIDTH = (int)(MAP_UNIT * 0.6f);
const int NUMBER_HEGHT = (int)(MAP_UNIT * 1.2f);
const int NOTICE_HEGHT = (int)(MAP_UNIT * 0.6f);
// 初始化地图数组
int map[20][10] = { 0 };
// 初始化方块数组
SDL_Texture * tetrics[6] = { nullptr };
// 文字
SDL_Texture *testText = nullptr;
SDL_Texture *testText2 = nullptr;
SDL_Texture *testText3 = nullptr;
SDL_Texture *testNotice1 = nullptr;
SDL_Texture *testNotice2 = nullptr;
SDL_Texture *noticeText1 = nullptr;
SDL_Texture *noticeText2 = nullptr;
SDL_Texture *noticeText3 = nullptr;
SDL_Texture *noticeText4 = nullptr;
SDL_Texture *noticeText5 = nullptr;
SDL_Texture *noticeText6 = nullptr;
// 分数的颜色
SDL_Color scoreColor = {255, 22, 250, 55};
// 初始化正在下落的、下一个
Tetric now, next;
// 游戏主循环
int process = 0;
int score = 0;
// 计算方式：一次消一行减去1，两行减去0.75f，三行0.50，四行0.25
float level = 100;
bool pause = false;
bool is_over = false;


SDL_RWops * getRWops(char * file)
{
	return SDL_RWFromFile(file, "rb");
}

SDL_Texture * getTexture(SDL_Renderer *&ren, char *file)
{
	SDL_RWops *rwops = getRWops(file);
	SDL_Surface *surf = nullptr;
	if (nullptr != strstr(file, ".jpg"))
	{
		surf = IMG_LoadJPG_RW(rwops);
	}
	else if (nullptr != strstr(file, ".png"))
	{
		surf = IMG_LoadPNG_RW(rwops);
	}
	else if (nullptr != strstr(file, ".gif"))
	{
		surf = IMG_LoadGIF_RW(rwops);
	}
	SDL_Texture *text = SDL_CreateTextureFromSurface(ren, surf);
	// 释放内存
	SDL_FreeRW(rwops);
	SDL_FreeSurface(surf);
	return text;
}

SDL_Texture * getFontTexture(const char * str, SDL_Color color, int fontSize) {
	// 初始化
	TTF_Init();
	// 第一个参数为字体路径，第二个参数为字体大小
	TTF_Font *font = TTF_OpenFont(FONT_, fontSize);
	// 将字符转化为SDL_Surface
	SDL_Surface * sur = TTF_RenderUTF8_Solid(font, str, color);
	// 创建纹理
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render, sur);
	TTF_Quit();
	return texture;
}

int initWindow()
{
	// 初始化失败
	if (SDL_Init(SDL_INIT_VIDEO)) {
		cout << "SDL2 could not initialize! SDL2_Error:" << SDL_GetError() << endl;
	}

	window = SDL_CreateWindow(
		WINDOW_TITLE,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_RESIZABLE);
	if (nullptr == window)
	{
		cout << "init window error!" << SDL_GetError() << endl;
		return -1;
	}

	cout << SDL_GetError() << endl;

	// 设置窗口图标
	SDL_RWops *rwops = getRWops(WINDOW_ICON);
	icon = IMG_LoadJPG_RW(rwops);
	SDL_FreeRW(rwops);
	SDL_SetWindowIcon(window, icon);
	// 创建成功，返回0
	return 0;
}

int initRender()
{
	// 创建渲染器
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (nullptr == window)
	{
		cout << "init window error!" << SDL_GetError() << endl;
		return -1;
	}
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	return 0;
}

// 释放窗口
void freeWindow()
{
	SDL_FreeSurface(icon);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

// 初始化游戏
void initGame(SDL_Renderer *&ren)
{
	// 初始化图片
	tetrics[0] = getTexture(ren, T_IMAGE_0);
	tetrics[1] = getTexture(ren, T_IMAGE_1);
	tetrics[2] = getTexture(ren, T_IMAGE_2);
	tetrics[3] = getTexture(ren, T_IMAGE_3);
	tetrics[4] = getTexture(ren, T_IMAGE_4);
	tetrics[5] = getTexture(ren, T_IMAGE_5);

	SDL_Color color = {255, 255, 255, 255};
	SDL_Color color2 = { 200, 200, 200, 200 };
	testText = getFontTexture("下一个", color, MAP_UNIT);
	testText2 = getFontTexture("分  数", color, MAP_UNIT);
	testText3 = getFontTexture("说  明", color, MAP_UNIT);
	noticeText1 = getFontTexture("左移：A、←", color2, MAP_UNIT * 0.6f);
	noticeText2 = getFontTexture("右移：D、→", color2, MAP_UNIT * 0.6f);
	noticeText3 = getFontTexture("加速：S、↓、Space", color2, MAP_UNIT * 0.6f);
	noticeText4 = getFontTexture("变形：Ctrl、R、L", color2, MAP_UNIT * 0.6f);
	noticeText5 = getFontTexture("重新开始：F2", color2, MAP_UNIT * 0.6f);
	noticeText6 = getFontTexture("暂停Enter、结束Esc", color2, MAP_UNIT * 0.6f);
}

void destroyGame()
{
	// 回收数组
	for (int i = 0; i < 6; i++)
	{
		SDL_DestroyTexture(tetrics[i]);
	}
	SDL_DestroyTexture(testText);
	SDL_DestroyTexture(testText2);
	SDL_DestroyTexture(testText3);
	SDL_DestroyTexture(noticeText1);
	SDL_DestroyTexture(noticeText2);
	SDL_DestroyTexture(noticeText3);
	SDL_DestroyTexture(noticeText4);
	SDL_DestroyTexture(noticeText5);
	SDL_DestroyTexture(noticeText6);
}

void restart()
{
	// 重置游戏
	memset(map, 0, sizeof(map));
	score = 0;
	level = 100;
	pause = false;
	is_over = false;
	now.reBuild();
	::next.reBuild();
}

// 判断是否超出地图,但不包括上边缘, 超出返回非0
int isOutOfMap(int x, int y, int shape)
{
	// 先找到对应的形状
	int m = shape / 4;
	int n = shape % 4;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			// 这时每个小方块的位置
			int realX = x + j;
			int realY = y + i;
			// 首先判断，竖方向有方块超出
			if (realY >= GAME_ROWS  && shapes[m][n][i][j] != 0)
			{
				return 1;
			}
			// 横方向
			if ((realX < 0 || realX >= 10) && shapes[m][n][i][j] != 0)
			{
				return 1;
			}

			// 判断是否与已有的方块重合, 方式：不为零
			if (realY >= 0 && realY < GAME_ROWS && realX >= 0 && realX < GAME_COLUMES
				&& shapes[m][n][i][j] != 0 && map[realY][realX] > 0)
			{
				return 1;
			}
		}
	}
	return 0;
}

// 落地，着陆的意思
void land()
{
	int m = now.getType() / 4;
	int n = now.getType() % 4;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			// 这时每个小方块的位置
			int realX = now.getX() + j;
			int realY = now.getY() + i;
			// 首先判断，竖方向有方块超出
			if (realX >= 0 && realX < GAME_COLUMES && realY >= 0 && realY < GAME_ROWS)
			{
				if (0 != shapes[m][n][i][j])
				{
					map[realY][realX] = now.getIndex() + 1;
				}
			}
		}
	}
}

// y为当前行
bool isFullRows(int y) {
	if (y >= 0 && y < GAME_ROWS)
	{
		return  map[y][0] > 0 &&
			map[y][1] > 0 &&
			map[y][2] > 0 &&
			map[y][3] > 0 &&
			map[y][4] > 0 &&
			map[y][5] > 0 &&
			map[y][6] > 0 &&
			map[y][7] > 0 &&
			map[y][8] > 0 &&
			map[y][9] > 0;
	}
	return false;
}

// y为当前行, 只要有内容就返回true
bool hasRect(int y) {
	if (y >= 0 && y < GAME_ROWS)
	{
		for (int i = 0; i < GAME_COLUMES; i++)
		{
			if (map[y][i] > 0) {
				return true;
			}
		}
	}
	return false;
}

// 重力效果使清空的那一行以上的内容下落
void gravity(int gY) {
	// 方法是冒泡，让空的一行上升，直到没有值为止
	for (int i = gY; i > 0; i--)
	{
		if (i - 1 >= 0 && hasRect(i - 1))
		{
			for (int j = 0; j < GAME_COLUMES; j++)
			{
				int flag = map[i][j];
				map[i][j] = map[i - 1][j];
				map[i - 1][j] = flag;
			}
		}
		else
		{
			break;
		}
	}
}

// 消除满一行的方块
void clear(int currY)
{
	void draw(bool);
	// 计算消除的行数
	int sumRow = 0;
	for (int i = currY; i < currY + 4; i++)
	{
		// 满行
		if (isFullRows(i))
		{
			for (int j = 0; j < GAME_COLUMES; j++)
			{
				map[i][j] = 0;
			}
			// 清除过后，下落
			draw(false);
			// 为了看到下落效果
			Sleep(100);
			gravity(i);
			sumRow++;
			draw(false);
			Sleep(100);
		}
	}

	// 计算分数
	if (1 == sumRow)
	{
		score += 10;
		if (level > 8)
		{
			level-=1.0f;
		}
	}
	else if (2 == sumRow)
	{
		score += 30;
		if (level > 8)
		{
			level-=0.75f;
		}
	}
	else if (3 == sumRow)
	{
		score += 50;
		if (level > 8)
		{
			level-=0.5f;
		}
	}
	else if (4 == sumRow)
	{
		score += 70;
		if (level > 8)
		{
			level-=0.25f;
		}
	}
	draw(false);
}

// 让方块下落,若遇到障碍返回非0
int drop()
{
	// 暂停状态
	if (pause || is_over)
	{
		return 1;
	}

	if (isOutOfMap(now.getX(), now.getY() + 1, now.getType()))
	{
		// 遍历赋值
		land();
		// 下落到底后，消除方块，这里注意，只需要清除当前方块以下四行，这里偷了懒的
		// 介于只是个demo就不在意细节了
		clear(now.getY());
		// 如果方块还没有完全显示就到底了,表示游戏结束
		if (now.getRealY() < 0)
		{
			cout << "Game Over!" << endl;
			is_over = true;
			return 1;
		}
		now.clone(::next);
		::next.reBuild();
		return 1;
	}
	now.setY(now.getY() + 1);
	return 0;
}

// 转向
int change()
{
	// 暂停状态
	if (pause || is_over)
	{
		return 1;
	}

	int m = now.getType() / 4;
	int n = now.getType() % 4;
	int i = (n + 1) % 4 + m * 4;
	if (isOutOfMap(now.getX(), now.getY(), i))
	{
		return 1;
	}
	// cout << m << "," << ((n + 1) % 4) << endl;
	now.setType(i);
	return 0;
}

// 左移qq
int moveLeft()
{
	// 暂停状态
	if (pause || is_over)
	{
		return 1;
	}

	if (isOutOfMap(now.getX() - 1, now.getY(), now.getType()))
	{
		return 1;
	}
	now.setX(now.getX() - 1);
	return 0;
}

// 右移
int moveRight()
{
	// 暂停状态
	if (pause || is_over)
	{
		return 1;
	}

	if (isOutOfMap(now.getX() + 1, now.getY(), now.getType()))
	{
		return 1;
	}
	now.setX(now.getX() + 1);
	return 0;
}

// 判断游戏是否结束
int game_over()
{
	return 0;
}

// 地图轮廓循环
SDL_Rect rect = {
	MAP_UNIT / 2 - 3 ,
	MAP_UNIT / 2 - 3 ,
	MAP_UNIT * GAME_COLUMES + 6 ,
	MAP_UNIT * GAME_ROWS + 6
};
SDL_Rect tetric = {
	0,0,MAP_UNIT, MAP_UNIT
};
SDL_Rect frame = {
	0, 0, WINDOW_WIDTH, WINDOW_HEIGHT
};

void draw(bool drawNow)
{

	// 清空渲染器
	SDL_RenderClear(render);
	SDL_SetRenderDrawColor(render, 0x33, 0x33, 0x33, 0xff);
	SDL_RenderFillRect(render, &frame);
	// 画出地图轮廓
	SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
	SDL_RenderDrawRect(render, &rect);
	// 画出文字引导部分

	// 画出地图部分
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			int random = map[j][i] - 1;
			// 把0作为空，但是赋值是加了个1
			if (random >= 0 && random < 6)
			{
				tetric.x = i * MAP_UNIT + MAP_UNIT / 2;
				tetric.y = j * MAP_UNIT + MAP_UNIT / 2;
				SDL_RenderCopy(render, tetrics[random], nullptr, &tetric);
			}
		}
	}

	// 画出下落方块
	// 先找到对应的形状
	int m = 0, n = 0;
	if (drawNow) {
		m = now.getType() / 4;
		n = now.getType() % 4;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				// 这时每个小方块的位置
				int realX = now.getX() + j;
				int realY = now.getY() + i;
				// 首先判断，竖方向有方块超出
				if (realX >= 0 && realX < GAME_COLUMES && realY >= 0 && realY < GAME_ROWS)
				{
					if (0 != shapes[m][n][i][j])
					{
						tetric.x = realX * MAP_UNIT + MAP_UNIT / 2;
						tetric.y = realY * MAP_UNIT + MAP_UNIT / 2;
						SDL_RenderCopy(render, tetrics[now.getIndex()], nullptr, &tetric);
					}
				}
			}
		}
	}

	// 下一个
	m = ::next.getType() / 4;
	n = ::next.getType() % 4;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			// 这时每个小方块的位置
			int realX = CONTENT_X + j;
			int realY = 2 + i;
			// 首先判断，竖方向有方块超出
			if (0 != shapes[m][n][i][j])
			{
				tetric.x = realX * MAP_UNIT + MAP_UNIT / 2;
				tetric.y = realY * MAP_UNIT + MAP_UNIT / 2;
				SDL_RenderCopy(render, tetrics[::next.getIndex()], nullptr, &tetric);
			}
		}
	}

	// 画文字
	SDL_Rect textNext;
	textNext.x = TITLE_X * MAP_UNIT;
	textNext.y = 1 * MAP_UNIT;
	textNext.w = TEXT_WIDTH * 3;
	textNext.h = TEXT_HEGHT;
	// 下一个
	SDL_RenderCopy(render, testText, nullptr, &textNext);
	// 分数
	textNext.y = 7 * MAP_UNIT;
	SDL_RenderCopy(render, testText2, nullptr, &textNext);
	// 说明
	textNext.y = 10 * MAP_UNIT;
	SDL_RenderCopy(render, testText3, nullptr, &textNext);

	// 分数值
	string sc = to_string(score);
	textNext.x = CONTENT_X * MAP_UNIT;
	textNext.y = 8 * MAP_UNIT + 10;
	textNext.w = sc.size() * NUMBER_WIDTH;
	textNext.h = NUMBER_HEGHT;
	SDL_Texture *scoreText = getFontTexture(sc.data(), scoreColor, MAP_UNIT);
	SDL_RenderCopy(render, scoreText, nullptr, &textNext);
	SDL_DestroyTexture(scoreText);

	// 提示信息
	textNext.y = 11 * MAP_UNIT + MAP_UNIT / 2;
	textNext.h = 24;
	textNext.w = (int)(NOTICE_HEGHT * 5.5f);
	SDL_RenderCopy(render, noticeText1, nullptr, &textNext);

	textNext.y = 12 * MAP_UNIT + MAP_UNIT / 2;
	textNext.w = (int)(NOTICE_HEGHT * 5.5f);
	SDL_RenderCopy(render, noticeText2, nullptr, &textNext);

	textNext.y = 13 * MAP_UNIT + MAP_UNIT / 2;
	textNext.w = (int)(NOTICE_HEGHT * 10.0f);
	SDL_RenderCopy(render, noticeText3, nullptr, &textNext);

	textNext.y = 14 * MAP_UNIT + MAP_UNIT / 2;
	textNext.w = (int)(NOTICE_HEGHT * 8.0f);
	SDL_RenderCopy(render, noticeText4, nullptr, &textNext);

	textNext.y = 15 * MAP_UNIT + MAP_UNIT / 2;
	textNext.w = (int)(NOTICE_HEGHT * 6.0f);
	SDL_RenderCopy(render, noticeText5, nullptr, &textNext);

	textNext.y = 16 * MAP_UNIT + MAP_UNIT / 2;
	textNext.w = (int)(NOTICE_HEGHT * 9.0f);
	SDL_RenderCopy(render, noticeText6, nullptr, &textNext);

	SDL_RenderPresent(render);
	// SDL_UpdateWindowSurface(window
	// cout << SDL_GetError() << endl;
}

int SDL_main(int argc, char *argv[])
{

	// 创建窗口
	if (initWindow() != 0)
	{
		return -1;
	}

	// 创建渲染器
	if (initRender() != 0)
	{
		return -1;
	}

	// 初始化游戏
	initGame(render);

	// 地图轮廓循环
	SDL_Rect rect = {
		MAP_UNIT / 2 - 3 ,
		MAP_UNIT / 2 - 3 ,
		MAP_UNIT * GAME_COLUMES + 6 ,
		MAP_UNIT * GAME_ROWS + 6
	};
	SDL_Rect tetric = {
		0,0,MAP_UNIT, MAP_UNIT
	};
	SDL_Rect frame = {
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT
	};

	while (running) {

		// 绘制部分
		int tel = (int)level;
		if (process % tel == 0)
		{
			// 游戏逻辑部分
			drop();
			draw(true);
		}
		// 监听事件,并做相应处理
		if (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			// 键盘按下
			else if (event.type == SDL_KEYDOWN)
			{
				// cout << event.key.keysym.sym << endl;
				if (event.key.keysym.sym == SDLK_SPACE)
				{
					while (!drop())
					{
						draw(true);
					}
				}
				else if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT)
				{
					moveLeft();
					draw(true);
				}
				else if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT)
				{
					moveRight();
					draw(true);
				}
				else if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
				{
					drop();
					draw(true);
				}
				else if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL
					|| event.key.keysym.sym == SDLK_r || event.key.keysym.sym == SDLK_l)
				{
					change();
					draw(true);
				}
				else if (event.key.keysym.sym == 13)
				{
					// 13为Enter键
					pause = !pause;
				}
				else if (event.key.keysym.sym == SDLK_F2)
				{
					// f2重新开始
					if (is_over)
					{
						restart();
					}
				}
				else if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					// ESC退出
					running = false;
				}
			}
		}

		// 保证每帧率控制在一定值
		time_t flag = time(nullptr) - timeStamp;
		if (flag < FPS)
		{
			Sleep(FPS - flag);
		}
		timeStamp = time(nullptr);
		// 计数器
		if (++process >= 100000 * level)
		{
			process = 0;
		}
	}

	destroyGame();
	freeWindow();
	return 0;
}