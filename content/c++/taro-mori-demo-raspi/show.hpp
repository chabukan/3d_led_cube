#pragma once

#include "led.h"
#include <opencv2/opencv.hpp>
#include <string>

/*!
XY平面のパネルを描画する
@param[out] canvas 描画するキャンバス
@param[in] lt 描画を開始する左上の座標
@param[in] led 座標ごとのLEDの色
@param[in] ledsize LEDの表示サイズ
@param[in] z Z座標
@param[in] wirelen ワイヤー長
@param[in] wirecolor ワイヤー色
*/
inline void DrawPanelXY(
	cv::Mat & canvas,
	cv::Point2f const & lt,
	int const (&led)[LED_WIDTH][LED_HEIGHT][LED_DEPTH],
	int ledsize,
	int z,
	float wirelen,
	cv::Scalar const & wirecolor)
{
	for (int x = 0; x < LED_WIDTH; ++x){
		for (int y = 0; y < LED_HEIGHT; ++y){
			if (x < LED_WIDTH - 1 && y < LED_HEIGHT - 1){
				cv::Point2f p(lt.x + x * wirelen, lt.y + y * wirelen);
				cv::Size2f s(wirelen + 1, wirelen + 1);
				cv::rectangle(canvas, cv::Rect(p, s), wirecolor);
			}
		}
	}
	for (int x = 0; x < LED_WIDTH; ++x){
		for (int y = 0; y < LED_HEIGHT; ++y){
			int rgb = led[x][y][z];
			if (rgb == 0){
				continue;
			}
			cv::Scalar c((rgb >> 0) & 0xFF & 0xFF, (rgb >> 8) & 0xFF, (rgb >> 16) & 0xFF);
			cv::Point2f p(lt.x + x * wirelen, lt.y + y * wirelen);
			cv::circle(canvas, p, ledsize, c, CV_FILLED);
		}
	}
}

/*!
YZ平面のパネルを描画する
@param[out] canvas 描画するキャンバス
@param[in] lt 描画を開始する左上の座標
@param[in] led 座標ごとのLEDの色
@param[in] ledsize LEDの表示サイズ
@param[in] x X座標
@param[in] wirelen ワイヤー長
*/
inline void DrawPanelYZ(
	cv::Mat & canvas,
	cv::Point2f const & lt,
    int const (&led)[LED_WIDTH][LED_HEIGHT][LED_DEPTH],
	int ledsize,
	int x,
	float wirelen)
{
	for (int y = 0; y < LED_HEIGHT; ++y){
		for (int z = 0; z < LED_DEPTH; ++z){
			int rgb = led[x][y][z];
			if (rgb == 0){
				continue;
			}
			cv::Scalar c((rgb >> 0) & 0xFF & 0xFF, (rgb >> 8) & 0xFF, (rgb >> 16) & 0xFF);
			cv::Point2f p(lt.x + (LED_DEPTH - z - 1) * wirelen, lt.y + y * wirelen);	// z軸を水平反転させないと表示が逆になる
			cv::circle(canvas, p, ledsize, c, CV_FILLED);
		}
	}
}

/*!
3D LEDのシミュレータをウィンドウに表示する
@param[in] title ウィンドウタイトル
@param[in] led 発光パターン
*/
inline void ShowWindow(std::string const & title, int const (&led)[LED_WIDTH][LED_HEIGHT][LED_DEPTH])
{
	// const parameters
	const auto backgroundcolor = cv::Scalar::all(0x20);	// 背景色
	const auto wirecolor = cv::Scalar::all(0x40); // 配線色
	const float ratio = 1.2f; // 画像全体の縮尺
	const cv::Size2f backgroundsize(400 * ratio, 450 * ratio);	// 背景サイズ
	const int ledsize = 3;	// LEDサイズ
	const float depthmargin = 10 * ratio;	// 奥行き
	const float leftmargin = 50 * ratio;	// 画面の左から箱の距離
	const float swing = 8.0f * ratio; // 振り幅

	static double s_pattern = 0.0;
	s_pattern += CV_PI / 20;
	float parallax = static_cast<float>(std::sin(s_pattern) * swing);
	const auto zfactor = 0.25f;
	cv::Mat canvas(backgroundsize, CV_8UC3, backgroundcolor);
	for (int z = LED_DEPTH - 1; 0 <= z; --z){
		const float wirelen = (depthmargin - z / 8.0f);
		cv::Point2f const lt(leftmargin + parallax * z*zfactor + (LED_DEPTH + 4)* wirelen, wirelen * (LED_DEPTH - z*zfactor));
		DrawPanelXY(canvas, lt, led, ledsize, z, wirelen, wirecolor);
	}
	for (int x = LED_WIDTH - 1; 0 <= x; --x){
		const float wirelen = (depthmargin - x / 8.0f);
		cv::Point2f const lt(leftmargin + parallax * x * zfactor, wirelen * (LED_DEPTH - x*zfactor));
		DrawPanelYZ(canvas, lt, led, ledsize, x, wirelen);
	}
	cv::imshow(title, canvas);
	cv::waitKey(1);	// 書いておかないと表示されない
}
