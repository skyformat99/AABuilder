#include <windows.h>
#include<stdio.h>
#include<vector>
using namespace std;


typedef struct {
	int x;
	int y;
	char c;
}CharSet;

int divideNumber = 20;
int fontSize = 5;
HFONT       hfontPrev;

// char         show[1000][1000];
// int          textXa[1000];
// static char		moji[500][500];
// unsigned char image1[1000][1000];

vector<vector<string>> show;
vector<vector<vector<char>>> characters;
vector<vector<unsigned char>> image1;

BITMAP bm;//�f�o�b�N�p�Ɉړ����Ă��܂���
HANDLE      fileH;
int textY = 0;
int imgX=0,imgY=0;
void MakePhotoBitmap(HBITMAP hbmp, HDC hdc);
void makeFile(vector<vector<int>> show);
void makeFile(vector<vector<string>> show);
void refreshFile();
BITMAP yokoBibun(BITMAP, int);
BITMAP tateBibun(BITMAP, int);
vector<vector<vector<int>>> setImageInArray(BITMAP bm);
vector<vector<int>> yokoBibun(vector<vector<vector<int>>> origin, vector<vector<int>> bibuned);
vector<vector<int>> tateBibun(vector<vector<vector<int>>> origin);


// string       KindC[] = { "0","1","2","3","4","5","6","7","8","9","�O","�P","�Q","�R","�S","�T","�U","�V","�W","�X",//20
// "a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z",//27
// "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z",//27
// "��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��",//46
// "�A","�C","�E","�G","�I","�J","�L","�N","�P","�R","�T","�V","�X","�Z","�\","�^","�`","�c","�e","�g","�i","�j","�k","�l","�m","�n","�q","�t","�w","�z","��","�~","��","��","��","��","��","��","��","��","��","��","��","��","��","��",//46
// "�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�",//46
// "�","�","�","�","�","�","�","�","�","��","��","��","��","��","��","��","��","��","�@","�B","�D","�F","�H","�b","��","��","��",//27
// "!","\"","#","$","&","\\","(",")","-","=","^","~","|","\\",",","<",".",">","/","?","_","@","`","[","{",";","+",":","*","]","}"," " };//32
 			
 string KindC[] = { "0","1","2","3","4","5","6","7","8","a","b","c","d","e","f","g","h","i","j","m","n","o","p","q","r","s","t","u","v","w","x","y","z"
	 ,"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"
	 ,"!","\"","#","$","%","&","(",")","=","~","|","-","^","@","`","[","]","{","}","+",";","*",":","_","\\","/","?",".",">",",","<"," "};
