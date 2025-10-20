#include "framework.h"
#include "CPanelIo.h"
#include <gdiplus.h>

CSwitchImg::CSwitchImg(
	INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
	Image** _ppimg, INT32 _n_switch, INT32 _fcount, Gdiplus::Graphics* _pgraphic
)
{
	set_id(_id);
	set_base(ppt, psz, ptext);
	for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
		if (i < _n_switch) pimg[i] = _ppimg[i];
		else pimg[i] = NULL;
	}
	n_switch = _n_switch;
	fcount = _fcount;

	Rect _rc(pt.X, pt.Y, sz.Width, sz.Height); rc = _rc;
	//背景色セット用の矩形 書き込みイメージの2倍サイズをクリアして前回描画の残像を消す
	rc_clear = { rc.X- rc.Width/2 , rc.Y - rc.Height / 2, rc.Width*2, rc.Height*2}; 
	RectF _frc((REAL)pt.X, (REAL)pt.Y, (REAL)sz.Width, (REAL)sz.Height); frc = _frc;

	rect = { pt.X,pt.Y,pt.X + sz.Width,pt.Y + sz.Height };

	pgraphics = _pgraphic;

	value = ID_PANEL_LAMP_OFF;
}
CSwitchImg:: ~CSwitchImg() {
	for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
		delete pimg[i];
	}
}


HRESULT CSwitchImg::setup_flick(INT32 _n_flick, INT32 _fcount, INT32* pid) {
	fcount = _fcount;
	n_flick = _n_flick;
	for (int i = 0; i < n_flick; i++) {//フリッカさせる画像の順序取り込み
		if (i < N_IMG_SWITCH_MAX) list_flick_id[i] = pid[i];
	}
	return S_OK;
}

/// <summary>
/// 画像表示を更新し、パネルランプの状態に応じて描画処理を行います。
/// </summary>
/// <returns>処理が正常に完了した場合は S_OK、無効な値の場合は S_FALSE を返します。</returns>

static SolidBrush blackBrush(Color::Black);

HRESULT CSwitchImg::update() {
	INT16 id = 0;
	if (value != ID_PANEL_LAMP_FLICK) {
		if (value < 0) return S_FALSE;			//-1は無効値
		if (value >= n_switch) return S_FALSE;	//n_switch以上は無効値
		id_disp = value;
		pgraphics->DrawImage(pimg[id_disp], rc);
		InvalidateRect(hWnd, &rect, FALSE);
		return S_OK;
	}
	else {
		count++; if (count > fcount) {
			count = 0;
			id_disp++; if (id_disp >= n_flick) id_disp = 0;
			pgraphics->DrawImage(pimg[list_flick_id[id_disp]], rc);
			InvalidateRect(hWnd, &rect, FALSE);
		}
		return S_OK;
	}
	return S_FALSE;
}

/// <summary>
/// 画像の中心を指定し表示指定位置に合わせて画像を表示します。
/// </summary>
/// <param name="x_center">中心のX座標。</param>
/// <param name="y_center">中心のY座標。</param>
/// <returns>処理が成功した場合は S_OK を返します。</returns>
HRESULT CSwitchImg::update(int x, int y) {
	if (value < 0) return S_FALSE;			//-1は無効値
	if (value >= n_switch) return S_FALSE;	//n_switch以上は無効値
	id_disp = value;

	Rect dest_rc(x,	y,rc.Width,rc.Height);

	pgraphics->FillRectangle(&blackBrush, rc_clear); // 背景色セット
	pgraphics->DrawImage(pimg[id_disp], dest_rc);
}

/// <summary>

/// 画像の中心はスケールに応じて補正し、表示指定位置に合わせて表示します。
/// </summary>
/// <param name="x_center">画像の中心のX座標。</param>
/// <param name="y_center">画像の中心のY座標。</param>
/// <param name="scale_x">X方向のスケール値（拡大,縮小率）</param>
/// <param name="scale_y">Y方向のスケール値（拡大,縮小率）</param>
/// <param name="angle">回転角度（度）。</param>
/// <returns>操作が成功した場合は S_OK を返します。</returns>

/// <summary>
/// 表示エリアで画像を回転させる中心点と回転角度を指定します。
/// この指定により描画画像の座標軸原点（回転軸）を表示先座標の回転中心に合わせます
/// 次に描画画像の座標を回転させこの座標上に描画すると表示先に重ね合わせで描画される形となります
/// 描画時に描画座標上に指定したオフセットと倍率で書き込んで位置の調整をします
/// </summary>
/// <param name="x_center"></param>
/// <param name="y_center"></param>
/// <param name="angle"></param>
/// <param name="offset_x"></param>
/// <param name="offset_y"></param>
/// <param name="scale_w"></param>
/// <param name="scale_h"></param>
/// <returns></returns>
HRESULT CSwitchImg::update(int x_center, int y_center, float angle, int offset_x, int offset_y, float scale_w, float scale_h) {
	if (value < 0) return S_FALSE;			//-1は無効値
	if (value >= n_switch) return S_FALSE;	//n_switch以上は無効値
	id_disp = value;

	// 1.グラフィックオブジェクト現在の状態を保存
	Gdiplus::GraphicsState state = pgraphics->Save();

	pgraphics->TranslateTransform(x_center, y_center);
	pgraphics->RotateTransform(angle);

	Rect rc_dest(-offset_x,	-offset_y, rc.Width* scale_w,rc.Height* scale_h);
	

	//描画
//	pgraphics->FillRectangle(&blackBrush, rc_clear); // 背景色セット
	pgraphics->DrawImage(pimg[id_disp], rc_dest);

	pgraphics->ResetTransform();

	return S_OK;
}

/// <summary>
/// indexの画像の切り取り範囲を指定して初期設定領域をX,Y方向にシフトして描画
/// </summary>
/// <param name="dest_x">描画先X座標</param>
/// <param name="dest_y">描画先Y座標</param>
/// <param name="srcx">描画元切り取りX座標</param>
/// <param name="srcy">描画元切り取りY座標</param>
/// <param name="srcwidth">描画元切り取り画像幅</param>
/// <param name="srcheight">描画元切り取り画像高さ</param>
/// <returns></returns>
HRESULT CSwitchImg::update(int dest_x, int dest_y, int srcx, int srcy, int srcwidth, int srcheight) {

//	rc_clear.X = dest_x; rc_clear.Y = dest_y;
//	pgraphics->FillRectangle(&blackBrush, rc_clear); // 背景色セット

	if (value < 0) return S_FALSE;					//-1は無効値
	if (value >= n_switch) return S_FALSE;			//n_switch以上は無効値
	id_disp = value;

	pgraphics->DrawImage(pimg[id_disp], dest_x, dest_y, srcx, srcy, srcwidth,srcheight,UnitPixel);
	return S_OK;
}