//char KindC[] = {'-','j'};
int len = sizeof(KindC) / sizeof(KindC[0]);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmdLine, int nCmdShow)
{
	TCHAR      szAppName[] = TEXT("sample");
	HWND       hwnd;
	MSG        msg;
	WNDCLASSEX wc;
	RECT       rc;
	DWORD      dwStyle;
	BOOL       bRet;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinst;
	wc.hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	wc.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;
	wc.hIconSm = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);

	if (RegisterClassEx(&wc) == 0)
		return 0;

	dwStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
	SetRect(&rc, 0, 0,/* 640, 480*/1280, 960);
	AdjustWindowRect(&rc, dwStyle, FALSE);

	hwnd = CreateWindowEx(0, szAppName, szAppName, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hinst, NULL);
	if (hwnd == NULL)
		return 0;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			msg.message = WM_DESTROY;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hbmpMemP = NULL;

	static HDC     hdcBackbuffer = NULL;
	static HBITMAP hbmpBackbuffer = NULL;
	static HBITMAP hbmpBackbufferPrev = NULL;

	static HFONT hfont = NULL;

	switch (uMsg) {

	case WM_CREATE: {
		HDC hdc;


		CharSet     cs;
		hdc = GetDC(hwnd);

		hfont = CreateFont(fontSize, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, 0, 0, DEFAULT_QUALITY, 0, TEXT("MS�S�V�b�N"));
		if (hfont == NULL) {
			ReleaseDC(hwnd, hdc);
			return -1;
		}
		hbmpMemP = (HBITMAP)LoadImage(NULL, TEXT("a.bmp"), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
		if (hbmpMemP == NULL) {
			return -1;
		}

		refreshFile();

		hdcBackbuffer = CreateCompatibleDC(hdc);
		hbmpBackbuffer = CreateCompatibleBitmap(hdc,/* 640, 480*/1280, 960);
		if (hbmpBackbuffer == NULL) {
			ReleaseDC(hwnd, hdc);
			return -1;
		}
		hfontPrev = (HFONT)SelectObject(hdcBackbuffer, hfont);
		MakePhotoBitmap(hbmpMemP, hdcBackbuffer);//�摜�̃r�b�g�}�b�v���擾
		hbmpBackbufferPrev = (HBITMAP)SelectObject(hdcBackbuffer, hbmpBackbuffer);

		ReleaseDC(hwnd, hdc);
		makeFile(show);
		return 0;
	}

	case WM_PAINT: {
		/*		HDC         hdc;

		PAINTSTRUCT ps;
		BITMAP      bm;



		hdc = BeginPaint(hwnd, &ps);
		//	GetObject(hbmpMemP, sizeof(BITMAP), &bm);



		SelectObject(hdcBackbuffer,hfontPrev);


		BitBlt(hdc, 0, 0,1280,960, hdcBackbuffer, 0, 0, SRCCOPY);//�����ŕ`�悵�Ă�

		EndPaint(hwnd, &ps);*/

		return 0;
	}

	case WM_DESTROY:
		if (hbmpMemP != NULL) {
			DeleteObject(hbmpMemP);
		}
		if (hdcBackbuffer != NULL) {
			if (hbmpBackbuffer != NULL) {
				SelectObject(hdcBackbuffer, hbmpBackbufferPrev);
				DeleteObject(hbmpBackbuffer);
			}
			DeleteDC(hdcBackbuffer);
		}
		CloseHandle(fileH);
		DeleteObject(hfont);
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


vector<vector<vector<int>>> setImageInArray(BITMAP bm){
	LPBYTE lp = (LPBYTE)bm.bmBits;
	int	w = (bm.bmWidthBytes / 3);//�P�s�̃o�C�g�����s�N�Z���r�b�g���Q�S�{�R�̔{���o�C�g�������ꍇ�ɑΉ�
	vector<vector<vector<int>>> res;

	for(int y=0, y_max=bm.bmHeight-1; y<=y_max; y_max--){
		vector<vector<int>> row;
		for(int x=0; x<w; x++){
			vector<int> rgb;
			rgb.push_back(lp[0]);
			rgb.push_back(lp[1]);
			rgb.push_back(lp[2]);
			row.push_back(rgb);
			//lp+=3;
			lp = (LPBYTE)bm.bmBits + (y_max * ((w * 3) + bm.bmWidthBytes % 3)) + x * 3;
		}
		// lp += bm.bmWidthBytes % 3;
		res.push_back(row);
	}
	return res;
}


//�Ȃ񂩂��Ƃ����ςȊ����ɂȂ��Ă�
vector<vector<int>> tateBibun(vector<vector<vector<int>>> origin){
	int sumPast[3];
	int sumNow[3];
		//	int sP = (lp[0] + lp[1] + lp[2]) * 2;
		//	int sN;
	sumPast[0] = origin[0][0][0] * 2;
	sumPast[1] = origin[0][0][1] * 2;//                                           ������
	sumPast[2] = origin[0][0][2] * 2;
	vector<vector<int>> ret;
	
	for (int y = 0,y_max=origin.size(); y < y_max; y++) {//                                 �c����
	//	try {
			vector<int> row;
			for (int x = 0, x_max = origin[y].size(); x < x_max; x++) {
				sumNow[0] = origin[y][x][0] * 2;
				sumNow[1] = origin[y][x][1] * 2;
				sumNow[2] = origin[y][x][2] * 2;
				// sN = (lp[0] + lp[1] + lp[2]) * 2;
				if ((abs(sumNow[0] - sumPast[0]) > divideNumber) || (abs(sumNow[1] - sumPast[1]) > divideNumber) || (abs(sumNow[2] - sumPast[2]) > divideNumber)
					) {
					// 	if((abs(sN - sP) > divideNumber)){
					row.push_back(1);
				}
				else {
					row.push_back(0);
				}

				sumPast[0] = sumNow[0];
				sumPast[1] = sumNow[1];
				sumPast[2] = sumNow[2];
				
			}
			ret.push_back(row);
	}
	return ret;
}

//�Ȃ񂩂��Ƃ����ςȊ����ɂȂ��Ă�
vector<vector<int>> yokoBibun(vector<vector<vector<int>>> origin, vector<vector<int>> bibuned){
	int sumPast[3];
	int sumNow[3];
	//	int sP = (lp[0] + lp[1] + lp[2]) * 2;
	//	int sN;
	sumPast[0] = origin[0][0][0] * 2;
	sumPast[1] = origin[1][0][0] * 2;
	sumPast[2] = origin[2][0][0] * 2;
	//	sP = (lp[0] + lp[1] + lp[2]) * 2;
	vector<vector<int>> ret;
	/**************************************************************************************/
	for (int x = 0, x_max=origin[0].size(); x < x_max; x++) {
		 for (int y = 0,y_max=origin.size(); y < y_max; y++){
			sumNow[0] = origin[y][x][0] * 2;
			sumNow[1] = origin[y][x][1] * 2;
			sumNow[2] = origin[y][x][2] * 2;
			//sN = (lp[0] + lp[1] + lp[2]) * 2;
			if ((abs(sumNow[0] - sumPast[0]) > divideNumber) || (abs(sumNow[1] - sumPast[1]) > divideNumber) || (abs(sumNow[2] - sumPast[2]) > divideNumber)
				) {
				//	if((abs(sN - sP) > divideNumber)){
				bibuned[y][x]=1;
			}
			sumPast[0] = sumNow[0];
			sumPast[1] = sumNow[1];
			sumPast[2] = sumNow[2];
			//sP = sN;
		}
	}
	return bibuned;
}

vector<vector<int>> readCharBitMap(char readChar, HDC hdc,  MAT2 mat2){
	TEXTMETRIC tm;
	GLYPHMETRICS gm;
	BYTE alpha;
	DWORD dwBufferSize = GetGlyphOutline(hdc, readChar, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat2);
	LPBYTE lpBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBufferSize);
	GetGlyphOutline(hdc, readChar, GGO_GRAY8_BITMAP, &gm, dwBufferSize, lpBuffer, &mat2);
	GetTextMetrics(hdc, &tm);

	vector<vector<int>> ret;
	int max_x;
	if (gm.gmptGlyphOrigin.x<0)
		max_x = gm.gmBlackBoxX - gm.gmptGlyphOrigin.x;
	else
		max_x = gm.gmCellIncX;
	max_x = fontSize;

	for (int i = 0; i<tm.tmHeight; i++) {
		vector<int> temp;
			for (int j = 0; j<max_x; j++) { //�S�ĂP�i���̃r�b�g�j�ŏ�����
				temp.push_back(0);
		}
		ret.push_back(temp);
	}

	int nLine = ((gm.gmBlackBoxX + 3) / 4) * 4;
	int nWidth = gm.gmBlackBoxX;
	int nHeight = gm.gmBlackBoxY;
	int max_ox;
	if (gm.gmptGlyphOrigin.x >= 0)// && gm.gmptGlyphOrigin.x+nWidth<=max_x)
		max_ox = gm.gmptGlyphOrigin.x;
	else 
		max_ox = 0;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			if(j+max_ox >= max_x)
				break;
			alpha = *(lpBuffer + (i * nLine) + j); //�s�N�Z���r�b�g�̒l���擾
			if (alpha) {
				ret[tm.tmHeight-(gm.gmptGlyphOrigin.y+tm.tmDescent) + i][max_ox + j]=1;//�ǉ��s �W�͍��r�b�g
			}
		}
	}
	HeapFree(GetProcessHeap(), 0, lpBuffer);/**/
//	string file{readChar};
//	file += ".txt";
	//makeFile(ret);
	return ret;
}

void makeFile(vector<vector<string>> show) {
	DWORD      fileP;
	DWORD        er;
	BOOL         check;
	int i = 0, j = 0;
	fileH = CreateFile(TEXT("bild.txt"), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileH == INVALID_HANDLE_VALUE)
		exit(-1);
	for(int i=0; i<show.size(); i++) {
		string str="";
		for(int j=0; j<show[i].size(); j++) {
			str += show[i][j];
		}
		WriteFile(fileH, str.c_str(), strlen(str.c_str()), &fileP,NULL);
		// WriteFile(fileH, lookFor[i], /*textXa[i]+1*/bm.bmWidth + 1, &fileP, NULL);
		WriteFile(fileH, "\r\n", strlen("\r\n"), &fileP, NULL);
		fileP += j-1;
	}
	//check = WriteFile(fileH, "\r\n", 4, fileP,NULL);
	CloseHandle(fileH);
}

void makeFile(vector<vector<int>> show) {
	DWORD      fileP;
	DWORD        er;
	BOOL         check;
	int i = 0, j = 0;

	fileH = CreateFile(TEXT("bild.txt"), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileH == INVALID_HANDLE_VALUE)
		exit(-1);
	for(int i=0; i<show.size(); i++) {
		string str="";
		for(int j=0; j<show[i].size(); j++) {
			if (show[i][j] == 0)
				str += "0";
			else
				str += "1";
		}
		WriteFile(fileH, str.c_str(), strlen(str.c_str()), &fileP,NULL);
		// WriteFile(fileH, lookFor[i], /*textXa[i]+1*/bm.bmWidth + 1, &fileP, NULL);
		WriteFile(fileH, "\r\n", strlen("\r\n"), &fileP, NULL);
		fileP += j-1;
	}
	//check = WriteFile(fileH, "\r\n", 4, fileP,NULL);
	CloseHandle(fileH);
}


void refreshFile() {
	fileH = CreateFile(TEXT("bild.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	CloseHandle(fileH);
}




void MakePhotoBitmap(HBITMAP hbmp, HDC hdc)
{
	//Photo�p
	int    x, y = 10;
	LPBYTE lp;


	int sumNow[3], sumPast[3];
	int sN, sP;
	int w;
	//
	//moji�p
	int          i, j;
	int          nWidth, nHeight;
	int          nLine;
	BYTE         alpha;
	DWORD        dwBufferSize;
	LPBYTE       lpBuffer;
	MAT2 mat2 = {
		{ 0, 1 },{ 0, 0 },{ 0, 0 },{ 0, 1 }
	};

	//
	//comvine�p
	int          m, n;
	int          currentX = 0, currentY = 0;
	int          checkSum = 0, checkSumMax = -(INT_MAX)+2;
	int         mojiCode = 0;
	int         flag = 0;
	static CharSet      cs;
	int          textX = 0;
	int          ze = 0;

	//



	/**  �ȉ�����   **/
	GetObject(hbmp, sizeof(BITMAP), &bm);

	vector<vector<vector<int>>> img = setImageInArray(bm);

	vector<vector<int>> temp = tateBibun(img);

	vector<vector<int>> editImg= yokoBibun(img, temp);

	//read char bitmaps
	vector<vector<vector<int>>> charList;
	for(int i=0; i<len; i++){
		charList.push_back(readCharBitMap(KindC[i][0], hdc, mat2));
	}


	//�摜���[�v
	for(int imageY=0,imageY_max=editImg.size() ; imageY<imageY_max; imageY++){
		vector<string> rowChara; 
		for(int imageX=0,imageX_max=editImg[0].size() ; imageX<imageX_max; imageX++){
			//���������������[�v
			int checkMax=0;
			int index=0;
			for(int chara=0,chara_max=charList.size(); chara<chara_max; chara++){
				int checkSum=0;
				for(int charaY=0, charaY_max=charList[chara].size(); charaY<charaY_max; charaY++){
					//�T���͈͂��摜�𒴂����玀
					if( (charaY+imageY) >= imageY_max)
						break;

					for(int charaX=0, charaX_max=charList[chara][charaY].size(); charaX<charaX_max; charaX++){
						//�T���͈͂��摜�𒴂����玀
						if( (charaX+imageX) >= imageX_max)
							break;

						if(charList[chara][charaY][charaX] == editImg[charaY+imageY][charaX+imageX]){
							checkSum++;
						}
						else{
							checkSum-=2;
						}
					}
				}

				if(checkSum > checkMax){
					checkMax = checkSum;
					index = chara;
				}
			}
			imageX += charList[index][0].size();
			rowChara.push_back(KindC[index]);
			index=0;
		}
		imageY += charList[0].size();
		show.push_back(rowChara);
	}
	// makeFile(editImg);
}













//backup
	// void MakePhotoBitmap(HBITMAP hbmp, HDC hdc)
	// {
	// 	//Photo�p
	// 	int    x, y = 10;
	// 	LPBYTE lp;


	// 	int sumNow[3], sumPast[3];
	// 	int sN, sP;
	// 	int w;
	// 	//
	// 	//moji�p
	// 	int          i, j;
	// 	int          nWidth, nHeight;
	// 	int          nLine;
	// 	BYTE         alpha;
	// 	DWORD        dwBufferSize;
	// 	LPBYTE       lpBuffer;
	// 	GLYPHMETRICS gm;
	// 	MAT2 mat2 = {
	// 		{ 0, 1 },{ 0, 0 },{ 0, 0 },{ 0, 1 }
	// 	};
	// 	TEXTMETRIC tm;

	// 	//
	// 	//comvine�p
	// 	int          m, n;
	// 	int          currentX = 0, currentY = 0;
	// 	int          checkSum = 0, checkSumMax = -(INT_MAX)+2;
	// 	int         mojiCode = 0;
	// 	int         flag = 0;
	// 	static CharSet      cs;
	// 	int          textX = 0;
	// 	int          ze = 0;

	// 	//



	// 	/**  �ȉ�����   **/
	// 	GetObject(hbmp, sizeof(BITMAP), &bm);

	// 	w = (bm.bmWidthBytes / 3);//�P�s�̃o�C�g�����s�N�Z���r�b�g���Q�S�{�R�̔{���o�C�g�������ꍇ�ɑΉ�


	// 	/**************************************************************************************/
	// 	//������
	// 	bm = yokoBibun(bm,w);
	// 	/**************************************************************************************/
	// 	//�c����
	// 	bm = tateBibun(bm,w);
	// 	/*******************************�����܂�Photo����*******************************************************/

	// 	while (currentY < imgY-1/*bm.bmHeight*/) {
	// 		while (currentX < imgX-1/*bm.bmWidth*/) {

	// 			dwBufferSize = GetGlyphOutline(hdc, '�', GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat2);
	// 			lpBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBufferSize);
	// 			GetGlyphOutline(hdc, mojiCode, GGO_GRAY8_BITMAP, &gm, dwBufferSize, lpBuffer, &mat2);
	// 			GetTextMetrics(hdc, &tm);
	// 			HeapFree(GetProcessHeap(), 0, lpBuffer);
	// 			ze = 0;
	// 			for (i = currentY; i<(currentY + tm.tmHeight); i++) {
	// 				for (j = currentX; j<(currentX + gm.gmCellIncX); j++) {
	// 					ze += image1[i][j];
	// 				}
	// 			}
	// 			if (ze == '0'*(tm.tmHeight)*(gm.gmCellIncX)) {
	// 				cs.x = gm.gmCellIncX;//                    ��v���̍��������̉����ۑ�  
	// 				cs.y = tm.tmHeight;//                 �V          �����ۑ�
	// 				cs.c = '-';//                               �V    �����R�[�h�ۑ�
	// 				show[textY][textX] = cs.c;

	// 				currentX += cs.x;//                           ���������܂��Ă���ʒu���X�V

	// 				textX++;

	// 			}
	// 			//���������
	// 			else {
	// 				ze = '0';
	// 				/*******************************��������moji����******************************************************/
	// 				for (mojiCode = 0; mojiCode <len; mojiCode++) {
	// 					dwBufferSize = GetGlyphOutline(hdc, KindC[mojiCode], GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat2);
	// 					lpBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBufferSize);
	// 					GetGlyphOutline(hdc, KindC[mojiCode], GGO_GRAY8_BITMAP, &gm, dwBufferSize, lpBuffer, &mat2);
	// 					GetTextMetrics(hdc, &tm);
	// 					for (i = 0; i<tm.tmHeight; i++) {
	// 							for (j = 0; j<gm.gmCellIncX; j++) { //�S�ĂP�i���̃r�b�g�j�ŏ�����
	// 									moji[i][j] = '0';
	// 						}
	// 					}
	// 					nLine = ((gm.gmBlackBoxX + 3) / 4) * 4;
	// 					nWidth = gm.gmBlackBoxX;
	// 					nHeight = gm.gmBlackBoxY;
	// 					for (i = 0; i < nHeight; i++) {
	// 							for (j = 0; j < nWidth; j++) {
	// 									alpha = *(lpBuffer + (i * nLine) + j); //�s�N�Z���r�b�g�̒l���擾
	// 									if (alpha) {
	// 											moji[(/*tm.tmHeight -*/ gm.gmptGlyphOrigin.y) + i][gm.gmptGlyphOrigin.x + j] = '1';//�ǉ��s �W�͍��r�b�g
	// 							}
	// 						}
	// 					}
	// 					HeapFree(GetProcessHeap(), 0, lpBuffer);/**/
	// /*****************************************************************************************************/

	// /******************************������艺�ɕ����Ɖ摜�̈�v���T��*************************************/
	// 					for (m = 0; (m<gm.gmCellIncX) && ((m + currentX)<imgX/*bm.bmWidth*/); m++) {//�����̉������z���Ȃ��� ���� ���ݒn���摜�̉����z���Ȃ���
	// 						for (n = 0; (n<tm.tmHeight) && ((n + currentX)<imgY/*bm.bmHeight*/); n++) {//�����̍������z���Ȃ��� ���� ���ݒn���摜�̍������z���Ȃ���
	// 							if ((image1[currentY + n][currentX + m]) == moji[n][m]) {
	// 								checkSum++;
	// 							}
	// 							else {
	// 								checkSum -= 2;
	// 							}
	// 						}
	// 						flag = 1;
	// 					}
	// 					if ((checkSumMax < checkSum) && (flag == 1)) {
	// 						checkSumMax = checkSum;
	// 						cs.x = gm.gmCellIncX;//                    ��v���̍��������̉����ۑ�  
	// 						cs.y = tm.tmHeight;//                 �V          �����ۑ�
	// 						cs.c = KindC[mojiCode];//                               �V    �����R�[�h�ۑ�
	// 					}
	// 					checkSum = 0;//                             checkSum�̏�����
	// 					flag = 0;
	// 				}
	// 				show[textY][textX] = cs.c;

	// 				currentX += cs.x;//                           ���������܂��Ă���ʒu���X�V
	// 				checkSumMax = -(INT_MAX)+2;

	// 				textX++;
	// 			}
	// 			//�����܂ŕ�
	// 		}//�������ŏ��ɑ����̔��肷��Ƃ��̏����z
	// 		textXa[textY] = textX;
	// 		currentY += cs.y;//                           ��s�I����Ă���ʒu���X�V
	// 		currentX = 0;
	// 		textX = 0;
	// 		textY++;
	// 	}
	// }



	// BITMAP tateBibun(BITMAP bm, int w){
	// 	int x=0,y = 0;
	// 	int sumPast[3];
	// 	int sumNow[3];
	// 	LPBYTE lp = (LPBYTE)bm.bmBits + (y * (w * 3));
	// 	int sP = (lp[0] + lp[1] + lp[2]) * 2;
	// 	int sN;
	// 	sumPast[0] = lp[0] * 2;
	// 	sumPast[1] = lp[1] * 2;//                                           ������
	// 	sumPast[2] = lp[2] * 2;

	// 	for (x = 0; x < w; x++) {//                                 �c����
	// 		imgY=0;
	// 		for (y = 0; y < bm.bmHeight; y++) {
	// 			sumNow[0] = lp[0] * 2;
	// 			sumNow[1] = lp[1] * 2;
	// 			sumNow[2] = lp[2] * 2;
	// 			sN = (lp[0] + lp[1] + lp[2]) * 2;
	// 			if ((abs(sumNow[0] - sumPast[0]) > divideNumber) || (abs(sumNow[1] - sumPast[1]) > divideNumber) || (abs(sumNow[2] - sumPast[2]) > divideNumber)
	// 				) {
	// 				// 	if((abs(sN - sP) > divideNumber)){
	// 				image1[bm.bmHeight - 1 - y][x] = '1';
	// 			}
	// 			sumPast[0] = sumNow[0];
	// 			sumPast[1] = sumNow[1];
	// 			sumPast[2] = sumNow[2];
	// 			sP = sN;
	// 			lp = (LPBYTE)bm.bmBits + (y * ((w * 3) + bm.bmWidthBytes % 3)) + x * 3;
	// 			imgY++;
	// 		}
	// 		imgX++;
	// 	}
	// 	return bm;
	// }


	// BITMAP yokoBibun(BITMAP bm, int w){
	// 	LPBYTE lp = (LPBYTE)bm.bmBits;
	// 	int x=0,y = 0;

	// 	int sumPast[3];
	// 	int sumNow[3];
	// 	int sP = (lp[0] + lp[1] + lp[2]) * 2;
	// 	int sN;
	// 	sumPast[0] = lp[0] * 2;
	// 	sumPast[1] = lp[1] * 2;
	// 	sumPast[2] = lp[2] * 2;
	// 	sP = (lp[0] + lp[1] + lp[2]) * 2;
	// 	/**************************************************************************************/
	// 	for (y = 0; y < bm.bmHeight; y++) {//                                 ������
	// 		for (x = 0; x < w; x++) {
	// 			sumNow[0] = lp[0] * 2;
	// 			sumNow[1] = lp[1] * 2;
	// 			sumNow[2] = lp[2] * 2;
	// 			sN = (lp[0] + lp[1] + lp[2]) * 2;
	// 			if ((abs(sumNow[0] - sumPast[0]) > divideNumber) || (abs(sumNow[1] - sumPast[1]) > divideNumber) || (abs(sumNow[2] - sumPast[2]) > divideNumber)
	// 				) {
	// 				//	if((abs(sN - sP) > divideNumber)){
	// 				image1[bm.bmHeight - 1 - y][x] = '1';
	// 			}
	// 			else {
	// 				image1[bm.bmHeight - 1 - y][x] = '0';
	// 			}
	// 			sumPast[0] = sumNow[0];
	// 			sumPast[1] = sumNow[1];
	// 			sumPast[2] = sumNow[2];
	// 			sP = sN;
	// 			lp += 3;//RGB�����łR�o�C�g����̂Ł{�R�Ŕ�΂�
	// 		}
	// 		lp += bm.bmWidthBytes % 3;//�P�s�̃o�C�g�����s�N�Z���r�b�g���Q�S�{�P�C�Q�o�C�g�������ꍇ�ɑΉ�

	// 	}
	// 	return bm;
	// }


	// void makeFile() {
	// 	DWORD      fileP;
	// 	DWORD        er;
	// 	BOOL         check;
	// 	static char         lookFor[1000][1000];
	// 	int i = 0, j = 0;
	// 	fileH = CreateFile(TEXT("bild.txt"), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// 	if (fileH == INVALID_HANDLE_VALUE)
	// 		exit(-1);
	// 	while (i<textY-1) {
	// 	// while (/*i<textY-1/*/i<bm.bmHeight) {
	// 		// while (/*j < textXa[i]/*/j<bm.bmWidth) {
	// 		while (j < textXa[i]) {
	// 			lookFor[i][j] = show[i][j];
	// 			// lookFor[i][j] = image1[i][j];
	// 			j++;
	// 		}
	// 		j = 0;
	// 		WriteFile(fileH, lookFor[i], textXa[i]+1, &fileP,NULL);
	// 		// WriteFile(fileH, lookFor[i], /*textXa[i]+1*/bm.bmWidth + 1, &fileP, NULL);
	// 		WriteFile(fileH, "\r\n", strlen("\r\n"), &fileP, NULL);
	// 		i++;
	// 		fileP += j-1;
	// 	}
	// 	//check = WriteFile(fileH, "\r\n", 4, fileP,NULL);
	// 	CloseHandle(fileH);
	// }

	// void refreshFile() {
	// 	fileH = CreateFile(TEXT("bild.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// 	CloseHandle(fileH);
	// }
